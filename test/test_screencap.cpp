#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <sys/ipc.h>
#include <sys/shm.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#endif  // !defined(_WIN32)

#include <cerrno>
#include <cstring>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace {

/**
 * PPM image buffer view.
 *
 * The buffer is expected to contain `width * height` RGB triplets.
 */
class ppm_view {
public:
  /**
   * Ctor.
   *
   * @param data Buffer to `width * height` RGB triplets (3 bytes each)
   * @param width Width in pixels
   * @param height Height in pixels
   */
  constexpr ppm_view(unsigned char* data, unsigned width, unsigned height) noexcept
    : data_{data}, width_{width}, height_{height}
  {}

  /**
   * Return image width in pixels.
   */
  constexpr auto width() const noexcept { return width_; }

  /**
   * Return image height in pixels.
   */
  constexpr auto height() const noexcept { return height_; }

  /**
   * Return total number of bytes in the buffer.
   */
  constexpr auto size() const noexcept
  {
    return 3u * width_ * height_;
  }

  /**
   * Return total pixels represented by the buffer.
   */
  constexpr auto pixels() const noexcept
  {
    return width_ * height_;
  }

  /**
   * Return a reference to the `i`th byte.
   */
  auto& operator[](std::size_t i) noexcept
  {
    return data_[i];
  }

  /**
   * Return a const reference to the `i`th byte,
   */
  auto& operator[](std::size_t i) const noexcept
  {
    return data_[i];
  }

private:
  unsigned char* data_;
  unsigned width_;
  unsigned height_;
};

/**
 * Write a PPM image buffer view to the given stream.
 *
 * The stream should have been opened in binary mode as bytes will be written.
 *
 * @param out Output stream
 * @param view PPM image buffer view
 */
auto& operator<<(std::ostream& out, const ppm_view& view)
{
  // whitespace-separated magic, width, height, max color value
  out <<
    "P6\n" <<
    std::to_string(view.width()) << " " <<
    std::to_string(view.height()) << "\n255\n";
  // write buffer bytes directly
  return out.write(reinterpret_cast<const char*>(&view[0]), view.size());
}

/**
 * Scoping helper to ensure that cleanup is done on scope exit.
 */
class scope_exit {
public:
  /**
   * Ctor.
   *
   * @note `std::function` move ctor is noexcept in C++20.
   *
   * @param f Nullary callable to invoke on scope exit
   */
  scope_exit(std::function<void()> f) /* noexcept */ : f_{std::move(f)} {}

  /**
   * Dtor.
   *
   * Invokes callable and swallows all exceptions.
   */
  ~scope_exit()
  {
    try { f_(); } catch (...) {}
  }

private:
  std::function<void()> f_;
};

}  // namespace

// TODO: wrap in separate function, use system_exception instead of exit, and
// have the actual main contain a wrapped try-block
// note: possibility for Windows: https://stackoverflow.com/a/3291261
int main()
{
  // open default X display (e.g. value given by DISPLAY)
  auto display = XOpenDisplay(nullptr);
  if (!display) {
    std::cerr << "Error: Unable to open X display" << std::endl;
    return EXIT_FAILURE;
  }
  // ensure display is cleaned up on scope exit
  scope_exit display_guard{[display] { XCloseDisplay(display); }};
  // default screen and root window
  int screen = DefaultScreen(display);
  auto root = RootWindow(display, screen);
  // window attributes
  XWindowAttributes window_attributes;
  if (!XGetWindowAttributes(display, root, &window_attributes)) {
    std::cerr << "Error: Unable to get X window attributes" << std::endl;
    return EXIT_FAILURE;
  }
  // window dimensions
  int width = window_attributes.width;
  int height = window_attributes.height;
  // create the image mapped in System V shared memory
  XShmSegmentInfo shminfo;
  // allocate XImage structure
  auto image = XShmCreateImage(
    display,
    DefaultVisual(display, screen),
    DefaultDepth(display, screen),
    ZPixmap,                         // RGB triplets
    nullptr,
    &shminfo,
    width,
    height
  );
  // create shared memory segment
  shminfo.shmid = shmget(
    IPC_PRIVATE,
    image->bytes_per_line * image->height,
    // TODO: maybe we can use more restrictive permissions here
    IPC_CREAT | 0777
  );
  if (!shminfo.shmid) {
    std::cerr << "Error: Unable to get shared memory segment: " <<
      std::strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }
  // ensure shmem is marked for destruction on scope exit
  scope_exit shm_guard{[id = shminfo.shmid] { shmctl(id, IPC_RMID, nullptr); }};
  // attach to shared memory segment
  shminfo.shmaddr = image->data = shmat(shminfo.shmid, nullptr, nullptr);
  if (shminfo.shmaddr == reinterpret_cast<void*>(-1)) {
    std::cerr << "Error: Unable to attach shared memory segment: " <<
      std::strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }
  // ensure detach is done on scope exit
  scope_exit shmdt_guard{[ptr = shminfo.shmaddr] { shmdt(ptr); }};
  // need to mark as writable
  shminfo.readOnly = False;
  // attach X server to shared memory segment
  if (!XShmAttach(display, &shminfo)) {
    std::cerr << "Error: Unable to attach X server to shared memory" << std::endl;
    return EXIT_FAILURE;
  }
  // capture root window display + ensure image is destroyed on scope exit
  // TODO: handle exit value
  XShmGetImage(display, root, image, 0, 0, AllPlanes);
  scope_exit image_guard{[image] { XDestroyImage(image); }};
  // ensure X shmem detach on scope exit
  // note: *must* be done *before* XDestroyImage() is called
  scope_exit xshm_guard{[display, &shminfo] { XShmDetach(display, &shminfo); }};
  // buffer for RGB triplets pixel data
  auto rgbs = std::make_unique<unsigned char[]>(3u * width * height);
  // iterate through pixels
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      // TDDO: consider handling byte order and color depth
      auto pixel = XGetPixel(image, x, y);
      // buffer + offset into data buffer
      auto offset = 3u * (y * width + x);
      auto buf = rgbs.get();
      buf[offset] = (pixel >> 16) & 0xFF;     // Red
      buf[offset + 1] = (pixel >> 8) & 0xFF;  // Green
      buf[offset + 2] = pixel & 0xFF;         // Blue
    }
  }
  // save the image data as a PPM file
  {
    constexpr auto ppm_path = "screenshot.ppm";
    std::ofstream fs{ppm_path, std::ios::binary};
    fs << ppm_view{rgbs.get(), width, height} << std::flush;
    std::cout << "Image saved to " << ppm_path << std::endl;
  }
  return EXIT_SUCCESS;
}

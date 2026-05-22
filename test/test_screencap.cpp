/**
 * @file test_screencap.cpp
 * @author Derek Huang
 * @brief C++ program that takes a screen capture and saves it as a PPM file
 * @copyright MIT License
 *
 * On Unix systems the running X window is captured while on Windows GDI is
 * used to screen capture the desktop display.
 */

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wingdi.h>
#include <WinUser.h>
#else
#include <sys/ipc.h>
#include <sys/shm.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif  // !defined(_WIN32)

#include <cstring>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

namespace {

/**
 * PPM image buffer.
 *
 * For an image with `width * height` pixels the number of bytes managed will
 * be `3 * width * height`, with pixels represented as RGB byte triplets.
 */
class ppm_image {
public:
  /**
   * Ctor.
   *
   * Allocates memory for a PPM image of size `width * height` in pixels.
   *
   * @param width Width in pixels
   * @param height Height in pixels
   */
  ppm_image(unsigned width, unsigned height)
    : data_{new unsigned char[3u * width * height]},
      width_{width},
      height_{height}
  {}

  /**
   * Copy ctor.
   */
  ppm_image(const ppm_image& other)
  {
    from(other);
  }

  /**
   * Move ctor.
   */
  ppm_image(ppm_image&& other) noexcept
  {
    from(std::move(other));
  }

  /**
   * Copy assignment operator.
   */
  auto& operator=(const ppm_image& other)
  {
    destroy();
    from(other);
    return *this;
  }

  /**
   * Move assignment operator.
   */
  auto& operator=(ppm_image&& other) noexcept
  {
    destroy();
    from(std::move(other));
    return *this;
  }

  /**
   * Dtor.
   */
  ~ppm_image()
  {
    destroy();
  }

  /**
   * Return a pointer to the first data byte.
   */
  auto data() const noexcept { return data_; }

  /**
   * Return image width in pixels.
   */
  auto width() const noexcept { return width_; }

  /**
   * Return image height in pixels.
   */
  auto height() const noexcept { return height_; }

  /**
   * Return total number of bytes in the image.
   */
  auto size() const noexcept
  {
    return 3u * width_ * height_;
  }

  /**
   * Return total pixels represented by the image.
   */
  auto pixels() const noexcept
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

  /**
   * Return an iterator to the first image byte.
   */
  auto begin() const noexcept
  {
    return data_;
  }

  /**
   * Return an iterator one past the last image byte.
   */
  auto end() const noexcept
  {
    return data_ + size();
  }

private:
  unsigned char* data_;
  unsigned width_;
  unsigned height_;

  /**
   * Copy-initialize from another PPM image.
   */
  void from(const ppm_image& other)
  {
    // allocate + copy data
    data_ = new unsigned char[other.size()];
    std::memcpy(data_, other.data_, other.size());
    // copy members
    width_ = other.width_;
    height_ = other.height_;
  }

  /**
   * Move-initialize from another PPM image.
   *
   * On completion the other PPM image has `nullptr` data and zero size.
   */
  void from(ppm_image&& other) noexcept
  {
    data_ = other.data_;
    width_ = other.width_;
    height_ = other.height_;
    other.data_ = nullptr;
    other.width_ = 0u;
    other.height_ = 0u;
  }

  /**
   * Deallocate the image bytes if any.
   */
  void destroy() noexcept
  {
    delete[] data_;
  }
};

/**
 * Write a PPM image buffer view to the given stream.
 *
 * The stream should have been opened in binary mode as bytes will be written.
 *
 * @param out Output stream
 * @param image PPM image
 */
auto& operator<<(std::ostream& out, const ppm_image& image)
{
  // whitespace-separated magic, width, height, max color value
  out <<
    "P6\n" <<
    std::to_string(image.width()) << " " <<
    std::to_string(image.height()) << "\n255\n";
  // write buffer bytes directly
  return out.write(reinterpret_cast<const char*>(image.data()), image.size());
}

/**
 * Scoping helper to ensure that cleanup is done on scope exit.
 */
class scope_exit {
public:
  /**
   * Ctor.
   *
   * @note `std::function` move ctor is `noexcept` in C++20.
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

/**
 * Return a PPM image representing the current screen capture.
 *
 * On Windows this is the screen capture of the entire desktop display while on
 * Unix/Linux the default X display is captured. GDI implementation referenced
 * https://learn.microsoft.com/en-us/windows/win32/gdi/capturing-an-image
 */
auto ppm_screen_capture()
{
#if defined(_WIN32)
  // device context for entire display
  auto hdc = GetDC(nullptr);
  if (!hdc)
    throw std::runtime_error{"unable to get device context for display"};
  // ensure device context handle is released on scope exit
  scope_exit hdc_guard{[hdc] { ReleaseDC(nullptr, hdc); }};
  // create target memory device context
  auto hdc_tgt = CreateCompatibleDC(hdc);
  if (!hdc_tgt)
    throw std::runtime_error{"unable to create target memory device context"};
  // ensure memory device context is destroyed on scope exit
  scope_exit hdc_tgt_guard{[hdc_tgt] { DeleteDC(hdc_tgt); }};
  // get dimensions of desktop window for CreateCompatibleBitMap()
  RECT dims;
  if (!GetClientRect(GetDesktopWindow(), &dims))
    throw std::system_error{
      static_cast<int>(GetLastError()), std::system_category(),
      "GetDesktopWindow()"
    };
  // desktop width and height
  auto width = dims.right;
  auto height = dims.bottom;
  // create compatible bitmap with the display device context + desktop dims
  auto hbm = CreateCompatibleBitmap(hdc, width, height);
  if (!hbm)
    throw std::runtime_error{"unable to create compatible bitmap"};
  // ensure bitmap is deleted on scope exit
  scope_exit hbm_guard{[hbm] { DeleteObject(hbm); }};
  // select bitmap into target device context
  if (!SelectObject(hdc_tgt, hbm))
    throw std::runtime_error{"unable to replace existing device context bitmap"};
  // bit-block transfer color data from display DC into target DC
  if (!BitBlt(hdc_tgt, 0, 0, width, height, hdc, 0, 0, SRCCOPY))
    throw std::system_error{
      static_cast<int>(GetLastError()), std::system_category(),
      "BitBlt()"
    };
  // populate BITMAPINFO structure for GetDIBits()
  BITMAPINFO bmi;
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = width;
  // note: use negated height for (0, 0) to be in top-left corner
  bmi.bmiHeader.biHeight = -height;
  bmi.bmiHeader.biPlanes = 1;
  // note: use 24 bits for BGR color
  bmi.bmiHeader.biBitCount = 24;
  bmi.bmiHeader.biCompression = BI_RGB;
  // note: can be zero for uncompressed bitmaps
  bmi.bmiHeader.biSizeImage = 0;
  bmi.bmiHeader.biXPelsPerMeter = 0;
  bmi.bmiHeader.biYPelsPerMeter = 0;
  // no color table indices used + all colors important
  bmi.bmiHeader.biClrUsed = 0;
  bmi.bmiHeader.biClrImportant = 0;
  // get actual bitmap buffer width in bytes that accounts for padding
  auto bmp_width = GDI_DIBWIDTHBYTES(bmi.bmiHeader);
  auto bmp_size = bmp_width * height;
  // buffer with appropriate padding for RGB pixels
  auto bmp = std::make_unique<unsigned char[]>(bmp_size);
  // retrieve device-independent bit representation into buffer
  switch (GetDIBits(hdc, hbm, 0, height, &bmp[0], &bmi, DIB_RGB_COLORS)) {
  case 0:
    throw std::runtime_error{"GetDIBits(): error"};
  case ERROR_INVALID_PARAMETER:
    throw std::runtime_error{"GetDIBits(): invalid parameter"};
  default:
    break;
  }
  // new PPM image + iterators
  // note: no list-init to allow narrowing conversion
  ppm_image img(width, height);
  auto img_it = img.begin();
  auto bmp_it = &bmp[0];
  // note: iterate row-by-row so height is leading dimension
  for (int y = 0; y < height; y++) {
    // fill row pixels
    for (int x = 0; x < width; x++) {
      *img_it++ = *(bmp_it + 2);  // B -> R
      *img_it++ = *(bmp_it + 1);  // G -> G
      *img_it++ = *bmp_it;        // R -> B
      bmp_it += 3;                // advance to next bitmap triplet
    }
    // skip the remaining padding bytes
    bmp_it += (bmp_width - 3 * width);
  }
  // done
  return img;
#else
  // open default X display (e.g. value given by DISPLAY)
  auto dpy = XOpenDisplay(nullptr);
  if (!dpy)
    throw std::runtime_error{"unable to open X display"};
  // get default root window + ensure display is cleaned up on scope exit
  auto win = DefaultRootWindow(dpy);
  scope_exit display_guard{[dpy] { XCloseDisplay(dpy); }};
  // window attributes
  XWindowAttributes winattr;
  if (!XGetWindowAttributes(dpy, win, &winattr))
    throw std::runtime_error{"unable to get X window attributes"};
  // window dimensions
  int width = winattr.width;
  int height = winattr.height;
  // obtain image of the entire display as RGB triplets
  auto ximg = XGetImage(dpy, win, 0, 0, width, height, AllPlanes, ZPixmap);
  if (!ximg)
    throw std::runtime_error{"unable to capture X display image"};
  // ensure XImage is cleaned up on scope exit
  scope_exit image_guard{[ximg] { XDestroyImage(ximg); }};
  // new PPM image + iterator
  ppm_image img{width, height};
  auto img_it = img.begin();
  // get image pixels and convert to RGB triplets
  // note: iterate row-by-row of the image so height is the leading dimension
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // get pixel
      // TDDO: consider handling byte order and color depth
      auto pixel = XGetPixel(ximg, x, y);
      // mask red, green, and blue bytes
      *img_it++ = (pixel >> 16) & 0xFF;
      *img_it++ = (pixel >> 8) & 0xFF;
      *img_it++ = pixel & 0xFF;
    }
  }
  // done
  return img;
#endif  // !defined(_WIN32)
}

}  // namespace

int main() try
{
  // get screen capture as a PPM image
  auto img = ppm_screen_capture();
  // write screen capture as a PPM image
  // TODO: path currently hardcoded
  constexpr auto ppm_path = "screencap.ppm";
  {
    std::ofstream fs{ppm_path, std::ios::binary};
    fs << img;
  }
  // print and exit
  std::cout << "Image saved to " << ppm_path << std::endl;
  return EXIT_SUCCESS;
}
catch (const std::exception& exc) {
  std::cerr << "Exception: " << exc.what() << std::endl;
  return EXIT_FAILURE;
}
catch (...) {
  std::cerr << "Exception: Unknown exception" << std::endl;
  return EXIT_FAILURE;
}

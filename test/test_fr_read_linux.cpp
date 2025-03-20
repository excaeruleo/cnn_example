#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to save image data as a PPM file
void save_image_as_ppm(const char* filename, int width, int height, unsigned char* data) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Unable to open file for writing\n");
        return;
    }
    fprintf(file, "P6\n%d %d\n255\n", width, height);
    fwrite(data, 1, width * height * 3, file);
    fclose(file);
    printf("Image saved as %s\n", filename);
}

int main() {
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    XWindowAttributes window_attributes;
    XGetWindowAttributes(display, root, &window_attributes);

    int width = window_attributes.width;
    int height = window_attributes.height;

    // Create the image
    XImage* image = NULL;
    XShmSegmentInfo shminfo;

    // Set up shared memory for capturing the screen
    image = XShmCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen),
                            ZPixmap, NULL, &shminfo, width, height);
    shminfo.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height, IPC_CREAT | 0777);
    shminfo.shmaddr = image->data = (char*)shmat(shminfo.shmid, 0, 0);
    shminfo.readOnly = False;

    if (!XShmAttach(display, &shminfo)) {
        fprintf(stderr, "Unable to attach shared memory\n");
        return 1;
    }

    // Capture the screen
    XShmGetImage(display, root, image, 0, 0, AllPlanes);

    // Convert image data from XImage to raw RGB format
    unsigned char* data = (unsigned char*)malloc(width * height * 3);
    if (!data) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned long pixel = XGetPixel(image, x, y);
            int index = (y * width + x) * 3;
            data[index] = (pixel >> 16) & 0xFF;   // Red
            data[index + 1] = (pixel >> 8) & 0xFF; // Green
            data[index + 2] = pixel & 0xFF;        // Blue
        }
    }

    // Save the image data as a PPM file
    save_image_as_ppm("screenshot.ppm", width, height, data);

    // Cleanup
    free(data);
    XShmDetach(display, &shminfo);
    XDestroyImage(image);
    shmdt(shminfo.shmaddr);
    shmctl(shminfo.shmid, IPC_RMID, 0);
    XCloseDisplay(display);

    return 0;
}


#ifndef OPENBL_H
#define OPENBL_H

#include <linux/fb.h>

#define INTERVAL 8333333L // 1/120 seconds
// #define INTERVAL 16666666L // 1/60 seconds
// #define INTERVAL 100000000L  // 1/10 seconds
#define ONE_SEC 1000000000L // 1s

#define STOPPED 0
#define RUNNING 1

typedef struct fb_var_screeninfo fb_var_screeninfo;
typedef struct fb_fix_screeninfo fb_fix_screeninfo;

typedef struct fb_data {
    int fd;
    unsigned width;
    unsigned height;
    float ratio;
    unsigned bpp;
    unsigned line_length;
    unsigned long screensize;
} fb_data_t;

void unmap_close(void *data, int size, int fd);
char *fb_init(fb_data_t *fb_data);

void *listen_input(void *args);
void update();

void print_shapes(void);

#endif // OPENBL_H

#include <fcntl.h>
#include <linux/fb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <math.h>

#include "include/OpenBL.h"
#include "include/b_lib.h"
#include "include/mesh.h"

char *buf;
char *fbp;

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
fb_data_t fb_data;

char *buf;
char *fbp;

mesh_t cube = {0};
mesh_t cube_out = {0};

void cube_init(mesh_t *mesh);
void cube_init_alt(mesh_t *mesh);

char is_running = RUNNING;

int main(int argc, const char *argv[]) {
    fbp = fb_init(&fb_data);
    if (fbp == NULL) {
        puts("Oopsie");
        return 1;
    }
    buf = malloc(fb_data.screensize);

    pthread_t pt;
    int res;
    if ((res = pthread_create(&pt, NULL, listen_input, NULL)) != 0) {
        printf("Failed to create thread: %d", res);
        return 1;
    }

    struct timespec ts = {
        .tv_nsec = INTERVAL,
        .tv_sec = 0,
    };
    cube_init_alt(&cube);
    cube_init_alt(&cube_out);

    while (is_running == RUNNING) {
        update();
        memcpy(fbp, buf, fb_data.screensize);
        memset(buf, 0, fb_data.screensize);
        nanosleep(&ts, NULL);
    }

    if ((res = pthread_join(pt, NULL)) != 0) {
        printf("Failed to join thread: %d", res);
    }
    free(buf);
    mesh_destroy(&cube);
    unmap_close(fbp, fb_data.screensize, fb_data.fd);
    return 0;
}
double dz = 0.1;
double theta = 0.1;
void update() {
    rotate_x(&cube, &cube_out, theta);
    rotate_y(&cube_out, &cube_out, theta);
    rotate_z(&cube_out, &cube_out, theta);

    // move cube further away so you can see it
    translate_z(&cube_out, &cube_out, 1);
    draw_mesh3d(&cube_out, 0x00FF00FF);
    theta += 0.01;
}

void cube_init(mesh_t *mesh) {
    mesh_init(mesh);
    double p = 0.25f, n = -0.25f;

    // SOUTH
    mesh_push(mesh, (dbl_tri_t){n, n, n, n, p, n, p, p, n});
    mesh_push(mesh, (dbl_tri_t){n, n, n, p, p, n, p, n, n});

    // EAST
    mesh_push(mesh, (dbl_tri_t){p, n, n, p, p, n, p, p, p});
    mesh_push(mesh, (dbl_tri_t){p, n, n, p, p, p, p, n, p});

    // NORTH
    mesh_push(mesh, (dbl_tri_t){p, n, p, p, p, p, n, p, p});
    mesh_push(mesh, (dbl_tri_t){p, n, p, n, p, p, n, n, p});

    // WEST
    mesh_push(mesh, (dbl_tri_t){n, n, p, n, p, p, n, p, n});
    mesh_push(mesh, (dbl_tri_t){n, n, p, n, p, n, n, n, n});

    // TOP
    mesh_push(mesh, (dbl_tri_t){n, p, n, n, p, p, p, p, p});
    mesh_push(mesh, (dbl_tri_t){n, p, n, p, p, p, p, p, n});

    // BOTTOM
    mesh_push(mesh, (dbl_tri_t){p, n, p, n, n, p, n, n, n});
    mesh_push(mesh, (dbl_tri_t){p, n, p, n, n, n, p, n, n});
}

// set up the points in a way that lets me skip drawing the diagonal
// to make a cleaner cube
void cube_init_alt(mesh_t *mesh) {
    mesh_init(mesh);
    double p = 0.25f, n = -0.25f;

    // SOUTH
    mesh_push(mesh, (dbl_tri_t){n, n, p, n, n, n, p, n, n});
    mesh_push(mesh, (dbl_tri_t){n, n, p, p, n, p, p, n, n});

    // EAST
    mesh_push(mesh, (dbl_tri_t){p, n, p, p, n, n, p, p, n});
    mesh_push(mesh, (dbl_tri_t){p, n, p, p, p, p, p, p, n});

    // NORTH
    mesh_push(mesh, (dbl_tri_t){n, p, p, p, p, p, p, n, p});
    mesh_push(mesh, (dbl_tri_t){n, p, p, n, n, p, p, n, p});

    // WEST
    mesh_push(mesh, (dbl_tri_t){n, n, p, n, n, n, n, p, n});
    mesh_push(mesh, (dbl_tri_t){n, n, p, n, p, p, n, p, n});

    // TOP
    mesh_push(mesh, (dbl_tri_t){n, p, p, n, n, p, p, n, p});
    mesh_push(mesh, (dbl_tri_t){n, p, p, p, p, p, p, n, p});

    // BOTTOM
    mesh_push(mesh, (dbl_tri_t){n, p, n, n, n, n, p, n, n});
    mesh_push(mesh, (dbl_tri_t){n, p, n, p, p, n, p, n, n});
}

void *listen_input(void *args) {
    getchar();
    is_running = STOPPED;
    return NULL;
}

char *fb_init(fb_data_t *fb_data) {
    int fd = open("/dev/fb0", O_RDWR);
    if (fd == -1) {
        perror("Failed to open framebuffer device");
        return NULL;
    }

    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Failed to get variable screen info");
        close(fd);
        return NULL;
    }

    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Failed to get fixed screen info");
        close(fd);
        return NULL;
    }

    // save static info in a struct for external use
    fb_data->width = vinfo.xres;
    fb_data->height = vinfo.yres;
    fb_data->ratio = (float)vinfo.yres / vinfo.xres;
    fb_data->bpp = vinfo.bits_per_pixel;
    fb_data->line_length = finfo.line_length;
    fb_data->screensize = finfo.line_length * vinfo.yres;

    char *fbp = (char *)mmap(0, fb_data->screensize, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);

    if (fbp == MAP_FAILED) {
        perror("Failed to mmap framebuffer device");
        close(fb_data->fd);
        return NULL;
    }
    return fbp;
}

void unmap_close(void *data, int size, int fd) {
    munmap(data, size);
    close(fd);
}

// early code for testing drawing, draws a pizza
void calc_points(point_t points[], int radius, int n) {
    double two_pi = M_PI * 2.0;
    for (int k = 0; k < n; k++) {
        points[k].x = sin(((float)k / n) * two_pi) * radius;
        points[k].y = cos(((float)k / n) * two_pi) * radius;
    }
}

void draw_shape(char *buf, fb_data_t fb_data, point_t centre, int radius,
                int segments, uint32_t col) {
    if (segments < 1)
        return;

    point_t points[segments];

    calc_points(points, radius, segments);

    for (int i = 0; i < segments; i++) {
        point_t end;
        p_add(centre, points[i], &end);
        draw_line(centre, end, col);
    }
    draw_circle(centre, radius, col);
}

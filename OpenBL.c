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

mesh_t mesh = {0};
mesh_t mesh_out = {0};

typedef enum {
    CUBE,
    TORUS,
} shape;
char *shapes[] = {"cube", "torus", "\0"};

void cube_init(mesh_t *mesh);
void torus_init(mesh_t *mesh, const double R, const double r);

void animate_torus(void);
void animate_cube(void);

char is_running = RUNNING;

int main(int argc, char *argv[]) {
    int shape = -1;
    if (argc >= 2) {
        for (int i = 0; *shapes[i] != '\0'; i++) {
            if (strcmp(argv[1], shapes[i]) == 0) {
                shape = i;
                break;
            }
        }
    }

    if (shape == -1) {
        print_shapes();
        return 1;
    }

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

    if (shape == CUBE) {
        cube_init(&mesh);
        cube_init(&mesh_out);
    } else if (shape == TORUS) {
        torus_init(&mesh, 0.35, 0.15);
        torus_init(&mesh_out, 0.35, 0.15);
    }

    while (is_running == RUNNING) {
        if (shape == CUBE) {
            animate_cube();
        } else if (shape == TORUS) {
            animate_torus();
        }

        memcpy(fbp, buf, fb_data.screensize);
        memset(buf, 0, fb_data.screensize);

        // uncomment this if display if refresh rate is like 5hz
        // (idk on my laptop the refresh rate is extremely poor, but
        // spamming \n fixes it) putc('\n', stdout);

        nanosleep(&ts, NULL);
    }

    if ((res = pthread_join(pt, NULL)) != 0) {
        printf("Failed to join thread: %d", res);
    }
    free(buf);
    mesh_destroy(&mesh);
    unmap_close(fbp, fb_data.screensize, fb_data.fd);
    return 0;
}

double theta = 0.1;
void animate_cube() {
    rotate_x(&mesh, &mesh_out, theta * 3);
    rotate_y(&mesh, &mesh_out, theta);
    rotate_z(&mesh_out, &mesh_out, sin(theta * 1.5));

    // move cube further away so you can see it
    translate_z(&mesh_out, &mesh_out, 1);
    draw_mesh3d(&mesh_out, 0x00FF00FF);
    theta += 0.01;
}

double theta2 = 0.01;
void animate_torus() {
    // this creates a really cool effect and i lowk dont know why
    rotate_y(&mesh, &mesh_out, -theta);
    rotate_x(&mesh, &mesh_out, theta);
    rotate_x(&mesh_out, &mesh_out, sin(theta2 * 2) * 0.5 + M_PI_2);

    // move torus further away so you can see it
    translate_z(&mesh_out, &mesh_out, 1);
    draw_mesh3d(&mesh_out, 0x00FF00FF);
    theta += 0.01;
    theta2 += 0.01;
}

// points are arranged a way that lets us skip drawing the diagonal line
// easily
void cube_init(mesh_t *mesh) {
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

void torus_init(mesh_t *mesh, const double R, const double r) {
    mesh_init(mesh);

    const int major_segments = 3;
    const int minor_segments = 4;

    for (int i = 0; i < major_segments; i++) {
        double u0 = (2.0 * M_PI * i) / major_segments;
        double u1 = (2.0 * M_PI * (i + 1)) / major_segments;

        for (int j = 0; j < minor_segments; j++) {
            double v0 = (2.0 * M_PI * j) / minor_segments;
            double v1 = (2.0 * M_PI * (j + 1)) / minor_segments;

            // Quad corners
            double a_x = (R + r * cos(v0)) * cos(u0);
            double a_y = r * sin(v0);
            double a_z = (R + r * cos(v0)) * sin(u0);

            double b_x = (R + r * cos(v0)) * cos(u1);
            double b_y = r * sin(v0);
            double b_z = (R + r * cos(v0)) * sin(u1);

            double c_x = (R + r * cos(v1)) * cos(u1);
            double c_y = r * sin(v1);
            double c_z = (R + r * cos(v1)) * sin(u1);

            double d_x = (R + r * cos(v1)) * cos(u0);
            double d_y = r * sin(v1);
            double d_z = (R + r * cos(v1)) * sin(u0);

            // Triangle 1: A -> B -> C
            mesh_push(mesh,
                      (dbl_tri_t){a_x, a_y, a_z, b_x, b_y, b_z, c_x, c_y, c_z});

            // Triangle 2: A -> D -> C
            mesh_push(mesh,
                      (dbl_tri_t){a_x, a_y, a_z, d_x, d_y, d_z, c_x, c_y, c_z});
        }
    }
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

void print_shapes(void) {
    puts("Available shapes:");
    for (int i = 0; *shapes[i] != '\0'; i++) {
        printf("\t%s\n", shapes[i]);
    }
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

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/b_lib.h"
#include "include/mesh.h"

#define MAX_RES 100
#define MIN_RES 1

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)

extern char *buf;
extern fb_data_t fb_data;

void extract_colour(uint32_t col, colour_t *out) {
    out->blue = (col & 0xFF000000) >> 24;
    out->green = (col & 0xFF0000) >> 16;
    out->red = (col & 0xFF00) >> 8;
    out->alpha = (col & 0xFF);
}

void p_print(char *narr, point_t p) {
    printf("%s: (%d, %d)\n", narr, p.x, p.y);
}

void p_subtract(point_t a, point_t b, point_t *out) {
    out->x = a.x - b.x;
    out->y = a.y - b.y;
}

void p_add(point_t a, point_t b, point_t *out) {
    out->x = a.x + b.x;
    out->y = a.y + b.y;
}

int put_pixel(int x, int y, colour_t col) {
    if (x <= 0 || y <= 0 || x >= fb_data.width || y >= fb_data.height) {
        return 1;
    }
    unsigned int loc = (y * fb_data.line_length) + (x * (fb_data.bpp >> 3));
    buf[loc] = col.blue;
    buf[loc + 1] = col.green;
    buf[loc + 2] = col.red;
    buf[loc + 3] = col.alpha;
    return 0;
}

int draw_line_low(point_t a, point_t b, uint32_t col) {
    colour_t col_info;
    extract_colour(col, &col_info);

    point_t dp;
    p_subtract(b, a, &dp);
    int yi = 1;
    if (dp.y < 0) {
        yi = -1;
        dp.y = -dp.y;
    }
    int D = 2 * dp.y - dp.x;
    int y = a.y;
    for (int x = a.x; x < b.x; x++) {
        put_pixel(x, y, col_info);

        if (D > 0) {
            y += yi;
            D += (2 * (dp.y - dp.x));
        } else {
            D += 2 * dp.y;
        }
    }
    return 0;
}

int draw_line_high(point_t a, point_t b, uint32_t col) {
    colour_t col_info;
    extract_colour(col, &col_info);

    point_t dp;
    p_subtract(b, a, &dp);
    int xi = 1;
    if (dp.x < 0) {
        xi = -1;
        dp.x = -dp.x;
    }
    int D = 2 * dp.x - dp.y;
    int x = a.x;
    for (int y = a.y; y < b.y; y++) {
        put_pixel(x, y, col_info);

        if (D > 0) {
            x += xi;
            D += (2 * (dp.x - dp.y));
        } else {
            D += 2 * dp.x;
        }
    }
    return 0;
}

int draw_line(point_t a, point_t b, uint32_t col) {
    if (abs(b.y - a.y) <= abs(b.x - a.x)) {
        if (a.x > b.x)
            draw_line_low(b, a, col);
        else
            draw_line_low(a, b, col);
    } else {
        if (a.y > b.y)
            draw_line_high(b, a, col);
        else
            draw_line_high(a, b, col);
    }
    return 0;
}

int draw_circle(point_t centre, int radius, uint32_t col) {
    colour_t col_info;
    extract_colour(col, &col_info);

    float res = M_PI * 2;
    double step = 1.0 / res;
    for (float t = 0; t < res; t += step) {
        int x = sin(t) * radius + centre.x;
        int y = cos(t) * radius + centre.y;
        put_pixel(x, y, col_info);
    }
    return 0;
}

#define Z_NEAR 0.1f
int to_screen_space(point3_t p, point_t *out) {
    if (p.z < Z_NEAR && p.z > -Z_NEAR)
        return 1;

    double x = p.x / p.z;
    double y = p.y / p.z;

    // Normalised device coordinates:
    // x: -1..1 -> 0..width
    // y: -1..1 -> height..0
    out->x = (int)((x + 1.0) * 0.5 * fb_data.width);
    out->y = (int)((1.0 - (y + 1.0) * 0.5) * fb_data.height);

    return 0;
}

int draw_mesh3d(mesh_t *mesh, uint32_t col) {
    int num_tris = mesh->size;
    point_t p1, p2, p3;

    for (int i = 0; i < num_tris; i++) {
        to_screen_space(mesh->data[i].p[0], &p1);
        to_screen_space(mesh->data[i].p[1], &p2);
        to_screen_space(mesh->data[i].p[2], &p3);

        draw_line(p1, p2, col);
        draw_line(p2, p3, col);

        // with cube_init_alt, this last one draws the diagonal
        // draw_line(p3, p1, col); 
    }

    return 0;
}

void translate_x(mesh_t *mesh, mesh_t *out, double dx) {
    for (int i = 0; i < mesh->size; i++) {

        out->data[i].p[0].x = mesh->data[i].p[0].x + dx;
        out->data[i].p[1].x = mesh->data[i].p[1].x + dx;
        out->data[i].p[2].x = mesh->data[i].p[2].x + dx;
    }
}

void translate_y(mesh_t *mesh, mesh_t *out, double dy) {
    for (int i = 0; i < mesh->size; i++) {

        out->data[i].p[0].y = mesh->data[i].p[0].y + dy;
        out->data[i].p[1].y = mesh->data[i].p[1].y + dy;
        out->data[i].p[2].y = mesh->data[i].p[2].y + dy;
    }
}

void translate_z(mesh_t *mesh, mesh_t *out, double dz) {
    for (int i = 0; i < mesh->size; i++) {

        out->data[i].p[0].z = mesh->data[i].p[0].z + dz;
        out->data[i].p[1].z = mesh->data[i].p[1].z + dz;
        out->data[i].p[2].z = mesh->data[i].p[2].z + dz;
    }
}

int rotate_x(mesh_t *mesh, mesh_t *out, double theta) {
    double c = cos(theta);
    double s = sin(theta);

    for (int i = 0; i < mesh->size; i++) {
        for (int j = 0; j < 3; j++) {
            double x = mesh->data[i].p[j].x;
            double y = mesh->data[i].p[j].y;
            double z = mesh->data[i].p[j].z;

            double cy = y * c - z * s;
            double cz = y * s + z * c;

            out->data[i].p[j].x = x;
            out->data[i].p[j].y = cy;
            out->data[i].p[j].z = cz;
        }
    }

    return 0;
}

int rotate_y(mesh_t *mesh, mesh_t *out, double theta) {
    double c = cos(theta);
    double s = sin(theta);

    for (int i = 0; i < mesh->size; i++) {
        for (int j = 0; j < 3; j++) {
            double x = mesh->data[i].p[j].x;
            double y = mesh->data[i].p[j].y;
            double z = mesh->data[i].p[j].z;

            double cx = x * c + z * s;
            double cz = z * c - x * s;

            out->data[i].p[j].x = cx;
            out->data[i].p[j].y = y;
            out->data[i].p[j].z = cz;
        }
    }

    return 0;
}

int rotate_z(mesh_t *mesh, mesh_t *out, double theta) {
    double c = cos(theta);
    double s = sin(theta);

    for (int i = 0; i < mesh->size; i++) {
        for (int j = 0; j < 3; j++) {
            double x = mesh->data[i].p[j].x;
            double y = mesh->data[i].p[j].y;
            double z = mesh->data[i].p[j].z;

            double cx = x * c - y * s;
            double cy = x * s + y * c;

            out->data[i].p[j].x = cx;
            out->data[i].p[j].y = cy;
            out->data[i].p[j].z = z;
        }
    }

    return 0;
}

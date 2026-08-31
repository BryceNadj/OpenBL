#ifndef G_LIB_C
#define G_LIB_C

#include <stdint.h>
#include "OpenBL.h"
#include "mesh.h"

typedef struct {
    int x;
    int y;
} point_t;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
} colour_t;

int draw_line(point_t a, point_t b, uint32_t col);
int draw_circle(point_t centre, int radius, uint32_t col);
int draw_mesh3d(mesh_t *mesh, uint32_t col);
void draw_triangle(dbl_tri_t tri, uint32_t col);

int rotate_x(mesh_t *mesh, mesh_t *out, double theta);
int rotate_y(mesh_t *mesh, mesh_t *out, double theta);
int rotate_z(mesh_t *mesh, mesh_t *out, double theta);

void translate_x(mesh_t *mesh, mesh_t *out, double dx);
void translate_y(mesh_t *mesh, mesh_t *out, double dy);
void translate_z(mesh_t *mesh, mesh_t *out, double dz);

void p_add(point_t a, point_t b, point_t *out);
void p_subtract(point_t a, point_t b, point_t *out);
void p_print(char *narr, point_t p);

#endif

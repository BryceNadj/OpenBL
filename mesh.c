/*
 * code shamelessly stolen from a uni assignment >:)
 * most of these arent even used but oh well
 */
#include "include/mesh.h"
#include <stdio.h>
#include <stdlib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void mesh_init(mesh_t *mesh) {
    // size_t new_mem_size = MESH_INITIAL_CAPACITY * sizeof(dbl_tri_t);
    mesh->capacity = MESH_INITIAL_CAPACITY;
    mesh->size = 0;
    // mesh->data = malloc(new_mem_size);
    mesh->data = calloc(MESH_INITIAL_CAPACITY, sizeof(dbl_tri_t));
    if (mesh->data == NULL) {
        perror("calloc");
    }
    mesh->init = 1;
}

void mesh_ensure_capacity(mesh_t *mesh, size_t new_size) {
    if (mesh->init != 1)
        return;

    if (new_size > mesh->capacity) {
        int new_capacity = MAX(mesh->capacity * MESH_GROWTH_FACTOR, new_size);
        size_t new_mem_size = new_capacity * sizeof(dbl_tri_t);

        mesh->capacity = new_capacity;
        mesh->data = realloc(mesh->data, new_mem_size);
    }
}

void mesh_destroy(mesh_t *mesh) {
    mesh->capacity = 0;
    mesh->size = 0;
    free(mesh->data);
    mesh->data = NULL;
}

void mesh_copy(mesh_t *src, mesh_t *dest) {
    if (src == dest || src->init != 1 || dest->init != 1)
        return;

    dest->size = src->size;

    mesh_ensure_capacity(dest, src->size);

    for (int i = 0; i < src->size; i++)
        dest->data[i] = src->data[i];
}

void mesh_clear(mesh_t *mesh) {
    if (mesh->init != 1)
        return;

    mesh->size = 0;
}

void mesh_push(mesh_t *mesh, dbl_tri_t new_item) {
    if (mesh->init != 1)
        return;

    mesh_ensure_capacity(mesh, mesh->size + 1);
    mesh->data[mesh->size] = new_item;
    mesh->size++;
}

void mesh_pop(mesh_t *mesh) {
    if (mesh->init != 1)
        return;

    mesh->size = mesh->size > 0 ? mesh->size - 1 : 0;
}

dbl_tri_t *mesh_last(mesh_t *mesh) {
    dbl_tri_t *result;

    result = mesh->size > 0 ? &(mesh->data[mesh->size - 1]) : NULL;
    return result;
}

void mesh_insert_at(mesh_t *mesh, size_t pos, dbl_tri_t new_item) {
    if (mesh->init != 1)
        return;
    mesh_ensure_capacity(mesh, mesh->size + 1);
    pos = MIN(pos, mesh->size);

    for (int i = mesh->size; i > pos; i--) {
        mesh->data[i] = mesh->data[i - 1];
    }

    mesh->data[pos] = new_item;
    mesh->size++;
}

void mesh_remove_at(mesh_t *mesh, size_t pos) {
    if (mesh->init != 1)
        return;

    pos = MIN(pos, mesh->size);
    for (int i = pos; i < mesh->size; i++) {
        mesh->data[i] = mesh->data[i + 1];
    }

    mesh->size = pos >= mesh->size ? mesh->size : mesh->size - 1;
}

void mesh_foreach(mesh_t *mesh, void (*callback)(dbl_tri_t *, void *),
                void *info) {
    if (mesh->init != 1)
        return;
    for (int i = 0; i < mesh->size; i++) {
        callback(&(mesh->data[i]), info);
    }
}

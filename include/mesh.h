/*
 * code shamelessly stolen from a uni assignment >:)
 * most of these arent even used but oh well
 */
#ifndef MESH_H
#define MESH_H
#include <math.h>
#include <stddef.h>

typedef struct point3 {
    double x;
    double y;
    double z;
} point3_t;

typedef struct dbl_tri {
    point3_t p[3];
} dbl_tri_t;

typedef struct mesh {
    // Amount of tris currently stored
    int size;

    // Total number of tris
    int capacity;

    // has data been initialised?
    char init;

    dbl_tri_t *data;
} mesh_t;

#define MESH_INITIAL_CAPACITY 2
#define MESH_GROWTH_FACTOR 1.25

/**
 * Prepares a vector for first-time use. This should be called before anything
 * else is done with a vector. It should not be called more than once.
 *
 * \param vec The address of a tri_vec_t to initialise.
 */
void mesh_init(mesh_t *vec);

/**
 * Ensures that the storage capacity of a vector is large enough to hold the
 * designated number of items, resizing the encapsulated array if necessary, but
 * keeping the size unchanged.
 *
 * \param vec A triangle to process.
 * \param new_size The required minimum capacity for the triangle.
 */
void mesh_ensure_capacity(mesh_t *vec, size_t new_size);

/**
 * Destroy a vector. After a call to this function, vec is undefined.
 *
 * \param vec The address of a tri_vec_t to be destroyed.
 */
void mesh_destroy(mesh_t *vec);

/**
 * Duplicate the contents of a vector (the source) into another (the
 * destination).
 *
 * \param vec The address of a tri_vec_t from which data is to be copied.
 * \param dest The address of a tri_vec_t into which data is to be copied.
 */
void mesh_copy(mesh_t *src, mesh_t *dest);

/**
 * Remove all elements from the vector, but keep storage and capacity as-is to
 * enable efficient regrowth if necessary.
 *
 * \param  The address of a tri_vec_t to be cleared.
 */
void mesh_clear(mesh_t *vec);

/**
 * Append a new item to the end of the vector.
 *
 * \param vec The address of a tri_vec_t to which a new item is to be appended.
 * \param new_item A double value to be appended to the triangle.
 */
void mesh_push(mesh_t *vec, dbl_tri_t new_item);

/**
 * Remove the last item (if any) from the end of the vector.
 *
 * \param vec The address of a tri_vec_t from which the last item is to be
 * removed.
 */
void mesh_pop(mesh_t *vec);

/**
 * Get the last item from the end of the vector, or NaN if the vector is empty.
 *
 * \param vec The address of a tri_vec_t from which the last item is to be
 * obtained.
 */
dbl_tri_t *mesh_last(mesh_t *vec);

/**
 * Insert an item at a designated position in the vector. If the position
 * equals or exceeds the size of the vector, the new item is appended.
 *
 * \param vec The address of a tri_vec_t from which the last item is to be
 * obtained.
 * \param pos The position at which the new item is to be inserted.
 * \param new_item The value to insert.
 */
void mesh_insert_at(mesh_t *vec, size_t pos, dbl_tri_t new_item);

/**
 * Remove the item at a designated position in the vector. If the position
 * equals or exceeds the size of the vector, no change is made.
 *
 * \param vec The address of a tri_vec_t from which the last item is to be
 * obtained.
 * \param pos The position at which the item is to be removed.
 */
void mesh_remove_at(mesh_t *vec, size_t pos);

/**
 * Traverses a vector, invoking the supplied function with each element in turn.
 *
 * \param vec The address of a tri_vec_t which is to be processed.
 * \param callback A function which will be applied to each element of the
 * triangle.
 * \param info The address of a user-supplied data object which will be passed
 * to callback along with each value.
 */
void mesh_foreach(mesh_t *vec, void (*callback)(dbl_tri_t *, void *),
                  void *info);

#endif // MESH_H

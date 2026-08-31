#include "include/mesh.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define P_STR_SIZE 32
#define T_STR_SIZE P_STR_SIZE << 2

#define NEWLINE putc('\n', stdout)

#define TEST_MESH 0x01
#define TEST_MESH_STR "mesh"

#define TEST_POINT 0x02
#define TEST_POINT_STR "point"

#define TEST_DBL_TRI 0x04
#define TEST_DBL_TRI_STR "dbl_tri"

void print_help(char *);
int test_mesh();
int test_point();
int test_dbl_tri();

// print funcs
/// writes point information to paramchar *out
/// ensure [out] has space for 32 chars, if not, chaos may or may not ensue
/// returns number of chars written
int point_to_string(char *narr, point3_t p, char *out);

/// writes tri information to param char *out
/// returns number of chars written
int tri_to_string(char *narr, dbl_tri_t t, char *out);

/// prints each triangle in a mesh line by line
void print_mesh(char *narr, mesh_t mesh);

int assert_point_equals(point3_t *p1, point3_t *p2);
int assert_point_not_equals(point3_t *p1, point3_t *p2);

int assert_tri_equals(dbl_tri_t *t1, dbl_tri_t *t2);
int assert_tri_not_equals(dbl_tri_t *t1, dbl_tri_t *t2);

const char *tests[] = {TEST_MESH_STR, TEST_POINT_STR, TEST_DBL_TRI_STR, "\0"};

int main(int argc, char *argv[]) {
    int opt;
    char test_flags, *arg;
    test_flags = 0;

    if (argc == 1)
        test_flags = 0xFF;

    else
        while ((opt = getopt(argc, argv, "hs:")) != -1) {
            switch (opt) {
            case 's':
                arg = optarg;
                if (strcmp(arg, TEST_MESH_STR) == 0) {
                    test_flags |= TEST_MESH;
                } else if (strcmp(arg, TEST_POINT_STR) == 0) {
                    test_flags |= TEST_POINT;
                } else if (strcmp(arg, TEST_DBL_TRI_STR) == 0) {
                    test_flags |= TEST_DBL_TRI;
                }
                break;

            case 'h':
                print_help(argv[0]);
                break;

            default:
                print_help(argv[0]);
            }
        }

    if (test_flags & TEST_POINT) {
        printf("Testing %s\n", TEST_POINT_STR);
        test_point();
        NEWLINE;
    }
    if (test_flags & TEST_DBL_TRI) {
        printf("Testing %s\n", TEST_DBL_TRI_STR);
        test_dbl_tri();
        NEWLINE;
    }
    if (test_flags & TEST_MESH) {
        printf("Testing %s\n", TEST_MESH_STR);
        test_mesh();
        NEWLINE;
    }
}

int test_point() {
    point3_t p1 = {1, 2, 3};
    point3_t p2 = {1, 2, 3};
    point3_t p3 = {2, 2, 3};
    point3_t p4 = {3, 2, 3};

    char p1_str[P_STR_SIZE];
    char p2_str[P_STR_SIZE];

    puts("Printing point p1");
    point_to_string("p1", p1, p1_str);
    printf("%s\n\n", p1_str);

    puts("Printing point p2 without narration");
    point_to_string(NULL, p2, p2_str);
    printf("%s\n\n", p2_str);

    printf("p1 == p2: %d\n", assert_point_equals(&p1, &p2));
    printf("p2 == p3: %d\n", assert_point_equals(&p2, &p3));
    printf("p1 != p2: %d\n", assert_point_not_equals(&p1, &p2));
    printf("p2 != p3: %d\n", assert_point_not_equals(&p2, &p3));
    printf("p3 == p1: %d\n", assert_point_equals(&p3, &p1));

    return TRUE;
}

int test_dbl_tri() {

    dbl_tri_t t1 = {1, 1, 1, 2, 2, 2, 3, 3, 3}; // matches t4
    dbl_tri_t t2 = {2, 1, 1, 2, 1, 2, 3, 3, 3}; // one digit difference from t1
    dbl_tri_t t3 = {5, 2, 3, 3, 2, 1, 1, 2, 3}; // matches t5
    dbl_tri_t t4 = {1, 1, 1, 2, 2, 2, 3, 3, 3}; // matches t1
    dbl_tri_t t5 = {5, 2, 3, 3, 2, 1, 1, 2, 3}; // matches t3

    puts("printing tri: t1");
    char str_tri1[T_STR_SIZE];
    tri_to_string("t1", t1, str_tri1);
    printf("%s\n", str_tri1);

    puts("printing tri without narration: t2");
    char str_tri2[T_STR_SIZE];
    tri_to_string(NULL, t2, str_tri2);
    printf("%s\n", str_tri2);

    printf("t1 == t1: %d\n", assert_tri_equals(&t1, &t1));
    printf("t1 == t2: %d\n", assert_tri_equals(&t1, &t2));
    printf("t1 == t4: %d\n", assert_tri_equals(&t1, &t4));
    printf("t2 == t5: %d\n", assert_tri_equals(&t2, &t5));
    printf("t5 == t3: %d\n", assert_tri_equals(&t5, &t3));

    printf("t1 != t1: %d\n", assert_tri_not_equals(&t1, &t1));
    printf("t1 != t2: %d\n", assert_tri_not_equals(&t1, &t2));
    printf("t1 != t4: %d\n", assert_tri_not_equals(&t1, &t4));
    printf("t2 != t5: %d\n", assert_tri_not_equals(&t2, &t5));
    printf("t5 != t3: %d\n", assert_tri_not_equals(&t5, &t3));

    return TRUE;
}

void mesh_cb(dbl_tri_t *t, void *info) {
    int *co = info;
    t->p[0].x *= *co;
    t->p[0].y *= *co;
    t->p[0].z *= *co;

    t->p[1].x *= *co;
    t->p[1].y *= *co;
    t->p[1].z *= *co;

    t->p[2].x *= *co;
    t->p[2].y *= *co;
    t->p[2].z *= *co;
}
int test_mesh() {
    mesh_t mesh1 = {0};
    mesh_t mesh2 = {0};
    mesh_t mesh3 = {0};

    mesh_init(&mesh1);
    mesh_init(&mesh2);
    print_mesh("PRINTING MESH3", mesh3); // mesh empty: should safely abort

    dbl_tri_t t1 = {1, 1, 1, 2, 2, 2, 3, 3, 3};
    dbl_tri_t t2 = {2, 1, 1, 2, 1, 2, 3, 3, 3};
    dbl_tri_t t3 = {3, 2, 3, 3, 2, 1, 1, 2, 3};
    dbl_tri_t t4 = {4, 1, 1, 2, 2, 2, 3, 3, 3};
    dbl_tri_t t5 = {5, 2, 3, 3, 2, 1, 1, 2, 3};

    // ******** PUSH ******** //
    puts("Populating mesh");
    mesh_push(&mesh1, t1);
    print_mesh("After pushing t1", mesh1);

    mesh_push(&mesh1, t2);
    print_mesh("After pushing t2", mesh1);

    mesh_push(&mesh1, t3);
    print_mesh("After pushing t3", mesh1);

    mesh_push(&mesh1, t4);
    print_mesh("After pushing t4", mesh1);

    mesh_push(&mesh1, t5);
    print_mesh("After pushing t5", mesh1);
    NEWLINE;

    // ******** COPY ******** //
    puts("Copying mesh1 to mesh2");
    mesh_copy(&mesh1, &mesh2);
    print_mesh("Copied mesh1 to mesh2. mesh2 contents", mesh2);

    puts("Attempting to copy mesh1 to mesh3");
    mesh_copy(&mesh1, &mesh3);
    print_mesh("Copied mesh1 to mesh3. mesh3 contents", mesh3);
    NEWLINE;

    // ******** DESTROY ******** //
    puts("Destroying mesh2");
    mesh_destroy(&mesh2);
    print_mesh("mesh2 after destruction", mesh2);
    NEWLINE;

    // ******** POP ******** //
    puts("Popping mesh1");
    mesh_pop(&mesh1);
    print_mesh("Popped mesh1", mesh1);
    NEWLINE;

    // ******** LAST ******** //
    puts("Getting last element of mesh1");
    dbl_tri_t *last;
    last = mesh_last(&mesh1);
    char mesh_last[T_STR_SIZE];
    tri_to_string("Last element of mesh1", *last, mesh_last);
    printf("%s\n", mesh_last);
    NEWLINE;

    // ******** INSERT AT ******** //
    puts("Inserting into mesh1");
    mesh_insert_at(&mesh1, 1, *last);
    print_mesh("Insert element at pos 1", mesh1);
    NEWLINE;

    // ******** REMOVE AT ******** //
    puts("Removing from mesh1");
    mesh_remove_at(&mesh1, 2);
    print_mesh("Remove element at pos 2", mesh1);
    NEWLINE;

    // ******** FOR EACH ******** //
    puts("Foreach on mesh1 and mesh3");
    mesh_init(&mesh3);
    mesh_copy(&mesh1, &mesh3);
    int co1 = 2;
    int co2 = 3;
    print_mesh("Before foreach", mesh1);
    mesh_foreach(&mesh1, mesh_cb, &co1);
    mesh_foreach(&mesh3, mesh_cb, &co2);
    print_mesh("mesh1 after foreach", mesh1);
    print_mesh("mesh3 after foreach", mesh3);

    return TRUE;
}

int point_to_string(char *narr, point3_t p, char *out) {
    int bufsize = P_STR_SIZE;
    char p_buf[bufsize];

    // if narration exists, write it
    int bytes_written = 0;
    if (narr != NULL)
        if ((bytes_written = snprintf(out, bufsize, "%s: ", narr)) == -1)
            perror("point_to_string: narr");

    // offset [out] by number of chars written so we can use snprintf again
    if (snprintf(out + bytes_written, bufsize - bytes_written,
                 "(%.2f, %.2f, %.2f)", p.x, p.y, p.z) == -1)
        perror("point_to_string: narr");
    return strlen(out);
}

int tri_to_string(char *narr, dbl_tri_t t, char *out) {
    int bufsize = P_STR_SIZE;
    int t_bufsize = T_STR_SIZE;

    char pa[bufsize];
    char pb[bufsize];
    char pc[bufsize];

    point_to_string(NULL, t.p[0], pa);
    point_to_string(NULL, t.p[1], pb);
    point_to_string(NULL, t.p[2], pc);

    int bytes_written = 0;
    // if narration exists, write it
    if (narr != NULL)
        if ((bytes_written = snprintf(out, bufsize, "%s: ", narr)) == -1)
            perror("tri_to_string: narr");

    // offset [out] by number of chars written so we can use snprintf again
    if (snprintf(out + bytes_written, t_bufsize - bytes_written, "(%s, %s, %s)",
                 pa, pb, pc) == -1)
        perror("point_to_string: narr");

    return strlen(out);
}

void print_mesh(char *narr, mesh_t mesh) {
    printf("%s\n", narr);
    if (mesh.data == NULL) {
        puts("mesh empty, aborting...");
        return;
    }

    printf("Num elements: %d\n", mesh.size);
    int bufsize = 256;
    char tri_str[bufsize];
    for (int i = 0; i < mesh.size; i++) {
        tri_to_string(NULL, mesh.data[i], tri_str);
        printf("%s\n", tri_str);
    }
}

int assert_point_equals(point3_t *p1, point3_t *p2) {
    if (p1->x == p2->x && p1->y == p2->y && p1->z == p2->z)
        return TRUE;
    return FALSE;
}
int assert_point_not_equals(point3_t *p1, point3_t *p2) {
    if (assert_point_equals(p1, p2) == TRUE)
        return FALSE;
    return TRUE;
}

int assert_tri_equals(dbl_tri_t *t1, dbl_tri_t *t2) {
    if (assert_point_equals(&(t1->p[0]), &(t2->p[0])) == TRUE &&
        assert_point_equals(&(t1->p[1]), &(t2->p[1])) == TRUE &&
        assert_point_equals(&(t1->p[2]), &(t2->p[2])) == TRUE)
        return TRUE;
    return FALSE;
}
int assert_tri_not_equals(dbl_tri_t *t1, dbl_tri_t *t2) {
    if (assert_tri_equals(t1, t2) == TRUE)
        return FALSE;
    return TRUE;
}
void print_help(char *name) {
    printf("\nUsage: %s [-s]\n", name);
    printf("[no opt]: Run all tests\n");
    NEWLINE;

    puts("-s: Specify which test to run, can be used successively");
    puts("\tAvailable tests: ");
    int i = 0;
    const char *str = tests[i];
    while (str[0] != '\0') {
        printf("\t%s\n", str);
        i++;
        str = tests[i];
    }
    NEWLINE;

    printf("-h: print this help text\n");
    return;
}

// test.c - minimal C parser test

#include <stdio.h>
#include <stdlib.h>

#define MAX_ITEMS 10
#define SQUARE(x) ((x) * (x))

typedef enum {
    RED,
    GREEN,
    BLUE
} Color;

typedef struct {
    int id;
    char name[32];
    Color color;
    int values[MAX_ITEMS];
} Item;

/* Forward declaration */
void print_item(const Item *it);

static inline int add(int a, int b) {
    return a + b;
}

Item *create_item(int id, const char *name, Color c) {
    Item *it = malloc(sizeof(Item));
    if (!it) return NULL;
    it->id = id;
    snprintf(it->name, sizeof(it->name), "%s", name);
    it->color = c;
    for (int i = 0; i < MAX_ITEMS; ++i) it->values[i] = i * i;
    return it;
}

void print_item(const Item *it) {
    if (!it) {
        puts("NULL item");
        return;
    }
    printf("Item %d: %s (color=%d)\n", it->id, it->name, (int)it->color);
    printf("Values:");
    for (int i = 0; i < MAX_ITEMS; ++i) printf(" %d", it->values[i]);
    putchar('\n');
}

int main(void) {
    Item *a = create_item(1, "Alpha", GREEN);
    Item *b = create_item(2, "Beta", BLUE);

    if (!a || !b) {
        fprintf(stderr, "alloc failed\n");
        free(a);
        free(b);
        return 1;
    }

    print_item(a);
    print_item(b);

    int sum = add(a->values[2], b->values[3]);
    printf("Sum: %d, Square: %d\n", sum, SQUARE(sum));

    // pointer arithmetic & loop with break/continue
    int *p = a->values;
    for (int i = 0; i < MAX_ITEMS; ++i) {
        if (p[i] == 4) continue;
        if (p[i] > 20) break;
        printf("a->values[%d]=%d\n", i, p[i]);
    }

    free(a);
    free(b);
    return 0;
}

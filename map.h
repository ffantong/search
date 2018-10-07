#ifndef HASH
#define HASH

typedef struct map_element {
    short key;
    void * entity;
    struct map_element * next;
} map_element;

void map_init();

void map_put(short, void *);

void * map_get(short);

void map_for_each(void(*)(short ,void *));

void map_destroy();
#endif

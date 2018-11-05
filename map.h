#ifndef HASH
#define HASH

struct map_element {
    uint16_t * key;
    void * entity;
    struct map_element * next;
};

void map_init(uint8_t);

void map_put(uint16_t *, void *);

void * map_get(uint16_t *);

void map_for_each(void(*)(uint16_t *,void *));

void map_destroy();
#endif

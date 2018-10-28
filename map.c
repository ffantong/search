#include "defines.h"

static float load_factor = 0.75;

static unsigned long capacity = 8192;

static map_element ** elements;

static uint32_t size;

static uint8_t key_len;

static uint32_t map_hash(uint16_t *key) {
    unsigned int h = 0;
    for(int i = 0; i < key_len; i++) {
        h = 31 * h + key[i];
    }
    h ^= h >> 16;
    return h & (capacity - 1);
}

static void _destroy(map_element ** elements, uint32_t capacity) {
    map_element *p, *q;
    for(uint32_t i = 0; i < capacity; i++) {
        p = elements[i];
        while(p != NULL) {
            q = p->next;
            if(p->key != NULL) {
                free(p->key);
            }
            free(p);
            p = q;
        }
    }
    free(elements);
}

static void extend(){
    uint32_t last_capacity = capacity;
    capacity = capacity << 1;
    map_element ** last_elements = elements;
    size = 0;
    map_init(key_len);
    map_element *p;
    for(uint32_t i = 0; i < last_capacity; i++) {
        p = last_elements[i];
        while(p != NULL) {
            map_put(p->key, p->entity);
            p = p->next;
        }
    }
    _destroy(last_elements, last_capacity);
}

void map_init(uint8_t len) {
    key_len = len;
    elements = malloc(sizeof(map_element) * capacity);
    if (elements == NULL) {
        printf("memory error.");
        exit(1);
    }
    for(int i = 0; i < capacity; i++) {
        elements[i] = NULL;
    }
}

void map_put(uint16_t *k, void * entity) {
    int len = sizeof(uint16_t) * key_len;
    uint16_t * key = malloc(len);
    MEM_CHECK(key);
    memcpy(key, k, len);
    int index = map_hash(key);
    map_element * element = malloc(sizeof(map_element));
    MEM_CHECK(element);
    element->key = key;
    element->entity = entity;
    element->next = NULL;
    map_element * p = elements[index];
    if (p == NULL) {
        elements[index] = element;
    } else {
        if(memcmp(p->key, key, len) == 0) {
            p->entity = entity;
            free(element);
            return;
        }
        while(p->next != NULL) {
            if(memcmp(p->key, key, len) == 0) {
                p->entity = entity;
                free(element);
                return;
            }
            p = p->next;
        }
        if(memcmp(p->key, key, len) == 0) {
            p->entity = entity;
            free(element);
            return;
        }
        p->next = element;
    }
    size++;
    if(size > capacity * load_factor) {
        extend();
    }
}

void map_for_each(void(*p_func)(uint16_t *,void *)) {
    map_element * p;
    for(int i = 0; i < capacity; i++) {
        p = elements[i];
        while(p != NULL) {
            p_func(p->key, p->entity);
            p = p->next;
        }
    }
}

void * map_get(uint16_t *key) {
    map_element *element = elements[map_hash(key)];
    while(element != NULL) {
        if (memcmp(element->key, key, key_len * sizeof(uint16_t)) == 0) {
            return element->entity;
        }
        element = element->next;
    }
    return NULL;
}

void map_destroy() {
    _destroy(elements, capacity);
}

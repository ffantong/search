#include "defines.h"

float load_factor = 0.75;

int capacity;

map_element ** elements;

int size;

static int map_hash(short key) {
    return key & capacity;
}

static void _destroy(map_element ** elements, int capacity) {
    map_element *p, *q;
    for(int i = 0; i < capacity; i++) {
        p = elements[i];
        while(p != NULL) {
            q = p->next;
            free(p);
            p = q;
        }
    }
    free(elements);
}

static void extend(){
    int last_capacity = capacity;
    capacity = (capacity << 1) + 1;
    map_element ** last_elements = elements;
    size = 0;
    map_init();
    map_element *p;
    for(int i = 0; i < last_capacity; i++) {
        p = last_elements[i];
        while(p != NULL) {
            map_put(p->key, p->entity);
            p = p->next;
        }
    }
    destroy(last_elements, last_capacity);
}

void map_init() {
    capacity = pow(2, 13) - 1;
    elements = malloc(sizeof(map_element) * (capacity + 1));
    if (elements == NULL) {
        printf("memory error.");
        exit(1);
    }
    for(int i = 0; i < capacity + 1; i++) {
        elements[i] = NULL;
    }
}

void map_put(short key, void * entity) {
    int index = map_hash(key);
    if(size > capacity * load_factor) {
        extend();
    }
    map_element * element = malloc(sizeof(map_element));
    if (element == NULL) {
        printf("memory error.");
        exit(1);
    }
    element->key = key;
    element->entity = entity;
    element->next = NULL;
    map_element * p = elements[index];
    if (p == NULL) {
        elements[index] = element;
    } else {
        if(p->key == key) {
            p->entity = entity;
            free(element);
            return;
        }
        while(p->next != NULL) {
            if(p->key == key) {
                p->entity = entity;
                free(element);
                return;
            }
            p = p->next;
        }
        if(p->key == key) {
            p->entity = entity;
            free(element);
            return;
        }
        p->next = element;
    }
    size++;
}

void map_for_each(void(*p_func)(short ,void *)) {
    map_element * p;
    for(int i = 0; i < capacity; i++) {
        p = elements[i];
        while(p != NULL) {
            p_func(p->key, p->entity);
            p = p->next;
        }
    }
}

void * map_get(short key) {
    map_element *element = elements[map_hash(key)];
    while(element != NULL) {
        if (element->key == key) {
            return element->entity;
        }
        element = element->next;
    }
    return NULL;
}

void map_destroy() {
    _destroy(elements, capacity);
}

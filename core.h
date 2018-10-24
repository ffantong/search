#ifndef CORE
#define CORE

#define MAX_ITEM 30
#define MAX_LINE 60
#define NEXT_FILE "next"
#define FIRST_FILE "first"
#define FIRST_LEN 4

typedef struct first_element {
    uint32_t position;
    struct first_element * next;
} first_element;

typedef struct first_model {
    uint16_t *ch;
    uint32_t length;
    uint32_t * elements;
} first_model;

typedef struct match_element {
    uint32_t position;
    uint8_t index;
    uint16_t s[MAX_ITEM];
    bool last;
    struct match_element * next;
} match_element;

typedef struct model_config {
    char * model_dir;
    char cache_lavel;
    uint32_t total;
    char * first_file_path;
    char * next_file_path;
} model_config;

typedef struct match_result {
    uint32_t position;
    char * match_str;
    struct match_result * next;
} match_result;

void file_to_model(char *, char *);

void load_model();

match_result * cut(char *, bool);

match_element * next_match(match_element *, uint16_t);

uint32_t set_last(uint32_t, bool);

bool is_last(uint32_t);

void init(char *, uint8_t);

void destroy();
#endif // CORE

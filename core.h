#ifndef CORE
#define CORE

#define MAX_ITEM 64
#define MAX_LINE 128
#define NEXT_FILE "next"
#define FIRST_FILE "first"
#define MAGIC_STR "FSEARCH"

struct first_element {
    uint32_t position;
    struct first_element * next;
};

struct first_model {
    uint16_t *ch;
    uint32_t length;
    uint32_t * elements;
};

struct match_element {
    uint32_t position;
    uint8_t index;
    uint16_t s[MAX_ITEM];
    bool last;
    struct match_element * next;
};

struct model_config {
    char * model_dir;
    char cache_lavel;
    uint32_t total;
    char * first_file_path;
    char * next_file_path;
    uint8_t first_len;
    uint8_t max_item;
    uint8_t rest_len;
    char * file_input;
};

struct match_result {
    uint32_t position;
    char * match_str;
    struct match_result * next;
};

void file_to_model(struct model_config *);

void load_model();

struct match_result * cut(char *, bool);

struct match_element * next_match(struct match_element *, uint16_t);

uint32_t set_last(uint32_t, bool);

bool is_last(uint32_t);

void init(char *, uint8_t);

void destroy();
#endif // CORE

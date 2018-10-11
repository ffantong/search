#ifndef CORE
#define CORE

#define MAX_ITEM 30
#define MAX_LINE 60
#define NEXT_FILE "next"
#define FIRST_FILE "first"
#define FIRST_LEN 4

typedef struct first_element {
    int position;
    struct first_element * next;
} first_element;

typedef struct first_model {
    short *ch;
    int length;
    int * elements;
} first_model;

typedef struct match_element {
    int position;
    int index;
    short s[MAX_ITEM];
    bool last;
    struct match_element * next;
} match_element;

typedef struct model_config {
    char * model_dir;
    int cache_lavel;
    char * first_file_path;
    char * next_file_path;
} model_config;

typedef struct match_result {
    int position;
    char * match_str;
    struct match_result * next;
} match_result;

void file_to_model(char *, char *);

void load_model();

match_result * cut(char *, bool);

match_element * next_match(match_element *, short);

char * get_index_name(match_result *, int);

int set_last(int, bool);

bool is_last(int);

void init(char *, int);

void destroy();
#endif // CORE

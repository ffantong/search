#include "defines.h"

uint16_t ** cache;

struct model_config * config;

static void put_first(uint16_t *key, struct first_element * first) {
    struct first_element * f = map_get(key);
    if(f != NULL) {
        first->next = f;
    }
    map_put(key, first);
}

static void write_model(FILE * fd) {
    void func(uint16_t *key, void * first) {
        struct first_element *p, *q, *head;
        if(first != NULL) {
            head = (struct first_element*)first;
            uint32_t size = 1;
            p = head->next;
            head->next = NULL;
            while(p != NULL) {
                size++;
                q = p->next;
                p->next = head;
                head = p;
                p = q;
            }
            fwrite(key, sizeof(uint16_t), config->first_len, fd);
            fwrite(&size, sizeof(uint32_t), 1, fd);
            p = head;
            while(p!= NULL) {
                fwrite(&p->position, sizeof(uint32_t), 1, fd);
                q = p->next;
                free(p);
                p = q;
            }
        }
    }
    fwrite(MAGIC_STR, sizeof(char), strlen(MAGIC_STR), fd);
    fwrite(&config->first_len, sizeof(uint8_t), 1, fd);
    fwrite(&config->max_item, sizeof(uint8_t), 1, fd);
    map_for_each(func);
}

static char * get_file_name(char * dir, char * name) {
    char * file_name = malloc(strlen(dir) + strlen(name) + 2);
    memset(file_name, 0, strlen(dir) + strlen(name) + 2);
    strncpy(file_name, dir, strlen(dir));
    strcat(file_name, "/");
    strcat(file_name, name);
    return file_name;
}

void file_to_model(struct model_config * cnf) {
    config = cnf;
    FILE * fd = fopen(config->file_input, "rb");
    if(access(config->model_dir, F_OK) != 0) {
        #ifdef WINVER
            mkdir(config->model_dir);
        #elif __linux
            mkdir(config->model_dir, 0755);
        #endif // __linux
    }
    printf("open file: %s\n", config->file_input);
    char * first_file_path = get_file_name(config->model_dir, FIRST_FILE);
    FILE * ffirst = fopen(first_file_path, "wb");
    printf("open first: %s\n", first_file_path);
    free(first_file_path);
    FILE_CHECK(ffirst);
    FILE * next_files[config->rest_len];
    FILE * fnext;
    char * next_file_path = get_file_name(config->model_dir, NEXT_FILE);
    char * prefix = malloc(sizeof(char) * (strlen(next_file_path) + 5));
    memset(prefix, 0, sizeof(char) * (strlen(next_file_path) + 5));
    for(int i = 0; i < config->rest_len; i++) {
        sprintf(prefix, "%s_%02d", next_file_path, i + 1);
        fnext = fopen(prefix, "wb");
        FILE_CHECK(fnext);
        next_files[i] = fnext;
    }
    free(prefix);
    free(next_file_path);
    char buf[MAX_LINE];
    int len;
    uint16_t * s;
    uint16_t mask = 0;
    uint32_t position = 0;
    struct first_element * first;
    while (!feof(fd)) {
        memset(buf, 0, MAX_LINE);
        fgets(buf, MAX_LINE - 1, fd);
        len = strlen(buf);
        if(len < MAX_LINE) {
            buf[len - 1] = 0;
        }else {
            buf[MAX_LINE - 1] = 0;
        }
        s = char_to_utf(buf);
        len = utf_len(s);
        if(len < 1 || len >= config->max_item) {
            if(len != 0) {
                printf("%s   %d   %d\n", buf, position, len);
            }
            free(s);
            continue;
        }
        first = malloc(sizeof(struct first_element));
        MEM_CHECK(first);
        first->position = position++;
        first->next = NULL;
        put_first(s, first);
        for(int i = config->first_len; i < config->max_item; i++) {
            if(i < len){
                fwrite(&s[i], sizeof(uint16_t), 1, next_files[i - config->first_len]);
            }else {
                fwrite(&mask, sizeof(uint16_t), 1, next_files[i - config->first_len]);
            }
        }
        free(s);
        if (position % 30000 == 0) {
            printf(".");
        }
    }
    write_model(ffirst);
    fclose(fd);
    fclose(ffirst);
    for(int i = 0; i < config->rest_len; i++) {
        fclose(next_files[i]);
    }
}

struct match_element * next_match(struct match_element * matchs, uint16_t ch){
    struct match_element *p = matchs, *q = NULL, *result = NULL, *tail;
    FILE * next_files[config->rest_len];
    for(int i = 0; i < config->rest_len; i++) {
        next_files[i] = NULL;
    }
    int prefix_len = strlen(config->next_file_path) + 5;
    char * prefix = malloc(sizeof(char) * (prefix_len));
    uint64_t cur_pos[config->rest_len];
    memset(cur_pos, 0, sizeof(uint64_t) * config->rest_len);
    uint64_t step_len;
    uint16_t c;
    FILE * fd;
    while(p != NULL) {
        p->index++;
        if(p->index >= config->max_item) {
            c = 0;
        } if(p->position < 0 || p->position > config->total){
            c = ch + 1;
        }else if(p->index - 1 < config->cache_lavel) {
            c = cache[p->index - 1][p->position];
        }else {
            step_len = p->position * sizeof(short);
            fd = next_files[p->index - 1];
            if(fd == NULL) {
                memset(prefix, 0, sizeof(uint8_t) * prefix_len);
                sprintf(prefix, "%s_%02d", config->next_file_path, p->index);
                fd = fopen(prefix, "rb");
                next_files[p->index - 1] = fd;
            }
            fseek(fd, step_len - cur_pos[p->index - 1], SEEK_CUR);
            fread(&c, sizeof(uint16_t), 1, fd);
            cur_pos[p->index - 1] = step_len + sizeof(uint16_t);
        }
        if(c == ch || c == 0) {
            q = p->next;
            p->s[p->index + config->first_len - 1] = c;
            if(result == NULL) {
                result = p;
                result->next = NULL;
                tail = result;
            }else {
                tail->next = p;
                p->next = NULL;
                tail = p;
            }
            if(c == 0) {
                tail->last = true;
            }else{
                tail->last = false;
            }
            p = q;
        }else{
            q = p->next;
            free(p);
            p = q;
        }
    }
    for(int i = 0; i < config->rest_len; i++) {
        if(next_files[i] != NULL) {
            fclose(next_files[i]);
        }
    }
    free(prefix);
    return result;
}

struct match_result * cut(char * str, bool greedy) {
    uint16_t * s = char_to_utf(str), *cur;
    int len = utf_len(s);
    struct first_model * first;
    struct match_element * current, * before = NULL, *p , *q, *tail;
    struct match_result * result = NULL, * result_tail;
    char * match_str;
    for(uint32_t i = config->first_len; i < len + config->first_len; i++) {
        cur = s + i - config->first_len;
        if (i - config->first_len < len) {
            first = map_get(cur);
        }else {
            first = NULL;
        }
        current = NULL;
        if(first != NULL) {
            for(uint32_t j = 0; j < first->length; j++) {
                if(current == NULL) {
                    current = malloc(sizeof(struct match_element));
                    memset(current, 0, sizeof(struct match_element));
                    tail = current;
                }else {
                    p = malloc(sizeof(struct match_element));
                    memset(p, 0, sizeof(struct match_element));
                    tail->next = p;
                    tail = p;
                }
                tail->position = first->elements[j];
                tail->index = 0;
                memcpy(tail->s, cur, sizeof(uint16_t) * config->first_len);
                tail->last = false;
                tail->next = NULL;
            }
        }
        before = next_match(before, s[i - 1]);
        p = before;
        while(p != NULL) {
            if(p->last) {
                if(result == NULL) {
                    result = malloc(sizeof(struct match_result));
                    result->position = p->position;
                    result->match_str = utf_to_char(p->s);
                    result->next = NULL;
                    result_tail = result;
                }else {
                    match_str = utf_to_char(p->s);
                    result_tail->next = malloc(sizeof(struct match_result));
                    result_tail = result_tail->next;
                    result_tail->position = p->position;
                    result_tail->match_str = match_str;
                    result_tail->next = NULL;
                }
                if(!greedy) {
                    p = before;
                    before = NULL;
                    q = p;
                    while(p != NULL) {
                        q = p->next;
                        free(p);
                        p = q;
                    }
                    break;
                }
                if(before == p) {
                    q = p->next;
                    free(p);
                    before = q;
                    p = q;
                }else {
                    q->next = p->next;
                    free(p);
                    p = q->next;
                }
            }else{
                q = p;
                p = p->next;
            }
        }
        if(before == NULL) {
            before = current;
        }else if(current != NULL) {
            tail->next = before;
            before = current;
        }
    }
    p = next_match(before, 0);
    q = p;
    while(p != NULL) {
        if(p ->last) {
            if(result == NULL) {
                result = malloc(sizeof(struct match_result));
                result->position = p->position;
                result->match_str = utf_to_char(p->s);
                result->next = NULL;
                result_tail = result;
            }else {
                match_str = utf_to_char(p->s);
                result_tail->next = malloc(sizeof(struct match_result));
                result_tail = result_tail->next;
                result_tail->position = p->position;
                result_tail->match_str = match_str;
                result_tail->next = NULL;
            }
            if(q == p) {
                q = p->next;
                free(p);
                p = q;
            }else {
                q->next = p->next;
                free(p);
                p = q->next;
            }
        }else{
            q = p->next;
            free(p);
            p = q;
        }
    }
    free(s);
    return result;
}

void load_model() {
    printf("load model: %s\n", config->first_file_path);
    FILE * fout = fopen(config->first_file_path, "rb");
    FILE_CHECK(fout);
    char magic_str[10] = {0};
    fread(magic_str, sizeof(char), strlen(MAGIC_STR), fout);
    if(strncmp(magic_str, MAGIC_STR, strlen(MAGIC_STR) == 0)) {
        printf("model error!");
        exit(1);
    }
    fread(&config->first_len, sizeof(uint8_t), 1, fout);
    fread(&config->max_item, sizeof(uint8_t), 1, fout);
    config->rest_len = config->max_item - config->first_len;
    map_init(config->first_len);
    struct first_model * first;
    int total_element = 0;
    while(!feof(fout)) {
        first = malloc(sizeof(struct first_model));
        memset(first, 0, sizeof(struct first_model));
        first->ch = malloc(sizeof(uint16_t) * config->first_len);
        MEM_CHECK(first);
        fread(first->ch, sizeof(uint16_t), config->first_len, fout);
        fread(&first->length, sizeof(int), 1, fout);
        if(first->length < 0) {
            printf("model error!");
            exit(1);
        }
        first->elements = malloc(sizeof(int) * first->length);
        MEM_CHECK(first->elements);
        fread(first->elements, sizeof(int), first->length, fout);
        map_put(first->ch, first);
        total_element += first->length;
    }
    config->total = total_element;
    char * prefix = malloc(sizeof(char) * (strlen(config->next_file_path) + 5));
    FILE * fd;
    printf("total element: %d\n", total_element);
    cache = malloc(sizeof(uint16_t *) * config->cache_lavel);
    for(int i = 0; i < config->cache_lavel && i < config->rest_len; i++) {
        memset(prefix, 0, sizeof(char) * (strlen(config->next_file_path) + 5));
        sprintf(prefix, "%s_%02d", config->next_file_path, i + 1);
        printf("load cache: %s\n", prefix);
        fd = fopen(prefix, "rb");
        cache[i] = malloc(sizeof(uint16_t) * total_element);
        if(fread(cache[i], sizeof(uint16_t), total_element, fd) != total_element) {
            printf("model error!");
            exit(1);
        }
        fclose(fd);
    }
    free(prefix);
    fclose(fout);
}

void init(char * model_dir, uint8_t cache_level) {
    config = malloc(sizeof(struct model_config));
    config->model_dir = model_dir;
    config->cache_lavel = cache_level;
    config->first_file_path = get_file_name(model_dir, FIRST_FILE);
    printf("first path: %s\n", config->first_file_path);
    config->next_file_path = get_file_name(model_dir, NEXT_FILE);
    printf("next  path: %s\n", config->next_file_path);
    load_model();
}

void destroy() {
    for(int i = 0; i < config->cache_lavel; i++) {
        free(cache[i]);
    }
    free(cache);
    free(config->first_file_path);
    free(config->next_file_path);
    free(config);
    void func(uint16_t *key, void * first) {
        struct first_model *p = (struct first_model *)first;
        free(p->ch);
        free(p->elements);
        free(p);
    }
    map_for_each(func);
    map_destroy();
}

uint32_t set_last(uint32_t position, bool last){
    uint32_t mask = last ? 1 : 0;
    mask = mask << 31;
    position = position | mask;
    return position;
}

bool is_last(uint32_t position) {
    return position >> 31;
}

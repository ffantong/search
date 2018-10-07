#include "defines.h"

short ** cache;

model_config * config;

static void put_first(short key, first_element * first) {
    first_element * f = map_get(key);
    if(f != NULL) {
        first->next = f;
    }
    map_put(key, first);
}

static void write_model(FILE * fd) {
    void func(short key, void * first) {
        first_element *p, *q, *head;
        if(first != NULL) {
            head = (first_element*)first;
            int size = 1;
            p = head->next;
            head->next = NULL;
            while(p != NULL) {
                size++;
                q = p->next;
                p->next = head;
                head = p;
                p = q;
            }
            fwrite(&key, sizeof(short), 1, fd);
            fwrite(&size, sizeof(int), 1, fd);
            p = head;
            while(p!= NULL) {
                fwrite(&p->position, sizeof(int), 1, fd);
                q = p->next;
                free(p);
                p = q;
            }
            //printf("%d  %d\n", key, size);
        }
    }
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

void file_to_model(char * file, char * model_dir) {
    FILE * fd = fopen(file, "rb");
    printf("open file: %s\n", file);
    char * first_file_path = get_file_name(model_dir, FIRST_FILE);
    FILE * ffirst = fopen(first_file_path, "wb");
    printf("open first: %s\n", first_file_path);
    free(first_file_path);
    FILE_CHECK(ffirst);
    FILE * next_files[MAX_ITEM - 1];
    FILE * fnext;
    char * next_file_path = get_file_name(model_dir, NEXT_FILE);
    char * prefix = malloc(sizeof(char) * (strlen(next_file_path) + 5));
    memset(prefix, 0, sizeof(char) * (strlen(next_file_path) + 5));
    for(int i = 0; i < MAX_ITEM - 1; i++) {
        sprintf(prefix, "%s_%02d", next_file_path, i + 1);
        fnext = fopen(prefix, "wb");
        FILE_CHECK(fnext);
        next_files[i] = fnext;
    }
    free(prefix);
    free(next_file_path);
    char * full_file_path = get_file_name(model_dir, FULL_FILE);
    FILE * fname = fopen(full_file_path, "wb");
    FILE_CHECK(fname);
    free(full_file_path);
    char buf[MAX_LINE];
    int len;
    short * s;
    short mask = 0;
    int position = 0;
    first_element * first;
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
        len = short_len(s);
        if(len < 1 || len >= MAX_ITEM) {
            if(len != 0) {
                printf("%s   %d   %d\n", buf, position, len);
            }
            free(s);
            continue;
        }
        first = malloc(sizeof(first_element));
        first->position = position++;
        first->next = NULL;
        put_first(s[0], first);
        fwrite(buf, sizeof(char), MAX_LINE, fname);
        for(int i = 1; i < MAX_ITEM; i++) {
            if(i < len){
                fwrite(&s[i], sizeof(short), 1, next_files[i - 1]);
            }else {
                fwrite(&mask, sizeof(short), 1, next_files[i - 1]);
            }
        }
        free(s);
        if (position % 30000 == 0)
            printf(".");
    }
    write_model(ffirst);
    fclose(fd);
    fclose(ffirst);
    for(int i = 0; i < MAX_ITEM - 1; i++) {
        fclose(next_files[i]);
    }
    fclose(fname);
}

char * get_index_name(int position){
    FILE * fd = fopen(config->full_file_path, "rb");
    char * result = malloc(sizeof(char) * (MAX_LINE + 1));
    memset(result, 0, sizeof(char) * MAX_LINE);
    long pos = sizeof(char) * position * MAX_LINE;
    fseek(fd,  pos, SEEK_SET);
    fread(result, sizeof(char), MAX_LINE, fd);
    fclose(fd);
    return result;
}

match_element * next_match(match_element * matchs, short ch){
    match_element *p = matchs, *q = NULL, *result = NULL, *tail;
    FILE * next_files[MAX_ITEM - 1] = {NULL};
    int prefix_len = strlen(config->next_file_path) + 5;
    char * prefix = malloc(sizeof(char) * (prefix_len));
    long cur_pos[MAX_ITEM - 1] = {0};
    long step_len;
    short c;
    FILE * fd;
    while(p != NULL) {
        p->index++;
        if(p->index >= MAX_ITEM) {
            c = 0;
        } else if(p->index - 1 < config->cache_lavel) {
            c = cache[p->index - 1][p->position];
        }else {
            step_len = p->position * sizeof(short);
            fd = next_files[p->index - 1];
            if(fd == NULL) {
                memset(prefix, 0, sizeof(char) * prefix_len);
                sprintf(prefix, "%s_%02d", config->next_file_path, p->index);
                fd = fopen(prefix, "rb");
                next_files[p->index - 1] = fd;
            }
            fseek(fd, step_len - cur_pos[p->index - 1], SEEK_CUR);
            fread(&c, sizeof(short), 1, fd);
            cur_pos[p->index - 1] = step_len + sizeof(short);
        }
        if(c == ch || c == 0) {
            q = p->next;
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
    for(int i = 0; i < MAX_ITEM - 1; i++) {
        if(next_files[i] != NULL) {
            fclose(next_files[i]);
        }
    }
    free(prefix);
    return result;
}

match_result * cut(char * str, bool greedy) {
    short * s = char_to_utf(str);
    int len = short_len(s);
    first_model * first;
    match_element * current, * before = NULL, *p , *q, *tail;
    match_result * result = NULL, * result_tail;
    for(int i = 0; i < len; i++) {
        first = map_get(s[i]);
        current = NULL;
        if(first != NULL) {
            for(int j = 0; j < first->length; j++) {
                if(current == NULL) {
                    current = malloc(sizeof(match_element));
                    tail = current;
                }else {
                    p = malloc(sizeof(match_element));
                    tail->next = p;
                    tail = p;
                }
                tail->position = first->elements[j];
                tail->index = 0;
                tail->last = false;
                tail->next = NULL;
            }
        }
        before = next_match(before, s[i]);
        p = before;
        while(p != NULL) {
            if(p->last) {
                if(result == NULL) {
                    result = malloc(sizeof(match_result));
                    result->match_str = get_index_name(p->position);
                    result->next = NULL;
                    result_tail = result;
                }else {
                    result_tail->next = malloc(sizeof(match_result));
                    result_tail = result_tail->next;
                    result_tail->match_str = get_index_name(p->position);
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
        p = before;
        q = p;
    }
    p = next_match(before, 0);
    q = p;
    while(p != NULL) {
        if(p ->last) {
            if(result == NULL) {
                result = malloc(sizeof(match_result));
                result->match_str = get_index_name(p->position);
                result->next = NULL;
                result_tail = result;
            }else {
                result_tail->next = malloc(sizeof(match_result));
                result_tail = result_tail->next;
                result_tail->match_str = get_index_name(p->position);
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
    first_model * first;
    int total_element = 0;
    while(!feof(fout)) {
        first = malloc(sizeof(first_model));
        memset(first, 0, sizeof(first_model));
        MEM_CHECK(first);
        fread(&first->ch, sizeof(short), 1, fout);
        fread(&first->length, sizeof(int), 1, fout);
        first->elements = malloc(sizeof(int) * first->length);
        MEM_CHECK(first->elements);
        fread(first->elements, sizeof(int), first->length, fout);
        map_put(first->ch, first);
        total_element += first->length;
    }
    char * prefix = malloc(sizeof(char) * (strlen(config->next_file_path) + 5));
    FILE * fd;
    cache = malloc(sizeof(short *) * config->cache_lavel);
    for(int i = 0; i < config->cache_lavel; i++) {
        memset(prefix, 0, sizeof(char) * (strlen(config->next_file_path) + 5));
        sprintf(prefix, "%s_%02d", config->next_file_path, i + 1);
        printf("load cache: %s\n", prefix);
        fd = fopen(prefix, "rb");
        cache[i] = malloc(sizeof(short) * total_element);
        fread(cache[i], sizeof(short), total_element, fd);
        fclose(fd);
    }
    free(prefix);
    fclose(fout);
}

void init(char * model_dir, int cache_level) {
    map_init();
    config = malloc(sizeof(model_config));
    config->model_dir = model_dir;
    config->cache_lavel = cache_level;
    config->first_file_path = get_file_name(model_dir, FIRST_FILE);
    printf("first path: %s\n", config->first_file_path);
    config->next_file_path = get_file_name(model_dir, NEXT_FILE);
    printf("next  path: %s\n", config->first_file_path);
    config->full_file_path = get_file_name(model_dir, FULL_FILE);
    printf("full  path: %s\n", config->first_file_path);
    load_model();
}

void destroy() {
    for(int i = 0; i < config->cache_lavel; i++) {
        free(cache[i]);
    }
    free(cache);
    free(config->first_file_path);
    free(config->next_file_path);
    free(config->full_file_path);
    free(config);
    void func(short key, void * first) {
        first_model *p = (first_model *)first;
        free(p->elements);
        free(p);
    }
    map_for_each(func);
    map_destroy();
}

int set_last(int position, bool last){
    int mask = last ? 1 : 0;
    mask = mask << 31;
    position = position | mask;
    return position;
}

bool is_last(int position) {
    return position >> 31;
}

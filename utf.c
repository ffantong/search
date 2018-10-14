#include "defines.h"

uint16_t * char_to_utf(char * ch) {
    uint32_t len = strlen(ch);
    uint32_t buffer_size = sizeof(uint16_t) * (len + 1);
    uint16_t * results = malloc(buffer_size);
    uint16_t result;
    const uint32_t mask = 0b0000000011111111;
    uint32_t cursor = 0;
    for(uint32_t i = 0; i < len; i++) {
        result = 0;
        if(ch[i] == '\n' || ch[i] == '\r') {
            break;
        }
        if(isspace(ch[i])) {
            continue;
        }
        if(ch[i] > 0 && ch[i] < 128) {
            result = result | (mask & ch[i]);
        }else {
            result = result | ch[i];
            result = result << 8;
            result = result | (mask & ch[i + 1]);
            i++;
        }
        results[cursor++] = result;
    }
    results[cursor] = 0;
    return results;
}

uint32_t utf_len(uint16_t * s){
    uint32_t len;
    for(len = 0; ; len++) {
        if(s[len] == 0) {
            break;
        }
    }
    return len;
}

char * utf_to_char(uint16_t * ch) {
    unsigned int size = utf_len(ch);
    int len = size * 2 + 2;
    char * result = malloc(len);
    memset(result, 0, len);
    int cursor = 0;
    short s;
    for(int i = 0; i < size; i++) {
        s = ch[i];
        if(s >> 8 == 0) {
            result[cursor++] = s;
        }else {
            result[cursor++] = s >> 8;
            result[cursor++] = s;
        }
    }
    result[cursor] = 0;
    return result;
}

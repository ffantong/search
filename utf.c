#include "defines.h"

short * char_to_utf(char * ch) {
    int len = strlen(ch);
    int buffer_size = sizeof(short) * len;
    short * results = malloc(buffer_size);
    short result;
    const int mask = 0b0000000011111111;
    int cursor = 0;
    for(int i = 0; i < len; i++) {
        result = 0;
        if(ch[i] == '\n' || ch[i] == '\r') {
            break;
        }
        if(isspace(ch[i])) {
            continue;
        }
        if(ch[i] > 0) {
            result = result | (mask & ch[i + 1]);
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

int short_len(short * s){
    int len;
    for(len = 0; ; len++) {
        if(s[len] == 0) {
            break;
        }
    }
    return len;
}

char * utf_to_char(short * ch) {
    int size = short_len(ch);
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

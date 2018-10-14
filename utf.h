#ifndef UTF
#define UTF

uint16_t * char_to_utf(char *);

char * utf_to_char(uint16_t *);

uint32_t utf_len(uint16_t *);
#endif

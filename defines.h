#ifndef DEFINES
#define DEFINES

#include <time.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "core.h"
#include "utf.h"
#include "map.h"
#include "jni_intf.h"

#define MEM_CHECK(ptr)              \
    if(ptr == NULL) {               \
        printf("memory error.");    \
        exit(1);                    \
    }                               \

#define FILE_CHECK(fd)                                       \
    if(fd == NULL) {                                         \
        printf("open file error: %s\n", strerror(errno));    \
        exit(1);                                             \
    }

#endif // DEFINES

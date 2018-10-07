WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -Wall -std=c99 -fPIC -z noexecstack -m64
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = 

INC_DEBUG = $(INC) -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
CFLAGS_DEBUG = $(CFLAGS) -g -std=c99 -fPIC -z noexecstack -m64
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj
DEP_DEBUG = 
OUT_DEBUG = bin/search
OUT_LIB = lib/libsearch.so

OBJ_DEBUG = $(OBJDIR_DEBUG)/core.o $(OBJDIR_DEBUG)/jni_intf.o $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/map.o $(OBJDIR_DEBUG)/utf.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/core.o $(OBJDIR_RELEASE)/jni_intf.o $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/map.o $(OBJDIR_RELEASE)/utf.o

all: debug

clean: clean_debug

before_debug: 
	test -d bin || mkdir -p bin
	test -d lib || mkdir -p lib
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug out_lib after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

out_lib: $(OBJ_DEBUG) $(DEP_DEBUG)
	$(CC) -z noexecstack -shared -o $(OUT_LIB) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/core.o: core.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c core.c -o $(OBJDIR_DEBUG)/core.o

$(OBJDIR_DEBUG)/jni_intf.o: jni_intf.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c jni_intf.c -o $(OBJDIR_DEBUG)/jni_intf.o

$(OBJDIR_DEBUG)/main.o: main.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.c -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/map.o: map.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c map.c -o $(OBJDIR_DEBUG)/map.o

$(OBJDIR_DEBUG)/utf.o: utf.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c utf.c -o $(OBJDIR_DEBUG)/utf.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin
	rm -rf lib
	rm -rf $(OBJDIR_DEBUG)

.PHONY: before_debug after_debug clean_debug


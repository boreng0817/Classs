.SUFFIXES: .cc .c .o

CC=g++
CXX=g++

SRCDIR=src/
INC=include/
LIBS=lib/

# SRCS:=$(wildcard src/*.c)
# OBJS:=$(SRCS:.c=.o)

# main source file
TARGET_SRC:=$(SRCDIR)main.cc
TARGET_OBJ:=$(SRCDIR)main.o

# Include more files if you write another source file.
SRCS_FOR_LIB:= \
	$(SRCDIR)api.c \
	$(SRCDIR)file_mgr.c \
	$(SRCDIR)buffer_mgr.c \
	$(SRCDIR)find.c \
	$(SRCDIR)delete.c \
	$(SRCDIR)insert.c \
	$(SRCDIR)open.c \
	$(SRCDIR)datastructure.c \
	$(SRCDIR)join.c
CXX_SRCS_FOR_LIB:= \
	$(SRCDIR)global_vars.cc\
	$(SRCDIR)trx.cc\
	$(SRCDIR)util.cc \

OBJS_FOR_LIB:=$(SRCS_FOR_LIB:.c=.o) 
OBJS_FOR_LIB+=$(CXX_SRCS_FOR_LIB:.cc=.o)

CFLAGS+= -g -fPIC -I $(INC) -std=c++11

TARGET=main

all: $(TARGET)

$(TARGET): $(TARGET_OBJ) $(LIBS)/libbpt.a
	$(CXX) $(CFLAGS) -o $@ $^ -L $(LIBS) -lbpt -lpthread

%.o:	%.c %.cc
	$(CC) $(CFLAGS) $^ -c -o $@
%.o:	%.cc
	$(CXX) $(CFLAGS) $^ -c -o $@

clean:
	rm -rf $(TARGET) $(TARGET_OBJ) $(OBJS_FOR_LIB) $(LIBS)*

library: $(OBJS_FOR_LIB)
	gcc -shared -Wl,-soname,libbpt.so -o $(LIBS)libbpt.so $(OBJS_FOR_LIB) 

$(LIBS)/libbpt.a: $(OBJS_FOR_LIB)
	ar cr $(LIBS)libbpt.a $(OBJS_FOR_LIB)

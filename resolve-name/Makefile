# CFLAGS
LDFLAGS = -lws2_32

all: build/program

build/program: obj/main.o
	gcc $^ -o $@ ${LDFLAGS}

obj/%.o: %.c
	gcc -c $< -o $@


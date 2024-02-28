# CFLAGS
LDFLAGS = -lws2_32
CFLAGS = -Wall

all: build/server

build/server: obj/main.o
	gcc $^ -o $@ ${LDFLAGS}

obj/%.o: %.c
	gcc -c $< -o $@ ${CFLAGS}

run: all
	build/server

clean:
	rm build/server


copy: all
	cp build/server /c/path


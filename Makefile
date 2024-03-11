CC = gcc
CC_FLAGS = -Wall -Werror -g
LDFLAGS = -lm

build: image-comp

tema2: image-comp.c image-comp.h
	$(CC) $(CC_FLAGS) image-comp.c -o quadtree $(LDFLAGS)

clean:
	rm -f quadtree.o

run: build
	./quadtree

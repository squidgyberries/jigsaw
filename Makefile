CC := cc
CFLAGS := -std=c17 -Os -Wall -Wextra -Wpedantic -Wno-unused-parameter -Ideps/glfw/include -Ideps/glad/include
LDFLAGS := deps/glad/src/gl.o deps/glfw/src/libglfw3.a -lX11 -lm

SRC := src/jigsaw.c
BUILDDIRS := build
IOBJ := $(SRC:.c=.o)
OBJ := $(subst src,build,$(IOBJ))

jigsaw: $(OBJ) libs
	$(CC) -o $@ $(LDFLAGS) $(OBJ)

run: jigsaw
	./jigsaw

$(OBJ): build/%.o: src/%.c $(BUILDDIRS)
	$(CC) -o $@ $(CFLAGS) -c $<

$(BUILDDIRS):
	mkdir -p $(BUILDDIRS)

libs:
	cd deps/glad && $(CC) -o src/gl.o -Iinclude -c src/gl.c
	cd deps/glfw && cmake -D CMAKE_C_COMPILER=$(CC) -D GLFW_BUILD_EXAMPLES=OFF -D GLFW_BUILD_TESTS=OFF -D GLFW_BUILD_DOCS=OFF -D GLFW_INSTALL=OFF . && make

clean:
	rm -rf build jigsaw
	cd deps/glad && rm src/gl.o
	cd deps/glfw && make clean

.PHONY: run libs clean

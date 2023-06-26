CC := cc
CFLAGS := -std=c17 -Os -Wall -Wextra -Wpedantic -Wno-unused-parameter -Ideps/glfw/include -Ideps/glad/include -Ideps/cglm/include
LDFLAGS := deps/glad/src/gl.o deps/glfw/src/libglfw3.a deps/cglm/libcglm.a -lX11 -lm

SRC := src/jigsaw.c src/shader.c
BUILDDIRS := build
IOBJ := $(SRC:.c=.o)
OBJ := $(subst src,build,$(IOBJ))

jigsaw: $(OBJ) libs
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

run: jigsaw
	./jigsaw

$(OBJ): build/%.o: src/%.c $(BUILDDIRS)
	$(CC) -o $@ $(CFLAGS) -c $<

$(BUILDDIRS):
	mkdir -p $(BUILDDIRS)

libs:
	cd deps/glad && $(CC) -o src/gl.o -Iinclude -c src/gl.c
	cd deps/glfw && cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_C_COMPILER=$(CC) -D GLFW_BUILD_EXAMPLES=OFF -D GLFW_BUILD_TESTS=OFF -D GLFW_BUILD_DOCS=OFF -D GLFW_INSTALL=OFF . && make
	cd deps/cglm && cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_C_COMPILER=$(CC) -D CGLM_SHARED=OFF -D CGLM_STATIC=ON . && make

clean:
	rm -rf build jigsaw
	cd deps/glad && rm src/gl.o
	cd deps/glfw && make clean
	cd deps/cglm && make clean

.PHONY: run libs clean

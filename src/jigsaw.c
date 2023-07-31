#include "shader.h"
#include "window.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include <cglm/cglm.h>

#define STBI_NO_HDR
#include <stb_image.h>

// window stuff
struct {
  GLFWwindow *window;
  int window_width;
  int window_height;
} win = {NULL, 800, 600};
// int window_width = 800;
// int window_height = 600;

// gl stuff
struct {
  mat4 view;
  uint32_t program;
  uint32_t view_loc;
  uint32_t model_loc;
} gl = {GLM_MAT4_IDENTITY_INIT, 0, 0, 0};
// mat4 view = GLM_MAT4_IDENTITY_INIT;
// uint32_t program = 0;
// uint32_t view_loc = 0;
// uint32_t model_loc = 0;

float zoom = 1.0f;

const float vertices[] = {
  -1.0f, 1.0f, 0.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  1.0f, -1.0f, 1.0f, 0.0f
};

void update_view(int width, int height, mat4 view) {
  glm_mat4_identity(view);
  glm_scale(view, (vec3){zoom/((float)width), zoom/((float)height), 1.0f});
  glUniformMatrix4fv(gl.view_loc, 1, GL_FALSE, (float *)view);
  glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    zoom *= 1.25f;
  }
  if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    zoom /= 1.25f;
  }
  update_view(win.window_height, win.window_width, gl.view);
}

int main(int argc, char *argv[]) {
  int rc = 0;

  srand(time(NULL));

  win.window = create_window(win.window_width, win.window_height);
  if (!win.window) {
    fprintf(stderr, "error: GLFW window creation failed\n");
    rc = 1;
    goto exit0;
  }
  glfwSetKeyCallback(win.window, key_callback);
  
  int version = gladLoadGL(glfwGetProcAddress);
  fprintf(stderr, "GL_VENDOR: %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "GL_RENDERER: %s\n", glGetString(GL_RENDERER));
  fprintf(stderr, "GL_VERSION: %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "Using OpenGL version %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  if ((rc = create_program(&gl.program))) {
    goto exit1;
  }

  uint32_t vao = 0, vbo = 0;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLsizei stride = sizeof(float) * 4;
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)(sizeof(float) * 2));
  glEnableVertexAttribArray(1);

  stbi_set_flip_vertically_on_load(true);
  int image_width, image_height, image_n;
  unsigned char *image = NULL;
  if (argc > 1) {
    image = stbi_load(argv[1], &image_width, &image_height, &image_n, 3);
  } else {
    image_width = 800;
    image_height = 600;
    image = malloc(sizeof(unsigned char) * image_width * image_height * 3);
    for (int i = 0; i < image_width * image_height * 3; ++i) {
      image[i] = rand() % 255;
    }
  }
  if (!image) {
    fprintf(stderr, "error: Image loading failed\n");
    rc = 1;
    goto exit2;
  }

  uint32_t texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  if (argc > 1) {
    stbi_image_free(image);
  } else {
    free(image);
  }
  image = NULL;

  glUseProgram(gl.program);

  glUniform1i(glGetUniformLocation(gl.program, "tex"), 0);

  gl.view_loc = glGetUniformLocation(gl.program, "view");
  gl.model_loc = glGetUniformLocation(gl.program, "model");

  update_view(win.window_width, win.window_height, gl.view);

  // float x = 0.0f;
  while (!glfwWindowShouldClose(win.window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwGetWindowSize(win.window, &win.window_width, &win.window_height);
    update_view(win.window_width, win.window_height, gl.view);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    // glm_translate(model, (vec3){x, 0.0f, 0.0f});
    glm_scale(model, (vec3){(float)image_width, (float)image_height, 1.0f});
    glUniformMatrix4fv(gl.model_loc, 1, GL_FALSE, (float *)model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // x += 1.0f;

    glfwSwapBuffers(win.window);
    glfwWaitEvents();
  }

  glDeleteTextures(1, &texture);
exit2:
  if (argc > 1) {
    stbi_image_free(image);
  } else {
    free(image);
  }
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
exit1:
  glDeleteProgram(gl.program);
  glfwDestroyWindow(win.window);
  glfwTerminate();
exit0:
  return rc;
}

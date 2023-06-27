#include "cglm/mat4.h"
#include "shader.h"

#include <stdio.h>
#include <stdint.h>

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

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

void update_view(int width, int height, mat4 view) {
  glm_mat4_identity(view);
  glm_scale(view, (vec3){1.0f/((float)width*0.5f), 1.0f/((float)height*0.5f), 1.0f});
  glUniformMatrix4fv(gl.view_loc, 1, GL_FALSE, (float *)view);
  glViewport(0, 0, width, height);
}

int main(void) {
  int rc = 0;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  win.window = glfwCreateWindow(win.window_width, win.window_height, "jigsaw", NULL, NULL);
  if (!win.window) {
    fprintf(stderr, "error: GLFW window creation failed\n");
    rc = 1;
    goto exit0;
  }
  glfwMakeContextCurrent(win.window);
  
  int version = gladLoadGL(glfwGetProcAddress);
  fprintf(stderr, "GL_VENDOR: %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "GL_RENDERER: %s\n", glGetString(GL_RENDERER));
  fprintf(stderr, "GL_VERSION: %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "Using OpenGL version %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  glfwSwapInterval(1);

  glViewport(0, 0, win.window_width, win.window_height);

  if ((rc = create_program(&gl.program))) {
    goto exit1;
  }

  float vertices[] = {
    0.0f, 1.0f,
    -1.0f, -1.0f,
    1.0f, -1.0f
  };

  uint32_t vao = 0, vbo = 0;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
  glEnableVertexAttribArray(0);

  glUseProgram(gl.program);

  gl.view_loc = glGetUniformLocation(gl.program, "view");
  gl.model_loc = glGetUniformLocation(gl.program, "model");

  update_view(win.window_width, win.window_height, gl.view);

  mat4 model = GLM_MAT4_IDENTITY_INIT;
  glm_scale(model, (vec3){100.0f, 100.0f, 1.0f});
  glUniformMatrix4fv(gl.model_loc, 1, GL_FALSE, (float *)model);

  while (!glfwWindowShouldClose(win.window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwGetWindowSize(win.window, &win.window_width, &win.window_height);
    update_view(win.window_width, win.window_height, gl.view);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(win.window);
    glfwWaitEvents();
  }

exit1:
  glDeleteProgram(gl.program);
  glfwDestroyWindow(win.window);
exit0:
  return rc;
}

#include "cglm/mat4.h"
#include "shader.h"

#include <stdio.h>
#include <stdint.h>

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

int main(void) {
  int rc = 0;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window = glfwCreateWindow(800, 600, "jigsaw", NULL, NULL);
  if (!window) {
    fprintf(stderr, "error: GLFW window creation failed\n");
    rc = 1;
    goto exit0;
  }
  glfwMakeContextCurrent(window);
  
  int version = gladLoadGL(glfwGetProcAddress);
  fprintf(stderr, "GL_VENDOR: %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "GL_RENDERER: %s\n", glGetString(GL_RENDERER));
  fprintf(stderr, "GL_VERSION: %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "Using OpenGL version %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  glfwSwapInterval(1);

  glViewport(0, 0, 800, 600);

  uint32_t program;
  if ((rc = create_program(&program))) {
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

  glUseProgram(program);

  mat4 view = GLM_MAT4_IDENTITY_INIT;
  glm_scale(view, (vec3){1.0f/8.0f, 1.0/6.0f, 1.0f});
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (float *)view);

  mat4 model = GLM_MAT4_IDENTITY_INIT;
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float *)model);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwWaitEvents();
  }

exit1:
  glDeleteProgram(program);
  glfwDestroyWindow(window);
exit0:
  return rc;
}

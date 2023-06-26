#include <stdio.h>
#include <stdint.h>

#include <glad/gl.h>

#include <GLFW/glfw3.h>

const char *vert_source =
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"out vec2 pos;\n"
"void main() {\n"
"  gl_Position = vec4(aPos, 0.0, 1.0);\n"
"  pos = aPos;\n"
"}\n";
const char *frag_source =
"#version 330 core\n"
"in vec2 pos;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"  FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";

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

  uint32_t vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, &vert_source, NULL);
  glCompileShader(vert_shader);

  int32_t success = 0;
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    int32_t max_length = 0;
    glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &max_length);
    char info_log[max_length];
    info_log[max_length-1] = '\0';
    glGetShaderInfoLog(vert_shader, max_length, NULL, info_log);
    fprintf(stderr, "error: Vertex shader compilation failed:\n");
    fputs(info_log, stderr);
    rc = 1;
    goto exit1;
  }
  
  uint32_t frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, &frag_source, NULL);
  glCompileShader(frag_shader);

  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    int32_t max_length = 0;
    glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &max_length);
    char info_log[max_length];
    info_log[max_length-1] = '\0';
    glGetShaderInfoLog(frag_shader, max_length, NULL, info_log);
    fprintf(stderr, "error: Fragment shader compilation failed:\n");
    fputs(info_log, stderr);
    rc = 1;
    goto exit2;
  }

  uint32_t program = glCreateProgram();
  glAttachShader(program, vert_shader);
  glAttachShader(program, frag_shader);
  glLinkProgram(program);
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    int32_t max_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
    char info_log[max_length];
    info_log[max_length-1] = '\0';
    glGetProgramInfoLog(program, max_length, NULL, info_log);
    fprintf(stderr, "error: Shader program linking failed:\n");
    fputs(info_log, stderr);
    rc = 1;
    goto exit3;
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

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

exit3:
  glDeleteProgram(program);
exit2:
  glDeleteShader(frag_shader);
exit1:
  glDeleteShader(vert_shader);
  glfwDestroyWindow(window);
exit0:
  return rc;
}

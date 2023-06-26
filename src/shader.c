#include "shader.h"

#include <stdio.h>

#include <glad/gl.h>

static const char *vert_source =
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"out vec2 pos;\n"
"void main() {\n"
"  gl_Position = vec4(aPos, 0.0, 1.0);\n"
"  pos = aPos;\n"
"}\n";

static const char *frag_source =
"#version 330 core\n"
"in vec2 pos;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"  FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";

int create_program(uint32_t *out) {
  int rc = 0;
  
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
    goto exit0;
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
    goto exit1;
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
    goto exit2;
  }

  *out = program;
  goto exit1;

exit2:
  glDeleteProgram(program);
exit1:
  glDeleteShader(frag_shader);
exit0:
  glDeleteShader(vert_shader);
  return rc;
}

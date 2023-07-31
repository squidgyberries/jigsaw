#include "window.h"

GLFWwindow *create_window(int width, int height) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  GLFWwindow *window = glfwCreateWindow(width, height, "jigsaw", NULL, NULL);
  if (!window) {
    return NULL;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  return window;
}

#include <glad/glad.h>//should always be first
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>//OpenGL Mathematics
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "../include/VBO.h"
#include "../include/VAO.h"
#include "../include/Shader.h"

#include "../include/BezierCurve.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

float SCR_WIDTH = 800;
float SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;



int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//opengl versions
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//set to core profile

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL application", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    glfwSwapInterval(0);//VSync
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//capture mouse

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    Shader lineShader("resources/shaders/lineShader_vs.glsl", "resources/shaders/lineShader_fs.glsl");

    BezierCurve bezierCurve;
    bezierCurve.points.push_back({100/800.0f*2.0f-1.0f,450/600.0f*2.0f-1.0f});
    bezierCurve.points.push_back({150/800.0f*2.0f-1.0f,480/600.0f*2.0f-1.0f});
    bezierCurve.points.push_back({210/800.0f*2.0f-1.0f,450/600.0f*2.0f-1.0f});
    bezierCurve.points.push_back({040/800.0f*2.0f-1.0f,200/600.0f*2.0f-1.0f});
    bezierCurve.points.push_back({340/800.0f*2.0f-1.0f,490/600.0f*2.0f-1.0f});
    bezierCurve.RecalculateLine();

    GLuint vbo;
    VBO::generate(vbo, sizeof(glm::vec2)*bezierCurve.linePoints.size(),bezierCurve.linePoints.data(), GL_STATIC_DRAW);
    VBO::bind(vbo);
    GLuint vao;
    VAO::generate(vao);
    VAO::bind(vao);
    VAO::addAttrib(vao, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    lineShader.use();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);// input

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_LINE_STRIP, 0,bezierCurve.linePoints.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos) {
    float xposIn = static_cast<float>(xpos);
    float yposIn = static_cast<float>(ypos);

    if (firstMouse) {
        lastX = xposIn;
        lastY = yposIn;
        firstMouse = false;
    }


    lastX = xposIn;
    lastY = yposIn;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    //if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    //}
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height); //0,0 - left bottom
}

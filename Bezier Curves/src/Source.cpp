#include <glad/glad.h>//should always be first
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>//OpenGL Mathematics
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <functional>

#include "../include/VBO.h"
#include "../include/VAO.h"
#include "../include/Shader.h"
#include "../include/Time.h"

#include "../include/BezierCurve.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
std::function<void()> shader_viewpoint_callback;

float SCR_WIDTH = 800;
float SCR_HEIGHT = 600;

class Mouse {
public:
    Mouse(const float x,const float y):pos({x,y}){}
    Mouse():Mouse(0,0){}
    glm::vec2 pos;
    bool leftPressed = false;
    bool rightPressed = false;
    bool wheelScrolled = false;
    bool firstInput = true;
};
Mouse mouse(SCR_WIDTH/2.0f,SCR_HEIGHT/2.0f);

class BezierCurveVisualizer {
	GLuint bc_vbo = 0;
	GLuint bc_vao = 0;
	GLuint points_vbo = 0;
	GLuint points_vao = 0;
	const float pointCaptureDistance = 10.0f;
    glm::vec2* capturedPoint = nullptr;
    int capturedPointIndex = 0;
public:
	BezierCurve bezierCurve;
	void Init() {
        bezierCurve.points.push_back({100,450});
        bezierCurve.points.push_back({150,480});
        bezierCurve.points.push_back({210,450});
        bezierCurve.points.push_back({040,200});
        bezierCurve.points.push_back({340,490});
        bezierCurve.RecalculateLine();

        VBO::generate(bc_vbo,sizeof(glm::vec2)*bezierCurve.linePoints.size(),bezierCurve.linePoints.data(),GL_STATIC_DRAW);
        VBO::bind(bc_vbo);
        VAO::generate(bc_vao);
        VAO::bind(bc_vao);
        VAO::addAttrib(bc_vao,0,2,GL_FLOAT,GL_FALSE,2 * sizeof(float),(void*)0);

        VBO::generate(points_vbo,sizeof(glm::vec2)*bezierCurve.points.size(),bezierCurve.points.data(),GL_STATIC_DRAW);
        VBO::bind(points_vbo);
        VAO::generate(points_vao);
        VAO::bind(points_vao);
        VAO::addAttrib(points_vao,0,2,GL_FLOAT,GL_FALSE,2 * sizeof(float),(void*)0);
    }
    void UpdateCurve() {
        bezierCurve.RecalculateLine();
        VBO::setData(bc_vbo,sizeof(glm::vec2)*bezierCurve.linePoints.size(),bezierCurve.linePoints.data(),GL_STATIC_DRAW);
        VBO::setData(points_vbo,sizeof(glm::vec2)*bezierCurve.points.size(),bezierCurve.points.data(),GL_STATIC_DRAW);
    }
    void Draw(const Shader& lineShader,const Shader& pointShader) const {
        VAO::bind(points_vao);
        pointShader.use();
        glPointSize(5);
        glDrawArrays(GL_POINTS,0,bezierCurve.points.size());
        
        VAO::bind(bc_vbo);
        lineShader.use();
        glDrawArrays(GL_LINE_STRIP,0,bezierCurve.linePoints.size());
    }
    void HandleMouse() {
        if(mouse.leftPressed) {
	        if(capturedPoint!=nullptr) {
	        	*capturedPoint = glm::clamp(mouse.pos, {0,0}, glm::vec2(SCR_WIDTH,SCR_HEIGHT)) ;
                UpdateCurve();
	        }
        }else {
            capturedPoint=nullptr;
        }
    }
    void NewPoint(const glm::vec2 pos) {
        bezierCurve.points.push_back(pos);
        capturedPoint = &bezierCurve.points.back();
        capturedPointIndex = bezierCurve.points.size()-1;
	}
    void EraseCapturedPoint() {
	    if(capturedPoint!=nullptr) {
            bezierCurve.points.erase(bezierCurve.points.begin()+capturedPointIndex);
            capturedPoint = nullptr;
            UpdateCurve();
	    }
    }
    void CheckCapturePoint() {
        int closestPoint = 0;
        float closestDist = FLT_MAX;
        for(int i=0;i<bezierCurve.points.size();i++) {
            const float dist = glm::distance(mouse.pos,bezierCurve.points[i]);
            if(dist < closestDist) {
                closestDist = dist;
                closestPoint = i;
            }
        }
        if(closestDist<=pointCaptureDistance) {
            capturedPointIndex = closestPoint;
            capturedPoint = &bezierCurve.points[closestPoint];
        }
    }
};
BezierCurveVisualizer bcVisualizer;

glm::mat4 projection = glm::ortho(0.0f,SCR_WIDTH,SCR_HEIGHT,0.0f);

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//opengl versions
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//set to core profile

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bezier Curve Simulator", NULL, NULL);
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
    glfwSetKeyCallback(window, key_callback);
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

    Time::Init();

    glm::mat4 model = glm::mat4(1.0f);

    Shader lineShader("resources/shaders/lineShader_vs.glsl", "resources/shaders/lineShader_fs.glsl");
    Shader pointShader("resources/shaders/point_vs.glsl", "resources/shaders/point_fs.glsl");
    lineShader.use();
    lineShader.setVec2("res",SCR_WIDTH,SCR_HEIGHT);
    lineShader.setMat4("projection",projection);
    lineShader.setMat4("model",model);
    pointShader.use();
    pointShader.setVec2("res",SCR_WIDTH,SCR_HEIGHT);
    pointShader.setMat4("projection",projection);
    pointShader.setMat4("model",model);

    shader_viewpoint_callback = [&]() {
        lineShader.use();
        lineShader.setVec2("res",SCR_WIDTH,SCR_HEIGHT);
        lineShader.setMat4("projection",projection);
        pointShader.use();
        pointShader.setVec2("res",SCR_WIDTH,SCR_HEIGHT);
        pointShader.setMat4("projection",projection);
    };

    bcVisualizer.Init();

    while (!glfwWindowShouldClose(window)) {
        Time::Update();
        //processInput(window);// input

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        bcVisualizer.Draw(lineShader,pointShader);
        bcVisualizer.HandleMouse();

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window,true);
	}
    if(key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
    	bcVisualizer.EraseCapturedPoint();
    }
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos) {
    float xposIn = static_cast<float>(xpos);
    float yposIn = static_cast<float>(ypos);

    if (mouse.firstInput) {
        mouse.pos.x = xposIn;
        mouse.pos.y = yposIn;
        mouse.firstInput = false;
    }

    mouse.pos.x = xposIn;
    mouse.pos.y = yposIn;
}

double doubleClickSpeed = 0.300; //sec
double lastClick = 0;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouse.leftPressed = true;
        bcVisualizer.CheckCapturePoint();
        if(Time::time-lastClick <= doubleClickSpeed) { // double click
            bcVisualizer.NewPoint(mouse.pos);
            bcVisualizer.UpdateCurve();
        }else{
            lastClick = Time::time;
        }
    }
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouse.leftPressed = false;
    }
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if(glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        bcVisualizer.bezierCurve.SetPrecision(bcVisualizer.bezierCurve.GetPrecision()*(1+0.1f*yoffset));
        bcVisualizer.bezierCurve.RecalculateLine();
        bcVisualizer.UpdateCurve();
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height); //0,0 - left bottom
	projection = glm::ortho(0.0f,SCR_WIDTH,SCR_HEIGHT,0.0f);
	shader_viewpoint_callback();
}

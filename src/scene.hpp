#ifndef SCENE_H
#define SCENE_H
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

class Event {
public:
    virtual ~Event() = default;
};

class CloseEvent : public Event {
};

class DropEvent : public Event {

};

class DragEvent : public Event {
    double x, y;
    public:
    DragEvent(double _x, double _y) : x(_x), y(_y) {}
    double get_x() { return x; }
    double get_y() { return y; }
};

class Scene {
    int event_state = 0;
public:
    GLFWwindow* window;
    Scene() {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        // glfw window creation
        // --------------------
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "The Game", NULL, NULL);
        if (window == nullptr)
        {
            std::cout << "Graphics :: PANIC, Failed to create GLFW window." << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);
       glfwSetFramebufferSizeCallback(window, [](GLFWwindow* gw, int w, int h) -> void { glViewport(0, 0, w, h); });

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Graphics :: PANIC, Failed to initialize GLAD." << std::endl;
            return;
        }        

        glEnable(GL_DEPTH_TEST);
    }

    ~Scene() {
        glfwTerminate();
    }


    Event* poll_event() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
            
        if (glfwWindowShouldClose(window)) {
            return new CloseEvent;
        }
            
        // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT == GLFW_PRESS)) {
        //     double xpos, ypos;
        //     glfwGetCursorPos(window, &xpos, &ypos);
        //     int height, width;
        //     glfwGetWindowSize(window, &width, &height);
        //     return new DragEvent(2 * (xpos / width) - 1, 2 * (-ypos / height) + 1);
        // }
        // std::cout << "WTF is " << event_state << std::endl;;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && event_state == 0) {
            event_state = 1;
            return new DropEvent;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            event_state = 0;
        }
        return new Event;
    }

    void predraw() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void postdraw() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

};



#endif
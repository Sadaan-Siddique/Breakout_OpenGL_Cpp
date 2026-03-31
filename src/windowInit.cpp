#include "windowInit.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "global_variables.h"
#include <iostream>
using std::cout, std::endl;

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void GetOpenGLVersionInfo()
{
    cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "Version: " << glGetString(GL_VERSION) << endl;
    cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

void windowInitialize()
{
    // Initialize Program
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    g_window = glfwCreateWindow(g_SCR_WIDTH, g_SCR_HEIGHT, "GLFW OpenGL Window", NULL, NULL);
    if(g_window == NULL)
    {
        cout << "Failed to create GLFW Window." << endl;
        glfwTerminate();
        // return -1;  
        exit(-1); // Instantly kills the program
    }

    glfwMakeContextCurrent(g_window);
    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        // return -1;
        exit(-1); // Instantly kills the program
    }
}

void mainRenderingLoop()
{
    while(!glfwWindowShouldClose(g_window))
    {
        processInput(g_window);
        
        // Pre Draw
        // glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glClearColor(1.f, 1.f, 0.f, 1.f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Without this, Framebuffer may behave weirdly.

        
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    GetOpenGLVersionInfo();
}

void terminateWindow()
{
    glfwTerminate();
}
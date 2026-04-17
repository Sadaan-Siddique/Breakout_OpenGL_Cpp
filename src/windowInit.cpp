#include "windowInit.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "global_variables.h"
#include "game.h"
#include <iostream>
using std::cout, std::endl;

Game Breakout(800, 800);

// 1. REPLACED old processInput with the proper GLFW callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // Close window on Escape
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Update the Game's input array when keys are pressed or released
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            Breakout.m_keys[key] = true;
        else if (action == GLFW_RELEASE)
            Breakout.m_keys[key] = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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
        exit(-1);
    }

    glfwMakeContextCurrent(g_window);
    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
    
    // 2. REGISTER THE CALLBACK HERE (Right after the framebuffer callback)
    glfwSetKeyCallback(g_window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}

void mainRenderingLoop()
{
    Breakout.Init(); // Initialize the game 

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while(!glfwWindowShouldClose(g_window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 3. REMOVED the old processInput(g_window) call here. 
        // The callback handles it automatically in the background now!
        
        Breakout.ProcessInput(deltaTime);
        Breakout.Update(deltaTime); 
        
        glDisable(GL_DEPTH_TEST); 
        glDisable(GL_CULL_FACE);
        
        glEnable(GL_BLEND);       
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Breakout.Render(); 
        
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    GetOpenGLVersionInfo();
}

void terminateWindow()
{
    glfwTerminate();
}
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "windowInit.h"
#include <filesystem>
#include <iostream>
using std::cout, std::endl;

int main()
{
    cout << std::filesystem::current_path() << endl;
    windowInitialize();

    mainRenderingLoop();

    terminateWindow(); // clean up

    return 0;
}
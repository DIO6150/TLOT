
#include "GLFW/glfw3.h"

#include <iostream>

#define TITLE "The Legend of Teto"
#define SIZE_W 800
#define SIZE_H 800

int main (__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
    GLFWwindow* window;

    if (!glfwInit ()) return (-1);

    window = glfwCreateWindow (SIZE_W, SIZE_H, TITLE, NULL, NULL);
    if (!window)
    {
        glfwTerminate ();
        return (-1);
    }

    glfwMakeContextCurrent (window);

    while (!glfwWindowShouldClose (window))
    {
        /* Render here */
        glClear (GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers (window);
        glfwPollEvents ();
    }

    glfwTerminate ();
    return (0);
}
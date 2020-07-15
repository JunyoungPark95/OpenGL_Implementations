#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <chrono>
#include <thread>



// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;
//define a constant for rotation
double degree;
double PI = 3.141592;
// The MAIN function, from here we start the application and run the game loop
int main()
{
    std::cout << "Starting OpenGL 3.1" << std::endl;
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "glskeleton", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

	// Define a initial vertex
	double v1_x = -0.5;
	double v2_x = 0.5;
	double v3_x = 0;
	double v1_y = -0.5;
	double v2_y = -0.5;
	double v3_y = 0.5;
	// Define a initial color
	double R = 0.7;
	double G = 0.3;
	double B = 0.3;
    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
		//Set the timer repeat for 10sec
		double timer = glfwGetTime();
        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
		//printf("%f\n", timer);
		degree = 10 * timer;
		//change the degree & color
		double theta = degree * (PI / 180.0);

		double v1_new_x = v1_x * cos(theta) - v1_y * sin(theta);
		double v1_new_y = v1_x * sin(theta) - v1_y * cos(theta);

		double v2_new_x = v2_x * cos(theta) - v2_y * sin(theta);
		double v2_new_y = v2_x * sin(theta) - v2_y * cos(theta);

		double v3_new_x = v3_x * cos(theta) - v3_y * sin(theta);
		double v3_new_y = v3_x * sin(theta) - v3_y * cos(theta);
		//change the color in constant period
			R = 0.7 * abs(cos(theta));
			G = 0.3 * abs(cos(theta));
			B = 0.3 * abs(cos(theta));
		//printf("%f\n", v1_new_x);
        glBegin(GL_TRIANGLES);
        glColor3f(R, G, B);
        glVertex3f(v1_new_x, v1_new_y, 0);
        glVertex3f(v2_new_x, v2_new_x, 0);
        glVertex3f(v3_new_x, v3_new_y, 0);
        glEnd();
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Terminates GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

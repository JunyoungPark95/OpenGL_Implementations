#include <iostream>
#include <time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "tinyobjloader/tiny_obj_loader.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Function prototypes
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void buffer(GLFWwindow *window, tinyobj::attrib_t attrib, std::vector<tinyobj::shape_t> shapes);
void Initialize();
void Uninitialize();
// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;
int color = 0;
unsigned char pixel[3];

//object loading variables
tinyobj::index_t index;
tinyobj::real_t vertex_x1, vertex_y1, vertex_z1,
vertex_x2, vertex_y2, vertex_z2,
vertex_x3, vertex_y3, vertex_z3;
tinyobj::real_t vertex_normal_x, vertex_normal_y, vertex_normal_z;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	//in order to check the object parsing time
	clock_t start, end;
	double result;

	start = clock(); // parsing time calc start

	//object file load
	std::string inputfile = "..\\..\\bunny.obj";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	end = clock(); // parsing time check done
	result = (double)(end - start);

	printf("##### bunny object vertices  = %d #####\n", (int)(attrib.vertices.size()) / 3);
	printf("##### bunny object normals   = %d #####\n", (int)(attrib.normals.size()) / 3);
	printf("##### bunny object shapes    = %d #####\n", (int)shapes.size());
	printf("##### bunny object parsing time   = %f #####\n", result / CLOCKS_PER_SEC);

	std::cout << "Starting GLFW context, OpenGL 3.1" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE); //Default : use a double buffer
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
	double xpos, ypos;
	
	//glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);
	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		//wait events
		glfwWaitEvents();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		
		//render
		if (color == 0)
		{
			glDrawBuffer(GL_FRONT);
			Initialize();
			buffer(window, attrib, shapes);
			glDrawBuffer(GL_BACK);
			Uninitialize();
			color = 2; //set different color object ID on the back buffer
			buffer(window, attrib, shapes);
			color = 0; //re-initialize the color value
		}
		else if(color == 1)
		{
			Uninitialize();
			glDrawBuffer(GL_FRONT);
			buffer(window, attrib, shapes);
		}
		glFinish();
	}

	// Terminates GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
//read pixel value when mouse event happened
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glReadBuffer(GL_BACK); // read a back buffer pixel
		glReadPixels(xpos, HEIGHT - ypos - 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);
		if (color == 0 && (double)pixel[0] == 102 && (double)pixel[1] == 127 && (double)pixel[2] == 153)
		{
			color = 1;
			std::cout << "Bunny object picked" << std::endl;
		}
		else if (color == 1 && (double)pixel[0] == 51 && (double)pixel[1] == 76 && (double)pixel[2] == 76)
		{
			color = 0;
			std::cout << "Bunny object released" << std::endl;
		}
		//std::cout << (double)pixel[0] << std::endl;
		//std::cout << (double)pixel[1] << std::endl;
		//std::cout << (double)pixel[2] << std::endl;
	}
}

//rendering function
void buffer(GLFWwindow *window, tinyobj::attrib_t attrib, std::vector<tinyobj::shape_t> shapes){
	glm::mat4 matProj = glm::perspective(glm::radians(60.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
	glm::mat4 matModel = glm::identity<glm::mat4>();
	glm::mat4 matView = glm::lookAt(glm::vec3(0, 4, 4),
									glm::vec3(0, 0, 0),
									glm::vec3(0, 1, 0));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(matProj));
	glm::mat4 modelView = matView * matModel;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(modelView));
	int j = 0;
	for (int i = 0; i < shapes[0].mesh.indices.size() / 3; i++)
	{
		glBegin(GL_TRIANGLES);
		if (color == 0)
			glColor3f(0.2, 0.4, 0.7);
		else if (color == 1)
			glColor3f(0.7, 0.3, 0.3);
		else if (color == 2)
			glColor3f(0.4, 0.5, 0.6);
		index = shapes[0].mesh.indices[j];
		vertex_normal_x = attrib.normals[3 * index.normal_index + 0];
		vertex_normal_y = attrib.normals[3 * index.normal_index + 1];
		vertex_normal_z = attrib.normals[3 * index.normal_index + 2];
		glNormal3f(vertex_normal_x, vertex_normal_y, vertex_normal_z);

		vertex_x1 = attrib.vertices[3 * index.vertex_index + 0];
		vertex_y1 = attrib.vertices[3 * index.vertex_index + 1];
		vertex_z1 = attrib.vertices[3 * index.vertex_index + 2];
		//printf("%f %f %f\n", vertex_x1, vertex_y1, vertex_z1);
		glVertex3f(vertex_x1, vertex_y1, vertex_z1);

		index = shapes[0].mesh.indices[j + 1];

		vertex_x2 = attrib.vertices[3 * index.vertex_index + 0];
		vertex_y2 = attrib.vertices[3 * index.vertex_index + 1];
		vertex_z2 = attrib.vertices[3 * index.vertex_index + 2];
		//printf("%f %f %f\n", vertex_x2, vertex_y2, vertex_z2);
		glVertex3f(vertex_x2, vertex_y2, vertex_z2);

		index = shapes[0].mesh.indices[j + 2];

		vertex_x3 = attrib.vertices[3 * index.vertex_index + 0];
		vertex_y3 = attrib.vertices[3 * index.vertex_index + 1];
		vertex_z3 = attrib.vertices[3 * index.vertex_index + 2];
		//printf("%f %f %f\n", vertex_x3, vertex_y3, vertex_z3);
		glVertex3f(vertex_x3, vertex_y3, vertex_z3);

		glEnd();

		j = j + 3;
	}
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
}
//Insert lighting
//Lights on
void Initialize()

{
	glDrawBuffer(GL_FRONT);
	glEnable(GL_LIGHTING);                                       
	glEnable(GL_LIGHT0);                       

	float AmbientColor[] = { 0.0f, 0.0f, 0.2f, 0.0f };      
	float DiffuseColor[] = { 0.5f, 0.5f, 0.5f, 0.0f };       
	float SpecularColor[] = { 0.5f, 0.5f, 0.5f, 0.0f };       
	float Position[] = { 100.0f, 100.0f, 400.0f, 1.0f };  

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);              
	glShadeModel(GL_SMOOTH);                            
	glEnable(GL_DEPTH_TEST);                              
	glEnable(GL_CULL_FACE);                            
	glFrontFace(GL_CCW);                               
	glEnable(GL_LIGHTING);                             
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);     
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);       
	glLightfv(GL_LIGHT0, GL_POSITION, Position);     
	glEnable(GL_LIGHT0);	

	float spotlightDirection[] = { 0.0f, 0.0f, -1.0f };     

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 80.0f);                  
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 80.0f);                
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotlightDirection);   
} 
//Lights off
void Uninitialize() {
	glDisable(GL_LIGHTING);                                        //조명 활성화
	glDisable(GL_LIGHT0);
	glDisable(GL_DEPTH_TEST);                              // 가려진 면 제거
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
}

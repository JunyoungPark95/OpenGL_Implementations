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

#include <glm/gtx/quaternion.hpp>


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_cursor_callback(GLFWwindow * window, double xpos, double ypos);
void mouse_botton_callback(GLFWwindow * window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;

//object loading variables
tinyobj::index_t index;
tinyobj::real_t vertex_x1, vertex_y1, vertex_z1,
				vertex_x2, vertex_y2, vertex_z2,
				vertex_x3, vertex_y3, vertex_z3;
tinyobj::real_t vertex_normal_x1, vertex_normal_y1, vertex_normal_z1,
				vertex_normal_x2, vertex_normal_y2, vertex_normal_z2,
				vertex_normal_x3, vertex_normal_y3, vertex_normal_z3;

bool point_light = true;
bool directional_light = true;

GLuint display_list;

float fov = 60.0f;

double pos_buffer[2] = {};
int i = 0;
bool trig = false;

//float eye_x = 0.0f;
//float eye_y = 4.0f;
//float eye_z = 4.0f;
//
//float head_x = 0.0f;
//float head_y = 1.0f;
//float head_z = 0.0f;

glm::mat4 matModel = glm::identity<glm::mat4>();

float eye_x = 0.0f;
float eye_y = 4.0f;
float eye_z = 4.0f;

float up_x = 0.0f;
float up_y = 1.0f;
float up_z = 0.0f;

glm::mat4 matView =  glm::lookAt(glm::vec3(eye_x, eye_y, eye_z), glm::vec3(0, 0, 0), glm::vec3(up_x, up_y, up_z));

glm::vec4 eye_vec = glm::vec4(eye_x, eye_y, eye_z, 1);
glm::vec4 up_vector = glm::vec4(up_x, up_y, up_z, 1);
float pi = 3.141592f;
float radius = 2.0f;
float angle = 0.0;

int delay_term = 0;

double xpos, ypos;
float dolly_scale = 1.0f;
int zoom_trig = 0;

glm::vec4 eye_vec2;
// The MAIN function, from here we start the application and run the game loop
int main()
{
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

	printf("##### bunny object vertices  = %d #####\n", (int)(attrib.vertices.size()) / 3);
	printf("##### bunny object normals   = %d #####\n", (int)(attrib.normals.size()) / 3);
	printf("##### bunny object shapes    = %d #####\n", (int)shapes.size());

    std::cout << "Starting GLFW context, OpenGL 3.1" << std::endl;
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
	glfwSetCursorPosCallback(window, mouse_cursor_callback);
	glfwSetMouseButtonCallback(window, mouse_botton_callback);
	glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

	display_list = glGenLists(1); // add a display list because of delay

	int j = 0;

	glNewList(display_list, GL_COMPILE); // make a display list
	for (int i = 0; i < shapes[0].mesh.indices.size() / 3; i++)
	{
		glBegin(GL_TRIANGLES);
		glColor3f(0, 0, 0);

		index = shapes[0].mesh.indices[j];

		vertex_normal_x1 = attrib.normals[3 * index.normal_index + 0];
		vertex_normal_y1 = attrib.normals[3 * index.normal_index + 1];
		vertex_normal_z1 = attrib.normals[3 * index.normal_index + 2];
		glNormal3f(vertex_normal_x1, vertex_normal_y1, vertex_normal_z1);

		vertex_x1 = attrib.vertices[3 * index.vertex_index + 0];
		vertex_y1 = attrib.vertices[3 * index.vertex_index + 1];
		vertex_z1 = attrib.vertices[3 * index.vertex_index + 2];
		//printf("%f %f %f\n", vertex_x1, vertex_y1, vertex_z1);
		glVertex3f(vertex_x1, vertex_y1, vertex_z1);

		index = shapes[0].mesh.indices[j + 1];

		vertex_normal_x2 = attrib.normals[3 * index.normal_index + 0];
		vertex_normal_y2 = attrib.normals[3 * index.normal_index + 1];
		vertex_normal_z2 = attrib.normals[3 * index.normal_index + 2];
		glNormal3f(vertex_normal_x2, vertex_normal_y2, vertex_normal_z2);

		vertex_x2 = attrib.vertices[3 * index.vertex_index + 0];
		vertex_y2 = attrib.vertices[3 * index.vertex_index + 1];
		vertex_z2 = attrib.vertices[3 * index.vertex_index + 2];
		//printf("%f %f %f\n", vertex_x2, vertex_y2, vertex_z2);
		glVertex3f(vertex_x2, vertex_y2, vertex_z2);

		index = shapes[0].mesh.indices[j + 2];

		vertex_normal_x3 = attrib.normals[3 * index.normal_index + 0];
		vertex_normal_y3 = attrib.normals[3 * index.normal_index + 1];
		vertex_normal_z3 = attrib.normals[3 * index.normal_index + 2];
		glNormal3f(vertex_normal_x3, vertex_normal_y3, vertex_normal_z3);

		vertex_x3 = attrib.vertices[3 * index.vertex_index + 0];
		vertex_y3 = attrib.vertices[3 * index.vertex_index + 1];
		vertex_z3 = attrib.vertices[3 * index.vertex_index + 2];
		//printf("%f %f %f\n", vertex_x3, vertex_y3, vertex_z3);
		glVertex3f(vertex_x3, vertex_y3, vertex_z3);

		glEnd();

		j = j + 3;
	}
	glEndList();
	
    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glm::mat4 matProj = glm::perspective(glm::radians(fov), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
		
		glm::mat4 modelView = matView * matModel;

		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		float ambientStrength = 0.1;
		
		float pointLight[] = { 0.0, 10.0, 0.0, 1.0 };
		float La[] = { 0.1, 0.1, 0.1, 1.0 };
		float Ld[] = { 1.0, 1.0, 1.0, 1.0 };
		float Ls[] = { 1.0, 1.0, 1.0, 1.0 };

		float ka[] = { 0.7, 0.7, 0.7, 1.0 };
		float kd[] = { 0.7, 0.7, 0.7, 1.0 };
		float ks[] = { 0.9, 0.9, 0.9, 1.0 };
		float shininess[] = { 30.0 };
		//float MatEmission[] = { 0.0, 0.0, 0.0, 1.0 };

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_SMOOTH);
		//glEnable(GL_CULL_FACE);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(matProj));
		
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(modelView));

		//light
		glLightfv(GL_LIGHT0, GL_POSITION, pointLight);
		glLightfv(GL_LIGHT0, GL_AMBIENT, La);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, Ld);
		glLightfv(GL_LIGHT0, GL_SPECULAR, Ls);
		//glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
		//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

		glEnable(GL_LIGHTING);
		if (point_light == true)
			glEnable(GL_LIGHT0);
		else if (point_light == false)
			glDisable(GL_LIGHT0);

		//material
		glMaterialfv(GL_FRONT, GL_AMBIENT, ka);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, kd);
		glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
		//glMaterialfv(GL_FRONT, GL_EMISSION, MatEmission);

		float directionalLight[] = { 0.0, -10.0, 0.0, 0.0 };
		glLightfv(GL_LIGHT1, GL_POSITION, directionalLight);
		glLightfv(GL_LIGHT1, GL_AMBIENT, La);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, Ld);
		glLightfv(GL_LIGHT1, GL_SPECULAR, Ls);
		if (directional_light == true)
			glEnable(GL_LIGHT1);
		else if (directional_light == false)
			glDisable(GL_LIGHT1);


		//bunny list call
		glCallList(display_list);
		

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

  //lighting on/off specifically point light and directional light
  if (key == GLFW_KEY_1)
	  point_light = true;
  if (key == GLFW_KEY_2)
	  point_light = false;
  if (key == GLFW_KEY_3)
	  directional_light = true;
  if (key == GLFW_KEY_4)
	  directional_light = false;

  if (key == GLFW_KEY_Q) //zoom in (need to add a clamp)
  {
	  printf("%f\n", fov);
	  if(fov < 77.25)
		fov += 1;
  }
  if (key == GLFW_KEY_W) //zoom out (need to add a clamp)
  {
	  printf("%f\n", fov);
	  if (fov > 33.75)
		fov -= 1;
  }

}

glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		rotationAxis = cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01)
			rotationAxis = cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;


	return glm::quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs, rotationAxis.z * invs);
}


glm::vec3 initial_pos;
glm::vec3 moved_pos;
int jj = 0;
float distance, normalize_term, zpos;
int kk = 0;
glm::vec4 initial_pos_buf, moved_pos_buf;

glm::vec3 j;
glm::vec3 k;
glm::vec3 jjj;
glm::vec3 kkk;
glm::mat4 R2 = glm::mat4(glm::vec4(1,0,0,0), glm::vec4(0,1,0,0), glm::vec4(0,0,1,0), glm::vec4(0,0,0,1));
void mouse_cursor_callback(GLFWwindow * window, double xpos, double ypos)
{
	
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			return;
		}


		if (delay_term == 1 && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			//std::cout << "now draged " << std::endl;
			glfwGetCursorPos(window, &xpos, &ypos);


			xpos = (radius*xpos - WIDTH) / WIDTH;
			ypos = (HEIGHT - radius*ypos) / HEIGHT;

			//std::cout << "initial position: " << xpos << " , " << ypos << std::endl;

			distance = (float)sqrt(xpos*xpos + ypos * ypos);

			if (xpos*xpos + ypos * ypos <= .5)
			{
				zpos = sqrt(1.0 - (xpos*xpos + ypos * ypos));
			}
			else
			{
				zpos = 0.5 / sqrt(xpos*xpos + ypos * ypos);
			}

			normalize_term = 1.0f / (float)sqrt(xpos * xpos + ypos * ypos + zpos * zpos);


			xpos *= normalize_term;
			ypos *= normalize_term;
			zpos *= normalize_term;

			moved_pos = glm::vec3(xpos, ypos, zpos);

			//initial_pos = initial_pos * glm::toMat3(RotationBetweenVectors(initial_pos, moved_pos));
			//glm::vec3 initial_pos_buf = initial_pos;
			
			//glm::mat4 dolly_mat = glm::mat4((dolly_scale));

			initial_pos_buf = glm::vec4(initial_pos.x, initial_pos.y, initial_pos.z, 1) * matView;
			//initial_pos_buf = glm::vec4(initial_pos.x, initial_pos.y, initial_pos.z, 1) * R2;
			moved_pos_buf = glm::vec4(moved_pos.x, moved_pos.y, moved_pos.z, 1) * matView;
			//moved_pos_buf = glm::vec4(moved_pos.x, moved_pos.y, moved_pos.z, 1) * R2;
			j = glm::vec3(initial_pos_buf.x, initial_pos_buf.y, initial_pos_buf.z);
			k = glm::vec3(moved_pos_buf.x, moved_pos_buf.y, moved_pos_buf.z);
			

			

			std::cout << "initial pos :" << initial_pos.x << ", " << initial_pos.y << ", " << initial_pos.z << "  moved pos :" << moved_pos.x << ", " << moved_pos.y << ", " << moved_pos.z << std::endl;

			const glm::mat4 R = glm::toMat4(RotationBetweenVectors(j,k));

			R2 = R2 * R;

			matView = matView * R;
			
			eye_vec = eye_vec * R;

			up_vector = up_vector * R;

			initial_pos = moved_pos;



		}

		//if (delay_term == 0 && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		//{

		//}
		
}

void mouse_botton_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);

		std::cout << "now clicked" << std::endl;

		xpos = (radius*xpos - WIDTH) / WIDTH;
		ypos = (HEIGHT - radius * ypos) / HEIGHT;

		distance = (float)sqrt(xpos*xpos + ypos * ypos);

		if (xpos*xpos + ypos * ypos <= .5)
		{
			zpos = sqrt(1.0 - (xpos*xpos + ypos * ypos));
		}
		else
		{
			zpos = 0.5 / sqrt(xpos*xpos + ypos * ypos);
		}

		normalize_term = 1.0f / (float)sqrt(xpos*xpos + ypos * ypos + zpos * zpos);
		xpos *= normalize_term;
		ypos *= normalize_term;
		zpos *= normalize_term;

		initial_pos = glm::vec3(xpos, ypos, zpos);
		
		delay_term = 1;

	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		std::cout << "now released" << std::endl;
		delay_term = 0;
		//initial_pos = glm::vec3(0.0f, 0.0f, 0.0f);
		//moved_pos = glm::vec3(0.0f, 0.0f, 0.0f);
		zoom_trig = 1;
	}
}
float inc = 0.1f;

float speed = 1.0f;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	if (yoffset > 0)
	{

		std::cout << "scrolled up" << std::endl;



		float cx, cy, cz;
		if (zoom_trig == 0)
		{
			//eye_vec2 = normalize(glm::vec4(0.0f, 4.0f, 4.0f, 1.0f));
			eye_vec2 = glm::vec4(0.0f, 4.0f, 4.0f, 0.0f);

		}
		else if (zoom_trig == 1)
		{
			//eye_vec2 = normalize(moved_pos_buf);
			eye_vec2 = glm::vec4(0.0f, 4.0f, 4.0f, 0.0f) * R2;
		}
		
		eye_vec2 = normalize(eye_vec2);

		cx = eye_vec2.x * -0.2;
		cy = eye_vec2.y * -0.2;
		cz = eye_vec2.z * -0.2;


		glm::mat4 trans(glm::vec4(1, 0, 0, cx), //translate
						glm::vec4(0, 1, 0, cy), 
						glm::vec4(0, 0, 1, cz), 
						glm::vec4(0, 0, 0, 1));

		eye_vec = eye_vec * trans;

		up_vector = up_vector;
		matView = glm::lookAt(glm::vec3(eye_vec.x, eye_vec.y, eye_vec.z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(up_vector.x, up_vector.y, up_vector.z));
		//matView = glm::lookAt(glm::vec3(eye_vec.x, eye_vec.y, eye_vec.z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		//matView = matView * trans * R2;
	}


	else
	{

		std::cout << "scrolled down at " << std::endl;

		float cx, cy, cz;

		if (zoom_trig == 0)
		{
			//eye_vec2 = normalize(glm::vec4(0.0f, 4.0f, 4.0f, 1.0f));
			eye_vec2 = glm::vec4(0.0f, 4.0f, 4.0f, 0.0f);

		}
		else if (zoom_trig == 1)
		{
			//eye_vec2 = normalize(moved_pos_buf);
			eye_vec2 = glm::vec4(0.0f, 4.0f, 4.0f, 0.0f) * R2;
		}

		eye_vec2 = normalize(eye_vec2);

		cx = eye_vec2.x * 0.2;
		cy = eye_vec2.y * 0.2;
		cz = eye_vec2.z * 0.2;

		glm::mat4 trans(glm::vec4(1, 0, 0, cx), glm::vec4(0, 1, 0, cy), glm::vec4(0, 0, 1, cz), glm::vec4(0, 0, 0, 1));

		eye_vec = eye_vec * trans;

		up_vector = up_vector;

		matView = glm::lookAt(glm::vec3(eye_vec.x, eye_vec.y, eye_vec.z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(up_vector.x, up_vector.y, up_vector.z));

		//matView = glm::lookAt(glm::vec3(eye_vec.x, eye_vec.y, eye_vec.z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	}
}



#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GLU.h>
#include <cstring>
#include <stdlib.h>		  // srand, rand
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <iostream>
#include "math.h"
#include <vector>
#include "Matrix4.h"
#include <fstream>
#include <string>
#include <sstream>
#include "Vector3.h"
#include "ObjReader.h"
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>

const int width_window = 640;
const int height_window = 640;

int main(void)
{
	GLFWwindow *window = nullptr;

	/* Initialize the library */
	if (!glfwInit()) return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width_window, height_window, "Hello World", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	// callbacks here

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glClearColor(1, 1, 1, 1); // while background

	printf("%s\n", glGetString(GL_VERSION));

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glOrtho(-1.2, 1.2, -1.2, 1.2, -100.0, 100.0);
	//gluLookAt(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0, 1, 0);
	gluLookAt(0.1, 0.1, 0.1, 0.0, 0.0, 0.0, 0, 1, 0);

	// read 3D model from obj file
	OBJReader obj_reader;
	obj_reader.readObj("torus.obj");
	//obj_reader.readObj("../../test-result-sphere_f80.obj");
	//obj_reader.dump();

	// copy geometry to buffer object
	GLuint vbo[3];
	glGenBuffers(3, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, obj_reader.pos_stack_.size() * sizeof(float) * 3,
		&obj_reader.pos_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, obj_reader.pos_stack_.size() * sizeof(float) * 3,
		&obj_reader.pos_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj_reader.ix_stack_.size() * sizeof(unsigned int) * 3,
		&obj_reader.ix_stack_[0], GL_STATIC_DRAW);

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_NORMALIZE);
	//glShadeModel(GL_SMOOTH);
	//glDisable(GL_COLOR_MATERIAL);

	//GLfloat ambientLight[] = { 0.1, 0.1, 0.1, 1.0f };
	//GLfloat lightColor[] = { 100.0f, 100.0f, 100.0f, 1.0f };
	//GLfloat lightPos[] = { 0.0, 0.3, 0.0, 1};

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	//GLfloat materialColor[] = { 1.0f, 0.1f, 0.1f, 1.0f };
	//GLfloat materialSpecular[] = { 0.2, 0.2, 0.2, 1.0 };
	//GLfloat materialEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//GLfloat shininess = 5;

	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	//glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
	//glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// update object
		// move light source
		// move objects
		// update buffers

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // <- depth test

															// non-shader version
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
		glPolygonMode(GL_FRONT, GL_FILL);
		glDrawElements(GL_TRIANGLES, obj_reader.ix_stack_.size() * 3, GL_UNSIGNED_INT, 0);

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		// xyz axes
		//glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);

		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);

		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 1.0);
		glEnd();
		//glEnable(GL_LIGHTING);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	//glDeleteBuffersARB(3, vbo);

	glfwTerminate();

	return 0;
}

//// update object	
//glm::quat rot = glm::angleAxis(3.14f / 4.0f *0.1f, glm::vec3(1.0, 0.0, 0.0));
//glm::mat4 m_rot = glm::toMat4(rot);
//
//std::vector<glm::vec3> &vertices = obj_reader.pos_stack_;
//
//for (int vix = 0; vix < vertices.size(); vix++)
//{
//	glm::vec4 v4 = glm::vec4(vertices[vix], 1.0f);
//	v4 = m_rot * v4;
//	vertices[vix] = glm::vec3(v4);
//	//vertices[vix] = m_rot * glm::vec4(vertices[vix], 1.0f);
//}

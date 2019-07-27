#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GLU.h>
#include <cstring>
#include <stdlib.h>        // srand, rand
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
	window = glfwCreateWindow(width_window, height_window, "week9 2013112015", NULL, NULL);

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

	//std::string vertex_shader, fragment_shader;

	//{std::ifstream inFile;
	//inFile.open("vertex_shader.glsl");//open the input file
	//std::stringstream strStream;
	//strStream << inFile.rdbuf();//read the file
	//vertex_shader = strStream.str();//str holds the content of the file
	//std::cout << vertex_shader << std::endl;//you can do anything with the string!!!
	//}

	//{std::ifstream inFile;
	//inFile.open("fragment_shader.glsl");//open the input file
	//std::stringstream strStream;
	//strStream << inFile.rdbuf();//read the file
	//fragment_shader = strStream.str();//str holds the content of the file
	//std::cout << fragment_shader << std::endl;//you can do anything with the string!!!
	//}

	// initialize shader programs
	// http://antongerdelan.net/opengl/hellotriangle.html
	//GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	//char const * vertex_source_pointer = vertex_shader.c_str();
	//glShaderSource(vs, 1, &vertex_source_pointer, NULL);
	//glCompileShader(vs);
	//GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	//char const * fragment_source_pointer = fragment_shader.c_str();
	//glShaderSource(fs, 1, &fragment_source_pointer, NULL);
	//glCompileShader(fs);

	//GLuint shader_programme = glCreateProgram();
	//glAttachShader(shader_programme, fs);
	//glAttachShader(shader_programme, vs);
	//glBindAttribLocation(shader_programme, 0, "a_pos");
	//glBindAttribLocation(shader_programme, 1, "a_color");
	//glLinkProgram(shader_programme);

	//// Check the link status
	//GLint linked = 0;
	//glGetProgramiv(shader_programme, GL_LINK_STATUS, &linked);
	//if (!linked)
	//{
	//   GLint infoLen = 0;

	//   glGetProgramiv(shader_programme, GL_INFO_LOG_LENGTH, &infoLen);

	//   if (infoLen > 1)
	//   {
	//      char* infoLog = (char*)malloc(sizeof(char) * infoLen);

	//      glGetProgramInfoLog(shader_programme, infoLen, NULL, infoLog);
	//      printf("Error linking program:\n%s\n", infoLog);

	//      free(infoLog);
	//   }

	//   glDeleteProgram(shader_programme);
	//   return false;
	//}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glOrtho(-1.2, 1.2, -1.2, 1.2, -100.0, 100.0);
	//gluLookAt(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0, 1, 0);
	gluLookAt(0.1, 0.1, 0.1, 0.0, 0.0, 0.0, 0, 1, 0);

	// read 3D model from obj file
	OBJReader obj_reader1;
	OBJReader obj_reader2;
	obj_reader1.readObj("IronMan_Full_smooth.obj");
	obj_reader2.readObj("earth.obj");

	// copy geometry to buffer object
	GLuint vbo1[3];
	glGenBuffers(3, vbo1);

	GLuint vbo2[3];
	glGenBuffers(3, vbo2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo1[0]); //��ġ
	glBufferData(GL_ARRAY_BUFFER, obj_reader1.pos_stack_.size() * sizeof(float) * 3,
		&obj_reader1.pos_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo1[1]); //��
	glBufferData(GL_ARRAY_BUFFER, obj_reader1.nor_stack_.size() * sizeof(float) * 3,
		&obj_reader1.nor_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo1[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj_reader1.ix_stack_.size() * sizeof(unsigned int) * 3,
		&obj_reader1.ix_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]); //��ġ
	glBufferData(GL_ARRAY_BUFFER, obj_reader2.pos_stack_.size() * sizeof(float) * 3,
		&obj_reader2.pos_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]); //��
	glBufferData(GL_ARRAY_BUFFER, obj_reader2.nor_stack_.size() * sizeof(float) * 3,
		&obj_reader2.nor_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo2[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj_reader2.ix_stack_.size() * sizeof(unsigned int) * 3,
		&obj_reader2.ix_stack_[0], GL_STATIC_DRAW);

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); //GL_LIGHT0 ~ GL_LIGHT7
	//glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_COLOR_MATERIAL);

	GLfloat ambientLight[] = { 0.1, 0.1, 0.1, 1.0f }; //��ü�� ������ �ִ� ���� ����
	GLfloat lightColor[] = { 100.0f, 100.0f, 100.0f, 1.0f }; //���� ������ ������ �ִ� ����(������ ���� ������ ��������)
	GLfloat lightPos[] = { 0.0, 2.0, 0.0, 1}; //���� ��ġ ����

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight); //GPU�� ���� �־��ش�.
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor); //���� ���� �־��ش�
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, &lightPos[0]); //light �������� �־��ش�.

	// GL_POSITION : ��� �������� ���� ��� �ȴ�. ������ �ִ� ���� ��Ÿ�� ��
	// GL_SPOT_DIRECTION : ���� ��� �������� ������� �Ǵ� ���� ���� lightPos���� 0���� ���� �ָ��ִ� ���� ��Ÿ�� ��


	//GLfloat materialColor[] = { 1.0f, 0.1f, 0.1f, 1.0f };
	//GLfloat materialSpecular[] = { 0.2, 0.2, 0.2, 1.0 };
	//GLfloat materialEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f }; //��ü�� �߱��� �ϴ� ��ü�� �ִµ� �װ��� �����ϴ� �뵵, ���� ���� ����ϴ� �뵵�� lamp(���� ������ �߱�)
	//GLfloat shininess = 5; //�� ���̴��� alpha ���� �ش� ���� ��¦�̴� ����(?)


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// update object
		glm::quat rot = glm::angleAxis(3.14f / 4.0f * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 m_rot = glm::toMat4(rot);

		//lightPos = m_rot * lightPos;
		std::vector<glm::vec3>& vertices1 = obj_reader1.pos_stack_;
		std::vector<glm::vec3>& normals1 = obj_reader1.nor_stack_;

		std::vector<glm::vec3>& vertices2 = obj_reader2.pos_stack_;
		std::vector<glm::vec3>& normals2 = obj_reader2.nor_stack_;

		for (int i = 0; i < vertices1.size(); i++) {
		   vertices1[i] = m_rot * glm::vec4(vertices1[i], 1);
		   normals1[i] = m_rot * glm::vec4(normals1[i], 0); //�����̹Ƿ� 0����
		}

		for (int i = 0; i < vertices2.size(); i++) {
			vertices2[i] = m_rot * glm::vec4(vertices2[i], 1);
			normals2[i] = m_rot * glm::vec4(normals2[i], 0); //�����̹Ƿ� 0����
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo1[0]); //��ġ
		glBufferData(GL_ARRAY_BUFFER, obj_reader1.pos_stack_.size() * sizeof(float) * 3,
		   &obj_reader1.pos_stack_[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo1[1]); //��
		glBufferData(GL_ARRAY_BUFFER, obj_reader1.nor_stack_.size() * sizeof(float) * 3,
		   &obj_reader1.nor_stack_[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]); //��ġ
		glBufferData(GL_ARRAY_BUFFER, obj_reader2.pos_stack_.size() * sizeof(float) * 3,
			&obj_reader2.pos_stack_[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]); //��
		glBufferData(GL_ARRAY_BUFFER, obj_reader2.nor_stack_.size() * sizeof(float) * 3,
			&obj_reader2.nor_stack_[0], GL_STATIC_DRAW);

		glLightfv(GL_LIGHT0, GL_POSITION, &lightPos[0]);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // <- depth test

	   // non-shader version
		glPushMatrix();
		glTranslatef(-1, 0, 0);
		GLfloat mat_ambient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
		GLfloat mat_diffuse[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
		GLfloat mat_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
		GLfloat shininess = 51.2f;

		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse); //����� ������ �κ� FRONT ���� �κ� BACK �ڿ��͵� ���� ����ϰ��� �ϸ� GL_FRONT_AND_BACK�� ��� �ڿ��͸� ����Ѵٸ� GL_BACK
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);

		glBindBuffer(GL_ARRAY_BUFFER, vbo1[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo1[1]);
		glEnableClientState(GL_NORMAL_ARRAY);
		//glColorPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo1[2]);
		glPolygonMode(GL_FRONT, GL_FILL);
		glDrawElements(GL_TRIANGLES, obj_reader1.ix_stack_.size() * 3, GL_UNSIGNED_INT, 0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(1, 0, 0);
		GLfloat mat_ambient2[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
		GLfloat mat_diffuse2[] = { 0.07568f, 0.61424f, 0.07568f, 0.55f };
		GLfloat mat_specular2[] = { 0.633f, 0.727811f, 0.633f, 0.55f };
		GLfloat shininess2 = 76.8f;

		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2); //����� ������ �κ� FRONT ���� �κ� BACK �ڿ��͵� ���� ����ϰ��� �ϸ� GL_FRONT_AND_BACK�� ��� �ڿ��͸� ����Ѵٸ� GL_BACK
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
		glEnableClientState(GL_NORMAL_ARRAY);
		//glColorPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo2[2]);
		glPolygonMode(GL_FRONT, GL_FILL);
		glDrawElements(GL_TRIANGLES, obj_reader2.ix_stack_.size() * 3, GL_UNSIGNED_INT, 0);
		glPopMatrix();

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

		//shader version
		//glUseProgram(shader_programme);   // activate your shader!

		//// draw here
		//glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer
		//(
		//   0,                  // attribute 0
		//   3,                  // size (r, g, b)
		//   GL_FLOAT,           // type
		//   GL_FALSE,           // normalized?
		//   0,                  // stride
		//   (void*)0            // array buffer offset
		//);

		//glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer
		//(
		//   1,                  // attribute 0
		//   3,                  // size (r, g, b)
		//   GL_FLOAT,           // type
		//   GL_FALSE,           // normalized?
		//   0,                  // stride
		//   (void*)0            // array buffer offset
		//);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glDrawElements(GL_TRIANGLES, obj_reader.ix_stack_.size() * 3, GL_UNSIGNED_INT, 0);

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

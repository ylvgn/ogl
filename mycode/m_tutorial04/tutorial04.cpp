#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <common/shader.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// me
GLFWwindow* window;
#include <iostream>
#include <conio.h>
#include <time.h>
using namespace std;
bool _err_occur;

void init_glfw()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		_err_occur = true;
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void init_window() {
	window = glfwCreateWindow(1024, 768, "Playground", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		_err_occur = true;
		return;
	}
	glfwMakeContextCurrent(window); // init Context
}

void init_glew() {
	glewExperimental = true; // Needed for core profile, glew bug
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		_err_occur = true;
		return;
	}
	  
	// Ensure we can capture the keycode being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f); // Dark blue background
}

template <class T>
inline GLuint m_glBufferData(T& buffer) {
	GLuint res;
	glGenBuffers(1, &res);
	glBindBuffer(GL_ARRAY_BUFFER, res);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);
	return res;
}

int main( void )
{
	init_glfw();
	if (_err_occur) return _getch(), -1;
	init_window();
	if (_err_occur) return _getch(), -1;
	init_glew();
	if (_err_occur) return _getch(), -1;

	//VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	// extension is no need, or you can also add .txt or .glsl
	GLuint programID = LoadShaders("vertex.shader",
								   "fragment.shader"
	);

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	glm::mat4 P = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 V = glm::lookAt(vec3(4, 3, -3), vec3(0), vec3(0, 1, 0));
	glm::mat4 M = glm::mat4(1.0f);
	auto MVP = P * V * M;

	// Our vertices. 3 consecutive floats give a 3D vertex;
	// 3 consecutive vertices give 1 triangle --> 9 float = 1 triangle
	// A cube has 6 faces with 2 triangles each
	// so this makes 6*2=12 triangles, and 12*3 vertices = 12*3*3 floats
	// --> 12 * 3 * 3 = 12 triangle = 12 * 3 vertexs = 36 floats
	static const GLfloat g_vertex_buffer_data[] =
	{	
		// x    y     z
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		 1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};
	
	// One color(vec3) for each vertex --> sizeof(g_color_buffer_data) == sizeof(g_vertex_buffer_data)
	static GLfloat g_color_buffer_data[sizeof(g_vertex_buffer_data) * 3];
	srand(time(0));
	for (int i = 0; i < sizeof(g_vertex_buffer_data); i++) {
		g_color_buffer_data[3 * i + 0] = (float)(rand() % 100) / 100.0f; // [0~1] r
		g_color_buffer_data[3 * i + 1] = (float)(rand() % 100) / 100.0f; // g
		g_color_buffer_data[3 * i + 2] = (float)(rand() % 100) / 100.0f; // b
	}
	
	GLuint vertexbuffer = m_glBufferData(g_vertex_buffer_data);
	GLuint colorbuffer = m_glBufferData(g_color_buffer_data);

	// Z-Buffer
	glEnable(GL_DEPTH_TEST); // Enable depth test
	glDepthFunc(GL_LESS);    // Accept fragment if it closer to the camera than the former one

	// running
	while (true)
	{
		// On 'ESC' KeyCode Press
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
			|| glfwWindowShouldClose(window) != 0) break;

		// Z-Buffer: GL_DEPTH_BUFFER_BIT
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
		// send MVP
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		// Draw the triangle : glDrawArrays(GL_TRIANGLES, 0, sizeof(vertexs))
		glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertex_buffer_data)); // 12 * 3 vertexs -> 12 triangles
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}


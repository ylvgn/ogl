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

    // shader
	GLuint programID = LoadShaders("vertex.shader", "fragment.shader" );

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	mat4 Projection = glm::perspective(
		glm::radians(60.0f), // fovy
		4.0f / 3.0f,         // aspect
		0.1f,                // zNear
		100.0f               // zFar
	);
	mat4 View = glm::lookAt(
		vec3(0, 0, -10), // Camera pos in World Space
		vec3(0, 0, 0),   // and looks at the origin pos
		vec3(0, 1, 0)    // Head is up (set to 0,-1,0 to look upside-down)
	);
	mat4 Model = glm::mat4(1.0f);
	mat4 MVP = Projection * View * Model;
	
	// vertex data
	static const GLfloat g_vertex_buffer_data[] =
	{
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};
	GLuint vertexbuffer = m_glBufferData(g_vertex_buffer_data);

	// running
	while (true)
	{
		// On 'ESC' KeyCode Press
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
			|| glfwWindowShouldClose(window) != 0) break;

		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}


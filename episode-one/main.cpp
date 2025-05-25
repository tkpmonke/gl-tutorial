#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

static const char* vertex_shader = 
"#version 330 core\n"
"layout (location = 0) in vec3 in_pos;\n"
"void main() {\n"
"    gl_Position = vec4(in_pos, 1);\n"
"}\0";

static const char* fragment_shader =
"#version 330 core\n"
"out vec4 color;\n"
"void main() {\n"
"   color = vec4(0.2f, 0.7f, 0.3f, 1.0f);\n"
"}\0";

/// Check if the shader was compiled correctly
bool check_shader_compilation(unsigned int shader) {
	int success;
	char info_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		std::cout << "Compilation Failed\n" << info_log << "\n";
	}

	return success;
}

/// Compile the shader
unsigned int compile_shader(const int type, const char* value) {
    unsigned int shader = glCreateShader(type);
	 glShaderSource(shader, 1, &value, NULL);
	 glCompileShader(shader);

	 return check_shader_compilation(shader) ? shader : 0;
}

/// Assemble the Shaders into a Program Object
unsigned int create_program(unsigned int vertex, unsigned int fragment) {
	unsigned int program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	/// delete the shader objects since we don't need them anymore
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return program;
}

int main(void) {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Episode One", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewInit();

	unsigned int vertex = compile_shader(GL_VERTEX_SHADER, vertex_shader);
	unsigned int fragment = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);
	unsigned int program = create_program(vertex, fragment);

	while (glfwWindowShouldClose(window) == false) {
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::string loadShaderSourceFromFile(const std::string& filePath);
unsigned int compileShader(GLenum type, const char* source);

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;
const int INFO_LOG_SIZE = 512;

int main() {
	//-------------------- Initialization and render loop --------------------\\

	// Initialize & configure GLFW (version 3.3, core profile)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFW window object & handle error
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Assign current OpenGL context to window

	// Initialize GLAD & handle error
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Create OpenGL viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Assign GLFW resize callback to OpenGL resize callback

	//-------------------- Shader handling --------------------\\

	// Load vertex shader source code and handle errors
	std::string vertexShaderSourceString = loadShaderSourceFromFile("vshader.vert");
	if (vertexShaderSourceString.empty()) {
		glfwTerminate();
		return -1;
	}
	const char* vertexShaderSource = vertexShaderSourceString.c_str();

	// Load fragment shader source code and handle errors
	std::string fragmentShaderSourceString = loadShaderSourceFromFile("fshader.frag");
	if (fragmentShaderSourceString.empty()) {
		glfwTerminate();
		return -1;
	}
	const char* fragmentShaderSource = fragmentShaderSourceString.c_str();

	// Create an OpenGL vertex shader object and handle errors
	unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	if (vertexShader == 0) {
		glfwTerminate();
		return -1;
	}

	// Create an OpenGL fragment shader object and handle errors
	unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	if (fragmentShader == 0) {
		glDeleteShader(vertexShader); // Clean up successfully compiled vertex shader
		glfwTerminate();
		return -1;
	}

	// Create shader program
	unsigned int shaderProgram = glCreateProgram();

	// Attach compiled shaders to the shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram); // Link vertex and fragment shaders

	// Handle linking errors
	int success;
	char infoLog[INFO_LOG_SIZE];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // Query shader linking status to success variable
	if (!success) {
		glGetProgramInfoLog(shaderProgram, INFO_LOG_SIZE, NULL, infoLog); // Log error message to infoLog variable
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl; // Print error message
		// Clean resources
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(shaderProgram);
		glfwTerminate();
		return -1;
	}

	// Clean up shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//-------------------- Vertex handling --------------------\\

	// Define a single triangle as a set of normalized coordinates
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	// Generate a Vertex Array Object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	// Bind vertex array object
	glBindVertexArray(VAO); // Subsequent vertex options are bound to this VAO

	// Generate vertex buffer object
	unsigned int VBO;
	glGenBuffers(1, &VBO); // Generates one OpenGL buffer at the memory address of VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind VBO to the GL_ARRAY_BUFFER target (any buffer calls made to GL_ARRAY_BUFFER configures VBO)
	
	// Copy vertex data to VBO using bound GL_ARRAY_BUFFER and GL_STATIC_DRAW
		// GL_STATIC_DRAW: the data is set only once and used by the GPU many times
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Interpret vertex data
	/*	The position data is stored as 32-bit (4 byte) floating point values.
		Each position is composed of 3 of those values.
		There is no space (or other values) between each set of 3 values. The values are tightly packed in the array.
		The first value in the data is at the beginning of the buffer.
		glVertexAttribPointer(index, size, type, normalized, stride, pointer)
		*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Render loop (keeps GLFW running)
	while (!glfwWindowShouldClose(window)) {
		// Set window color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Use shader program to draw vertices
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Handle input
		processInput(window);

		// Check events
		glfwPollEvents();

		// Double buffer
		glfwSwapBuffers(window);
	}

	// Clean resources and exit program
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	glfwTerminate(); 
	return 0;
}

// OpenGL callback function on window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height); // Resize OpenGL viewport on window resize
}

// Handle input
void processInput(GLFWwindow* window) {
	// Close window by pressing escape
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// Load shader source file to a string
std::string loadShaderSourceFromFile(const std::string& filePath) {
	std::ifstream shaderFile;
	// Create exceptions for shaderFile
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		shaderFile.open(filePath);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		return shaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << "\n" << e.what() << std::endl;
		return "";
	}
}

// Compile shaders
unsigned int compileShader(GLenum type, const char* source) {
	// Create and compile shader
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	// Handle errors
	int success;
	char infoLog[INFO_LOG_SIZE];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // Query compile status to success variable
	if (!success) {
		glGetShaderInfoLog(shader, INFO_LOG_SIZE, NULL, infoLog); // Log error message to infoLog variable
		const char* shaderTypeStr = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT"; // Check if its a vertex or fragment shader error
		std::cerr << "ERROR::SHADER::" << shaderTypeStr << "::COMPILATION_FAILED\n" << infoLog << std::endl; // Print error message
		glDeleteShader(shader); // Clean resources
		return 0;
	}
	return shader;
}
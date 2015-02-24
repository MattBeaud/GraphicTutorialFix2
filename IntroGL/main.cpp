#include <GL/glew.h>
#include <GL/wglew.h>
#include <SOIL.h>

//GLEW includes must come before anything else
#include <vector>
#include <string>
#include <fstream>
#include <GLFW/glfw3.h>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ctime>
#include <iostream>


//Shader Sources BRUH
const GLchar* vertexSource =
"#version 330\n"
"in vec2 position;"
"in vec3 color;"
"in vec2 texcoord;"

"out vec3 Color;"
"out vec2 Texcoord;"

"uniform mat4 trans;"
//"uniform mat4 view;"
//"uniform mat4 proj;"


"void main() "
"{"
"	Texcoord = texcoord;"
"   Color = color;"
"	gl_Position = trans * vec4(position, 0.0, 1.0);"
"}";

const GLchar* fragmentSource =
"#version 330\n"
//"uniform vec3 triangleColor;"
"in vec3 Color;"
"in vec2 Texcoord;"
"out vec4 outColor;"
"uniform sampler2D texKitten;"
"uniform sampler2D texPuppy;"
"void main() "
"{"
"	outColor = texture(texKitten, Texcoord) * vec4(Color, 1.0);"
"}";

GLuint CreateShader(GLenum a_eShaderType, const char *a_strShaderFile)
{
	std::string strShaderCode;
	//open shader file
	std::ifstream shaderStream(a_strShaderFile);
	//if that worked ok, load file line by line
	if (shaderStream.is_open())
	{
		std::string Line = "";
		while (std::getline(shaderStream, Line))
		{
			strShaderCode += "\n" + Line;
		}
		shaderStream.close();
	}

	//convert to cstring
	char const *szShaderSourcePointer = strShaderCode.c_str();

	//create shader ID
	GLuint uiShader = glCreateShader(a_eShaderType);
	
	//load source code
	glShaderSource(uiShader, 1, &szShaderSourcePointer, NULL);
	
	//compile shader
	glCompileShader(uiShader);
	

	//Check for compiliation errors and output them
	GLint iStatus;
	glGetShaderiv(uiShader, GL_COMPILE_STATUS, &iStatus);
	if (iStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(uiShader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(uiShader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch (a_eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return uiShader;
}
GLuint CreateProgram(const char *a_vertex, const char *a_frag)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, a_vertex));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, a_frag));

	//create shader program ID
	GLuint uiProgram = glCreateProgram();

	//attach shaders
	for (auto shader = shaderList.begin(); shader != shaderList.end(); shader++)
		glAttachShader(uiProgram, *shader);

	//link program
	glLinkProgram(uiProgram);

	//check for link errors and output them
	GLint status;
	glGetProgramiv(uiProgram, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(uiProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(uiProgram, infoLogLength, NULL, strInfoLog);

		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for (auto shader = shaderList.begin(); shader != shaderList.end(); shader++)
	{
		glDetachShader(uiProgram, *shader);
		glDeleteShader(*shader);
	}

	return uiProgram;
}

//float vertices[] =
//{
//	0.0f, 0.5f,   1.0f, 0.0f, 0.0f, // Vertex 1 (X, Y) red
//	0.5f, -0.5f,  0.0f, 1.0f, 0.0f, // Vertex 2 (X, Y) green
//	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f // Vertex 3 (X, Y)  blue
//};
//float vertices[] =
//{
//	-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, //top-left
//	0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, //top-right
//	0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, //butt-right
//	-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f //bottom left

	//0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, //butt-right
	//-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, //butt-left
	//-0.5f,  0.5f, 1.0f, 0.0f, 0.0f  //top left
//};

float vertices[] = 
{
	//  Position   Color             Texcoords
	-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
	0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.1f, 0.0f,  // Top-right
	0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.1f, 0.33f, // Bottom-right
	-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.33f,// Bottom-left
};

int main()
{
	//Initilization of GLFW
	if (!glfwInit())
	{
		return -1;
	}

	//create a windowed mode winow and it's OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//make the window's context current
	glfwMakeContextCurrent(window);

	//.............................................................................
	//START GLEW BRUH
	if (glewInit() != GLEW_OK)
	{
		// OpenGL didn't start-up! shutdown GLFW and return an error code bruh
		glfwTerminate();
		return -1;
	}
	//..............................................................................
	//looooppppooop unitl user closes windooe

	// GENERATE BUFFERS
	GLuint vao;
	glGenVertexArrays(1, &vao);
	//Start using by bindiing;sehti
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	

	GLuint ebo;
	glGenBuffers(1, &ebo);

	GLuint elements[] =
	{
		0, 1, 2,
		2, 3, 0
	};
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// GENERATE SHADERS
//VERTEX
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	//TEST
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

	if (status == GL_TRUE)
	{
		printf("Vertex shader compiled successfully bruh\n");
		printf("I furted\n");
	}
	else if (status == GL_FALSE)
	{
		printf("Vertex shader error.\n");
	}
	char buffer[512];
	glGetShaderInfoLog(vertexShader, 512, NULL, buffer);

	printf(buffer);
//-------------------------------------------------------------
	//FRAGMENT
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	//TEST(SHIT)
	GLint SHIT;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &SHIT);

	if (SHIT == GL_TRUE)
	{
		printf("Shit's working fragment shader \n");
	}
	else if (SHIT == GL_FALSE)
	{
		printf("SHIT AINT REAL BRUH\n");

	}

	// SHADER PROGRAM
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	//glBindFragDataLocation(shaderProgram, 0, "outColor");
	//Linksshit
	glLinkProgram(shaderProgram);
	//start using shit
	glUseProgram(shaderProgram);

	GLint statusProgram;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &statusProgram);
	if (statusProgram == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, strInfoLog);

		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
//=================================================================

	// LOAD TEXTURE
	GLuint textures[2];
	glGenTextures(2, textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	int width, height;
	unsigned char* image =
		SOIL_load_image("Sonic.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
		image = SOIL_load_image("sample2.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Retrieving Postion
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	
	
	glEnableVertexAttribArray(posAttrib);
	glEnableVertexAttribArray(texAttrib);
	glEnableVertexAttribArray(colAttrib);

	
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));	



	//---------------------------------------------------------------------------------------------------------
	//3D tranforms stuffs
	GLint uniTrans = glGetUniformLocation(shaderProgram, "trans");

	//glm::mat4 view = glm::lookAt
	//	(
	//	glm::vec3(1.2f, 1.2f, 1.2f),
	//	glm::vec3(0.0f, 0.0f, 0.0f),
	//	glm::vec3(0.0f, 0.0f, 1.0f)
	//	);
	//GLint uniView = glGetUniformLocation(shaderProgram, "view");
	//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	//glm::mat4 proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 10.0f);
	//GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	//glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	
	glfwSetTime(0);
	float time = 0;


	while (!glfwWindowShouldClose(window))
	{
		time = glfwGetTime();
		if (time > 1.0f / 10.0f)
		{
			time -= 1.0f / 10.0f;

			// background color
			glClearColor(0.5f, 0.3f, 0.6f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Calculate transformation

			glm::mat4 trans;
			trans = glm::rotate(
				trans,
				(float)clock() / (float)CLOCKS_PER_SEC * glm::radians(0.0f), //Spin and rate
				glm::vec3(0.0f, 0.0f, 1.0f)
				);

			glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));

			vertices[5] += 0.1f;
			vertices[12] += 0.1f;
			vertices[19] += 0.1f;
			vertices[26] += 0.1f;

			if (vertices[5] == 1)
			{
				vertices[5] = 0.0f;
				vertices[12] = 0.1f;
				vertices[19] = 0.1f;
				vertices[26] = 0.0f;
			}


			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			//glDrawArrays(GL_TRIANGLES, 0, 6);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

			//swap front and back buffers
			glfwSwapBuffers(window);
		}
		//poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;

}
#include "shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(std::string vsPath, std::string fsPath) 
{
	shaderProgramID = loadShaders(vsPath.c_str(), fsPath.c_str());
}

Shader::Shader(std::string vsPath, std::string tcPath, std::string tePath, std::string fsPath)
{
	shaderProgramID = loadShaders(vsPath.c_str(), tcPath.c_str(), tePath.c_str(), fsPath.c_str());
}

GLint Shader::getUniformLocation(const std::string& name)
{
	if (uniformMap.find(name) == uniformMap.end())
	{
		GLuint location = glGetUniformLocation(shaderProgramID, name.c_str());
		uniformMap[name] = location;
	}
	return uniformMap[name];
}

void Shader::setUniform1i(const std::string& name, const int value)
{
	GLint location = getUniformLocation(name);
	glUniform1i(location, value);
}

void Shader::setUniform1f(const std::string& name, const float value)
{
	GLint location = getUniformLocation(name);
	glUniform1f(location, value);
}

void Shader::setUniform2f(const std::string& name, const glm::vec2& value)
{
	GLint location = getUniformLocation(name);
	glUniform2f(location, value.x, value.y);
}

void Shader::setUniform3f(const std::string& name, const glm::vec3& value)
{
	GLint location = getUniformLocation(name);
	glUniform3f(location, value.x, value.y, value.z);
}

void Shader::setUniform4f(const std::string& name, const glm::vec4& value)
{
	GLint location = getUniformLocation(name);
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::setUniformMat3(const std::string& name, const glm::mat3& value)
{
	GLint location = getUniformLocation(name);
	glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setUniformMat4(const std::string& name, const glm::mat4& value)
{
	GLint location = getUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

GLuint Shader::loadShader(const char* path, GLenum shaderType)
{
	GLint result = GL_FALSE;
	int infoLogLength;

	GLuint shaderID = glCreateShader(shaderType);

	std::string code;
	std::ifstream vsStream(path, std::ios::in);
	if (vsStream.is_open())
	{
		std::stringstream sstr;
		sstr << vsStream.rdbuf();
		code = sstr.str();
		vsStream.close();
	}

	std::cout << "Compiling shader: " << path << std::endl;
	char const* pointer = code.c_str();
	glShaderSource(shaderID, 1, &pointer, nullptr);
	glCompileShader(shaderID);

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> errorMessage(infoLogLength + 1);
		glGetShaderInfoLog(shaderID, infoLogLength, nullptr, &errorMessage[0]);
		printf("%s\n", &errorMessage[0]);
	}

	return shaderID;
}

GLuint Shader::linkProgram(std::vector<GLuint> shaders)
{
	GLint result = GL_FALSE;
	int infoLogLength;

	std::cout << "Linking program" << std::endl;
	GLuint programID = glCreateProgram();

	for (GLuint shaderID : shaders)
	{
		glAttachShader(programID, shaderID);
	}
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	for (GLuint shaderID : shaders)
	{
		glDetachShader(programID, shaderID);
		glDeleteShader(shaderID);
	}

	return programID;
}

GLuint Shader::loadShaders(const char* vsPath, const char* fsPath)
{
	GLuint vertexShaderID = loadShader(vsPath, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = loadShader(fsPath, GL_FRAGMENT_SHADER);

	std::vector<GLuint> shaders = { vertexShaderID, fragmentShaderID };

	return linkProgram(shaders);
}

GLuint Shader::loadShaders(const char* vsPath, const char* tcPath, const char* tePath, const char* fsPath)
{
	GLuint vertexShaderID = loadShader(vsPath, GL_VERTEX_SHADER);
	GLuint controlShaderID = loadShader(tcPath, GL_TESS_CONTROL_SHADER);
	GLuint evaluationShaderID = loadShader(tePath, GL_TESS_EVALUATION_SHADER);
	GLuint fragmentShaderID = loadShader(fsPath, GL_FRAGMENT_SHADER);

	std::vector<GLuint> shaders = { vertexShaderID, controlShaderID, evaluationShaderID, fragmentShaderID };

	return linkProgram(shaders);
}
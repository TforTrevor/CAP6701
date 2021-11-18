#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <unordered_map>

class Shader
{
public:
	Shader(std::string vsPath, std::string tcPath);
	Shader(std::string vsPath, std::string tcPath, std::string tePath, std::string fsPath);

	void bind() { glUseProgram(shaderProgramID); }
	void unbind() { glUseProgram(0); }
	GLint getUniformLocation(const std::string& name);

	void setUniform1i(const std::string& name, const int value);
	void setUniform1f(const std::string& name, const float value);
	void setUniform2f(const std::string& name, const glm::vec2& value);
	void setUniform3f(const std::string& name, const glm::vec3& value);
	void setUniform4f(const std::string& name, const glm::vec4& value);
	void setUniformMat3(const std::string& name, const glm::mat3& value);
	void setUniformMat4(const std::string& name, const glm::mat4& value);

private:
	GLuint shaderProgramID;
	std::unordered_map<std::string, GLint> uniformMap;

	GLuint loadShader(const char* path, GLenum shaderType);
	GLuint linkProgram(std::vector<GLuint> shaders);
	GLuint loadShaders(const char* vsPath, const char* fsPath);
	GLuint loadShaders(const char* vsPath, const char* tcPath, const char* tePath, const char* fsPath);
};
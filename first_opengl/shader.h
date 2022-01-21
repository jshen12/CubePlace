#ifndef SHADER_H_
#define SHADER_H_

#include <glad.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

class Shader {
public:
	unsigned int ID; // program ID

	// arguments: paths to vertex and fragment shader code
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		// read shader code from filepath
		std::string vertexCode, fragmentCode;
		std::ifstream vShaderFile, fShaderFile;
		// check exception handling
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read buffer content to streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream to string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch(std::ifstream::failure e)
		{
			printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// compile shaders
		GLuint vertex, fragment;
		int success;
		char infoLog[512];
		// vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED %s\n", infoLog);
		}
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED %s\n", infoLog);
		}
		// shader
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		// check linking errors
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		// delete shaders
		glDeleteShader(vertex);
		glDeleteShader(fragment);

	}
	void use()
	{
		glUseProgram(ID);
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
};


#endif SHADER_H_

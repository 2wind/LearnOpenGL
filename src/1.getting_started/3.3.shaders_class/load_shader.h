
// load_shader.h
// 셰이더를 불러오는 코드를 분리했습니다. 다른 예제에서는 
// #include <learnopengl/shader_s.h> 을 활용해주세요.

// https://heinleinsgame.tistory.com/8?category=757483

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // 필요한 모든 OpenGL의 헤더파일을 가져오기 위해 glad를 포함합니다.
  
#include <string>
#include <fstream> // file stream으로 불러온다
#include <sstream> 
#include <iostream>
  
class Shader
{
public:
    // program ID
    unsigned int ID;
  
    // 생성자는 shader를 읽고 생성합니다.
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath){
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();

        }
        catch(std::ifstream::failure& e){
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }
        const char * vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();

        // 버텍스 셰이더
        // 비슷하게 uint ID를 정의하고, vertex_shader를 만들어 할당한다.
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        // 그 id에 할당될 source를 가져온다. 
        glShaderSource(vertexShader, 1, &vShaderCode, NULL);
        glCompileShader(vertexShader);

        // 컴파일 성공 여부를 확인한다.
        int  success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // 프래그먼트 셰이더
        // 비슷하게 uint ID를 정의하고, fragment_shader를 만들어 할당한다.
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        // 그 id에 할당될 source를 가져온다. 
        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShader);

        // 컴파일 성공 여부를 확인한다.
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGEMNT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Shader Program
        // 프로그래머가 정의한 셰이더를 결합해준다. 링크를 해 주고, 이 shader program을 활성화하면 된다.
        
        ID = glCreateProgram();

        // 우리가 컴파일한 셰이더들을 붙여준다.
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);
        
        // 성패여부 확인
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;

        }
        // 연결한 셰이더 객체는 제거해야 한다.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

    }
    // shader를 활성화하고 사용합니다.
    void use(){
        // 이제 프로그램이 나왔다. 활성화하자.
        glUseProgram(ID);

    }
    // Uniform 유틸리티 함수들
    void setBool(const std::string &name, bool value) const{
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
    }  
    void setInt(const std::string &name, int value){
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);

    }  
    void setFloat(const std::string &name, float value) const{
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
};


  
#endif
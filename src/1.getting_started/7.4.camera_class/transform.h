
#include <glad/glad.h> // 필요한 모든 OpenGL의 헤더파일을 가져오기 위해 glad를 포함합니다.

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

class Transform{
    public:
        glm::mat4 T;
        glm::mat4 R; 
        glm::mat4 Sh;
        glm::mat4 S;
        glm::vec3 pivot;

        Transform(){
            T = glm::mat4(1.0f);
            R = glm::mat4(1.0f); 
            Sh= glm::mat4(1.0f);
            S = glm::mat4(1.0f);
            pivot = glm::vec3(0.0f);
        }

        Transform(glm::mat4 t, glm::mat4 r, glm::mat4 sh, glm::mat4 s, glm::vec3 p){
            T = t;
            R = r;
            Sh= sh;
            S = s;
            pivot = p;
        }
        
                
        glm::mat4 GetMatrix(){
            return glm::translate(T, -pivot) * glm::translate(glm::mat4(1.0f), pivot) * R * glm::translate(glm::mat4(1.0f), -pivot)* Sh * S;
        }

        glm::mat4 GetRawTranslation(){
            return T;
        }        
        glm::mat4 GetTranslation(){
            return glm::translate(T, -pivot);
        }


};


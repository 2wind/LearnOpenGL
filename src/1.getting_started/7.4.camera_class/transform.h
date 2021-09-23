
#include <glad/glad.h> // 필요한 모든 OpenGL의 헤더파일을 가져오기 위해 glad를 포함합니다.
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

class Transform{
    public:

        Transform * parent;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        glm::vec3 pivot;

        Transform(){
            parent = NULL;
            position = glm::vec3(0.0f);
            rotation = glm::quat(glm::vec3(0.0f));
            scale = glm::vec3(1.0f, 1.0f, 1.0f);
            pivot = glm::vec3(0.0f);
        }
                
        glm::mat4 GetMatrix(){
            glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
            glm::mat4 RS = glm::translate(glm::toMat4(rotation) * glm::translate(S, -pivot), pivot);
            glm::mat4 TRS = glm::translate(RS, position - pivot);

            return TRS;

        }

        void SetParent(Transform & t){
            parent = &t;
        }

        glm::mat4 GetRawTranslation(){
            return glm::translate(glm::mat4(1.0f), position);
        }        
        glm::mat4 GetTranslation(){
            return glm::translate(GetRawTranslation(), -pivot);
        }


};

glm::mat4 get_world_matrix(Transform transform){
    glm::mat4 local = transform.GetMatrix();
    glm::mat4 world = local;

    while (transform.parent != NULL){
        glm::mat4 parent = transform.parent->GetMatrix();
        world = world * parent;
        transform = *transform.parent;
    }


    return world;
}


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

        glm::vec2 shear_x;
        glm::vec2 shear_y;
        glm::vec2 shear_z;

        Transform(){
            parent = NULL;
            position = glm::vec3(0.0f);
            rotation = glm::quat(glm::vec3(0.0f));
            scale = glm::vec3(1.0f, 1.0f, 1.0f);
            pivot = glm::vec3(0.0f);

            shear_x = glm::vec2(0.0f);
            shear_y = glm::vec2(0.0f);
            shear_z = glm::vec2(0.0f);
        }

                
        glm::mat4 GetMatrix(){
            glm::mat4 S = GetShearMatrix() * glm::scale(glm::mat4(1.0f), scale);
            glm::mat4 RS = glm::translate(glm::toMat4(rotation) * glm::translate(S, -pivot), pivot);
            glm::mat4 TRS = glm::translate(RS, position);

            return TRS;

        }
        glm::mat4 GetMatrixWithoutShear(){
            glm::mat4 S =  glm::scale(glm::mat4(1.0f), scale);
            glm::mat4 RS = glm::translate(glm::toMat4(rotation) * glm::translate(S, -pivot), pivot);
            glm::mat4 TRS = glm::translate(RS, position);

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

    private:
        glm::mat4 GetShearMatrix(){
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix[0][1] = shear_x[0];
            matrix[0][2] = shear_x[1];
            matrix[1][0] = shear_y[0];
            matrix[1][2] = shear_y[1];
            matrix[2][0] = shear_z[0];
            matrix[2][1] = shear_z[1];
            return matrix;
        }
};

glm::mat4 get_world_matrix(Transform transform){
    glm::mat4 local = transform.GetMatrix();
    glm::mat4 world = local;
    Transform * iter = transform.parent;

    while (iter != NULL){
        glm::mat4 parent = iter->GetMatrixWithoutShear();
        world = world * parent;
        iter = iter->parent;
    }


    return world;
}

glm::vec3 get_world_position(Transform transform){
    glm::vec3 worldPosition = transform.position;
    Transform * iter = transform.parent;
    while (iter != NULL){
        worldPosition = worldPosition * iter->scale;
        worldPosition = worldPosition * iter->rotation;
        worldPosition += iter->position;

        iter = iter->parent;
    }
    return worldPosition;
}


#include <glad/glad.h> // 필요한 모든 OpenGL의 헤더파일을 가져오기 위해 glad를 포함합니다.
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

class Transform{
    public:

        Transform * parent;
        std::vector<Transform> children;
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
            rotation = glm::identity<glm::quat>();
            scale = glm::vec3(1.0f, 1.0f, 1.0f);
            pivot = glm::vec3(0.0f);

            shear_x = glm::vec2(0.0f);
            shear_y = glm::vec2(0.0f);
            shear_z = glm::vec2(0.0f);
        }

                
        glm::mat4 GetMatrix(){
            return GetMatrixWithoutShear() * GetShearMatrix();
        }


        glm::mat4 GetMatrixWithoutShear(){
            glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
            glm::mat4 TR = glm::translate(glm::translate(T, pivot) * glm::toMat4(rotation), -pivot);
            glm::mat4 TRS = glm::scale(TR, scale);

            return TRS;
        }

        void SetParent(Transform & t){
            parent = &t;
            t.children.push_back(*this);
        }

        glm::mat4 GetRawTranslation(){
            return glm::translate(glm::mat4(1.0f), position);
        }        
        glm::mat4 GetTranslation(){
            return glm::translate(GetRawTranslation(), -pivot);
        }

        glm::mat4 GetWorldMatrix(){
            glm::mat4 world = GetMatrix();
            Transform * iter = parent;
            int count = 0;
            while (iter != NULL){
                glm::mat4 parentM = iter->GetMatrixWithoutShear();
                world = parentM * world;
                iter = iter->parent;
                count++;
            }
            std::cout << count << std::endl;
            return world;
        }

        glm::quat GetWorldRotation(){
            glm::quat worldRotation = rotation;
            Transform * parent = parent;
            
            while (parent != NULL){
                worldRotation = parent->rotation * worldRotation;
                parent = parent->parent;
            }
            return worldRotation;
        }

        glm::vec3 GetWorldPosition(){
            glm::vec3 worldPosition = position;
            Transform * parent = parent;
            while (parent != NULL){
                worldPosition = worldPosition * parent->scale;
                worldPosition = worldPosition * parent->rotation;
                worldPosition += parent->position;

                parent = parent->parent;
            }
            return worldPosition;
        }

        void SetWorldRotation(glm::quat rot){
            if (parent == NULL){
                rotation = rot;
                return;
            }
            glm::quat parentWorld = parent->GetWorldRotation();
            glm::quat inv = glm::inverse(parentWorld);
            rotation = inv * rot;
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

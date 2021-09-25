#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <glm/gtx/string_cast.hpp>

// Learn OpenGL 7 Camera
// https://heinleinsgame.tistory.com/12?category=757483

#include <iostream>
#include <vector>
#include <learnopengl/shader_m.h>

#include "transform.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow * window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 cameraPos   = glm::vec3(0.0f, 1.5f,  sqrt(3.0));
float down_angle = 30.0f;
glm::vec3 cameraFront = glm::vec3(0.0f, -sin(glm::radians(down_angle)), -cos(glm::radians(down_angle)));
glm::vec3 cameraUp    = glm::vec3(0.0f,  cos(glm::radians(down_angle)), -sin(glm::radians(down_angle)));

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float yaw = 0.0f;
float pitch = 0.0f;

bool firstMouse = true;

float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;

float fov = 45.0f;
// 버텍스 셰이더와 프래그먼트 셰이더를 CPP 내부에 포함시킬 수 있는 방법.
// 더 좋은 방법이 있을까...
// 있다. 

GLFWwindow* initialize(){
    // ======================== 초기화 부분 =============================
    glfwInit(); // GLFW 초기화
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3으로 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // OpenGL Core mode
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn OpenGL", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window); // 우리 윈도우의 컨텍스트를 현재 스레드의 주 컨텍스트로 지정하겠다고 알리기

    // 마우스 잠금
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD를 초기화해야 한다.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }    
    // Viewport 설정
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); 
    // 창 크기 변경시 불릴 콜백 함수 등록
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 커서 및 스크롤 콜백 함수 등록
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);

    // depth buffer 활성화
    glEnable(GL_DEPTH_TEST);  

    return window;
}

unsigned int load_texture(const char * location, GLenum format, GLint x_wrap, GLint y_wrap, GLint min_filter, GLint mag_filter){
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, x_wrap);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, y_wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);


    // 텍스처 이미지를 불러온다.
    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // 파일 경로, 너비, 높이, 컬러 채널 수를 지정한다.
    unsigned char * data = stbi_load(location, &width, &height, &nrChannels, 0);

    if (data)
    {
        // 위의 data를 2D 텍스처로 할당한다.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // 밉맵을 생성한다.
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    //이제 이미지 데이터는 free를 해준다.
    stbi_image_free(data);

    return texture;

}

int main(){

    GLFWwindow * window = initialize();


    // 새로 만든 셰이더 클래스를 이용해 셰이더를 텍스트 파일에서 불러오고 초기화한다.
    Shader ourShader("7.1.camera.vs", "7.1.camera.fs");
    Shader simpleShader("shader.vs", "shader.fs");
    // ==================== 초기화 부분 끝 ===========================


    // cube vertices.
        float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    // world space positions of our cubes

    // 동일한 큐브에 대해서, model matrix를 다르게 해주는 것으로 위치를 다르게 해줄 수 있다.
    // 조금 더 생각해보면, matrix의 "stack"에서 특정 height까지만 곱하는 것으로 matrix stack에 해당하는
    // model space를 구현할 수 있지 않을까.
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    const int NUM_VERTICES_CIRCLE = 16;
    float radius = 0.5f;
    float height = 1.0f;
    std::vector<float> cylinder;
    for (size_t i = 0; i < NUM_VERTICES_CIRCLE; i++)
    {
        float theta = i * glm::two_pi<float>() / NUM_VERTICES_CIRCLE;

        // upper circle
        cylinder.push_back(radius * cos(theta)); // x
        cylinder.push_back(radius * sin(theta)); // y
        cylinder.push_back(height / 2);          // z

        // lower circle
        cylinder.push_back(radius * cos(theta)); // x
        cylinder.push_back(radius * sin(theta)); // y
        cylinder.push_back(-height / 2);         // z
    }

    std::vector<unsigned int> cylinder_indices;
    // indices for side
    for (size_t i = 0; i < NUM_VERTICES_CIRCLE * 2; i++)
    {
        int first  =  i;
        int second = (i + 1) % (NUM_VERTICES_CIRCLE * 2);
        int third  = (i + 2) % (NUM_VERTICES_CIRCLE * 2);

        cylinder_indices.push_back(first);
        cylinder_indices.push_back(second);
        cylinder_indices.push_back(third);
    }
    for (size_t i = 1; i < NUM_VERTICES_CIRCLE - 1; i++){
        int first = 0;
        int second = i * 2;
        int third = (i + 1) * 2;

        cylinder_indices.push_back(first);
        cylinder_indices.push_back(second);
        cylinder_indices.push_back(third);
    }
    for (size_t i = 1; i < NUM_VERTICES_CIRCLE - 1; i++){
        int first = 1;
        int second = (i * 2) + 1;
        int third = (i + 1) * 2 + 1;

        cylinder_indices.push_back(first);
        cylinder_indices.push_back(second);
        cylinder_indices.push_back(third);
    }
    
    // cylinder.data()로 데이터를 부를 수 있다. 여기에 텍스쳐는 없으므로 그냥 단색으로 나오게 만들던가 하자.
    // [x0, y0, z0, x1, y1, z1, ... ] (3 * NUM_VERTICES_CIRCLE 길이, stride 3 * sizeof(float))
    


    /* 좌표 변환의 해석
     * 여기에 있는 좌표는 local space라고 할 수 있다.
     * 이 좌표들을 model matrix를 이용해 world space로 변환한다.
     * 그 뒤 view matrix를 적용하면 카메라의 시점인 view space로 바뀌게 된다.
     * Projection matrix에 의해 Clip space로 투영되고, 마지막으로
     * Viewport 변환에 의해 screen space로 바뀐다.
     * 즉 Vfinal = Vp * P * V * M * Vlocal
     * 
     */


    // =================== 정점 데이터 해석 ==================

    // Vertex buffer object를 이용해 정점들의 데이터를 정리할 수 있다. 
    unsigned int VBO, VAO;

    // 버퍼 id를 생성한다. 
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    /* 이제 저장된 데이터를 어떻게 해석해야 하는지 OpenGL에 알려주자.
     * 0 : 설정한 vertex 속성. 입력 변수의 location을 0으로 정의했다. 
     * 3 : vertex 속성의 크기. vec3 타입이므로 크기는 3이다.
     * GL_FLOAT : vec3이 float이므로.
     * GL_FALSE : 데이터의 정규화 여부. 우리는 그대로 쓰면 되므로 False로 한다.
     * 3*sizeof(float): 데이터와 데이터 사이의 stride. 3 float만큼이므로.
     * (tightly packed이므로 0을 주어도 OpenGL이 잘 추측한다.)
     * (void*) 0: 버퍼에서 데이터가 시작하는 지점. offset = 0이다.(지금은)
     * 우리는 vertex 정보에 대한 3개 location을 정의해야 한다.
    */ 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // 원기둥 부분

    unsigned int cylinderVBO, cylinderVAO;
    unsigned int cylinderEBO;


    // 버퍼 id를 생성한다. 
    glGenVertexArrays(1, &cylinderVAO);
    glGenBuffers(1, &cylinderVBO);
    glGenBuffers(1, &cylinderEBO);

    glBindVertexArray(cylinderVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cylinder.size(), cylinder.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * cylinder_indices.size(), cylinder_indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    // ==================== VAO, VBO 끝 ==============================
    // ============= 텍스처 해석 ===============

    // 텍스처도 id로 관리된다.
    unsigned int texture1, texture2;

    texture1 = load_texture( "../../resources/textures/container.jpg", GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);

    // 두번째 텍스처 로딩
    texture2 = load_texture("../../resources/textures/awesomeface.png", GL_RGBA, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);


    // Engine 준비

    // 우리가 만든 셰이더를 활성화하고,
    ourShader.use();
    // 텍스처 바인딩을 해 준다.
    ourShader.setInt("texture1", 0); // shader class 이용.
    ourShader.setInt("texture2", 1); // shader class 이용.


    simpleShader.use();
    simpleShader.setVec3("baseColor", glm::vec3(0.5f, 0.6f, 0.2f));


    std::vector<Transform> transforms;


    const int NUM_BOXES = 12;
    const float angle = glm::two_pi<float>() / NUM_BOXES;

    // SET initial transforms
    for (int i = 0; i < NUM_BOXES + 1; i++){
        Transform transform = Transform();
        transforms.push_back(transform);

    }
    transforms[0].scale = glm::vec3(0.1f, 0.1f, 0.1f);
    // transforms[6].scale = glm::vec3(0.2f, 2.0f, 1.0f);
    for (int i = 1; i < NUM_BOXES + 1; i++){
        transforms[i].SetParent(transforms[i-1]);
    }

    
    
    // 메인 루프
    // 창을 닫아야 할 필요가 없을 동안...
    while(!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);       // 입력을 처리하고, (여기서 원하는 다른 함수들을 넣을 수 있다)
                                    // 이 자리에 렌더링 코드를 넣을 수 있다.
                                    
        glClearColor(0.2f, 0.3f, 0.3f, 0.6f);   // 1. 짙은 청녹색(RGBA값)으로 설정된 값으로,
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);           // 컬러 버퍼를 지운다. 깊이 버퍼도 지운다.

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);


        // 변환 행렬 준비


        
        // perform Transform in LOCAL SPACE
        // DO NOT COMBINE WITH OTHERS AT HERE.

        glm::quat root_rotation = transforms[0].rotation;


        if (lastFrame < 3.0f){
            for (int i = 1; i < NUM_BOXES + 1; i++){
                Transform current = transforms[i];

                glm::vec3 pos = current.position + glm::vec3(0.0f, deltaTime / 12, 0.0f);
                if (i == 1){
                    pos += glm::vec3(0.0f, deltaTime / 6, 0.0f);
                }
                
                current.position = pos;

                // // slerp between angle 0 and desginated ANGLE
                // glm::quat rot = glm::slerp(glm::quat(glm::vec3(0.0f)),
                //                          glm::quat(glm::vec3(0.0f, angle, 0.0f)), lastFrame / 3);             
                
                // current.rotation = rot;

                // copy back to original vector
                transforms[i] = current;
            } 
        }

        else if (lastFrame < 9.0f)
        {
            transforms[0].position -= glm::vec3(0.0f, deltaTime/24, 0.0f);
            transforms[1].rotation = glm::slerp(glm::quat(glm::vec3(0.0f)),
                                         glm::quat(glm::vec3(0.0f, 0.0f, -(glm::half_pi<float>() + angle))), (lastFrame-3 )/ 6);        
            int i = ((static_cast<int>((lastFrame - 3) * 2)));
            if (i > 1 and i < 13){
                Transform current = transforms[i];

                current.position -= glm::vec3(0.0f, deltaTime / 12, 0.0f);

                // slerp between angle 0 and desginated ANGLE
                glm::quat rot = glm::slerp(glm::quat(glm::vec3(0.0f)),
                                         glm::quat(glm::vec3(0.0f, 0.0f, -angle)), ((lastFrame-3.0f)*2 - i));             
                
                current.rotation = rot;

                // if (i > 0) current.pivot = glm::vec3(0.0f, -0.5f, 0.0f);

                transforms[i] = current;
            }



        }
        else if (lastFrame < 21.0f){
            transforms[0].pivot = -transforms[0].position;
            transforms[0].rotation = glm::slerp(glm::quat(glm::vec3(0.0f)),
                                         glm::quat(glm::vec3(0.0f, 0.0f, -glm::half_pi<float>())), (lastFrame-9 )/ 3);
        }
        // else if (lastFrame < 21.0f){
        //     transforms[0].pivot = glm::vec3(0.0f, 0.5f, 0.0f);
        //     transforms[0].rotation = glm::slerp(glm::quat(glm::vec3(0.0f, 0.0f, -glm::half_pi<float>())),
        //                                  glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), (lastFrame-15 )/ 3);
        // }
        // else if (lastFrame < 15.0f){
        //     for (int i = 0; i < NUM_BOXES; i++){
        //         Transform current = transforms[i];
        //         // current.pivot = glm::vec3(0.5f, 0.5f, 0.0f);

        //         // slerp between angle 0 and desginated ANGLE
        //         glm::quat rot = glm::slerp(glm::quat(glm::vec3(0.0f, 0.0f, -angle)),
        //                                  glm::quat(glm::vec3(0.0f, 0.0f, glm::two_pi<float>()- 2* angle)), (lastFrame-6.0f) / 6);             
                
        //         current.rotation = rot;

        //         // if (i > 0) current.pivot = glm::vec3(0.0f, -0.5f, 0.0f);

        //         transforms[i] = current;
        //     } 
        // }
        // else if (lastFrame < 9.5f){
        //     for (size_t i = 0; i < NUM_BOXES; i++)
        //     {
        //         Transform current = transforms[i];
        //         current.pivot = current.position - current.GetWorldPosition();
        //         // glm::quat rot = glm::slerp(glm::quat(glm::vec3(0.0f, 0.0f, glm::pi<float>()/4)), 
        //         //             glm::quat(glm::vec3(glm::pi<float>()/4, 0.0f, glm::pi<float>()/4)), // x axis 45 degree
        //         //             (lastFrame-9.0f) / 3);    
        //         // current.rotation = rot;

        //         transforms[i] = current;
        //     }
        //     root_rotation = transforms[0].rotation;
            
        // }
        // else if (lastFrame < 12.5f){
        //     for (int i = 1; i < NUM_BOXES; i++){
        //         Transform current = transforms[i];

        //         glm::vec3 pos = current.position - glm::vec3(0.0f, deltaTime / 20, 0.0f);
                
        //         current.position = pos;


        //         // copy back to original vector
        //         transforms[i] = current;
        //     } 
        // }
        

            

        ourShader.use();

        // view matrix

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);

        // Projection matrix
        glm::mat4 proj = glm::mat4(1.0f);
        // Orthographic Projection
        // glm::ortho(0.0f, SCR_WIDTH, 0.0f, SCR_HEIGHT,0.1f, 100.0f);
        // Perspective Projection
        // FoV, Aspect ratio, near clip, far clip.
        proj = glm::perspective(glm::radians(fov), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);

        // unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        ourShader.setMat4("projection", proj); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        


        //std::cout << glm::to_string(transforms[2].R) << std::endl;

        // render boxes
        glBindVertexArray(VAO);

        // COMBINING MATRICES WITH PARENT
        // (SO THAT COMPLEX MATRICES CAN BE BUILT)
        for (unsigned int i = 1; i < NUM_BOXES - 5; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = transforms[i].GetWorldMatrix();
            
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }




        // render cylinder
        simpleShader.use();
        glBindVertexArray(cylinderVAO); // Changing VAO on the fly is expensive: consider using single VAO.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);

        simpleShader.setMat4("view", view);
        simpleShader.setMat4("projection", proj);
        // COMBINING MATRICES WITH PARENT
        // (SO THAT COMPLEX MATRICES CAN BE BUILT)
        for (unsigned int i = 7; i < NUM_BOXES+1; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = transforms[i].GetWorldMatrix(); // Calculating model matrix in top-down manner will save calculation.
            
            simpleShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, cylinder_indices.size(), GL_UNSIGNED_INT, 0);
        }



        glfwSwapBuffers(window);    // 버퍼를 교체하고,
        glfwPollEvents();           // 이벤트를 폴링한다.
    }   

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);



    // 루프가 끝나면 glfw를 정리한다.
    glfwTerminate();


    return 0;

}

void processInput(GLFWwindow * window){
    float cameraSpeed = 2.5f * deltaTime;
    // 만약 window에서 ESCAPE 키가 "눌렸을" 경우
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        // window should close 플래그를 True로 만든다.
        // 그러면 while loop가 종료된다.
        glfwSetWindowShouldClose(window, true);
    }
    // 앞이나 뒤로 가는 것은 Position에 CameraFront 방향을 곱해서 더해주면 된다.
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    // 좌우로 가는 것은 Front와 Up 방향을 외적한 다음(Right 방향이 된다) normalize를 하고 속도를 곱해 더해주면 된다.
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// 프레임버퍼의 크기라 달라진 것을 Viewport에 적용하는 함수
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // y 좌표의 범위는 밑에서부터 위로가기 때문에 반대로 바꿉니다.
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;  

    if(pitch > 89.0f)
        pitch =  89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    cameraFront = glm::normalize(front);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    if(fov >= 1.0f && fov <= 45.0f)
  	    fov -= yoffset;
    if(fov <= 1.0f)
  	    fov = 1.0f;
    if(fov >= 45.0f)
  	    fov = 45.0f;
}
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
// Learn OpenGL 4 Texture
// https://heinleinsgame.tistory.com/9?category=757483

#include <iostream>

#include <learnopengl/shader_s.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow * window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 버텍스 셰이더와 프래그먼트 셰이더를 CPP 내부에 포함시킬 수 있는 방법.
// 더 좋은 방법이 있을까...
// 있다. 

int main(){

    // ======================== 초기화 부분 =============================
    glfwInit(); // GLFW 초기화
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3으로 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // OpenGL Core mode
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn OpenGL", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // 우리 윈도우의 컨텍스트를 현재 스레드의 주 컨텍스트로 지정하겠다고 알리기
    // GLAD를 초기화해야 한다.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    
    // Viewport 설정
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); 
    // 창 크기 변경시 불릴 콜백 함수 등록
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 새로 만든 셰이더 클래스를 이용해 셰이더를 텍스트 파일에서 불러오고 초기화한다.
    Shader ourShader("../../src/1.getting_started/4.1.textures/shader.vs", "../../src/1.getting_started/4.3.textures_exercise1/shader.fs");
    // ==================== 초기화 부분 끝 ===========================

    // 사각형을 위한 삼각형 2개를 준비한다. 좌표값은 X, Y, Z로 정의된다.
    // float vertices[] = {
    //     // 첫 번째 삼각형
    //     0.5f,  0.5f, 0.0f,  // 우측 상단
    //     0.5f, -0.5f, 0.0f,  // 우측 하단
    //     -0.5f,  0.5f, 0.0f,  // 좌측 상단 
    //     // second triangle
    //     0.5f, -0.5f, 0.0f,  // 우측 하단
    //     -0.5f, -0.5f, 0.0f,  // 좌측 하단
    //     -0.5f,  0.5f, 0.0f   // 좌측 상단
    // }; 
    // 이렇게 하면 총 "6개"의 정점을 써야 한다. 대신, 좌표와 삼각형을 그리는 인덱스를 이용해보자.
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.51f, 0.51f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.51f, 0.49f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.49f, 0.49f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.49f, 0.51f  // top left 
    };
    unsigned int indices[] = {  // 0부터 시작한다는 것을 명심하세요!
        0, 1, 3,   // 첫 번째 삼각형
        1, 2, 3    // 두 번째 삼각형
    };  



    // =================== 정점 데이터 해석 ==================

    // Vertex buffer object를 이용해 정점들의 데이터를 정리할 수 있다. 
    unsigned int VBO, VAO, EBO;

    // 버퍼 id를 생성한다. 
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 이제 저장된 데이터를 어떻게 해석해야 하는지 OpenGL에 알려주자.
    // 0 : 설정한 vertex 속성. 입력 변수의 location을 0으로 정의했다. 
    // 3 : vertex 속성의 크기. vec3 타입이므로 크기는 3이다.
    // GL_FLOAT : vec3이 float이므로.
    // GL_FALSE : 데이터의 정규화 여부. 우리는 그대로 쓰면 되므로 False로 한다.
    // 3*sizeof(float): 데이터와 데이터 사이의 stride. 3 float만큼이므로.
    // (tightly packed이므로 0을 주어도 OpenGL이 잘 추측한다.)
    // (void*) 0: 버퍼에서 데이터가 시작하는 지점. offset = 0이다.(지금은)
    // 
    // 우리는 vertex 정보에 대한 3개 location을 정의해야 한다.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // ==================== VAO, VBO 끝 ==============================
    // ============= 텍스처 해석 ===============

    // 텍스처도 id로 관리된다.
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    // 텍스처 이미지를 불러온다.
    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // 파일 경로, 너비, 높이, 컬러 채널 수를 지정한다.
    unsigned char * data = stbi_load("../../resources/textures/container.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        // 위의 data를 2D 텍스처로 할당한다.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // 밉맵을 생성한다.
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    //이제 이미지 데이터는 free를 해준다.
    stbi_image_free(data);

    // 두번째 텍스처 로딩
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  
    data = stbi_load("../../resources/textures/awesomeface.png", &width, &height, &nrChannels, 0);

    if (data)
    {
        // PNG 이미지이므로 반투명 정보도 존재한다.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        // 밉맵을 생성한다.
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    //이제 이미지 데이터는 free를 해준다.
    stbi_image_free(data);

    // Engine 준비

    // 우리가 만든 셰이더를 활성화하고,
    ourShader.use();
    // 텍스처 바인딩을 해 준다.
    ourShader.setInt("ourTexture1", 0); // shader class 이용.
    ourShader.setInt("ourTexture2", 1); // shader class 이용.


    // 메인 루프
    // 창을 닫아야 할 필요가 없을 동안...
    while(!glfwWindowShouldClose(window)){
        processInput(window);       // 입력을 처리하고, (여기서 원하는 다른 함수들을 넣을 수 있다)
                                    // 이 자리에 렌더링 코드를 넣을 수 있다.
                                    
        glClearColor(0.2f, 0.3f, 0.3f, 0.6f);   // 1. 짙은 청녹색(RGBA값)으로 설정된 값으로,
        glClear(GL_COLOR_BUFFER_BIT);           // 컬러 버퍼를 지운다. 다른 버퍼도 가능.

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);


        // render container
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // EBO에 담긴 데이터를 불러와,
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);       // 삼각형 타입을 6개 정점을 그리는데, 0에서부터 그린다.

        // 우리는 VAO에 담긴 데이터를 가지고 작업하기 때문에, 셰이더와 VAP 부분을 잘 수정했다면
        // 여기는 건드릴 필요가 없다.

        // 그리고 Learn OpenGL과 좌하단 / 우하단 vertex 순서가 반대이기 때문에 색도 반대로 나온다.
        // 이는 정상이다.

        // vertex에 대해서만 색을 지정해도, 기본적으로 그 사이의 pixel 색상의 경우 그 중간값을 interpolate한다.
        // fragment shader의 모든 in 값에 적용된다.

        glfwSwapBuffers(window);    // 버퍼를 교체하고,
        glfwPollEvents();           // 이벤트를 폴링한다.
    }   

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);


    // 루프가 끝나면 glfw를 정리한다.
    glfwTerminate();


    return 0;

}

void processInput(GLFWwindow * window){
    // 만약 window에서 ESCAPE 키가 "눌렸을" 경우
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        // window should close 플래그를 True로 만든다.
        // 그러면 while loop가 종료된다.
        glfwSetWindowShouldClose(window, true);
    }
}

// 프레임버퍼의 크기라 달라진 것을 Viewport에 적용하는 함수
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}
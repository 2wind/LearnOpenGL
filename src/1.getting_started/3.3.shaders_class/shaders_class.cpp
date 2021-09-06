#include <glad/glad.h>
#include <GLFW/glfw3.h>
// Learn OpenGL 3.3 Shaders Class
// https://heinleinsgame.tistory.com/8?category=757483

#include <iostream>

#include "load_shader.h"
// #include <learnopengl/shader_s.h>

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
    Shader ourShader("../../src/1.getting_started/3.3.shaders_class/shader.vs", "../../src/1.getting_started/3.3.shaders_class/shader.fs");
    // ==================== 초기화 부분 끝 ===========================

    // 삼각형을 위한 버텍스 3개를 정의한다. 좌표값은 X, Y, Z로 정의된다.
    // 뒤에 color값을 추가한다. 
    // 그러면 vertex shader도 수정해야 하고, VAO, VBO도 손봐야 한다.
    float vertices[] = {
        // 위치,            // 컬러
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 우하단
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // 좌하단
        0.0f, 0.5f, 0.0f,   0.0f, 0.0f, 1.0f  // 위
    };

    // Vertex buffer object를 이용해 정점들의 데이터를 정리할 수 있다. 
    unsigned int VBO;
    // 버퍼 id를 생성한다. 
    glGenBuffers(1, &VBO);

    // Vertex Array Object에 이 속성을 집어넣을 수 있다.
    // 그러면 매 번 부르는 일을 막을 수 있다.
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    // 버텍스 버퍼 오브젝트의 유형인 GL_ARRY_BUFFER로 할당한다.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 그러면 우리가 호출하는 모든 버퍼는 VBO를 타겟으로 하게 된다.

    // 버퍼의 유형에 대해, 사용자가 정의한 vertices의 데이터를 복사한다. 이 때 static하다는 것을 알려준다. 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 정점 데이터는 이제 그래픽 카드의 메모리에 저장되게 된다.

    // 이제 저장된 데이터를 어떻게 해석해야 하는지 OpenGL에 알려주자.
    // 0 : 설정한 vertex 속성. 입력 변수의 location을 0으로 정의했다. 
    // 3 : vertex 속성의 크기. vec3 타입이므로 크기는 3이다.
    // GL_FLOAT : vec3이 float이므로.
    // GL_FALSE : 데이터의 정규화 여부. 우리는 그대로 쓰면 되므로 False로 한다.
    // 3*sizeof(float): 데이터와 데이터 사이의 stride. 3 float만큼이므로.
    // (tightly packed이므로 0을 주어도 OpenGL이 잘 추측한다.)
    // (void*) 0: 버퍼에서 데이터가 시작하는 지점. offset = 0이다.(지금은)
    // 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*) 0);

    glEnableVertexAttribArray(0);

    // vertex 자료에서 color도 어떻게 해석할지 알려줘야 한다.
    // Color의 location을 "1"로 정의했었고,
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*) (3*sizeof(float)));

    glEnableVertexAttribArray(1);
    // ==================== VAO, VBO 끝 ==============================

    // Engine 준비
    // 메인 루프
    // 창을 닫아야 할 필요가 없을 동안...
    while(!glfwWindowShouldClose(window)){
        processInput(window);       // 입력을 처리하고, (여기서 원하는 다른 함수들을 넣을 수 있다)
                                    // 이 자리에 렌더링 코드를 넣을 수 있다.
                                    
        glClearColor(0.2f, 0.3f, 0.3f, 0.6f);   // 1. 짙은 청녹색(RGBA값)으로 설정된 값으로,
        glClear(GL_COLOR_BUFFER_BIT);           // 컬러 버퍼를 지운다. 다른 버퍼도 가능.

        ourShader.use();             // 2. 이제 위에서 만든 ourShader를 써서,

        glBindVertexArray(VAO);                 // VAO에 담긴 데이터를 불러와,
        glDrawArrays(GL_TRIANGLES, 0, 3);       // 삼각형 타입을 vertex[0]에서부터 3개 정점을 그린다.

        // 우리는 VAO에 담긴 데이터를 가지고 작업하기 때문에, 셰이더와 VAP 부분을 잘 수정했다면
        // 여기는 건드릴 필요가 없다.

        // 그리고 Learn OpenGL과 좌하단 / 우하단 vertex 순서가 반대이기 때문에 색도 반대로 나온다.
        // 이는 정상이다.

        // vertex에 대해서만 색을 지정해도, 기본적으로 그 사이의 pixel 색상의 경우 그 중간값을 interpolate한다.
        // fragment shader의 모든 in 값에 적용된다.

        glfwSwapBuffers(window);    // 버퍼를 교체하고,
        glfwPollEvents();           // 이벤트를 폴링한다.
    }   

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
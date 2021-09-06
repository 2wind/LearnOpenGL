#include <glad/glad.h>
#include <GLFW/glfw3.h>
// Learn OpenGL 2.1 Hello triangle
// https://heinleinsgame.tistory.com/7?category=757483

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow * window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 버텍스 셰이더와 프래그먼트 셰이더를 CPP 내부에 포함시킬 수 있는 방법.
// 더 좋은 방법이 있을까...

// 버텍스 셰이더: 입력 데이터를 OpenGL 좌표의 값으로 적절하게 바꾸어준다.
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
// 프래그먼트 셰이더: 픽셀의 출력 컬러 값을 계산한다.
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";


int main(){
    glfwInit(); // GLFW 초기화
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3으로 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // OpenGL Core mode

    // Window 객체 설정

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


    // 삼각형을 위한 버텍스 3개를 정의한다. 좌표값은 X, Y, Z로 정의된다.
    float vertices[] = {
        -0.8f, -0.5f, 0.0f,
        -0.2f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f, 
        0.8f, -0.5f, 0.0f,
        0.2f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f

    };

    // 버텍스 셰이더
    // 비슷하게 uint ID를 정의하고, vertex_shader를 만들어 할당한다.
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // 그 id에 할당될 source를 가져온다. 
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
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
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
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
    
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // 우리가 컴파일한 셰이더들을 붙여준다.
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // 성패여부 확인
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;

    }

    // 이제 프로그램이 나왔다. 활성화하자.
    glUseProgram(shaderProgram);

    // 연결한 셰이더 객체는 제거해야 한다.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

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
    // (void*) 0: 버퍼에서 데이터가 시작하는 지점. offset = 0이다.(지금은)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    // Engine 준비
    // 메인 루프
    // 창을 닫아야 할 필요가 없을 동안...
    while(!glfwWindowShouldClose(window)){
        processInput(window);       // 입력을 처리하고, (여기서 원하는 다른 함수들을 넣을 수 있다)
                                    // 이 자리에 렌더링 코드를 넣을 수 있다.
                                    
        glClearColor(0.2f, 0.3f, 0.3f, 0.6f);   // 1. 짙은 청녹색(RGBA값)으로 설정된 값으로,
        glClear(GL_COLOR_BUFFER_BIT);           // 컬러 버퍼를 지운다. 다른 버퍼도 가능.

        glUseProgram(shaderProgram);            // 2. 이제 위에서 만든 shaderProgram을 써서,
        glBindVertexArray(VAO);                 // VAO에 담긴 데이터를 불러와,
        glDrawArrays(GL_TRIANGLES, 0, 3);       // 삼각형 타입을 vertex[0]에서부터 3개 정점을 그린다.
        glDrawArrays(GL_TRIANGLES, 3, 3);       // 삼각형 타입을 vertex[0]에서부터 3개 정점을 그린다.
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
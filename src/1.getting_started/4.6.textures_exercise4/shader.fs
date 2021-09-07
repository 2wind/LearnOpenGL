#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// 이 변수를 이용해서 텍스처를 fragment shader로 전달할 수 있다.
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform float mixRatio;

void main()
{
    // texture() 함수를 이용해 fragment color를 텍스처의 것으로 지정한다.
    // 당연히 더 복잡한 처리도 가능하다.
    FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), mixRatio);
}
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform float offset;

// 버텍스 셰이더는 원시 버텍스 정보를 (-1, 1)으로 된 normalized coordinate로 변환하는 역할을 한다.
void main()
{
    gl_Position = vec4(aPos.x + offset, -aPos.y, aPos.z, 1.0);
    ourColor = aColor;
}
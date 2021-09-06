#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform float offset;

// 버텍스 셰이더는 원시 버텍스 정보를 (-1, 1)으로 된 normalized coordinate로 변환하는 역할을 한다.
// 이렇게 하면 우측 하단은 검정색으로 나온다. 해당 위치의 coordinate는 0보다 작은데,
// 일단 우리가 aPos.z(z축 정보)를 0으로 주었기 때문에 검은색으로 나온다.
// 다른 위치는 positive value가 존재하므로 특정한 색상으로 나온다.
void main()
{
    gl_Position = vec4(aPos.x + offset, -aPos.y, aPos.z, 1.0);
    ourColor = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
}
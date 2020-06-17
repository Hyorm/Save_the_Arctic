#version 430

in vec4 vPosition_mc; // vertex position in modeling coordinates 
in vec4 vNormal_mc; // normal in modeling coordinates 
in vec4 vColor; // vertex color 
in vec2 vTexcoord; // texture coordinate

out vec4 fColor;
out vec4 fNormal;
out vec4 fPosition;
out vec2 fTexcoord; // output texture coordinates

layout(location = 1)uniform mat4 M;
layout(location = 2)uniform mat4 V;
layout(location = 3)uniform mat4 P;

layout(location=7) uniform int draw_mode;

void main()
{
	gl_Position = P * V * M * vPosition_mc;
	fNormal = vNormal_mc;

	fPosition = vPosition_mc;

	fTexcoord = vTexcoord;
	

}
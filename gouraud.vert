#version 430

out vec4 fColorFront; // color for the front-facing fragment 
out vec4 fColorBack; // color for the back-facing fragment 
out vec2 fTexcoord; // output texture coordinates

in vec4 vPosition_mc; // vertex position in modeling coordinates 
in vec4 vNormal_mc; // normal in modeling coordinates 
in vec4 vColor; // vertex color 
in vec2 vTexcoord; // texture coordinate

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 U;

uniform vec3 Ia= vec3(0.2,0.2,0.2);
uniform vec3 Il = vec3(1.0);
uniform vec3 Ka;
uniform vec3 Ks;
uniform vec3 Kd;
uniform float c[3] = {0.001, 0.001, 0.0};
uniform float n;
uniform vec4 LightPos_wc;/* = vec4(-100, 100, -100, 1);*/

vec4 shading(vec3 LightPos_ec, vec3 vPosition_ec, vec3 vNormal_ec)
{
	vec3 N = normalize(vNormal_ec);
	vec3 L = LightPos_ec - vPosition_ec;
	float d = length(L); L = L/d;
	vec3 V = normalize(vec3(0.0) - vPosition_ec);
	vec3 R = reflect(-L, N);

	float fatt = min(1.0 / (c[0] + c[1]*d + c[2]*d*d), 1.0);

	float cos_theta = max(dot(N,L),0);
	float cos_alpha = max(dot(V,R),0);

	vec3 I = Ia * Ka + fatt * Il * (Kd * cos_theta + Ks * pow(cos_alpha, n))/*vec3(vColor)*/;

	return vec4(I,1);
}

void main()
{
	mat4 VMT = M*V;
	mat4 U = transpose(inverse(M));
	vec3 vNormal_ec = vec3(normalize(U*vNormal_mc));
	vec3 vPosition_ec = vec3(VMT * vPosition_mc);
	vec3 LightPos_ec = vec3(V * LightPos_wc);

	gl_Position = P * V * M * vPosition_mc;

	fColorFront = shading(LightPos_ec, vPosition_ec,  vNormal_ec); 
	fColorBack = shading(LightPos_ec, vPosition_ec, -vNormal_ec); 
	fTexcoord = vTexcoord;

}
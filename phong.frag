#version 430

out vec4 FragColor;
in vec4 fColor;
in vec4 fPosition;
in vec4 fNormal;
in vec2 fTexcoord; 

uniform int is_tex_valid;
uniform int ColorMode;// color mode 
uniform int ObjectCode;// object code 
uniform sampler2D sampler;// sampler2D 

uniform mat4 M;
uniform mat4 P;
uniform mat4 V;
uniform mat4 U;

vec3 Ia = vec3(0.2,0.2,0.2);
vec3 Il = vec3(1.0);
uniform vec3 Ka;
uniform float Ks;
uniform vec3 Kd;
uniform float c[3] = {0.01, 0.01, 0.0};
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

	vec3 I = Ia * Ka + fatt * Il * (Kd * cos_theta + Ks * pow(cos_alpha, n))/**vec3(vColor)*/;

	return vec4(I, 1);
}

void phong_shading()
{
	mat4 VMT = V*M;
	mat4 U = transpose(inverse(VMT));
	vec3 fNormal_ec = vec3(normalize(U*fNormal));
	vec3 fPosition_ec = vec3(VMT * fPosition);
	vec3 LightPos_ec = vec3(V * LightPos_wc);

	FragColor = shading(LightPos_ec, vec3(fPosition_ec), vec3(fNormal_ec));
}

void main()
{	
	if (ColorMode == 1) 
		FragColor = vec4(0,0,1,1); 
	else if(ColorMode == 2) { 
		float nc = ObjectCode / 255.0; 
		FragColor = vec4(nc, nc, nc, 1); 
	}
	else { 
		phong_shading();
		if(is_tex_valid == 1)
			FragColor *= texture(sampler, fTexcoord);
	}
}
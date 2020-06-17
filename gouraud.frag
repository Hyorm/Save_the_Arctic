#version 430

in vec4 fColorFront, fColorBack; 
in vec2 fTexcoord; 

uniform int is_tex_valid;

uniform int ColorMode;// color mode 
uniform int ObjectCode;// object code 
uniform sampler2D sampler;// sampler2D 

out vec4 FragColor; 

void main() { 
	if (ColorMode == 1) 
		FragColor = vec4(0,0,1,1); 
	else if(ColorMode == 2) { 
		float nc = ObjectCode / 255.0; 
		FragColor = vec4(nc, nc, nc, 1); 
	}
	else { 
		if( gl_FrontFacing ) 
			FragColor = fColorFront; 
		else                 
			FragColor = fColorBack; 
		if(is_tex_valid == 1)
			FragColor *= texture(sampler, fTexcoord);
	} 
}

#version 410 core                                                              

uniform sampler2D tex;
uniform float colorError = 0.0f;

out vec4 color;                                                                

in VS_OUT                                                                      
{                                                                              
vec2 texcoord;                                                             
} fs_in;                                                                       

float PI = 3.14159;

void main(void)                                                                
{                                                                              
	vec4 texture_color = texture(tex, fs_in.texcoord);
	color = vec4(
		(1 - texture_color.r * sin(2 * PI * colorError)), 
		(1 - texture_color.g * sin(2 * PI * colorError)), 
		(1 - texture_color.b * sin(2 * PI * colorError)), 
		1.0);
	
}                                                                              
#version 410 core                                                              

uniform sampler2D tex;
uniform float colorError = 0.0f;
uniform float colorAdjust = 1.2f;
uniform float fxRadius = 5.0f;

out vec4 color;                                                                

in VS_OUT                                                                      
{                                                                              
vec2 texcoord;                                                             
} fs_in;                                                                       

float PI = 3.14159;

void main(void)                                                                
{                                                                              
	vec4 texture_color = texture(tex, fs_in.texcoord);
	if(((fs_in.texcoord.x - 0.5f) * (fs_in.texcoord.x - 0.5f) + (fs_in.texcoord.y - 0.5f) * (fs_in.texcoord.y - 0.5f)) < fxRadius * fxRadius)
	{
		color = vec4(
		(1 - texture_color.r) * sin(2 * PI * colorError) * colorAdjust, 
		(1 - texture_color.g) * sin(2 * PI * colorError + 2 * PI / 3) * colorAdjust, 
		(1 - texture_color.b) * sin(2 * PI * colorError + 4 * PI / 3) * colorAdjust, 
		1.0);
	}
	else
	{
		color = texture_color;
	}
}                                                                              
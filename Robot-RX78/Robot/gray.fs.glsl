#version 410 core                                                              

uniform sampler2D tex;
uniform float colorError = 0.0f;
uniform float colorAdjust = 1.2f;
uniform float fxRadius = 5.0f;
uniform float time = 0;

out vec4 color;                                                                

in VS_OUT                                                                      
{                                                                              
vec2 texcoord;                                                             
} fs_in;                                                                       

float PI = 3.14159;
float interval = 0.05f;

void main(void)                                                                
{                                                                              
	vec2 toCenter = normalize(vec2(0.5f - fs_in.texcoord.x, 0.5f - fs_in.texcoord.y));
	float toClevel = sqrt(pow(0.5f - fs_in.texcoord.x, 2) + pow(0.5f - fs_in.texcoord.y, 2));

	float level = 0.5f * sin( -pow(PI * toClevel / fxRadius, 2)) * ((toClevel) / fxRadius);
	vec2 coord = toCenter * level + fs_in.texcoord;
	float radius = cos(abs(10 * time / PI)) * fxRadius;

	if(((fs_in.texcoord.x - 0.5f) * (fs_in.texcoord.x - 0.5f) + (fs_in.texcoord.y - 0.5f) * (fs_in.texcoord.y - 0.5f)) < fxRadius * fxRadius)
	{
		vec4 texture_color = texture(tex, coord);
		color = vec4(
		(1 - texture_color.r * 0.8f) * colorAdjust, 
		(1 - texture_color.g) * colorAdjust, 
		(1 - texture_color.b * 1.2f) * colorAdjust, 
		1.0);
	}
	else
	{
		vec4 texture_color = texture(tex, fs_in.texcoord);
		color = texture_color;
	}
}                                                                              
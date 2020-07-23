#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{   
	vec4 color = texture(texture1, TexCoords);
	if(color.x>0)
	{
		FragColor = vec4(1,0,0,1);
	}
	else if(color.y>0)
	{
		FragColor = vec4(0,1,0,1);
	}
	else if(color.z>0)
	{
		FragColor = vec4(0,0,1,1);
	}
	else
	{
		FragColor =vec4(1,1,1,1);
	}

//	FragColor = vec4(color.z,0,0,1);
	FragColor = vec4(1,0,0,1);
}

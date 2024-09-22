#version 330 core
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

uniform vec4 color = vec4(1);

in float fsForkLevel;

void main()
{
	if(fsForkLevel < 0.5f)
	{
	    fragColor = vec4(1,0,0,1);
	}
	else if(fsForkLevel < 1.5f)
	{
		fragColor = vec4(0,1,0,1);
	}
	else if(fsForkLevel < 2.5f)
	{
		fragColor = vec4(0,0,1,1);
	}
	else if(fsForkLevel < 3.5f)
	{
		fragColor = vec4(0,1,1,1);
	}
	else if(fsForkLevel < 4.5f)
	{
		fragColor = vec4(1,0,1,1);
	}
	else if(fsForkLevel < 5.5f)
	{
		fragColor = vec4(1,1,0,1);
	}
	else
	{
		fragColor = color;
	}
	
    brightColor = vec4(0);
}

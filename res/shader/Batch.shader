#shader vertex
#version 330 core

layout(location = 0)in vec4 position;
layout(location = 1)in vec4 a_color;
layout(location = 2)in vec2 a_TextCoord;
layout(location = 3)in float a_TextIndex;

out vec4 v_color;
out vec2 v_TextCoord;
out float v_TextIndex;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP*position;
	v_color=a_color;
	v_TextCoord = a_TextCoord;
	v_TextIndex=a_TextIndex;
};

#shader fragment
#version 330 core

layout(location = 0)out vec4 color;
in vec4 v_color;
in vec2 v_TextCoord;
in float v_TextIndex;

uniform sampler2D u_Texture;
uniform sampler2D u_Textures[2];
// void main()
// {
// 	// color = v_color;
// 	int index=int(v_TextIndex);
// 	vec4 textColor = texture(u_Texture, v_TextCoord);
// 	color = textColor;
// };

void main()
{
	// color = v_color;
	int index=int(v_TextIndex);

    vec4 textColor;
    switch (index){
            case 0:
                textColor = texture(u_Textures[0], v_TextCoord);
                break;
            case 1:
                textColor = texture(u_Textures[1], v_TextCoord);
                break;
        }

	color = textColor;
};
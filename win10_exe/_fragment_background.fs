#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D tex;

vec4 texColor;

void main()
{   
    texColor = texture(tex, TexCoords);
    if(texColor.a < 0.1)
        discard;

    FragColor = texColor;
}

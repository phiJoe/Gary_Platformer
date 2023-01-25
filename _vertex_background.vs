#version 330 core
layout (location = 0) in vec3 vertex3D;
layout (location = 1) in vec2 texCoord;

uniform mat4 model;
out vec2 TexCoords;

void main()
{   
    gl_Position = model * vec4(vertex3D, 1.0);
    TexCoords = texCoord;
}

#version 330 core
// appdata
layout(location = 0) in vec3 _pos;
layout(location = 1) in vec3 _color;

// v2f
out vec3 color;

// uniform
uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(_pos, 1);
    color = _color;
}
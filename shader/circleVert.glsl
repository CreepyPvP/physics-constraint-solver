#version 440

layout(location = 0) in vec2 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 pos;

void main() {
    // gl_Position = projection * view * model * vec4(aPos, 0, 1);
    gl_Position = projection * view * model * vec4(aPos, 0, 1);
    pos = aPos;
}

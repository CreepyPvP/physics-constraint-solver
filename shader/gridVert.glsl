#version 440

layout(location = 0) in vec2 aPos;

uniform vec2 screenDimensions;

out vec2 pos;

void main() {
    gl_Position = vec4(aPos, 0, 1);
    pos = aPos * screenDimensions;
}

#version 440

in vec2 pos;

out vec4 out_Color;

void main() {
    if (length(pos) <= 1) {
        out_Color = vec4(1, 1, 1, 1);
    } else {
        out_Color = vec4(0, 0, 0, 0);
    }
}

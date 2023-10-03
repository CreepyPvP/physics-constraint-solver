#version 440

in vec2 pos;

out vec4 out_Color;

void main() {
    // TODO: move me to uniform!
    const vec4 background = vec4(0.1, 0.1, 0.1, 1);
    const vec4 lineColor = vec4(0.3, 0.3, 0.3, 1);
    const float lineSpacing = 150.0f;
    const float lineThickness = 2;

    if (mod(pos.x, lineSpacing) < lineThickness || 
        mod(pos.y, lineSpacing) < lineThickness 
    ) {
        out_Color = lineColor;
    } else {
        out_Color = background;
    }

    if (pos.x * pos.x + pos.y * pos.y < 20*20) {
        out_Color = vec4(0.5, 0.5, 0.5, 1);
    }
}

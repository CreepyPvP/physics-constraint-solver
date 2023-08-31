#version 440

in vec2 uv;

out vec4 out_Color;

void main() {
    // TODO: move me to uniform!
    const vec4 background = vec4(0.1, 0.1, 0.1, 1);
    const vec4 lineColor = vec4(0.3, 0.3, 0.3, 1);
    const float lineSpacing = 150.0f;
    const float lineThickness = 2;
    if (mod(gl_FragCoord.x, lineSpacing) < lineThickness || 
        mod(gl_FragCoord.y, lineSpacing) < lineThickness 
    ) {
        out_Color = lineColor;
    } else {
        out_Color = background;
    }
}

#version 330 core
layout (location = 0) in vec2 aUnitPos;
layout (location = 1) in vec4 aRect;
layout (location = 2) in vec4 aColour;

out vec4 vColor;

void main() {
    vec2 worldPos = (aUnitPos * aRect.zw) + aRect.xy;
    gl_Position = vec4(worldPos, 0.0, 1.0);
    vColor = aColour;
}
#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aRect;
layout (location = 2) in uint aMaterialId;

uniform mat4 uProjection;

out vec2 vUV;
flat out uint vMaterialId;

void main() {
    vUV = aPos;
    vMaterialId = aMaterialId;

    vec2 worldPos = (aPos * aRect.zw) + aRect.xy;
    gl_Position = uProjection * vec4(worldPos, 0.0, 1.0);
}
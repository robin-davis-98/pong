#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aRect;
layout (location = 2) in uint aMaterialID;
layout (location = 3) in vec4 aUV;
layout (location = 4) in float aRotation;

uniform mat4 uProjection;

struct Material {
    vec4 albedo;
    uint shader_id;
    uint texture_id;
    uint _pad0;
    uint _pad1;
};

layout(std140, binding = 0) uniform MaterialBlock {
    Material materials[8];
};

out vec2 vUV;
out vec4 vColor;

void main() {
    vUV = aPos;
    vColor = materials[aMaterialID].albedo;

    vec2 worldPos = (aPos * aRect.zw) + aRect.xy;
    gl_Position = uProjection * vec4(worldPos, 0.0, 1.0);
}
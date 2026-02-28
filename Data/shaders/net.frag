#version 430 core

in vec2 vUV;
flat in uint vMaterialId;
out vec4 FragColor;

uniform float uTime;

struct Material {
    vec4 albedo;
    uint shader_id;
    uint texture_id;
    float rotation;
    uint _pad1;
};

layout (std140, binding = 0) uniform MaterialBlock {
    Material materials[8];
};

void main() {
    // --- 1. THE NET MASK ---
    float numDashes = 25.0;
    float dashPattern = fract(vUV.y * numDashes);

    // If we're in a gap, stop immediately
    if (dashPattern > 0.5) {
        discard;
    }

    FragColor = materials[vMaterialId].albedo.rgba;
}
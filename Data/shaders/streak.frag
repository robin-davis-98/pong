#version 430 core

in vec2 vUV;
flat in uint vMaterialId;
out vec4 FragColor;

struct Material {
    vec4 albedo;
    uint shader_id;
    uint texture_id;
    uint _pad0;
    uint _pad1;
};

layout (std140, binding = 0) uniform MaterialBlock {
    Material materials[8];
};

void main() {
    vec4 color = materials[vMaterialId].albedo;

     // Soften the edges of the line slightly to hide the segment "seams"
     // This creates a slight rounded glow effect
     float edge = smoothstep(0.0, 0.1, vUV.y) * smoothstep(1.0, 0.9, vUV.y);

     // We don't taper the X-axis here because we are doing it in C++
     // to make the WHOLE trail one long tapered shape.

     FragColor = vec4(color.rgb, color.a * edge);
}
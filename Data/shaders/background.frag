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
    // 1. Pixelation Math
    vec2 resolution = vec2(160.0, 90.0);
    vec2 gridUV = floor(vUV * resolution) / resolution;
    vec2 uv = gridUV * 2.0 - 1.0;

    // 2. Balatro Melt logic
    float time = uTime * 0.4;
    float noise = sin(uv.x * 4.0 + time) + sin(uv.y * 2.0 + time * 1.5);
    noise += sin((uv.x + uv.y) * 3.0 + time * 0.8);

    // Normalize t to [0, 1]
    float t = (noise + 3.0) / 6.0;

    // 3. Color Stepping (Posterization)
    t = floor(t * 6.0) / 6.0;

    // 4. Color Mixing
    // colA comes from the material you created in setup_Game
    vec3 colA = materials[vMaterialId].albedo.rgb;

    // Predefined secondary color (Deep Black/Purple)
    vec3 colB = vec3(0.02, 0.0, 0.05);

    // Mix between your material color and the predefined dark color
    vec3 finalCol = mix(colB, colA, t);

    FragColor = vec4(finalCol, 1.0);
}
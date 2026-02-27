#version 430 core

in vec2 vUV;
flat in uint vMaterialId;
out vec4 FragColor;

uniform float uTime;


void main() {
// --- 1. THE NET MASK ---
float numDashes = 25.0;
float dashPattern = fract(vUV.y * numDashes);

// If we're in a gap, stop immediately
if (dashPattern > 0.5) {
discard;
}

// --- 2. THE BACKGROUND EFFECT (Copied from your bg shader) ---
vec2 resolution = vec2(160.0, 90.0);
vec2 gridUV = floor(vUV * resolution) / resolution;
vec2 uv = gridUV * 2.0 - 1.0;

float time = uTime * 0.4;
float noise = sin(uv.x * 4.0 + time) + sin(uv.y * 2.0 + time * 1.5);
noise += sin((uv.x + uv.y) * 3.0 + time * 0.8);

float t = (noise + 3.0) / 6.0;
t = floor(t * 6.0) / 6.0;

vec3 colA = materials[vMaterialId].albedo.rgb;
vec3 colB = vec3(0.02, 0.0, 0.05);
vec3 finalCol = mix(colB, colA, t);

FragColor = vec4(finalCol, 1.0);
}
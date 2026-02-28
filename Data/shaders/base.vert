#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aRect;
layout (location = 2) in uint aMaterialId;
layout (location = 3) in vec4 aUV;
layout (location = 4) in float aRotation;

uniform mat4 uProjection;

out vec2 vUV;
flat out uint vMaterialId;

void main() {
    vUV = aPos;
    vMaterialId = aMaterialId;

    vec2 finalPos;
    // Use a small epsilon to catch near-zero floats
    if (abs(aRotation) < 0.0001) {
        // Standard Rect (Background/Paddles)
        finalPos = (aPos * aRect.zw);
    } else {
        // Trail Pivot (Rotate around center-left)
        vec2 localPos = vec2(aPos.x * aRect.z, (aPos.y - 0.5) * aRect.w);
        float s = sin(aRotation);
        float c = cos(aRotation);
        finalPos = vec2(localPos.x * c - localPos.y * s, localPos.x * s + localPos.y * c);
    }

    vec2 worldPos = finalPos + aRect.xy;
    gl_Position = uProjection * vec4(worldPos, 0.0, 1.0);
}
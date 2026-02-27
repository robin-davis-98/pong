#version 430 core

in vec2 vUV;
in vec4 vColor;
out vec4 FragColor;

void main() {
    float distance = length(vUV - vec2(0.5));

    float edge = smoothstep(0.5, 0.48, distance);

    if(distance > 0.5) {
        discard;
    }

    FragColor = vec4(vColor.rgb, vColor.a * edge);
}
#version 430 core

uniform sampler2D uTexture;

in vec2 vUV;
in vec4 vColor;
in vec4 vAtlasUV;

out vec4 FragColor;

void main() {
    vec2 texCoord = vAtlasUV.xy + (vUV * vAtlasUV.zw);
    vec4 texSample = texture(uTexture, texCoord);

    // Change .a to .r to see if the data is in the color channel
    FragColor = vec4(vColor.rgb, vColor.a * texSample.r);
}
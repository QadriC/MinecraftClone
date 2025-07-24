#version 330 core

in vec3 posSCO;

out vec4 FragColor;

void main() {
    const vec4 fogColor = vec4(0.35, 0.8, 1.0, 1.0);
    float dMin = 5.0, dMax = 64.0;
    float d = length(posSCO);
    float f = smoothstep(dMin, dMax, d);
    FragColor = mix(vec4(0.0f, 0.4f, 0.9f, 0.5f), fogColor, f);
    //FragColor = vec4(0.0f, 0.4f, 0.9f, 0.5f);
}
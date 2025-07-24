#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 posSCO;
in float ao;

uniform sampler2D atlas;
uniform int underWater;

void main() {
    const vec4 fogColor = vec4(0.35, 0.8, 1.0, 1.0);
    float dMin = 5.0, dMax = 64.0;
    float d = length(posSCO);
    float f = smoothstep(dMin, dMax, d);

    vec4 texColor = texture(atlas, TexCoord);

    float brightness = mix(0.6, 1.0, ao);
    texColor = vec4(texColor.rgb * brightness, texColor.a);

    if(underWater == 1) {
        vec3 tint = vec3(0.0f, 0.4f, 0.9f);
        float blendAmount = 0.5f;
        texColor.rgb = mix(texColor.rgb, tint, blendAmount);
    }

    texColor = mix(texColor, fogColor, f);

    FragColor = texColor;

    // white + ao for debug purposes
    //FragColor = vec4(vec3(1.0) * brightness, 1.0);
}
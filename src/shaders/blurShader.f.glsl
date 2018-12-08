#version 330 core

in vec2 texCoord;

uniform sampler2D fbo;
uniform float systime;
uniform float distance;
uniform float a;

out vec4 fragColorOut;

void main() {
    // Get actual Texel
    vec4 texel = texture( fbo, texCoord );
    // Get Texel above
    vec4 texU = texture(fbo, texCoord+vec2(0, (1+sin(systime)) * distance));
    // Get Texel below
    vec4 texD = texture(fbo, texCoord+vec2(0, -distance*(1-sin(systime))));
    // Get Texel left
    vec4 texR = texture(fbo, texCoord+vec2(distance*(1+cos(systime)), 0));
    // Get Texel right
    vec4 texL = texture(fbo, texCoord+vec2(-distance*(1-cos(systime)), 0));
    // Mix texel with shifted texels
    texel = a*(texel) +(1-a)*((texU+texD+texR+texL)/4);
    float scale = 6;
    texel.r = sqrt(round(texel.r*texel.r*scale)/scale);
    texel.g = sqrt(round(texel.g*texel.g*scale)/scale);
    texel.b = sqrt(round(texel.b*texel.b*scale)/scale);
    fragColorOut = texel;
}

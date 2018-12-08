#version 330 core

in vec2 texCoord;

uniform sampler2D fbo;
uniform float systime;
uniform float distance;
uniform float a;

out vec4 fragColorOut;

void main() {
    vec4 texel = texture( fbo, texCoord );
    vec4 texU = texture(fbo, texCoord+vec2(0, (1+sin(systime)) * distance));
    vec4 texD = texture(fbo, texCoord+vec2(0, -distance*(1-sin(systime))));
    vec4 texR = texture(fbo, texCoord+vec2(distance*(1+cos(systime)), 0));
    vec4 texL = texture(fbo, texCoord+vec2(-distance*(1-cos(systime)), 0));
    vec4 finalColor = texel;
    
    // TODO #A
    
    texel = a*(texel) +(1-a)*((texU+texD+texR+texL)/4);
    fragColorOut = texel;
}

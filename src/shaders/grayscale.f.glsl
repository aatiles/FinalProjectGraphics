#version 330 core

in vec2 texCoord;

uniform sampler2D fbo;

out vec4 fragColorOut;

void main() {
    vec4 texel = texture( fbo, texCoord );
    
    vec4 finalColor = texel;
    
    // TODO #A
    
    float texelSum = texel.r + texel.g + texel.b;
    texelSum /= 3.0;
    finalColor = vec4(texel.r * .1,texel.g * .95,texel.b * .2,1);
	float rez = length(texCoord - vec2(.5,.5));
	if (rez > .48)
		finalColor = vec4(0, 0, 0, 1);
	if (texCoord.t > .405 && texCoord.t < .595 && texCoord.s > .495 && texCoord.s < .505)
		finalColor = vec4(1, 0, 0, 1);
	if (texCoord.t > .495 && texCoord.t < .505 && texCoord.s > .405 && texCoord.s < .595)
		finalColor = vec4(1, 0, 0, 1);
    fragColorOut = clamp(finalColor, 0.0, 1.0);
}

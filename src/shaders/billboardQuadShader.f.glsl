/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

// TODO J: add varying input
in vec2 texCoordG;

out vec4 fragColorOut;

// TODO K: add texture uniform
uniform sampler2D texMap;

void main() {

    //*****************************************
    //******* Final Color Calculations ********
    //*****************************************
    
    // TODO L: load texel
    fragColorOut = vec4( 1, 1, 0, 1 );
    fragColorOut = texture(texMap, texCoordG);
    if (fragColorOut.a < 0.1)
        discard;
}

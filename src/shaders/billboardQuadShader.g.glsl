/*
 *   Geometry Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

// TODO #A: set primitive input
layout( points ) in;
// TODO #B: set primitive output
layout( triangle_strip, max_vertices= 4) out;

uniform mat4 projMatrix;

// TODO I: add varying output
out vec2 texCoordG;

void main() {
    //*****************************************8
    //********* Vertex Calculations  **********
    //*****************************************
    // TODO #C: add first point
    float height = 10; // Height of Tree
    float width = 4; // Width of Tree
    gl_Position = projMatrix*(gl_in[0].gl_Position + vec4( width/2, height, 0, 0));
    texCoordG = vec2(1,1);
    // TODO #D: emit!
    EmitVertex();

    // TODO #F: add 2nd point
    gl_Position = projMatrix*(gl_in[0].gl_Position + vec4( width/2, 0.1, 0, 0));
    texCoordG = vec2(1,0);
    EmitVertex();
    
    
    // TODO #G: add 3rd point
    gl_Position = projMatrix*(gl_in[0].gl_Position + vec4( -width/2, height, 0, 0));
    texCoordG = vec2(0,1);
    EmitVertex();
    
    // TODO #H: add 4th point
    gl_Position = projMatrix*(gl_in[0].gl_Position + vec4( -width/2, 0.1, 0, 0));
    texCoordG = vec2(0,0);
    EmitVertex();
    
    // TODO #E: end!
    EndPrimitive();
}

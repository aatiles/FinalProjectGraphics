/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core
in vec3 aNormal;
out vec3 Normal;

out vec3 FragPos;
in vec3 vPosition;
out vec3 theColor;
uniform mat4 model;
uniform mat4 mvpMatrix;


void main() {
    //*****************************************
    //********* Vertex Calculations  **********
    //*****************************************

    gl_Position = mvpMatrix * vec4(vPosition, 1.0);
	FragPos = vec3(model * vec4(vPosition, 1.0));
	//Normal = mat3(transpose(inverse(model)))*aNormal;
	Normal = aNormal;
	theColor = vPosition;
}

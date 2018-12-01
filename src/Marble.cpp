#include "Marble.h"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects3.hpp>

#include <math.h>
#include <stdlib.h>

Marble::Marble() {
    radius = 0.5;
    location = glm::vec3(0,0,0);
    direction = glm::vec3(1,0,0);
    _rotation = 0;
    _color = glm::vec3(rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5);
}

Marble::Marble( glm::vec3 l, glm::vec3 d, double r ) : radius(r), location(l), direction(d) {
    _rotation = 0;
    _color = glm::vec3(rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5);
    type = 0;
}
Marble::Marble( glm::vec3 l, glm::vec3 d, double r, int t) : radius(r), location(l), direction(d) {
    _rotation = 0;
    _color = glm::vec3(rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5);
    type = t;
}

void Marble::draw( glm::mat4 modelMtx, GLint uniform_modelMtx_loc, GLint uniform_color_loc ) {

    if (direction.x != 0 || direction.y != 0 || direction.z != 0){
        direction = glm::normalize( direction );
    }
    glm::vec3 rotationAxis = glm::cross( direction, glm::vec3(0,1,0) );

    modelMtx = glm::translate( modelMtx, location );
    modelMtx = glm::translate( modelMtx, glm::vec3( 0, radius, 0 ) );
    modelMtx = glm::rotate( modelMtx, (float)_rotation, rotationAxis );
    glUniformMatrix4fv( uniform_modelMtx_loc, 1, GL_FALSE, &modelMtx[0][0] );

    glUniform3fv( uniform_color_loc, 1, &_color[0] );
    if (type == 0)
        CSCI441::drawSolidSphere( radius, 16, 16 );
    if (type == 1)
        CSCI441::drawSolidCylinder( 0.44, 0.44, 1, 16, 16 );
}

void Marble::moveForward() {
    location += direction*0.1f;
    _rotation -= 0.1;
    if( _rotation < 0 ) {
        _rotation += 6.28;
    }
}

void Marble::moveBackward() {
    location -= direction*0.1f;
    _rotation += 0.1;
    if( _rotation > 6.28 ) {
        _rotation -= 6.28;
    }
}

#ifndef _MARBLE_H_
#define _MARBLE_H_ 1

#include <glm/glm.hpp>

#include <GL/glew.h>

class Marble {
public:
    // CONSTRUCTORS / DESTRUCTORS
    Marble();
    Marble( glm::vec3 loc, glm::vec3 dir, double r );
    Marble( glm::vec3 loc, glm::vec3 dir, double r, int t );

    // MISCELLANEOUS
    double radius;
    glm::vec3 location;
    glm::vec3 direction;
    int type;

    void draw( glm::mat4 modelMtx, GLint uniform_modelMtx_loc, GLint uniform_color_loc );
    void moveForward();
    void moveBackward();

private:
    double _rotation;
    glm::vec3 _color;
};

#endif

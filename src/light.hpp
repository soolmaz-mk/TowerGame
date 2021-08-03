#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>

class Light {
public:
    glm::vec3 get_position() const { return  glm::vec3(4.0f, 10.0f, 2.0f); }
    glm::vec3 get_ambient() const { return glm::vec3(0.3f); }
    glm::vec3 get_diffuse() const { return glm::vec3(0.8f); }
    glm::vec3 get_specular() const { return glm::vec3(1.0f); }
};

#endif
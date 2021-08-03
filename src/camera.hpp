#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>
#include <vector>

class Camera {
public:
    glm::vec3 subject;
    std::function<glm::vec3(float)> camera_path;
    
    Camera(std::function<glm::vec3(float)> _camera_path, glm::vec3 _subject = glm::vec3(0.0f, 0.0f, 0.0f))
    {
        subject = _subject;
        camera_path = _camera_path;
    }
    
    void set_subject(glm::vec3 _subject) {
        subject = _subject;
    }
    glm::mat4 get_view_matrix() const {
        return  glm::lookAt(get_position() + subject, subject, glm::vec3(0.0, 1.0, 0.0));  
    }

    glm::mat4 get_projection_matrix() const {
        return glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
    }

    glm::vec3 get_position() const {
        return camera_path(glfwGetTime()) + subject;
    }
};
#endif
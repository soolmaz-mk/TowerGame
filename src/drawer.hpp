#ifndef DRAWER_H
#define DRAWER_H

#include "camera.hpp"
#include "light.hpp"
#include "graphics/shader.hpp"
#include "graphics/data.hpp"

#include "physics.hpp"

class Material {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;    
    float shininess;

    Material(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _shininess) : 
        ambient(_ambient), diffuse(_diffuse), specular(_specular), shininess(_shininess) {}
    Material(glm::vec3 color) : Material(color, color, glm::vec3(0.5), 32.0f) {}
};

const char* vcode = R"code(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)code";
    
const char* fcode = R"code(
#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)code";

class SolidRigidDrawer {
    VertexArray obj;
    Shader shader;
    Material m;
public:
    SolidRigidDrawer(std::vector<glm::vec3> shape, std::vector<glm::vec3> norms, Material _m) : 
    shader(vcode, fcode), m(_m) {
        Attrib3f pa(shape);
        Attrib3f na(norms);
        pa.bind_to(obj, 0);
        na.bind_to(obj, 1);
    }

    void draw(const Camera &c, const Light &l, const RigidBody &r) { // TODO light
        shader.use();
        shader.setMat4("model", glm::translate(glm::mat4(1.0), r.get_cm_pose()) * glm::toMat4(r.get_ang_pose()));
        shader.setMat4("view", c.get_view_matrix()); 
        shader.setMat4("projection", c.get_projection_matrix());

        shader.setVec3("light.position", l.get_position());
        shader.setVec3("viewPos", c.get_position());
        shader.setVec3("light.ambient", l.get_ambient());
        shader.setVec3("light.diffuse", l.get_diffuse());
        shader.setVec3("light.specular", l.get_specular());

        // material properties
        shader.setVec3("material.ambient", m.ambient);
        shader.setVec3("material.diffuse", m.diffuse);
        shader.setVec3("material.specular", m.specular);
        shader.setFloat("material.shininess", m.shininess);
        
        obj.draw();
    }
};

class CubeDrawer : public SolidRigidDrawer {
    const Cube &cube;
public:
    CubeDrawer(const Cube &_cube, Material _m) : cube(_cube), SolidRigidDrawer(std::vector<glm::vec3> {
        glm::vec3(-_cube.width / 2, -_cube.height / 2, -_cube.depth / 2),
        glm::vec3(-_cube.width / 2, -_cube.height / 2, _cube.depth / 2),
        glm::vec3(-_cube.width / 2, _cube.height / 2, _cube.depth / 2),
        glm::vec3(-_cube.width / 2, -_cube.height / 2, -_cube.depth / 2),
        glm::vec3(-_cube.width / 2, _cube.height / 2, -_cube.depth / 2),
        glm::vec3(-_cube.width / 2, _cube.height / 2, _cube.depth / 2),
        
        glm::vec3(_cube.width / 2, -_cube.height / 2, -_cube.depth / 2),
        glm::vec3(_cube.width / 2, -_cube.height / 2, _cube.depth / 2),
        glm::vec3(_cube.width / 2, _cube.height / 2, _cube.depth / 2),
        glm::vec3(_cube.width / 2, -_cube.height / 2, -_cube.depth / 2),
        glm::vec3(_cube.width / 2, _cube.height / 2, -_cube.depth / 2),
        glm::vec3(_cube.width / 2, _cube.height / 2, _cube.depth / 2),
        
        glm::vec3(-_cube.width / 2, -_cube.height / 2, -_cube.depth / 2),
        glm::vec3(_cube.width / 2, -_cube.height / 2, -_cube.depth / 2),
        glm::vec3(_cube.width / 2, _cube.height / 2, -_cube.depth / 2),
        glm::vec3(-_cube.width / 2, -_cube.height / 2, -_cube.depth / 2),
        glm::vec3(-_cube.width / 2, _cube.height / 2, -_cube.depth / 2),
        glm::vec3(_cube.width / 2, _cube.height / 2, -_cube.depth / 2),
        
        glm::vec3(-_cube.width / 2, -_cube.height / 2, _cube.depth / 2),
        glm::vec3(_cube.width / 2, -_cube.height / 2, _cube.depth / 2),
        glm::vec3(_cube.width / 2, _cube.height / 2, _cube.depth / 2),
        glm::vec3(-_cube.width / 2, -_cube.height / 2, _cube.depth / 2),
        glm::vec3(-_cube.width / 2, _cube.height / 2, _cube.depth / 2),
        glm::vec3(_cube.width / 2, _cube.height / 2, _cube.depth / 2),

        glm::vec3(-_cube.width / 2, -_cube.height / 2, -_cube.depth / 2),
        glm::vec3(-_cube.width / 2, -_cube.height / 2, _cube.depth / 2),
        glm::vec3(_cube.width / 2, -_cube.height / 2, _cube.depth / 2),
        glm::vec3(-_cube.width / 2, -_cube.height / 2, -_cube.depth / 2),
        glm::vec3(_cube.width / 2, -_cube.height / 2, -_cube.depth / 2),
        glm::vec3(_cube.width / 2, -_cube.height / 2, _cube.depth / 2),
        
        glm::vec3(-_cube.width / 2, _cube.height / 2, -_cube.depth / 2),
        glm::vec3(-_cube.width / 2, _cube.height / 2, _cube.depth / 2),
        glm::vec3(_cube.width / 2, _cube.height / 2, _cube.depth / 2),
        glm::vec3(-_cube.width / 2, _cube.height / 2, -_cube.depth / 2),
        glm::vec3(_cube.width / 2, _cube.height / 2, -_cube.depth / 2),
        glm::vec3(_cube.width / 2, _cube.height / 2, _cube.depth / 2)
    },std::vector<glm::vec3> {
        glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),

        glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),

        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)
        }, _m) {
    }
    void draw(const Camera &c, const Light &l) {
        SolidRigidDrawer::draw(c, l, cube);
    }
};

#endif
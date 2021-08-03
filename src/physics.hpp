#ifndef PHYSICS_H
#define PHYSICS_H
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext.hpp>

#define GLM_ENABLE_EXPERIMENTAL

class Field {
public:
    virtual glm::vec3 get_force(glm::vec3 pose) {
        return glm::vec3(0.0f);
    };
};


class DynamicObject {
public:
    Field* field = nullptr;
    virtual void step(float dt) = 0;
    virtual void pulse(glm::vec3 pulse, glm::vec3 position) = 0;
    virtual void set_field(Field* _field) {
        field = _field;
    }
};

class RigidBody : public DynamicObject {
protected:
    glm::vec3 cm_pose;
    glm::vec3 cm_momentum;
    glm::quat ang_pose;
    glm::vec3 ang_momentum;
    float radius;
    float m = 1;
    float I = 40;
public:
    RigidBody(glm::vec3 _cm_pose, glm::quat _ang_pos) : 
        cm_pose(_cm_pose), 
        cm_momentum(glm::vec3(0.0)),
        ang_pose(_ang_pos), 
        ang_momentum(glm::vec3(0.0)) {
    }
public:
    virtual void step(float dt) {
        cm_pose += cm_momentum * dt;
        if (glm::length(ang_momentum) != 0)
            ang_pose *= glm::angleAxis(glm::length(ang_momentum * dt), glm::normalize(ang_momentum));
        if (field != nullptr)
            pulse(field->get_force(cm_pose) * dt, cm_pose);

        cm_momentum = glm::mat3(0.99) * cm_momentum;
        ang_momentum = glm::mat3(0.99) * ang_momentum;
    }

    virtual void pulse(glm::vec3 pulse, glm::vec3 position) {
        cm_momentum += pulse / m;
        ang_momentum += glm::cross(position - cm_pose, pulse) / I;
    }

    glm::vec3 get_cm_pose() const { return cm_pose; };
    glm::quat get_ang_pose() const { return ang_pose; };

    glm::vec3 get_speed_at_point(glm::vec3 p) const { 
        return cm_momentum + glm::cross(ang_momentum, p - cm_pose);
    };
};

class Cube : public RigidBody {
public:
    float width, height, depth;
    Cube(float _width, float _height, float _depth, glm::vec3 _cm_pose) :
        width(_width), height(_height), depth(_depth), RigidBody(_cm_pose, glm::quat()) {
        radius = sqrt(width * width + height * height + depth * depth);
    }
};


class GravityField : public Field {
    virtual glm::vec3 get_force(glm::vec3 pose) {
        return -glm::vec3(0, 1.0, 0);//-(pose - glm::vec3(0, 1.0, 0));
    }
};

class Earth {

};

class Glue {
    // 
};

class Collision {
public:
    glm::vec3 position;
    glm::vec3 norm;
    glm::vec3 relative_speed;
    RigidBody *a, *b;
    Collision(glm::vec3 _position, glm::vec3 _norm, glm::vec3 _speed, RigidBody *_a, RigidBody *_b) : 
        position(_position), norm(_norm), relative_speed(_speed), a(_a), b(_b) {}
};

std::vector<Collision*> check_collide_nonsymmetric(Cube *c1, Cube *c2) {
    std::vector<Collision*> vs;
    for (int ii = -1; ii <= 1; ii += 2)
        for (int jj = -1; jj <= 1; jj += 2)
            for (int kk = -1; kk <= 1; kk += 2) {
                auto abs_p = c1->get_cm_pose() + glm::toMat3(c1->get_ang_pose()) *
                     glm::vec3(ii * c1->width / 2, jj * c1->height / 2, kk * c1->depth / 2);
                auto rel_p = glm::inverse(glm::toMat3(c2->get_ang_pose())) * (abs_p - c2->get_cm_pose());
                
                if (abs(rel_p.x) <= c2->width / 2 &&
                    abs(rel_p.y) <= c2->height / 2 &&
                    abs(rel_p.z) <= c2->depth / 2) {
                        glm::vec3 norm;
                        auto p1 = glm::normalize(c1->get_cm_pose() - c2->get_cm_pose());
                        auto pt = glm::normalize(glm::toMat3(c2->get_ang_pose()) *
                     glm::vec3(c2->width / 2,  c2->height / 2, c2->depth / 2));
                        glm::vec3 pp = glm::normalize(glm::toMat3(c2->get_ang_pose()) *
                     glm::vec3(c2->width / 2, 0, 0));
                        if (abs(glm::dot(p1, pp)) >= abs(glm::dot(pt, pp))) {
                            norm = glm::normalize(glm::toMat3(c2->get_ang_pose()) *
                     glm::vec3(c2->width / 2, 0, 0));
                        }
                        pp = glm::normalize(glm::toMat3(c2->get_ang_pose()) *
                     glm::vec3(0, c2->height / 2, 0));
                        if (abs(glm::dot(p1, pp)) >= abs(glm::dot(pt, pp))) {
                            norm = glm::normalize(glm::toMat3(c2->get_ang_pose()) *
                     glm::vec3(0, c2->height / 2, 0));
                        }
                        pp = glm::normalize(glm::toMat3(c2->get_ang_pose()) *
                     glm::vec3(0, 0, c2->depth / 2));
                        if (abs(glm::dot(p1, pp)) >= abs(glm::dot(pt, pp))) {
                            norm = glm::normalize(glm::toMat3(c2->get_ang_pose()) *
                     glm::vec3(0, 0, c2->depth / 2));
                        }
                    vs.push_back(new Collision(abs_p, norm, c1->get_speed_at_point(abs_p) - c2->get_speed_at_point(abs_p), c1, c2));
                }
            }
    return vs;
}

#define JUMP 0.85
#define FRAC 1.0

void earth_impulse(std::vector<Collision*> vs) {
    int cc = 0;
    for (const auto & c : vs) {
        if (glm::dot(c->norm, c->relative_speed) < 0)
            cc ++;
    }

    for (const auto & c : vs) {
        float v = glm::dot(c->norm, c->relative_speed);
        if (v < 0) {
            glm::vec3 remaining = c->relative_speed - v * c->norm;
            c->a->pulse(glm::mat3(-2 * JUMP * v / cc) * c->norm - glm::mat3(FRAC / cc) * remaining, c->position);
        }
    }
}


void object_impulse(std::vector<Collision*> vs) {
    int cc = 0;
    for (const auto & c : vs) {
        if (glm::dot(c->norm, c->relative_speed) < 0)
            cc ++;
    }

    for (const auto & c : vs) {
        float v = glm::dot(c->norm, c->relative_speed);
        if (v < 0) {
            glm::vec3 remaining = c->relative_speed - v * c->norm;
            c->a->pulse(glm::mat3(-1 * JUMP * v / cc) * c->norm - glm::mat3(FRAC / cc / 2) * remaining, c->position);
            c->b->pulse(glm::mat3( 1 * JUMP * v / cc) * c->norm + glm::mat3(FRAC / cc / 2) * remaining, c->position);
        }
    }
}

#endif
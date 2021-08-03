#include "scene.hpp"
#include "drawer.hpp"
// from https://learnopengl.com/

int main() {
    srand((unsigned)time(NULL));

    Scene gg;
    glm::vec3 target_view(0, 0, 0);
    Camera cam([](float t) { return glm::vec3(2, 4, 2); }, target_view);
    Light light;

    Cube earth(10.0, 1.0, 10.0, glm::vec3(0, -0.5, 0));
    CubeDrawer ed(earth, Material(glm::vec3(0.4), glm::vec3(0.4), glm::vec3(0.0), 1.0f));

    std::vector<glm::vec3> circle_triangles;
    std::vector<glm::vec3> circle_norms;
    for (int i = 0; i < 16; i ++) {
        circle_triangles.push_back(glm::vec3(0.1 * cos(2 * i * M_PI / 16), 0, 0.1 * sin(2 * i * M_PI / 16)));
        circle_triangles.push_back(glm::vec3(0.1 * cos(2 * (i + 1) * M_PI / 16), 0, 0.1 * sin(2 * (i + 1) * M_PI / 16)));
        circle_triangles.push_back(glm::vec3(0, 0, 0));
        circle_norms.push_back(glm::vec3(0, 1, 0));
        circle_norms.push_back(glm::vec3(0, 1, 0));
        circle_norms.push_back(glm::vec3(0, 1, 0));
    }
    SolidRigidDrawer sign_drawer(circle_triangles, circle_norms, Material(glm::vec3(1, 0, 0)));
    SolidRigidDrawer sign_drawer_shadow(circle_triangles, circle_norms, Material(glm::vec3(0.2, 0.2, 0.2)));
    
    GravityField gravity;

    std::vector<Cube*> cubes;
    std::vector<CubeDrawer*> cds;
    cubes.push_back(new Cube(1.0, 1.0, 1.0, glm::vec3(0.0, 4.0, 0.0)));
    
    for (const auto & c : cubes) {
        c->set_field(dynamic_cast<Field*>(&gravity));
        cds.push_back(new CubeDrawer(*c, Material(glm::vec3(0.1, 0.4, 0.6))));
    }


    bool finished = false;
    while (!finished) {
        if (target_view.y < cubes.size()) {
            target_view += glm::vec3(0, 0.1, 0);
            cam.set_subject(target_view);
        }
        for (const auto & c : cubes) {
            c->step(0.1);
            auto res = check_collide_nonsymmetric(c, &earth);
            if (!res.empty()) {
                earth_impulse(res);
                for (const auto & r : res) delete r;
            }
        }

        for (int i = 0; i < cubes.size(); i ++) {
            for (int j = 0; j < cubes.size(); j ++) {
                if (i == j)
                    continue;
                auto res = check_collide_nonsymmetric(cubes[i], cubes[j]);
                if (!res.empty()) {
                    object_impulse(res);
                    for (const auto & r : res) delete r;
                }
            }
        }

        gg.predraw();
        ed.draw(cam, light);
        for (const auto & cd : cds) {
            cd->draw(cam, light);
        }
        auto sign_vec = glm::vec3(sin(glfwGetTime()), 0, sin(2 * glfwGetTime()));
        sign_drawer.draw(cam, light, RigidBody(target_view + glm::vec3(0, 1, 0) + sign_vec, glm::quat()));
        sign_drawer_shadow.draw(cam, light, RigidBody(glm::vec3(0, 0.01, 0), glm::quat()));
        gg.postdraw();
        auto *ev = gg.poll_event();
        if (dynamic_cast<CloseEvent*>(ev) != nullptr) {
            finished = true;
        }
        if (cubes.size() > 1 && cubes.back()->get_cm_pose().y < 1.0) {
            finished = true;
            std::cout << "GAME OVER" << std::endl;
        } 
        if (dynamic_cast<DropEvent*>(ev) != nullptr) {
            std::cout << "Score : " << cubes.size() << std::endl;
            auto size = exp(-0.6 * cubes.size());
            auto c = new Cube(1.0 * size, 1.0  * size, 1.0 * size, cubes.back()->get_cm_pose() + glm::vec3(0.0, 6.0, 0.0) + sign_vec);
            c->set_field(dynamic_cast<Field*>(&gravity));
            cds.push_back(new CubeDrawer(*c, Material(glm::vec3((float) rand()/RAND_MAX, (float) rand()/RAND_MAX, (float) rand()/RAND_MAX))));
            cubes.push_back(c);
        }
        delete ev;
    }
}
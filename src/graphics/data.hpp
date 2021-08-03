#ifndef DATA_H
#define DATA_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class VertexArray {
    unsigned int vao;
    int size = -1;

public:
    VertexArray() {
        glGenVertexArrays(1, &vao);
    }
    void draw() {
        bind();
        glDrawArrays(GL_TRIANGLES, 0, size);
        unbind();
    }

    void report_size(int i) {
        if (size == -1)
            size = i;
    }

    void bind() {
        glBindVertexArray(vao);
    }

    void unbind() {
        glBindVertexArray(0);
    }
};

class Attrib3f {
    unsigned int vbo;
    int size;

public:
    Attrib3f(std::vector<glm::vec3> ds) {    
        size = ds.size();
        float *raw = new float[3 * size];
        for (int i = 0; i < size; i ++) {
            raw[i * 3] = ds[i].x;
            raw[i * 3 + 1] = ds[i].y;
            raw[i * 3 + 2] = ds[i].z;
        }

        glGenBuffers(1, &vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * size * sizeof(float), raw, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
    }

    ~Attrib3f() {

    }

    void bind_to(VertexArray& va, int index) {
        va.report_size(size);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        va.bind();
        glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(index);
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        va.unbind();
    }
};


#endif
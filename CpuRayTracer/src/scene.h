#ifndef SCENE_H
#define SCENE_H

#include "ray.hpp"

#include "objects.h"

class Scene {
private:
    std::vector<Object*> m_objects;

public:
    Scene(){};
    void add(Object *object);
    ObjectIntersection intersect(const Ray &ray);
    glm::dvec3 trace_ray(const Ray &ray, int depth, unsigned short*Xi);
};

#endif //SCENE_H
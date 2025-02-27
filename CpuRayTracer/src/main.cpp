#include <stdio.h>
#include <stdlib.h>
#include "time.h"


#include "material.h"
#include "objects.h"
#include "camera.hpp"
#include "scene.h"
#include "renderer.h"







int main(int argc, char *argv[]) {

    time_t start, stop;
    time(&start);

    int samples = 50;            // Default samples per pixel

    Camera camera = Camera(glm::dvec3(-2, -5, 2.5), glm::dvec3(0,0,0), 1280, 720);     // Create camera

    Scene scene = Scene();// Create scene

    // Add objects to scene
    scene.add( dynamic_cast<Object*>(new Sphere(glm::dvec3(0,0,-1000), 1000, Material())) );
    scene.add( dynamic_cast<Object*>(new Sphere(glm::dvec3(-1004,0,0), 1000, Material(DIFF, glm::dvec3(0.85,0.4,0.4)))) );
    scene.add( dynamic_cast<Object*>(new Sphere(glm::dvec3(1004,0,0), 1000, Material(DIFF, glm::dvec3(0.4,0.4,0.85)))) );
    scene.add( dynamic_cast<Object*>(new Sphere(glm::dvec3(0,1006,0), 1000, Material())) );
    scene.add( dynamic_cast<Object*>(new Sphere(glm::dvec3(0,0,110), 100, Material(EMIT, glm::dvec3(1,1,1), glm::dvec3(2.2,2.2,2.2)))) );
    scene.add( dynamic_cast<Object*>(new Mesh(glm::dvec3(), "./obj/GTO_Judge.obj", Material(DIFF, glm::dvec3(0.9, 0.9, 0.9)))) );


    Renderer renderer = Renderer(&scene, &camera);
    renderer.render(samples);
//    renderer.runInLoop();
    renderer.save_image("./render.png");
    time(&stop);

    double diff = difftime(stop, start);
    int hrs = (int)diff/3600;
    int mins = ((int)diff/60)-(hrs*60);
    int secs = (int)diff-(hrs*3600)-(mins*60);
    printf("\rRendering (%i samples): Complete!\nTime Taken: %i hrs, %i mins, %i secs\n\n", samples, hrs, mins, secs);



    return 0;
}
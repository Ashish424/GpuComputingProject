#include <math.h>
#include <iostream>
#include <stdio.h>


#include "ray.hpp"
#include "material.h"
#include "objects.h"
//#include "../lib/fastbvh/BVH.h"


ObjectIntersection::ObjectIntersection(bool hit_, double u_, glm::dvec3 n_, Material m_)
{
	hit=hit_, u=u_, n=n_, m=m_;
}


Sphere::Sphere( glm::dvec3 p_, double r_, Material m_ ) {
	m_p=p_, m_r=r_, m_m=m_;
}

double Sphere::get_radius() { return m_r; }
Material Sphere::get_material() { return m_m; }

// Check if ray intersects with sphere. Returns ObjectIntersection data structure
ObjectIntersection Sphere::get_intersection(const Ray &ray) {
	// Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	bool hit = false;
	double distance = 0;
	glm::dvec3 n = glm::dvec3();

	glm::dvec3 op = m_p-ray.origin;
	double t, eps=1e-4, b=dot(op,ray.direction), det=b*b-dot(op,op)+m_r*m_r;
	if (det<0) return ObjectIntersection(hit, distance, n, m_m); 
	else det=sqrt(det);
	distance = (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
	if (distance != 0) hit = true, 
		n = normalize((ray.origin + ray.direction * distance) - m_p);

	return ObjectIntersection(hit, distance, n, m_m);
}


Mesh::Mesh(glm::dvec3 p_, const char* file_path, Material m_) {

	m_p=p_, m_m=m_;

    std::string mtlbasepath;
    std::string inputfile = file_path;
    unsigned long pos = inputfile.find_last_of("/");
    mtlbasepath = inputfile.substr(0, pos+1);

    printf("Loading %s...\n", file_path);
    // Attempt to load mesh
	std::string err = tinyobj::LoadObj(m_shapes, m_materials, inputfile.c_str(), mtlbasepath.c_str());

	if (!err.empty()) {
		std::cerr << err << std::endl;
		exit(1);
	}
	printf(" - Generating k-d tree...\n\n");

    long shapes_size, indices_size, materials_size;
    shapes_size = m_shapes.size();
    materials_size = m_materials.size();

    // Load materials/textures from obj
    // TODO: Only texture is loaded at the moment, need to implement material types and colours
    for (int i=0; i<materials_size; i++) {
        std::string texture_path = "";

        if (!m_materials[i].diffuse_texname.empty()){
            if (m_materials[i].diffuse_texname[0] == '/') texture_path = m_materials[i].diffuse_texname;
            texture_path = mtlbasepath + m_materials[i].diffuse_texname;
            materials.push_back( Material(DIFF, glm::dvec3(1,1,1), glm::dvec3(), texture_path.c_str()) );
        }
        else {
            materials.push_back( Material(DIFF, glm::dvec3(1,1,1), glm::dvec3()) );
        }

    }

    // Load triangles from obj
    for (int i = 0; i < shapes_size; i++) {
        indices_size = m_shapes[i].mesh.indices.size() / 3;
        for (size_t f = 0; f < indices_size; f++) {

            // Triangle vertex coordinates
            glm::dvec3 v0_ = glm::dvec3(
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f] * 3     ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f] * 3 + 1 ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f] * 3 + 2 ]
            ) + m_p;

            glm::dvec3 v1_ = glm::dvec3(
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 1] * 3     ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 1] * 3 + 1 ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 1] * 3 + 2 ]
            ) + m_p;

            glm::dvec3 v2_ = glm::dvec3(
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 2] * 3     ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 2] * 3 + 1 ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 2] * 3 + 2 ]
            ) + m_p;

            glm::dvec3 t0_, t1_, t2_;

            //Attempt to load triangle texture coordinates
            if (m_shapes[i].mesh.indices[3 * f + 2] * 2 + 1 < m_shapes[i].mesh.texcoords.size()) {
                t0_ = glm::dvec3(
                        m_shapes[i].mesh.texcoords[m_shapes[i].mesh.indices[3 * f] * 2],
                        m_shapes[i].mesh.texcoords[m_shapes[i].mesh.indices[3 * f] * 2 + 1],
                        0
                );

                t1_ = glm::dvec3(
                        m_shapes[i].mesh.texcoords[m_shapes[i].mesh.indices[3 * f + 1] * 2],
                        m_shapes[i].mesh.texcoords[m_shapes[i].mesh.indices[3 * f + 1] * 2 + 1],
                        0
                );

                t2_ = glm::dvec3(
                        m_shapes[i].mesh.texcoords[m_shapes[i].mesh.indices[3 * f + 2] * 2],
                        m_shapes[i].mesh.texcoords[m_shapes[i].mesh.indices[3 * f + 2] * 2 + 1],
                        0
                );
            }
            else {
                t0_=glm::dvec3();
                t1_=glm::dvec3();
                t2_=glm::dvec3();
            }

            if (m_shapes[i].mesh.material_ids[ f ] < materials.size())
                tris.push_back(new Triangle(v0_, v1_, v2_, t0_, t1_, t2_, &materials[ m_shapes[i].mesh.material_ids[ f ] ]));
            else
                tris.push_back(new Triangle(v0_, v1_, v2_, t0_, t1_, t2_, &m_m));
        }
    }

    // Clean up
    m_shapes.clear();
    m_materials.clear();
    node = KDNode().build(tris, 0);
    printf("\n");
	//bvh = BVH(&tris);
}

// Check if ray intersects with mesh. Returns ObjectIntersection data structure
ObjectIntersection Mesh::get_intersection(const Ray &ray) {
    double t=0, tmin=INFINITY;
    glm::dvec3 normal = glm::dvec3();
    glm::dvec3 colour = glm::dvec3();
    bool hit = node->hit(node, ray, t, tmin, normal, colour);
    //bool hit = bvh.getIntersection(ray, t, tmin, normal);
    return ObjectIntersection(hit, tmin, normal, Material(DIFF, colour, glm::dvec3()));

}
#include "integrator.hpp"
#include "glm/gtc/random.hpp"
#include "neon/intersection.hpp"
#include "neon/material.hpp"
#include "neon/scene.hpp"
namespace ne {

namespace core {

glm::vec3 Integrator::integrate(const ne::Ray &ray,
                                std::shared_ptr<ne::Scene> scene, int depth) {
  glm::vec3 L{0.0f};
  ne::Ray r{ray};
  ne::Intersection hit;
  ne::Intersection shadow_hit;
  ne::Ray r_out{ray};
  // ne::Ray shadow_ray;
  float eps = std::numeric_limits<float>::epsilon();
  glm::vec3 weight_factor = glm::vec3(1.0f);
  // if ray hit object in scene, apply shading
  float weight = 1.0f/3.0f;
  if (depth < 15 && scene->rayIntersect(r, hit)) {

    weight = weight * weight;

    MaterialPointer material = hit.material;

    const glm::vec3 albedo = material->attenuation();

    // material->scatter(r, hit, r_out);
    hit.material->scatter(r, hit, r_out);

    // shadow ray
    ne::Ray Direct_light;

    // unit hemisphere point generate
    glm::vec3 point;
    //do {
    //  glm::vec3 random_vector = glm::vec3(0.2 * glm::linearRand(0.0f, 1.0f),
    //                                      0.2 * glm::linearRand(0.0f, 1.0f),
    //                                      0.2 * glm::linearRand(0.0f, 1.0f));
    //  point = random_vector - glm::vec3(0.1, 0.1, 0.1);
    //} while (dot(point, point) >= 0.1);
    do {
      glm::vec3 random_vector = glm::vec3(2.0 * glm::linearRand(0.0f, 1.0f),
                                          2.0 * glm::linearRand(0.0f, 1.0f),
                                          2.0 * glm::linearRand(0.0f, 1.0f));
      point = random_vector - glm::vec3(1.0, 1.0, 1.0);
    } while (dot(point, point) >= 1.0);

    // set hemisphere sample point
    //glm::vec3 hemi_sample_point = hit.p + hit.n + point * glm::vec3(0.05, 0.05, 0.05);
    glm::vec3 hemi_sample_point = hit.p + point;
    // set direction and origin of shadow ray glm::vec3(0,1,-1)
    Direct_light.o = hit.p + eps;
    Direct_light.dir = glm::vec3(0, 1, -1) + hemi_sample_point - hit.p; //testscene1 
    //Direct_light.dir = glm::vec3(-1.5f, 1.5f, 0) + hemi_sample_point - hit.p; //testscene2
    
    //Direct_light.dir = normalize(Direct_light.dir);
    // translate hemisphere sample at the light
    // position then, set a direction of shadow ray
    // Direct_light.t = glm::length(Direct_light.dir);
    Direct_light.t = std::numeric_limits<float>::max();

    // ray intersection test
    // Weight = Area / r * r ( dot(N_light , -L) where area is light source
    // area, r is distance (hit point, source point), N_light is normal of light
    // point, L is unit vector is path point
    if (scene->rayIntersect(Direct_light, shadow_hit)) {
      MaterialPointer material2 = shadow_hit.material;
      if (material2->emitted() !=
          glm::vec3{0, 0, 0}) // if the hit object is diffuse light, then apply
                              // direct light to the pixel L.
        return material->emitted() +
               albedo * scene->sampleDirectLight(Direct_light, shadow_hit) * weight +
               albedo * integrate(r_out, scene, depth + 1);
    }
    return material->emitted() + albedo * integrate(r_out, scene, depth + 1);
  }
  // background color
  else {
    // L = scene->sampleBackgroundLight(r_out.dir);
    return scene->sampleBackgroundLight(r.dir);
  }

  return scene->sampleBackgroundLight(r.dir);
}

} // namespace core
} // namespace ne

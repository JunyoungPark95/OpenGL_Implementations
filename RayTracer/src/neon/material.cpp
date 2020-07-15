#include "neon/material.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <random>

namespace ne {

glm::vec3 randomness_generate() { // project point to hemisphere surface
  glm::vec3 point;
  do {
    glm::vec3 random_vector = glm::vec3(2.0 * glm::linearRand(0.0f, 1.0f),
                                        2.0 * glm::linearRand(0.0f, 1.0f),
                                        2.0 * glm::linearRand(0.0f, 1.0f));
    point = random_vector - glm::vec3(1, 1, 1);
  } while (dot(point, point) >= 1.0);

  return point;
}

glm::vec3 reflect(const glm::vec3 &vector, const glm::vec3 &normal) {  //reflect a ray from hit point from ray_in 
  return vector - 2 * dot(vector, normal) * normal;
}

bool refract(const glm::vec3 &dir, const glm::vec3 &normal, float ni, //specially defined for dielectric refract ray in the material
             glm::vec3 &refracted) {
  glm::vec3 unit_dir = normalize(dir);
  float dt = dot(unit_dir, normal);
  float discri = 1.0 - ni * ni * (1 - dt * dt);
  if (discri > 0) {
    refracted = ni * (unit_dir - normal * dt) - normal * sqrt(discri);
    return true;
  } else {
    return false;
  }
}

bool DiffuseLight::scatter(const ne::Ray &r_in, const ne::Intersection &hit,
                           ne::Ray &r_out) const {
  //Implement your code
  r_out.o = hit.p;
  r_out.t = std::numeric_limits<float>::max();
  // add a normal
  glm::vec3 uniform_hemisphere_sample_point =
      hit.p + hit.n + randomness_generate();
  r_out.dir = uniform_hemisphere_sample_point - hit.p;

  return true;
}

glm::vec3 DiffuseLight::emitted() const {
  // Implement your code
  //return glm::vec3(1.0f, 1.0f, 1.0f);
  return color_;
}

glm::vec3 DiffuseLight::attenuation() const {
  // implement your code
  return glm::vec3(0.0f, 0.0f, 0.0f);
  ;
}

bool Dielectric::scatter(const ne::Ray &r_in, const ne::Intersection &hit,
                         ne::Ray &r_out) const {
  // Implement your code
  glm::vec3 out_normal;
  glm::vec3 reflect_ray_dir = reflect(r_in.dir, hit.n);
  float cos;
  float ni_nt;
  float reflect_probability;
  float r_0;
  

  if (dot(r_in.dir, hit.n) > 0) {
    out_normal = -hit.n;
    ni_nt = IOR_; // Indices of refraction
    cos = IOR_ * dot(r_in.dir, hit.n) / r_in.dir.length();
  } else {
    out_normal = hit.n;
    ni_nt = 1.0 / IOR_;
    cos = -dot(r_in.dir, hit.n) / r_in.dir.length();
  }
  glm::vec3 refracted;
  if (refract(r_in.dir, out_normal, ni_nt, refracted)) {
    // Do Schlick approximation
    r_0 = (((1 - IOR_) / (1 + IOR_))) * (((1 - IOR_) / (1 + IOR_)));
    reflect_probability = r_0 + (1 - r_0) * pow((1 - cos), 5); // Fresnel equation
    glm::vec3 unit_vector = normalize(r_in.dir);
    float dot_n_to_dir = dot(unit_vector, out_normal);
    float disc = 1.0 - ni_nt * ni_nt * (1 - dot_n_to_dir * dot_n_to_dir);
    r_out.dir = ni_nt * (unit_vector - out_normal * dot_n_to_dir) - out_normal * sqrt(disc);
    r_out.o = hit.p;
    r_out.dir = normalize(r_out.dir);//
  } else {
    r_out.dir = reflect_ray_dir;
    r_out.o = hit.p;
    r_out.dir = normalize(r_out.dir);//
    reflect_probability = 1.0;
  }
  
  r_out.t = std::numeric_limits<float>::max();
  return (dot(r_out.dir, hit.n) >= 0);
}

glm::vec3 Dielectric::attenuation() const {
  // implement your code
  return glm::vec3(1.0f);
}

bool Lambertian::scatter(const ne::Ray &r_in, const ne::Intersection &hit,
                         ne::Ray &r_out) const {
  // Implement your code

   r_out.o = hit.p;
   r_out.t = std::numeric_limits<float>::max();
   //add a normal
   glm::vec3 uniform_hemisphere_sample_point = hit.p + hit.n + randomness_generate(); // hit.p + randomness_generate(); + hit.n;
   r_out.dir = uniform_hemisphere_sample_point - hit.p;
   r_out.dir = normalize(r_out.dir);
  return (dot(r_out.dir, hit.n) >= 0 );
}

glm::vec3 Lambertian::attenuation() const {
  // implement your code
  return color_;
}

bool Metal::scatter(const ne::Ray &r_in, const ne::Intersection &hit,
                    ne::Ray &r_out) const {
  // Implement your code
  glm::vec3 reflected = reflect(normalize(r_in.dir), hit.n); //generates reflected ray
   r_out.dir = reflected + roughness_ * randomness_generate();
   r_out.o = hit.p;
   r_out.t = std::numeric_limits<float>::max();
   r_out.dir = normalize(r_out.dir);

   return (dot(r_out.dir, hit.n) >= 0);
}

glm::vec3 Metal::attenuation() const {
  // implement your code
  return color_;
}

} // namespace ne

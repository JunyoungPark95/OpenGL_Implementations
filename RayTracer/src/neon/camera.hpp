#ifndef __CAMERA_H_
#define __CAMERA_H_

#include "neon/blueprint.hpp"
#include "neon/ray.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

namespace ne {

glm::vec3 rand_x_y() {
  glm::vec3 point;
  do 
  {
    point = glm::vec3(2.0 * glm ::linearRand(-1.0f, 1.0f),
                      2.0 * glm ::linearRand(-1.0f, 1.0f), 
					  0);
  } while (dot(point, point) >= 1.0);

  return point;
}

class Camera {
public:
  Camera() {}

  Camera(const glm::vec3 &origin, const glm::vec3 &lookat,
         const glm::vec3 &world_up, float vfov, float aspect = 1.0f,
         float aperture = 60.0f, float focusDist = 2.0f)
      : origin(origin) {
    w = glm::normalize(lookat - origin);
    u = glm::normalize(glm::cross(w, world_up));
    v = glm::normalize(glm::cross(u, w));

    float theta = glm::radians(vfov);
    float half_height = glm::tan(theta / 2.0f) * focusDist;
    float half_width = aspect * half_height;

    // 3D world position of bottom left corner pixel
    bottomLeft = origin - (half_width * u) - (half_height * v) + focusDist * w;
    horizontal = 2 * half_width * u;
    vertical = 2 * half_height * v;

    parameters.lensRadius = aperture / 2.0f;
    parameters.focalLength = focusDist;
    parameters.vfov = vfov;
  }

  inline ne::Ray sample(float s, float t) const {
    glm::vec3 view_plane_point = bottomLeft + s * horizontal + t * vertical;
    glm::vec3 rand_xy = parameters.lensRadius * rand_x_y();
    glm::vec3 delta = u * rand_xy.x + v * rand_xy.y;
    return ne::Ray(origin + delta, view_plane_point - origin - delta);
  }

  struct {
    float vfov;
    float lensRadius;
    float focalLength; // focus distance
  } parameters;

  glm::vec3 origin;
  glm::vec3 bottomLeft;
  glm::vec3 horizontal;
  glm::vec3 vertical;
  glm::vec3 u, v, w;
};

} // namespace ne

#endif // __CAMERA_H_

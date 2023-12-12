#include "sparks/renderer/path_tracer.h"

#include "sparks/util/util.h"
#include <math.h>
namespace sparks {
PathTracer::PathTracer(const RendererSettings *render_settings,
                       const Scene *scene) {
  render_settings_ = render_settings;
  scene_ = scene;
}

glm::vec3 PathTracer::SampleRay(glm::vec3 origin,
                                glm::vec3 direction,
                                int x,
                                int y,
                                int sample) const {
  // return glm::vec3(50.0f, 0.0f, 0.0f);
  direction = glm::normalize(direction);
  glm::vec3 throughput{1.0f};
  glm::vec3 radiance{0.0f};
  HitRecord hit_record;
  const int max_bounce = render_settings_->num_bounces;
  std::mt19937 rd(sample ^ x ^ y ^ rand());
  for (int i = 0; i < max_bounce; i++) {
    direction = glm::normalize(direction);
    auto t = scene_->TraceRay(origin, direction, 1e-3f, 1e4f, &hit_record);
    if (t > 0.0f) {
      auto &material =
          scene_->GetEntity(hit_record.hit_entity_id).GetMaterial();
      if(glm::dot(hit_record.normal, direction) > 0.0f) {
        hit_record.normal = -hit_record.normal;
      }
      if (material.material_type == MATERIAL_TYPE_EMISSION) {
        radiance += throughput * material.emission * material.emission_strength;
        // if( i != 0) {
        //   std::cerr << i << "!" << throughput.x << " " << throughput.y << " " << throughput.z << " " << radiance.x << " " << radiance.y << " " << radiance.z << std::endl;
        // }
        break;
      } else if (material.material_type == MATERIAL_TYPE_LAMBERTIAN){
        // radiance = material.albedo_color;
        // // std::cerr << "there!" << i << ":" << throughput.x << " " << throughput.y << " " << throughput.z << std::endl;
        float phi = std::uniform_real_distribution<float>(0.0f, 1.0f)(rd) * PI;
        float rho = std::uniform_real_distribution<float>(0.0f, 1.0f)(rd) * 2.0f * PI;
        auto wi = glm::vec3(
          std::sinf(rho) * std::cosf(phi),
          std::cosf(rho) * std::cosf(phi),
          std::sinf(phi)
        );
        wi = glm::normalize(wi);
        if(glm::dot(hit_record.normal, wi) < 0.0f) {
          wi = -wi;
        }
        throughput *= glm::dot(wi, hit_record.normal) * (material.albedo_color / PI) *
        glm::vec3{scene_->GetTextures()[material.albedo_texture_id].Sample(hit_record.tex_coord)} * (2.0f * PI);
        origin = hit_record.position;
        direction = wi;
      } else if (material.material_type == MATERIAL_TYPE_SPECULAR) {
        if(glm::dot(hit_record.normal, direction) > 0.0f) {
          hit_record.normal = -hit_record.normal;
        }
        hit_record.normal = normalize(hit_record.normal);
          // // std::cerr << RRR(rd) << std::endl;
        glm::vec3 wi = direction - 2.0f * hit_record.normal;
        if(glm::dot(hit_record.normal, wi) < 0.0f) {
          wi = -wi;
        }
        // this is calculate shading
        throughput *= 1;
        origin = hit_record.position;
        direction = wi;
      } else {
        throughput *=
            material.albedo_color *
            glm::vec3{scene_->GetTextures()[material.albedo_texture_id].Sample(
                hit_record.tex_coord)};
        origin = hit_record.position;
        direction = scene_->GetEnvmapLightDirection();
        radiance += throughput * scene_->GetEnvmapMinorColor();
        throughput *=
            std::max(glm::dot(direction, hit_record.normal), 0.0f) * 2.0f;
        if (scene_->TraceRay(origin, direction, 1e-3f, 1e4f, nullptr) < 0.0f) {
          radiance += throughput * scene_->GetEnvmapMajorColor();
        }
        break;
      }
    } else {
      radiance += throughput * glm::vec3{scene_->SampleEnvmap(direction)};
      // if (i != 0) {
      //   std::cerr << throughput.x << " " << throughput.y << " " << throughput.z << ":::" << radiance.x << " " << radiance.y << " " << radiance.z << std::endl;
      // }
      break;
    }
  }
  return radiance;
}
}  // namespace sparks

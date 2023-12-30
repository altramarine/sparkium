
struct Material {
  vec3 albedo_color;
  int albedo_texture_id;
  vec3 emission;
  float emission_strength;
  float alpha;
  float iot;
  int emission_texture_id;
  uint material_type;
  float others[8];
};

#define MATERIAL_TYPE_LAMBERTIAN 0
#define MATERIAL_TYPE_SPECULAR 1
#define MATERIAL_TYPE_TRANSMISSIVE 2
#define MATERIAL_TYPE_PRINCIPLED 3
#define MATERIAL_TYPE_EMISSION 4

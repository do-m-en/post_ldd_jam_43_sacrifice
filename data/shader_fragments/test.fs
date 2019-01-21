#version 300 es

precision mediump float;

struct Base_light
{
  vec3 color;
  float intensity;
};

struct Directional_light
{
  Base_light base;
  vec3 direction;
};

in vec2 tex_coord_0;
in vec3 normal_0;
in vec3 world_position_0; // location of pixel in the world
layout (location = 0) out vec4 frag_color;

uniform sampler2D diffuse;
uniform vec3 ambient_light;
uniform vec3 eye_position;

//uniform Directional_light directional_light;

vec4 calculate_light(Base_light base, vec3 direction, vec3 normal)
{
  // calculate how much diffuse light reflects from the surface towards our object
  // that's the reason for negative direction
  float diffuse_factor = dot(normal, -direction);

  vec4 diffuse_color = vec4(0, 0, 0, 0);
  vec4 specular_color = vec4(0, 0, 0, 0);

  if(diffuse_factor > 0.f)
  {
    diffuse_color = vec4(base.color, 1.0) * base.intensity * diffuse_factor;
    
    // direction between current pixel and our camera
    vec3 direction_to_eye = normalize(eye_position - world_position_0);

    // direction the light reflects off the surface
    vec3 reflect_direction = normalize(reflect(direction, normal));
    
    float specular_factor = dot(direction_to_eye, reflect_direction); // 1 if they are the same and less if they diverge

    // TODO bring this in as an uniform
    float specular_intensity = 2.f;
    float specular_width = 32.f; // exponent is it narrow or spread out (specular_power)
    
    specular_factor = pow(specular_factor, specular_width); // take into accound specular power

    if(specular_factor > 0.f)
    {
      specular_color = vec4(base.color, 1.0) * specular_intensity * specular_factor;
    }
  }
  
  return diffuse_color + specular_color;
}

vec4 calculate_directional_light(Directional_light directional, vec3 normal)
{
  return calculate_light(directional.base, -directional.direction, normal);
}

void main()
{
  // TODO bring this in as an uniform
  // https://www.youtube.com/watch?v=zoZ-1TOy71E&index=25&list=PLEETnX-uPtBXP_B2yupUKlflXBznWIlL5
  Directional_light directional_light;
  directional_light.base.color = vec3(0.5, 0.5, 0.5);
  directional_light.base.intensity = 0.8;
  directional_light.direction = vec3(0, 1, 0);

  vec4 total_light = texture(diffuse, tex_coord_0) * vec4(ambient_light, 1.f);
  
  vec3 normal = normalize(normal_0);
  
  total_light += calculate_directional_light(directional_light, normal);
  
  frag_color = total_light;
}

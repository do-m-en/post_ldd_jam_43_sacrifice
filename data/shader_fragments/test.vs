#version 300 es

precision mediump float;

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 tex_coord;

out vec2 tex_coord_0;
out vec3 normal_0;
out vec3 world_position_0;

uniform mat4 transform; // variable that can be set by a cpu
//uniform mat4 transform_projected;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  //gl_Position = transform_projected * vec4(position, 1.0);
  gl_Position = projection * view * model * vec4(position, 1.0);
  tex_coord_0 = tex_coord;
  normal_0 = (transform * vec4(normal, 0.0)).xyz;
  world_position_0 = (transform * vec4(position, 1.0)).xyz;
}

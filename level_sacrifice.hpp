#ifndef cxx_gd_Level_noahs_ark_HPP_INCLUDED
#define cxx_gd_Level_noahs_ark_HPP_INCLUDED

#include <array>
#include <chrono>
#include <random>

#include <entt/entt.hpp>

#include "display.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "game_over.hpp"
#include "material.hpp"

namespace cxx_gd
{
  using seconds_f = std::chrono::duration<float>;

  class Level_sacrifice
  {
  public:
    Level_sacrifice(Display& display);

    void update(seconds_f delta);
    void render(seconds_f delta);

  private:
    Display& display_;

    std::array<Texture, 6> textures_;
    std::array<Shader, 5> shaders_;
    std::array<Mesh, 3> meshes_;
    Shader draw_shader_;

    entt::Registry<std::uint32_t> registry_;
    Camera camera_;
    glm::vec2 mouse_coords_;

    std::random_device random_device_;
    std::default_random_engine random_engine_;
    seconds_f from_last_spawn_;
    std::array<unsigned short, 4> hardness_level_;
    std::size_t victory_counter_;

    std::optional<std::uint32_t> caught_entity_;
    std::uint32_t mouth_entity_;
    std::array<std::pair<bool, float>, 4> demand_positions_;
    seconds_f from_last_demand_spawn_;

    bool game_running_;
    Game_over game_over_;

    struct Spawner;
    friend class Spawner;
  };
} // ns

#endif // cxx_gd_Level_noahs_ark_HPP_INCLUDED

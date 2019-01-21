#ifndef gxx_gd_GAME_OVER_HPP_INCLUDED
#define gxx_gd_GAME_OVER_HPP_INCLUDED

#include <entt/entt.hpp>
#include "texture.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "display.hpp"
#include <optional>
#include <functional>

namespace cxx_gd
{
  class Game_over
  {
  public:
    Game_over(
        Display& display
      , entt::Registry<std::uint32_t>& registry
      , std::function<void()> reset_function);

    void active(bool active, bool won);

  private:
    Display& display_;
    entt::Registry<std::uint32_t>& registry_;
    std::function<void()> reset_function_;
    Texture texture_;
    Shader shader_;
    Mesh mesh_;
    std::optional<std::uint32_t> game_over_entity_;
  };
}

#endif // gxx_gd_GAME_OVER_HPP_INCLUDED
#ifndef gxx_gd_GAME_OVER_HPP_INCLUDED
#define gxx_gd_GAME_OVER_HPP_INCLUDED

#include <entt/entt.hpp>
#include "texture.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "display.hpp"
#include <optional>
#include <functional>
#include <chrono>

namespace cxx_gd
{
  using seconds_f = std::chrono::duration<float>;

  class Game_over
  {
  public:
    Game_over(
        Display& display
      , std::function<void()> reset_function);

    void active(bool active, bool won);
    void render(seconds_f delta);

  private:
    Display& display_;
    // TODO waste of registry... change engine so that render function won't need it directly
    entt::Registry<std::uint32_t> registry_;
    std::function<void()> reset_function_;
    Texture texture_;
    Shader shader_;
    Mesh mesh_;
    bool active_;
  };
}

#endif // gxx_gd_GAME_OVER_HPP_INCLUDED
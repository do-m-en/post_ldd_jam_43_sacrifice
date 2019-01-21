#include "game_over.hpp"

#include "primitives.hpp"
#include "material.hpp"
#include "ecs_helpers.hpp"

using namespace cxx_gd;

Game_over::Game_over(
      Display& display
    , entt::Registry<std::uint32_t>& registry
    , std::function<void()> reset_function
  )
  : display_{display}
  , registry_{registry}
  , reset_function_{reset_function}
  , texture_{"game_over", 2} // TODO implement texture resource container so that we won't load the same thing multiple times...
  , shader_{"sprite_animation", "basic"}
  , mesh_{
    []() -> Mesh
    {
      constexpr std::array<glm::vec3, 4> rectangle_triangle_strip_3_large =
        {{
          {-10.f, 4.f, 0.f}, //vertex 1
          {-10.f, -4.f, 0.f}, //vertex 2
          {10.f, 4.f, 0.f}, //vertex 3
          {10.f, -4.f, 0.f} // vertex 4
        }};

      return
        {
          rectangle_triangle_strip_3_large,
          cxx_gd::primitive::rectangle_normal_,
          cxx_gd::primitive::rectangle_texture_coordinates_
        };
    }()}
  , game_over_entity_{0}
{
  //
}

void Game_over::active(bool active, bool won)
{
  if(!game_over_entity_)
  {
    if(active)
    {
      game_over_entity_ = registry_.create();
      registry_.assign<std::reference_wrapper<Mesh>>(game_over_entity_.value(), mesh_);
      auto& material = registry_.assign<Material>(game_over_entity_.value(), &shader_, std::move(std::vector{&texture_}));
      material.set("layer", static_cast<int>(!won));
      auto& transform = registry_.assign<Transform>(game_over_entity_.value());
      registry_.assign<Parent>(game_over_entity_.value(), game_over_entity_.value());

      display_.register_mouse_cursor_pos_callback(
        [](double x, double y)
        {
          //
        });

      display_.register_mouse_button_callback(
        [this](int button, int action, int modifiers)
        {
          reset_function_();
        });
    }
  }
  else
  {
    registry_.destroy(game_over_entity_.value());
    game_over_entity_ = std::nullopt;
  }
}
#include "property_move.hpp"

#include "transform.hpp"
#include "material.hpp"
#include <cmath>

using namespace cxx_gd;

void Property_move::update(entt::Registry<std::uint32_t>& registry, std::chrono::duration<float> delta)
{
  auto view = registry.view<Transform, Property_move, Material>();
  for(auto entity : view)
  {
    auto& transform = view.get<Transform>(entity);
    auto& move = view.get<Property_move>(entity);
    auto start_position_x = transform.get_position().x;
    transform.get_position().x =
        transform.get_position().x
      + (5.f * static_cast<int>(move.direction_) * delta.count());

    if(move.direction_ == Move_direction::Left && transform.get_position().x < -20)
      registry.destroy(entity);
    else if(move.direction_ == Move_direction::Right && transform.get_position().x > 20)
      registry.destroy(entity);
    else
    {
      move.elapsed_ += delta;

      using namespace std::chrono_literals;
      if(move.elapsed_ > 0.2s)
      {
        move.elapsed_ = 0s;

        auto& material = view.get<Material>(entity);
        material.set("layer", (material.get<int>("layer") + 1) % 2);
      }
    }
  }
}

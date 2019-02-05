#include "component_fall.hpp"

#include "transform.hpp"
#include "component_god_demand.hpp"
#include <cmath>

using namespace cxx_gd;

bool Component_fall::update(entt::Registry<std::uint32_t>& registry, std::chrono::duration<float> delta)
{
  auto view = registry.view<Transform, Component_fall>();

  for(auto entity : view)
  {
    auto& transform = view.get<Transform>(entity);
    transform.edit().move(
      glm::vec3{
          0.f
        , -(10.f * delta.count())
        , 0.f
      });

    if(transform.get_position().y < -20.f)
    {
      if(registry.has<Component_god_demand>(entity))
      {
        if(!Component_life::destroy_one(registry))
          return false;
      }

      registry.destroy(entity);
    }
  }

  return true;
}

#include "property_fall.hpp"

#include "transform.hpp"
#include "property_god_demand.hpp"
#include <cmath>

using namespace cxx_gd;

bool Property_fall::update(entt::Registry<std::uint32_t>& registry, std::chrono::duration<float> delta)
{
  auto view = registry.view<Transform, Property_fall>();

  for(auto entity : view)
  {
    auto& transform = view.get<Transform>(entity);
    transform.get_position().y = transform.get_position().y - 10.f * delta.count();

    if(transform.get_position().y < -20.f)
    {
      if(registry.has<Property_god_demand>(entity))
      {
        if(!Property_life::destroy_one(registry))
          return false;
      }

      registry.destroy(entity);
    }
  }

  return true;
}

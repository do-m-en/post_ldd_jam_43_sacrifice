#include "property_fall.hpp"

#include "transform.hpp"
#include <cmath>

using namespace cxx_gd;

void Property_fall::update(entt::Registry<std::uint32_t>& registry, std::chrono::duration<float> delta)
{
  auto view = registry.view<Transform, Property_fall>();

  for(auto entity : view)
  {
    auto& transform = view.get<Transform>(entity);
    transform.get_position().y = transform.get_position().y - 10.f * delta.count();

    if(transform.get_position().y < -20.f)
      registry.destroy(entity);
  }
}

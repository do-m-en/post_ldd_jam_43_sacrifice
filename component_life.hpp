#ifndef cxx_gd_COMPONENT_LIFE_HPP_INCLUDE
#define cxx_gd_COMPONENT_LIFE_HPP_INCLUDE

#include "material.hpp"
#include <entt/entt.hpp>

namespace cxx_gd
{
  class Component_life
  {
  public:
    static bool destroy_one(entt::Registry<std::uint32_t>& registry)
    {
      auto view = registry.view<Component_life, Material>();
      std::size_t lives_left = view.size();

      if(view.size())
      {
        auto& material = view.get<Material>(*view.begin());
        material.set("layer", 0);
        registry.remove<Component_life>(*view.begin());
        --lives_left;
      }

      return lives_left;
    }
  };
}

#endif // cxx_gd_COMPONENT_LIFE_HPP_INCLUDE
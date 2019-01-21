#ifndef cxx_gd_PROPERTY_LIFE_HPP_INCLUDE
#define cxx_gd_PROPERTY_LIFE_HPP_INCLUDE

#include "material.hpp"
#include <entt/entt.hpp>

namespace cxx_gd
{
  class Property_life
  {
  public:
    static bool destroy_one(entt::Registry<std::uint32_t>& registry)
    {
      auto view = registry.view<Property_life, Material>();
      std::size_t lives_left = view.size();

      if(view.size())
      {
        auto& material = view.get<Material>(*view.begin());
        material.set("layer", 0);
        registry.remove<Property_life>(*view.begin());
        --lives_left;
      }

      return lives_left;
    }
  };
}

#endif // cxx_gd_PROPERTY_LIFE_HPP_INCLUDE
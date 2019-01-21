#ifndef cxx_gd_PROPERTY_FALL_HPP_INCLUDED
#define cxx_gd_PROPERTY_FALL_HPP_INCLUDED

#include <chrono>
#include <cstdint>
#include <entt/entt.hpp>

namespace cxx_gd
{
  class Property_fall
  {
  public:
    static void update(entt::Registry<std::uint32_t>& registry, std::chrono::duration<float> delta);
  };
}

#endif // cxx_gd_PROPERTY_FALL_HPP_INCLUDED
#ifndef cxx_gd_PROPERTY_MOVE_HPP_INCLUDED
#define cxx_gd_PROPERTY_MOVE_HPP_INCLUDED

#include <chrono>
#include <cstdint>
#include <entt/entt.hpp>

namespace cxx_gd
{

enum class Move_direction
{
  Left = -1,
  Right = 1
};

class Property_move
{
public:
  static void update(entt::Registry<std::uint32_t>& registry, std::chrono::duration<float> delta);

  Property_move(Move_direction direction)
    : direction_{direction}
    , start_{std::chrono::high_resolution_clock::now()}
  {
    //
  }

private:
  Move_direction direction_ = Move_direction::Left;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

} // ns

#endif // cxx_gd_PROPERTY_MOVE_HPP_INCLUDED

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
    , elapsed_{0}
  {
    //
  }

private:
  Move_direction direction_ = Move_direction::Left;
  std::chrono::duration<float> elapsed_;
};

} // ns

#endif // cxx_gd_PROPERTY_MOVE_HPP_INCLUDED

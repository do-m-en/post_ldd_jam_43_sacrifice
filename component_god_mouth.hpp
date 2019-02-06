#ifndef cxx_gd_COMPONENT_GOD_MOUTH_HPP_INCLUDED
#define cxx_gd_COMPONENT_GOD_MOUTH_HPP_INCLUDED

#include "material.hpp"
#include <entt/entt.hpp>
#include "entity.hpp"

namespace cxx_gd
{
  enum class Mouth_state
  {
    Open,
    Close
  };

  class Component_god_mouth
  {
  public:
    using Mouth =
      Entity_view<
          Component_god_mouth
        , Material
      >;

    static void on_collision_enter(
      Mouth mouth
    )
    {
      mouth.get<Component_god_mouth>()
        .mouth(Mouth_state::Open, mouth.get<Material>());
    }

    static void on_collision_exit(
      Mouth mouth
    )
    {
      mouth.get<Component_god_mouth>()
        .mouth(Mouth_state::Close, mouth.get<Material>());
    }

    struct Shared_collision_object
    {
      std::size_t& victory_counter;
      std::array<unsigned short, 4>& hardness_level;
      std::function<void()> spawn;
    };

    static void update( // TODO on_update?
    )
    {

    }

    static void on_collision( // TODO collision?
      Mouth mouth,
      Empty_entity_span other,
      Shared_collision_object& collision_object)
    {
      other.destroy();
      on_collision_exit(mouth);
      ++collision_object.victory_counter;

      for(std::size_t i = 0; i < collision_object.hardness_level.size(); ++i)
      {
        auto& hardness = collision_object.hardness_level[i];

        if(hardness < (i + 1))
        {
          ++hardness;

          if(i == 2 && hardness == 1)
            collision_object.spawn();

          break;
        }
      }
    }

    Component_god_mouth(Material& material)
    {
      material.set("layer", static_cast<int>(Mouth_state::Close));
    }

  private:
    void mouth(Mouth_state state, Material& material)
    {
      material.set("layer", static_cast<int>(state));
    }
  };
}

#endif // cxx_gd_COMPONENT_GOD_MOUTH_HPP_INCLUDED
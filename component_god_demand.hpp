#ifndef cxx_gd_COMPONENT_GOD_DEMAND_HPP_INCLUDED
#define cxx_gd_COMPONENT_GOD_DEMAND_HPP_INCLUDED

#include "component_animal.hpp"
#include "component_life.hpp"
#include "component_fall.hpp"
#include "material.hpp"
#include <array>
#include <algorithm>
#include <string_view>
#include <chrono>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <cmath>
#include <tuple>

#include "entity.hpp"

namespace cxx_gd
{
  enum class God_demand_stage
  {
    Done,
    Mistery,
    Demand
  };

  class Component_god_demand
  {
  public:
    struct Shared_collision_object
    {
      std::array<std::pair<bool, float>, 4>& demand_positions;
      std::chrono::duration<float>& from_last_demand_spawn;
    };

    using Demand =
      Modifiable_Entity<
          Component_god_demand
        , Material
      >;

    using Animal = Modifiable_Entity<Component_animal>;

    // TODO anoying dependencies... consider how this could be split so that each property handles its own part (and that level state doesn't leak here...)
    static void on_collision(
        Demand demand
      , Animal animal
      , Shared_collision_object& collision_object
    )
    {
      if(demand.get<Component_god_demand>().remove_requirement(
        demand.get<Material>(), animal.get<Component_animal>().type()))
      {
        animal.destroy();
        if(demand.get<Component_god_demand>().done(demand.get<Material>()))
        {
          demand.assign<Component_fall>();
          std::find_if(
              std::begin(collision_object.demand_positions),
              std::end(collision_object.demand_positions),
              [&demand = demand.get<Component_god_demand>()](auto const& item)
              {
                return item.second == demand.position();
              }
            )->first = false;

          using namespace std::chrono_literals;
          collision_object.from_last_demand_spawn = 0s;
        }
      }
      else
        animal.assign<Component_fall>(); // the animal escapes!
    }

    static bool update(
      entt::Registry<std::uint32_t>& registry,
      std::chrono::duration<float> delta,
      std::array<std::pair<bool, float>, 4>& positions)
    {
      auto view = registry.view<Component_god_demand, Material>();
      for(auto entity : view)
      {
        auto& demand = view.get<Component_god_demand>(entity);
        demand.add_life_lenght(delta);

        if(!demand.is_done())
        {
          using namespace std::chrono_literals;
          if(demand.get_life_lenght() < 15s)
          {
            view.get<Material>(entity)
              .set(
                "displacement_factor",
                demand.get_displacement_factor());
          }
          else
          {
            std::find_if(
              std::begin(positions),
              std::end(positions),
              [&demand](auto const& item) {return item.second == demand.position();})->first = false;

            registry.destroy(entity);

            if(!Component_life::destroy_one(registry)) // TODO require healt system?
              return false;
          }
        }
      }

      return true;
    }

    Component_god_demand(std::array<int, 4> types, Material& material, int position) // would be nice to have enum mapping in c++...
      : types_{std::move(types)}
      , stage_{God_demand_stage::Demand}
      , elapsed_{std::chrono::seconds{0}}
      , position_{position}
    {
      update_uniforms_(material);
      material.set("layer", static_cast<int>(stage_));
    }

  private:
    bool remove_requirement(Material& material, Animal_type type)
    {
      if(auto it = std::find(types_.begin(), types_.end(), static_cast<int>(type)); it != std::end(types_))
      {
        *it = max_animal_colors_plus_;
        update_uniforms_(material);

        using namespace std::chrono_literals;
        elapsed_ = 0s;

        return true;
      }

      return false;
    }

    bool done(Material& material)
    {
      if(stage_ == God_demand_stage::Demand)
      {
        stage_ = God_demand_stage::Mistery;
        material.set("layer", static_cast<int>(stage_));
      }

      bool done = std::all_of(types_.begin(), types_.end(), [](int item) { return item == max_animal_colors_plus_; });

      if(done)
      {
        stage_ = God_demand_stage::Done;
        material.set("layer", static_cast<int>(stage_));
        material.set("displacement_factor", 1.f);
      }

      return done;
    }

    int position() const
    {
      return position_;
    }

    bool is_done()
    {
      return stage_ == God_demand_stage::Done;
    }

    void add_life_lenght(std::chrono::duration<float> delta)
    {
      elapsed_ += delta;
    }

    std::chrono::duration<float> get_life_lenght() const
    {
      return elapsed_;
    }

    float get_displacement_factor() const
    {
      using namespace std::chrono_literals;
      if(elapsed_ < 5s)
        return 1.f;

      return std::fmod(elapsed_.count(), 0.2f) + 1.f;
    }

    void update_uniforms_(Material& material)
    {
      std::sort(types_.begin(), types_.end());
      constexpr std::array<std::string_view, 4> names{{"first", "second", "third", "fourth"}};

      for(std::size_t i = 0; i < types_.size(); ++i)
      {
        if(types_[i] < max_animal_colors_plus_)
          material.set(
            names[i],
            std::move(
              glm::vec4{
                Component_animal::type_to_color(static_cast<Animal_type>(types_[i])),
                1.f}));
        else
          material.set(names[i], std::move(glm::vec4{0.5f, 0.5f, 0.5f, 1.f}));
      }
    }

    std::array<int, 4> types_;
    God_demand_stage stage_;
    std::chrono::duration<float> elapsed_;
    int position_;

    static constexpr int max_animal_colors_plus_ = 5;
  };
}

#endif // cxx_gd_COMPONENT_GOD_DEMAND_HPP_INCLUDED
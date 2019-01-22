#include "level_sacrifice.hpp"

#include <vector>
#include <functional>
#include <algorithm>

#include "ecs_helpers.hpp"
#include "screen_to_ray.hpp"

#include "property_move.hpp"
#include "property_fall.hpp"
#include "property_animal.hpp"
#include "property_god_demand.hpp"
#include "property_god_mouth.hpp"
#include "property_conversion_machine.hpp"
#include "property_life.hpp"

#include "collider_sphere.hpp"
#include "collider_oriented_bounding_box.hpp"

#include "object_loaders/obj.hpp"
#include "primitives.hpp"

#include <iostream>

using namespace cxx_gd;

namespace
{
  struct Vertex_view
  {
    std::vector<glm::vec3>& vertices;
    std::vector<glm::vec3>& normals;
    std::vector<glm::vec2>& textures;

    Vertex_view(
        std::vector<glm::vec3>& vertices_,
        std::vector<glm::vec3>& normals_,
        std::vector<glm::vec2>& textures_)
      : vertices{vertices_}
      , normals{normals_}
      , textures{textures_}
    {
    }

    void emplace_back(
        glm::vec3 const& vertex,
        glm::vec3 const& normal,
        glm::vec2 const& texture)
    {
      vertices.push_back(vertex);
      normals.push_back(normal);
      textures.push_back(texture);
    }

    std::size_t size() const {return vertices.size();}
  };

  Mesh load_obj_to_mesh(std::string_view name)
  {
    if(name == "small_sprite")
    {
      return
        {
          cxx_gd::primitive::rectangle_triangle_strip_3_,
          cxx_gd::primitive::rectangle_normal_,
          cxx_gd::primitive::rectangle_texture_coordinates_
        };
    }
    else if(name == "god_mouth")
    {
      constexpr std::array<glm::vec3, 4> rectangle_triangle_strip_3_large =
        {{
          {-2.f, 1.5f, 0.f}, //vertex 1
          {-2.f, -1.5f, 0.f}, //vertex 2
          {2.f, 1.5f, 0.f}, //vertex 3
          {2.f, -1.5f, 0.f} // vertex 4
        }};

      return
        {
          rectangle_triangle_strip_3_large,
          cxx_gd::primitive::rectangle_normal_,
          cxx_gd::primitive::rectangle_texture_coordinates_
        };
    }
    else
    {
      constexpr std::array<glm::vec3, 4> rectangle_triangle_strip_3_large =
        {{
          {-2.f, 2.f, 0.f}, //vertex 1
          {-2.f, -2.f, 0.f}, //vertex 2
          {2.f, 2.f, 0.f}, //vertex 3
          {2.f, -2.f, 0.f} // vertex 4
        }};

      return
        {
          rectangle_triangle_strip_3_large,
          cxx_gd::primitive::rectangle_normal_,
          cxx_gd::primitive::rectangle_texture_coordinates_
        };
    }
  }
}

struct Level_sacrifice::Spawner
{
  static void spawn_animal_token(Level_sacrifice& level, Animal_type type, glm::vec3 const& position)
  {
    auto animal = level.registry_.create();
    level.registry_.assign<std::reference_wrapper<Mesh>>(animal, level.meshes_[0]);
    auto& material = level.registry_.assign<Material>(animal, &level.shaders_[1], std::move(std::vector{&level.textures_[0]}));
    material.set("orientation_x", -1);

    auto& animal_type = level.registry_.assign<Property_animal>(animal, type);
    material.set("animal_color", animal_type.color());

    auto& transform = level.registry_.assign<Transform>(animal);
    transform.get_position() = position;
    level.registry_.assign<Parent>(animal, animal);
    level.registry_.assign<Property_move>(animal, Property_move(Move_direction::Left));
    level.registry_.assign<collider::Oriented_bounding_box>(animal, cxx_gd::primitive::rectangle_triangle_strip_3_);
  }

  static void register_mouse_cursor_pos_callback(Level_sacrifice& level)
  {
    level.display_.register_mouse_cursor_pos_callback(
      [&level](double x, double y)
      {
        level.mouse_coords_ = {x, y};

        if(level.caught_entity_)
        {
          auto ray_direction = screen_pos_to_world_direction(
              level.mouse_coords_,
              {800, 600},
              level.camera_.get_position(),
              level.camera_.get_view_matrix(),
              level.camera_.get_projection_matrix()
            );
          auto& transform = level.registry_.get<Transform>(level.caught_entity_.value());
          auto point_1 = level.camera_.get_position();
          auto t = (transform.get_position().z - point_1.z)/ray_direction.z;
          auto point_2 = point_1 + t * ray_direction;

          transform.get_position() = point_2;

          bool is_god_demand = level.registry_.has<Property_god_demand>(level.caught_entity_.value());
          if(is_god_demand)
          {
            auto picked_entity =
              collider::Oriented_bounding_box::pick_if_all<Property_god_mouth>(
                level.registry_, level.camera_.get_position(), ray_direction);

            level.registry_
              .get<Property_god_mouth>(level.mouth_entity_)
              .mouth(
                picked_entity ? Mouth_state::Open : Mouth_state::Close,
                level.registry_.get<Material>(level.mouth_entity_));
          }
        }
      });
  }

  // TODO refactor the whole thing as there's far too much logic in this function that doesn't even belong here...
  static void register_mouse_button_callback(Level_sacrifice& level)
  {
    level.display_.register_mouse_button_callback(
      [&level](int button, int action, int modifiers)
      {
        auto ray_direction = screen_pos_to_world_direction(
            level.mouse_coords_,
            {800, 600},
            level.camera_.get_position(),
            level.camera_.get_view_matrix(),
            level.camera_.get_projection_matrix()
          );

        if(action)
        {
          auto picked_entity =
            collider::Oriented_bounding_box::pick_if_any<Property_move, Property_fall>(
              level.registry_, level.camera_.get_position(), ray_direction);

          if(picked_entity)
          {
            if(level.registry_.has<Property_move>(picked_entity.value()))
              level.registry_.remove<Property_move>(picked_entity.value());

            if(level.registry_.has<Property_fall>(picked_entity.value()))
              level.registry_.remove<Property_fall>(picked_entity.value());
            else
            {
              auto& transform = level.registry_.get<Transform>(picked_entity.value());
              transform.get_position().z = transform.get_position().z + 1;
            }

            level.caught_entity_ = picked_entity;
          }
        }
        else if(level.caught_entity_)
        {
          bool is_god_demand = level.registry_.has<Property_god_demand>(level.caught_entity_.value());
          auto picked_entity =
              is_god_demand
            ? collider::Oriented_bounding_box::pick_if_all<Property_god_mouth>(
                level.registry_, level.camera_.get_position(), ray_direction)
            : collider::Oriented_bounding_box::pick_if_any<Property_god_demand, Property_conversion_machine>(
                level.registry_, level.camera_.get_position(), ray_direction);

          if(picked_entity)
          {
            if(is_god_demand)
            {
              level.registry_.destroy(level.caught_entity_.value());
              level.registry_
                .get<Property_god_mouth>(level.mouth_entity_)
                .mouth(
                  Mouth_state::Close,
                  level.registry_.get<Material>(level.mouth_entity_));
              ++level.victory_counter_;

              for(std::size_t i = 0; i < level.hardness_level_.size(); ++i)
              {
                if(level.hardness_level_[i] < (i + 1))
                {
                  ++level.hardness_level_[i];

                  if(i == 2 && level.hardness_level_[i] == 1)
                    Spawner::spawn_conversion_machine(level, {10.f, 1.f, 0.f});

                  break;
                }
              }
            }
            else if(level.registry_.has<Property_god_demand>(picked_entity.value()))
            {
              auto& demand = level.registry_.get<Property_god_demand>(picked_entity.value());
              auto& material = level.registry_.get<Material>(picked_entity.value());
              auto animal_type = level.registry_.get<Property_animal>(level.caught_entity_.value()).type();

              if(demand.remove_requirement(material, animal_type))
              {
                level.registry_.destroy(level.caught_entity_.value());
                if(demand.done(material))
                {
                  level.registry_.assign<Property_fall>(picked_entity.value());
                  std::find_if(
                    std::begin(level.demand_positions_),
                    std::end(level.demand_positions_),
                    [&demand](auto const& item) {return item.second == demand.position();})->first = false;

                  using namespace std::chrono_literals;
                  level.from_last_demand_spawn_ = 0s;
                }
              }
              else
                level.registry_.assign<Property_fall>(level.caught_entity_.value()); // the animal escapes!
            }
            else // Property_conversion_machine
            {
              auto& conversion_machine =
                level.registry_
                  .get<Property_conversion_machine>(picked_entity.value());
              bool done =
                conversion_machine
                  .add_animal(
                      level.registry_.get<Property_animal>(level.caught_entity_.value()).type()
                    , level.registry_.get<Material>(picked_entity.value()));
              level.registry_.destroy(level.caught_entity_.value());
              auto& transform =
                level.registry_
                  .get<Transform>(picked_entity.value());

              if(done)
              {
                auto position = transform.get_position();
                position.y -= 2.f;
                spawn_animal_token(level, conversion_machine.get_animal_type(), position);
              }
            }
          }
          else
          {
            level.registry_.assign<Property_fall>(level.caught_entity_.value());
          }

          level.caught_entity_ = std::nullopt;
        }
        else
        {
          auto picked_entity =
            collider::Oriented_bounding_box::pick_if_any<Property_conversion_machine>(
              level.registry_, level.camera_.get_position(), ray_direction);

          if(picked_entity)
            level.registry_
              .get<Property_conversion_machine>(picked_entity.value())
              .next_animal(level.registry_.get<Material>(picked_entity.value()));
        }
      });
  }

  static void spawn_background(Level_sacrifice& level)
  {
    auto background = level.registry_.create();
    level.registry_.assign<std::reference_wrapper<Mesh>>(background, level.meshes_[0]);
    level.registry_.assign<Material>(background, &level.shaders_[2], std::move(std::vector{&level.textures_[1]}));
    level.registry_.assign<Parent>(background, background);
    auto& transform = level.registry_.assign<Transform>(background);
    transform.get_scale().x = 20.f;
    transform.get_scale().y = 15.f;
    transform.get_position().z = -1.f;
  }

  static void spawn_gigant_god_mouth(Level_sacrifice& level)
  {
    auto gigant_mouth = level.registry_.create();
    level.registry_.assign<std::reference_wrapper<Mesh>>(gigant_mouth, level.meshes_[2]);
    auto& material = level.registry_.assign<Material>(gigant_mouth, &level.shaders_[0], std::move(std::vector{&level.textures_[2]}));
    auto& transform = level.registry_.assign<Transform>(gigant_mouth);
    transform.get_position() = glm::vec3{0.f, 10.f, 0.f};
    level.registry_.assign<Parent>(gigant_mouth, gigant_mouth);
    level.registry_.assign<Property_god_mouth>(gigant_mouth, material);

    constexpr std::array<glm::vec3, 4> rectangle_triangle_strip_3_large = {{
            {-2.f, 1.f, 0.f}, //vertex 1
            {-2.f, -1.f, 0.f}, //vertex 2
            {2.f, 1.f, 0.f}, //vertex 3
            {2.f, -1.f, 0.f} // vertex 4
          }};

    level.registry_.assign<collider::Oriented_bounding_box>(gigant_mouth, rectangle_triangle_strip_3_large);

    level.mouth_entity_ = gigant_mouth;
  }

  static void spawn_house(Level_sacrifice& level, glm::vec3 const& position)
  {
    auto house = level.registry_.create();
    level.registry_.assign<std::reference_wrapper<Mesh>>(house, level.meshes_[0]);
    auto& material = level.registry_.assign<Material>(house, &level.shaders_[0], std::move(std::vector{&level.textures_[5]}));
    material.set("layer", 1);
    auto& transform = level.registry_.assign<Transform>(house);
    transform.get_position() = position;
    level.registry_.assign<Parent>(house, house);
    level.registry_.assign<Property_life>(house);
  }

  static void spawn_conversion_machine(Level_sacrifice& level, glm::vec3 const& position)
  {
    auto conversion_machine = level.registry_.create();
    level.registry_.assign<std::reference_wrapper<Mesh>>(conversion_machine, level.meshes_[1]);
    auto& material = level.registry_.assign<Material>(conversion_machine, &level.shaders_[4], std::move(std::vector{&level.textures_[4]}));
    auto& transform = level.registry_.assign<Transform>(conversion_machine);
    transform.get_position() = position;
    level.registry_.assign<Parent>(conversion_machine, conversion_machine);
    level.registry_.assign<Property_conversion_machine>(conversion_machine, material);

    constexpr std::array<glm::vec3, 4> rectangle_triangle_strip_3_large = {{
            {-2.f, 2.f, 0.f}, //vertex 1
            {-2.f, -2.f, 0.f}, //vertex 2
            {2.f, 2.f, 0.f}, //vertex 3
            {2.f, -2.f, 0.f} // vertex 4
          }};

    level.registry_.assign<collider::Oriented_bounding_box>(conversion_machine, rectangle_triangle_strip_3_large);
  }

  static void spawn_god_demand(seconds_f delta, Level_sacrifice& level)
  {
    level.from_last_demand_spawn_ += delta;

    using namespace std::chrono_literals;
    if(level.from_last_demand_spawn_ > 10s)
    {
      std::stable_sort(
        std::begin(level.demand_positions_),
        std::end(level.demand_positions_),
        [](auto const& item1, auto const& item2){return item1.first > item2.first;});

      auto item =
        std::find_if(
          std::begin(level.demand_positions_),
          std::end(level.demand_positions_),
          [](auto const& item) {return !item.first;});

      if(item != std::end(level.demand_positions_))
      {
        std::shuffle(item, std::end(level.demand_positions_), level.random_engine_);
        item->first = true;

        std::uniform_int_distribution<unsigned short> number_of_colors{
          static_cast<unsigned short>(0),
          std::accumulate(
            std::begin(level.hardness_level_),
            std::end(level.hardness_level_),
            static_cast<unsigned short>(0)
          )};
        auto number = number_of_colors(level.random_engine_);

        std::uniform_int_distribution<int> random_type{0, 4};
        std::uniform_int_distribution<float> random_offset{-2.f, 3.f};

        std::array<int, 4> demand_colors{{
            random_type(level.random_engine_),
            (number > level.hardness_level_[0] ? random_type(level.random_engine_) : 5),
            (number > (level.hardness_level_[0] + level.hardness_level_[1]) ? random_type(level.random_engine_) : 5),
            (number > (level.hardness_level_[0] + level.hardness_level_[1] + level.hardness_level_[2]) ? random_type(level.random_engine_) : 5)
          }};

        auto demand = level.registry_.create();
        level.registry_.assign<std::reference_wrapper<Mesh>>(demand, level.meshes_[0]);
        auto& material = level.registry_.assign<Material>(demand, &level.shaders_[3], std::move(std::vector{&level.textures_[3]}));
        level.registry_.assign<Parent>(demand, demand);
        auto& transform = level.registry_.assign<Transform>(demand);
        transform.get_position() = glm::vec3{item->second, random_offset(level.random_engine_) + 10.f, 0.f};
        level.registry_.assign<Property_god_demand>(demand, std::move(demand_colors), material, static_cast<int>(item->second));

        constexpr std::array<glm::vec3, 4> rectangle_triangle_strip_3_large = {{
            {-1.5f, 1.5f, 0.f}, //vertex 1
            {-1.5f, -1.5f, 0.f}, //vertex 2
            {1.5f, 1.5f, 0.f}, //vertex 3
            {1.5f, -1.5f, 0.f} // vertex 4
          }};

        level.registry_.assign<collider::Oriented_bounding_box>(demand, rectangle_triangle_strip_3_large);
      }

      level.from_last_demand_spawn_ = 0s;
    }
  }

  static void spawn_animal(seconds_f delta, Level_sacrifice& level)
  {
    level.from_last_spawn_ += delta;

    using namespace std::chrono_literals;
    if(level.from_last_spawn_ > 1.7s)
    {
      level.from_last_spawn_ = 0s;
      std::uniform_int_distribution<float> random_y_position{-10.f, -3.f};
      std::uniform_int_distribution<int> random_direction{0, 1};
      std::uniform_int_distribution<int> random_type{0, 4};

      auto direction =
          random_direction(level.random_engine_)
        ? Move_direction::Left
        : Move_direction::Right;

      auto animal = level.registry_.create();
      level.registry_.assign<std::reference_wrapper<Mesh>>(animal, level.meshes_[0]);
      auto& material = level.registry_.assign<Material>(animal, &level.shaders_[1], std::move(std::vector{&level.textures_[0]}));
      material.set("orientation_x", direction == Move_direction::Left ? -1 : 1);

      auto& animal_type = level.registry_.assign<Property_animal>(animal, static_cast<Animal_type>(random_type(level.random_engine_)));
      material.set("animal_color", animal_type.color());

      auto& transform = level.registry_.assign<Transform>(animal);
      transform.get_position() =
          glm::vec3{
            direction == Move_direction::Left ? 20.f : -20.f,
            random_y_position(level.random_engine_),
            0.f};
      level.registry_.assign<Parent>(animal, animal);
      level.registry_.assign<Property_move>(animal, Property_move(direction));
      level.registry_.assign<collider::Oriented_bounding_box>(animal, cxx_gd::primitive::rectangle_triangle_strip_3_);
    }
  }

  static void activate(Level_sacrifice& level)
  {
    level.registry_.reset();

    Spawner::register_mouse_cursor_pos_callback(level);
    Spawner::register_mouse_button_callback(level);
    Spawner::spawn_background(level);
    Spawner::spawn_gigant_god_mouth(level);
    Spawner::spawn_conversion_machine(level, {14.f, 1.f, 0.f});
    Spawner::spawn_house(level,{-12.f, 2.9f, 0.f});
    Spawner::spawn_house(level,{-7.f, 1.2f, 0.f});
    Spawner::spawn_house(level,{-2.f, 2.7f, 0.f});

    using namespace std::chrono_literals;
    level.game_running_ = true;
    level.from_last_spawn_ = 0s;
    level.hardness_level_ = {1, 0, 0, 0};
    level.victory_counter_ = 0;
    level.from_last_demand_spawn_ = 5s;
    level.game_over_.active(false, false);

    level.demand_positions_[0].first = false;
    level.demand_positions_[1].first = false;
    level.demand_positions_[2].first = false;
    level.demand_positions_[3].first = false;

    level.caught_entity_ = std::nullopt;
  }
};

Level_sacrifice::Level_sacrifice(Display& display)
  : display_{display}
  , textures_{
        Texture{"animal", 2}
      , Texture("background")
      , Texture{"god_mouth", 2}
      , Texture{"god_demand", 3}
      , Texture("conversion_machine")
      , Texture{"house", 2}}
  , shaders_{
        Shader{"sprite_animation", "basic"}
      , Shader{"oriented_sprite_animation", "basic"}
      , Shader{"textured", "basic"}
      , Shader{"god_demand", "vertex_displacement"}
      , Shader{"conversion_machine", "basic"}}
  , meshes_{
        load_obj_to_mesh("small_sprite")
      , load_obj_to_mesh("conversion_machine")
      , load_obj_to_mesh("gigant_mouth")}
  , draw_shader_{"debug_lines", "basic"}
  , camera_{ glm::vec3{0.f, 0.f, 30.f}, 70, 1.33333f, 0.01f, 1000}
  , random_engine_{random_device_()}
  , demand_positions_{{
        {false, -15.f}
      , {false, -8.f}
      , {false, 8.f}
      , {false, 15.f}}}
  , game_over_{
      display_,
      [this](){ Spawner::activate(*this); }
    }
{
  Spawner::activate(*this);
}

void Level_sacrifice::update(seconds_f delta)
{
  if(game_running_)
  {
    if(victory_counter_ < 20)
    {
      if(Property_god_demand::update(registry_, delta, demand_positions_)
        && Property_fall::update(registry_, delta))
      {
        Property_move::update(registry_, delta);
        Spawner::spawn_animal(delta, *this);
        Spawner::spawn_god_demand(delta, *this);
      }
      else
      {
        game_running_ = false;
        game_over_.active(true, false);
      }
    }
    else
    {
      registry_.destroy(mouth_entity_);
      game_running_ = false;
      game_over_.active(true, true);
    }
  }

  display_.set_active_camera(&camera_);
}

void Level_sacrifice::render(seconds_f delta)
{
  display_.render_start();

  cxx_gd::render(registry_, delta);
  game_over_.render(delta);

  Debug_mesh::render(registry_, draw_shader_);

  display_.render_end();
}
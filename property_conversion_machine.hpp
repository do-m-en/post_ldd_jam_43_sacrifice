#ifndef cxx_gd_PROPERTY_CONVERSION_MACHINE_HPP_INCLUDED
#define cxx_gd_PROPERTY_CONVERSION_MACHINE_HPP_INCLUDED

#include <glm/glm.hpp>
#include <optional>

namespace cxx_gd
{
  class Property_conversion_machine
  {
  public:
    Property_conversion_machine(Material& material)
      : conversion_{Animal_type::cat}
    {
      update_uniforms_(material);
    }

    void next_animal(Material& material)
    {
      conversion_ =
        static_cast<Animal_type>(
          (static_cast<int>(conversion_) + 1) % 5);

      update_uniforms_(material);
    }

    bool add_animal(Animal_type type, Material& material)
    {
      bool done = false;

      if(!first_)
        first_ = type;
      else if(!second_)
        second_ = type;
      else
      {
        first_ = std::nullopt;
        second_ = std::nullopt;
        done = true;
      }

      update_uniforms_(material);

      return done;
    }

    Animal_type get_animal_type() const {return conversion_;}

  private:
    void update_uniforms_(Material& material)
    {
      material.set(
        "first"
        ,   first_
          ? std::move(
              glm::vec4{
                Property_animal::type_to_color(first_.value()),
                1.f})
          : default_value_);
      material.set(
        "second"
        ,   second_
          ? std::move(
              glm::vec4{
                Property_animal::type_to_color(second_.value()),
                1.f})
          : default_value_);
      material.set("third", default_value_);
      material.set(
        "conversion",
        std::move(
          glm::vec4{
            Property_animal::type_to_color(conversion_),
            1.f}));
    }

    std::optional<Animal_type> first_;
    std::optional<Animal_type> second_;
    Animal_type conversion_;

    static constexpr glm::vec4 default_value_{1.f, 0.3f, 0.3f, 1.f};
  };
}

#endif // cxx_gd_PROPERTY_CONVERSION_MACHINE_HPP_INCLUDED
#ifndef cxx_gd_COMPONENT_ANIMAL_HPP_INCLUDED
#define cxx_gd_COMPONENT_ANIMAL_HPP_INCLUDED

#include <glm/glm.hpp>

namespace cxx_gd
{
  enum class Animal_type
  {
      cat
    , rabit
    , wolf
    , lion
    , fox
  };

  class Component_animal
  {
  public:
    Component_animal(Animal_type type)
      : type_{type}
      , color_{type_to_color(type_)}
    {
      //
    }

    Animal_type type() const
    {
      return type_;
    }

    glm::vec3 const& color() const
    {
      return color_;
    }

    static glm::vec3 type_to_color(Animal_type type)
    {
      if(type == Animal_type::cat)
        return {0.f, 0.f, 1.f};
      else if(type == Animal_type::rabit)
        return {1.f, 1.f, 1.f};
      else if(type == Animal_type::wolf)
        return {0.f, 0.f, 0.f};
      else if(type == Animal_type::lion)
        return {1.f, 1.f, 0.f};
      else if(type == Animal_type::fox)
        return {1.f, 0.f, 0.f};

      return {0.f, 0.f, 1.f};
    }

  private:
    Animal_type type_;
    glm::vec3 color_;
  };
}

#endif // cxx_gd_COMPONENT_ANIMAL_HPP_INCLUDED
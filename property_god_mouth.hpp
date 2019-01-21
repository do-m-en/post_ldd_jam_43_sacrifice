#ifndef cxx_gd_PROPERTY_GOD_MOUTH_HPP_INCLUDED
#define cxx_gd_PROPERTY_GOD_MOUTH_HPP_INCLUDED

#include "material.hpp"

namespace cxx_gd
{
  enum class Mouth_state
  {
    Open,
    Close
  };

  class Property_god_mouth
  {
  public:
    Property_god_mouth(Material& material)
    {
      material.set("layer", static_cast<int>(Mouth_state::Close));
    }

    void mouth(Mouth_state state, Material& material)
    {
      material.set("layer", static_cast<int>(state));
    }
  };
}

#endif // cxx_gd_PROPERTY_GOD_MOUTH_HPP_INCLUDED
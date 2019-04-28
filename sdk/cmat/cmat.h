#pragma once
#include "../sdk.h"
class IMaterial;

namespace Materials {
  extern IMaterial *shaded, *glow, *shiny;
  void Initialize();
  
  IMaterial *CreateMaterial( bool IgnoreZ, bool Flat, bool Wireframe, bool Shiny );
  void ResetMaterial();
  
  void ForceMaterial( IMaterial *material, Color color );
}
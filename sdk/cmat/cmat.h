#pragma once
#include "../sdk.h"
class IMaterial;

namespace Materials {
  extern IMaterial *shaded, *glow, *shiny;
  void Initialize();
  
  IMaterial *CreateMaterial( bool Flat );
  void ResetMaterial();
  
  void ForceMaterial( IMaterial *material, Color color );
}
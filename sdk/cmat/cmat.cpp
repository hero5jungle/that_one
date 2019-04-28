#include "cmat.h"
#include "keyvalues.h"
namespace Materials {
  IMaterial *shaded, *glow, *shiny;
  
  void  Initialize() {
    glow = gInts.MatSystem->FindMaterial( "dev/glow_color", "Model textures" );
    shaded = CreateMaterial( false, false, false, false );
    shiny = CreateMaterial( false, false, false, true );
    glow->IncrementReferenceCount();
    shaded->IncrementReferenceCount();
    shiny->IncrementReferenceCount();
  }
  
  IMaterial *CreateMaterial( bool IgnoreZ, bool Flat, bool Wireframe, bool Shiny ) {
    static int created = 0;
    static const char tmp[] = {
      "\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"%s\"\
		\n\t\"$normalmapalphaenvmapmask\" \"%i\"\
		\n\t\"$envmapcontrast\" \"%i\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
        \n}\n"
    };
    char *baseType = ( Flat ? "UnlitGeneric" : "VertexLitGeneric" );
    char material[512];
    sprintf_s( material, sizeof( material ), tmp, baseType, Shiny ? "env_cubemap" : "", Shiny ? 1 : 0, Shiny ? 1 : 0, ( IgnoreZ ) ? 1 : 0, ( Wireframe ) ? 1 : 0 );
    char name[512];
    sprintf_s( name, sizeof( name ), "#mat_%i.vmt", created );
    created++;
    KeyValues *keyValues = ( KeyValues * )malloc( sizeof( KeyValues ) );
    keyValues->Initialize( keyValues, baseType );
    keyValues->LoadFromBuffer( keyValues, name, material );
    IMaterial *createdMaterial = gInts.MatSystem->CreateMaterial( name, keyValues );
    
    if( !createdMaterial )
      return nullptr;
      
    createdMaterial->IncrementReferenceCount();
    return createdMaterial;
  }
  
  void  ResetMaterial() {
    static const float flDefault[3] = { 1, 1, 1 };
    gInts.RenderView->SetBlend( 1 );
    gInts.RenderView->SetColorModulation( flDefault );
    gInts.MdlRender->ForcedMaterialOverride( nullptr );
  }
  void ForceMaterial( IMaterial *material, Color color ) {
    if( material ) {
      float base[3];
      base[0] = color[0] / 255.0f;
      base[1] = color[1] / 255.0f;
      base[2] = color[2] / 255.0f;
      float alpha = color[3] / 255.0f;
      gInts.RenderView->SetBlend( alpha );
      gInts.RenderView->SetColorModulation( base );
      gInts.MdlRender->ForcedMaterialOverride( material );
    }
  }
}

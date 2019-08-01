#include "cmat.h"
#include "keyvalues.h"
namespace Materials {
	IMaterial* shaded, * glow;

	void  Initialize() {
		glow = gInts.MatSystem->FindMaterial( "dev/glow_color", "Model textures" );
		glow->IncrementReferenceCount();
		shaded = CreateMaterial( false );
	}

	IMaterial* CreateMaterial( bool flat ) {
		static int created = 0;
		string type = flat ? "UnlitGeneric" : "VertexLitGeneric";
		const string data = "\""s + type + R"#(" {
      "$basetexture" "vgui/white_additive"
      "$envmap" ""
      "$model" "1"
      "$flat" "1"
      "$nocull"  "0"
      "$selfillum" "1"
      "$halflambert" "1"
      "$nofog"  "0"
      "$znearer" "0"
      "$wireframe" "0"
      "$ignorez" "0"
    })#"s;
		const string name = "#mat_"s + to_string( created ) + ".vmt"s;
		created++;
		KeyValues* keyValues = new KeyValues;
		keyValues->Initialize( keyValues, const_cast<char*>( type.c_str() ) );
		keyValues->LoadFromBuffer( keyValues, name.c_str(), data.c_str() );
		IMaterial* createdMaterial = gInts.MatSystem->CreateMaterial( name.c_str(), keyValues );

		if( !createdMaterial ) {
			return nullptr;
		}

		createdMaterial->IncrementReferenceCount();
		return createdMaterial;
	}

	void  ResetMaterial() {
		static const float flDefault[3] = { 1, 1, 1 };
		gInts.RenderView->SetBlend( 1 );
		gInts.RenderView->SetColorModulation( flDefault );
		gInts.MdlRender->ForcedMaterialOverride( nullptr );
	}

	void ForceMaterial( IMaterial* material, Color color ) {
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

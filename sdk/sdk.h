#pragma once
#include <cmath>
#include <Windows.h>
#include "cglobalvars.h"
#include "../sdk/cnetvars/cnetvars.h"
#include "../sdk/convar/convar.h"
#include "../tools/vmt/getvfunc.h"
#include "../tools/vmt/vmthooks.h"
#include "headers/vector.h"
#include "headers/color.h"
#include "headers/dt_recv2.h"
#include "headers/studio.h"
#include "headers/weaponlist.h"
#include "headers/bspflags.h"
using namespace std;

#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4996)
#pragma warning(disable:4244)

using CreateInterfaceFn = void *( * )( const char *pName, int *pReturnCode );

class CGameTrace;
class CUtil;
class CNetVars;
class CBaseCombatWeapon;
class matrix3x4_t;
class IMaterialVar;
class IMesh;
class IVertexBuffer;
class IIndexBuffer;
struct MaterialSystem_Config_t;
class VMatrix;
class matrix3x4_t;
class ITexture;
struct MaterialSystemHWID_t;
class IShader;
class IVertexTexture;
class IMorph;
class IMatRenderContext;
class ICallQueue;
struct MorphWeight_t;
class IFileList;
struct VertexStreamSpec_t;
struct ShaderStencilState_t;
struct MeshInstanceData_t;
class IClientMaterialSystem;
class CPaintMaterial;
class IPaintMapDataManager;
class IPaintMapTextureManager;
class GPUMemoryStats;
struct AspectRatioInfo_t;
struct CascadedShadowMappingState_t;
struct model_t;
struct VisibleFogVolumeInfo_t;
struct VisOverrideData_t;
struct WorldListInfo_t;
class IWorldRenderList;
class IBrushRenderer;
class IClientRenderable;
class IHandleEntity;
class bf_write;
class IMaterialProxyFactory;
class ITexture;
class IMaterialSystemHardwareConfig;
class CShadowMgr;
class IMaterial;
class KeyValues;
class InputEvent_t;
class INetMessage;
class INetChannelHandler;

using VertexFormat_t = unsigned __int64;
using trace_t = class CGameTrace;
using matrix3x4 = float[3][4];
using ModelInstanceHandle_t = int;
using FileHandle_t = void*;
using MaterialHandle_t = unsigned short;

#define me gInts.Engine->GetLocalPlayer()
#define GetBaseEntity gInts.EntList->GetClientEntity
#define GetBaseEntityFromHandle gInts.EntList->GetClientEntityFromHandle
#define FL_ONGROUND 1
#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1


inline wstring ToWstring( const char *text ) {
  size_t length = strlen( text ) + 1;
  wstring wc( length, L'#' );
  mbstowcs( &wc[0], text, length );
  return wc;
}

#define XASSERT( x ) if( !(x) ) Fatal( #x );

inline void Fatal( const char *fault ) {
  MessageBoxA( nullptr, fault, "FATAL ERROR", MB_ICONERROR | MB_TOPMOST );
  ExitProcess( 0 );
}

typedef struct player_info_s {
  char name[32];
  int userID;
  char guid[33];
  unsigned long friendsID;
  char friendsName[32];
  bool fakeplayer;
  bool ishltv;
  unsigned long customFiles[4];
  unsigned char filesDownloaded;
} player_info_t;

enum MoveType_t {
  MOVETYPE_NONE = 0, 
  /**< never moves */
  MOVETYPE_ISOMETRIC, 
  /**< For players */
  MOVETYPE_WALK, 
  /**< Player only - moving on the ground */
  MOVETYPE_STEP, 
  /**< gravity, special edge handling -- monsters use this */
  MOVETYPE_FLY, 
  /**< No gravity, but still collides with stuff */
  MOVETYPE_FLYGRAVITY, 
  /**< flies through the air + is affected by gravity */
  MOVETYPE_VPHYSICS, 
  /**< uses VPHYSICS for simulation */
  MOVETYPE_PUSH, 
  /**< no clip to world, push and crush */
  MOVETYPE_NOCLIP, 
  /**< No gravity, no collisions, still do velocity/avelocity
       */
  MOVETYPE_LADDER, 
  /**< Used by players only when going onto a ladder */
  MOVETYPE_OBSERVER, 
  /**< Observer movement, depends on player's observer mode*/
  MOVETYPE_CUSTOM, 
  /**< Allows the entity to describe its own physics */
  
  MOVETYPE_LAST = MOVETYPE_CUSTOM, 
  
  MOVETYPE_MAX_BITS = 4, 
};

class ClientClass {
 private:
  BYTE padding[8];
  
 public:
  const char *name;
  RecvTable *table;
  ClientClass *next;
  int classId;
};

class CHLClient {
 public:
  ClientClass *GetAllClasses() {
    using OriginalFn = ClientClass * ( __thiscall * )( PVOID );
    return getvfunc<OriginalFn>( this, 8 )( this );
  }
};

class CGlobals {
 public:
  float realtime;
  int framecount;
  float absoluteframetime;
  float curtime;
  float frametime;
  int maxclients;
  int tickcount;
  float interval_per_tick;
  float interpolation_amount;
  int Shots;
};

class CUserCmd {
 public:
  virtual ~CUserCmd() {}; // Destructor 0
  int command_number; // 4
  int tick_count; // 8
  Vector viewangles; // C
  float forwardmove; // 18
  float sidemove; // 1C
  float upmove; // 20
  int buttons; // 24
  byte impulse; // 28
  int weaponselect; // 2C
  int weaponsubtype; // 30
  int random_seed; // 34
  short mousedx; // 38
  short mousedy; // 3A
  bool hasbeenpredicted; // 3C;
};

enum ClientFrameStage_t {
  FRAME_UNDEFINED = -1, 
  // haven't run any frames yet
  FRAME_START, 
  // A network packet is being recieved
  FRAME_NET_UPDATE_START, 
  // Data has been received and we're going to start calling PostDataUpdate
  FRAME_NET_UPDATE_POSTDATAUPDATE_START, 
  // Data has been received and we've called PostDataUpdate on all data
  // recipients
  FRAME_NET_UPDATE_POSTDATAUPDATE_END, 
  // We've received all packets, we can now do interpolation, prediction, etc..
  FRAME_NET_UPDATE_END, 
  // We're about to start rendering the scene
  FRAME_RENDER_START, 
  // We've finished rendering the scene.
  FRAME_RENDER_END
};

enum tf_classes {
  TF2_Scout = 1, 
  TF2_Soldier = 3, 
  TF2_Pyro = 7, 
  TF2_Demoman = 4, 
  TF2_Heavy = 6, 
  TF2_Engineer = 9, 
  TF2_Medic = 5, 
  TF2_Sniper = 2, 
  TF2_Spy = 8, 
};

class CBaseEntity {
 public:
  template <typename T>
  T get( uint32_t off ) {
    return *( T * )( ( uint32_t )this + off );
  }
  
  template <typename T>
  void set( uint32_t off, T val ) {
    ( *( T * )( ( uint32_t )this + off ) ) = val;
  }
  
  Vector GetViewOffset() {
    DYNVAR_RETURN( Vector, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]" );
  }
  
  void SetCurrentCommand( CUserCmd *cmd ) {
    DYNVAR_SET( CUserCmd *, this - 4, cmd, "DT_BasePlayer", "localdata", "m_hConstraintEntity" );
  }
  
  CUserCmd *GetCurrentCommand() {
    DYNVAR_RETURN( CUserCmd *, this - 4, "DT_BasePlayer", "localdata", "m_hConstraintEntity" );
  }
  
  Vector GetVecOrigin() {
    DYNVAR_RETURN( Vector, this, "DT_BaseEntity", "m_vecOrigin" );
  }
  
  void SetVecOrigin( Vector origin ) {
    DYNVAR_SET( Vector, this, origin, "DT_BaseEntity", "m_vecOrigin" );
  }
  
  Vector &GetAbsOrigin() {
    typedef Vector&( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 9 )( this );
  }
  
  int ammo() {
    DYNVAR_RETURN( int, this + 4, "DT_BasePlayer", "localdata", "m_iAmmo" );
  }
  
  bool IsBaseCombatWeapon() {
    typedef bool( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 137 )( this );
  }
  
  int GetHitboxSet() {
    DYNVAR_RETURN( int, this, "DT_BaseAnimating", "m_nHitboxSet" );
  }
  
  int  GetTickBase( ) {
    DYNVAR_RETURN( int, this, "DT_BasePlayer", "localdata", "m_nTickBase" );
  }
  
  void UpTickBase() {
    static CDynamicNetvar<int >n( "DT_BasePlayer", "localdata", "m_nTickBase" );
    n.SetValue( this, n.GetValue( this ) + 1 );
  }
  
  float flSimulationTime() {
    DYNVAR_RETURN( float, this, "DT_BaseEntity", "m_flSimulationTime" );
  }
  
  MoveType_t GetMoveType() {
    DYNVAR_RETURN( MoveType_t, this, "DT_BaseEntity", "movetype" );
  }
  
  Vector GetAngles() {
    DYNVAR_RETURN( Vector, this, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]" );
  }
  
  Vector GetAnglesHTC() {
    DYNVAR_RETURN( Vector, this, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[1]" );
  }
  
  float GetPitch() {
    DYNVAR_RETURN( float, this, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]" );
  }
  
  float GetYaw() {
    DYNVAR_RETURN( float, this, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[1]" );
  }
  
  bool CanBackStab() {
    DYNVAR_RETURN( bool, this, "DT_TFWeaponKnife", "m_bReadyToBackstab" );
  }
  unsigned int ObserverTarget() {
    DYNVAR_RETURN(unsigned int, this, "DT_BasePlayer", "m_hObserverTarget");
  }
  int ObserverMode() {
    DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_iObserverMode");
  }

  Vector &GetAbsAngles() {
    typedef Vector&( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 10 )( this );
  }
  
  Vector GetWorldSpaceCenter( ) {
    Vector vMin, vMax;
    this->GetRenderBounds( vMin, vMax );
    Vector vWorldSpaceCenter = this->GetAbsOrigin();
    vWorldSpaceCenter.z += ( vMin.z + vMax.z ) / 2;
    return vWorldSpaceCenter;
  }
  
  DWORD *GetModel() {
    PVOID pRenderable = ( PVOID )( this + 0x4 );
    typedef DWORD*( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( pRenderable, 9 )( pRenderable );
  }
  
  bool SetupBones( matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime ) {
    PVOID pRenderable = ( PVOID )( this + 0x4 );
    typedef bool( __thiscall * OriginalFn )( PVOID, matrix3x4 *, int, int, float );
    return getvfunc<OriginalFn>( pRenderable, 16 )( pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime );
  }
  
  ClientClass *GetClientClass() {
    PVOID pNetworkable = ( PVOID )( this + 0x8 );
    typedef ClientClass*( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( pNetworkable, 2 )( pNetworkable );
  }
  
  bool IsDormant() {
    PVOID pNetworkable = ( PVOID )( this + 0x8 );
    typedef bool( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( pNetworkable, 8 )( pNetworkable );
  }
  
  int GetIndex() {
    PVOID pNetworkable = ( PVOID )( this + 0x8 );
    typedef int( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( pNetworkable, 9 )( pNetworkable );
  }
  
  void GetRenderBounds( Vector &mins, Vector &maxs ) {
    PVOID pRenderable = ( PVOID )( this + 0x4 );
    typedef void( __thiscall * OriginalFn )( PVOID, Vector &, Vector & );
    getvfunc<OriginalFn>( pRenderable, 20 )( pRenderable, mins, maxs );
  }
  
  matrix3x4 &GetRgflCoordinateFrame() {
    PVOID pRenderable = ( PVOID )( this + 0x4 );
    typedef matrix3x4&( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( pRenderable, 34 )( pRenderable );
  }
  
  int GetMaxHealth() {
    typedef int( __thiscall * OriginalFn )( void * );
    return getvfunc<OriginalFn>( this, 107 )( this );
  }
  int GetGroundEntity() {
    DYNVAR_RETURN( int, this, "DT_BasePlayer", "localdata", "m_hGroundEntity" );
  }
  
  int GetHealth() {
    DYNVAR_RETURN( int, this, "DT_BasePlayer", "m_iHealth" );
  }
  
  int GetTeamNum() {
    DYNVAR_RETURN( int, this, "DT_BaseEntity", "m_iTeamNum" );
  }
  
  int GetFlags() {
    DYNVAR_RETURN( int, this, "DT_BasePlayer", "m_fFlags" );
  }
  
  BYTE GetLifeState() {
    DYNVAR_RETURN( BYTE, this, "DT_BasePlayer", "m_lifeState" );
  }
  
  int GetClassId() {
    return this->GetClientClass()->classId;
  }
  int GetClass() {
    DYNVAR_RETURN( int, this, "DT_TFPlayer", "m_PlayerClass", "m_iClass" );
  }
  char *szGetClass() {
    DYNVAR( iClass, int, "DT_TFPlayer", "m_PlayerClass", "m_iClass" );
    
    switch( iClass.GetValue( this ) ) {
    case TF2_Scout:
      return "Scout";
      
    case TF2_Soldier:
      return "Soldier";
      
    case TF2_Pyro:
      return "Pyro";
      
    case TF2_Demoman:
      return "Demoman";
      
    case TF2_Heavy:
      return "Heavy";
      
    case TF2_Engineer:
      return "Engineer";
      
    case TF2_Medic:
      return "Medic";
      
    case TF2_Sniper:
      return "Sniper";
      
    case TF2_Spy:
      return "Spy";
      
    default:
      return "Other";
    }
  }
  
  int GetCond() {
    DYNVAR_RETURN( int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCond" );
  }
  
  Vector GetCollideableMins() {
    DYNVAR_RETURN( Vector, this, "DT_BaseEntity", "m_Collision", "m_vecMins" );
  }
  
  Vector GetCollideableMaxs() {
    DYNVAR_RETURN( Vector, this, "DT_BaseEntity", "m_Collision", "m_vecMaxs" );
  }
  
  Vector GetEyePosition() {
    DYNVAR_RETURN( Vector, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]" ) + this->GetAbsOrigin();
  }
  
  Vector GetEyeAngles() {
    DYNVAR_RETURN( Vector, this, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]" );
  }
  
  void SetEyeAngles( Vector angle ) {
    DYNVAR( eye, Vector, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]" );
    eye.SetValue( this, angle );
  }
  
  Vector GetAbsEyePosition() {
    DYNVAR_RETURN( Vector, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]" );
  }
  
  //CBaseEntity.cpp
  Vector GetHitbox( int hitbox );
  CBaseCombatWeapon *GetActiveWeapon();
  bool CanSee( CBaseEntity *pEntity, const Vector &pos );
  int GetCanSeeHitbox( CBaseEntity *pEntity, const Vector &pos );
};

class CObject : public CBaseEntity {
 public:
  CObject *ToBaseObject( CBaseEntity *pBaseEntity ) {
    return ( CObject * )( pBaseEntity );
  }
  
  CBaseEntity *GetOwner() {
    DYNVAR_RETURN( CBaseEntity *, this, "DT_BaseObject", "m_hBuilder" );
  }
  
  int GetLevel() {
    DYNVAR_RETURN( int, this, "DT_BaseObject", "m_iUpgradeLevel" );
  }
  
  bool IsSapped() {
    DYNVAR_RETURN( bool, this, "DT_BaseObject", "m_bHasSapper" );
  }
  
  bool IsBuilding() {
    DYNVAR_RETURN( bool, this, "DT_BaseObject", "m_bBuilding" );
  }
  
  float GetPercentageConstructed() {
    if( IsBuilding() ) {
      DYNVAR_RETURN( float, this, "DT_BaseObject", "m_flPercentageConstructed" );
    }
    
    return 0;
  }
  
  int GetHealth() {
    DYNVAR_RETURN( int, this, "DT_BaseObject", "m_iHealth" );
  }
  
  int GetState() {
    DYNVAR_RETURN( int, this, "DT_ObjectSentrygun", "m_iState" );
  }
  
  int GetUpgradedMetal() {
    DYNVAR_RETURN( int, this, "DT_BaseObject", "m_iUpgradeMetal" );
  }
  
  int GetTeamNum() {
    DYNVAR_RETURN( int, this, "DT_BaseEntity", "m_iTeamNum" );
  }
};

class CObjectDispenser : public CObject {
 public:
  int GetMetalReserve() {
    DYNVAR_RETURN( int, this, "DT_ObjectDispenser", "m_iAmmoMetal" );
  }
};

class CObjectSentryGun : public CObject {
 public:
  int GetRocket() {
    if( GetLevel() == 3 ) {
      DYNVAR_RETURN( int, this, "DT_ObjectSentrygun", "m_iAmmoRockets" );
    }
    
    return NULL;
  }
  
  int GetAmmo() {
    DYNVAR_RETURN( int, this, "DT_ObjectSentrygun", "m_iAmmoShells" );
  }
  
  int IsControlled() {
    DYNVAR_RETURN( bool, this, "DT_ObjectSentrygun", "m_bPlayerControlled" );
  }
  
  wchar_t *GetStateString() {
    switch( GetState() ) {
    case 1: {
      return L"Idle";
    }
    
    case 2: {
      return L"Attacking";
    }
    
    case 3: {
      return L"Upgrading";
    }
    
    default:
      return L"Building";
    }
  }
};

enum ButtonCode_t {
  BUTTON_CODE_INVALID = -1, 
  BUTTON_CODE_NONE = 0, 
  
  KEY_FIRST = 0, 
  
  KEY_NONE = KEY_FIRST, 
  KEY_0, 
  KEY_1, 
  KEY_2, 
  KEY_3, 
  KEY_4, 
  KEY_5, 
  KEY_6, 
  KEY_7, 
  KEY_8, 
  KEY_9, 
  KEY_A, 
  KEY_B, 
  KEY_C, 
  KEY_D, 
  KEY_E, 
  KEY_F, 
  KEY_G, 
  KEY_H, 
  KEY_I, 
  KEY_J, 
  KEY_K, 
  KEY_L, 
  KEY_M, 
  KEY_N, 
  KEY_O, 
  KEY_P, 
  KEY_Q, 
  KEY_R, 
  KEY_S, 
  KEY_T, 
  KEY_U, 
  KEY_V, 
  KEY_W, 
  KEY_X, 
  KEY_Y, 
  KEY_Z, 
  KEY_PAD_0, 
  KEY_PAD_1, 
  KEY_PAD_2, 
  KEY_PAD_3, 
  KEY_PAD_4, 
  KEY_PAD_5, 
  KEY_PAD_6, 
  KEY_PAD_7, 
  KEY_PAD_8, 
  KEY_PAD_9, 
  KEY_PAD_DIVIDE, 
  KEY_PAD_MULTIPLY, 
  KEY_PAD_MINUS, 
  KEY_PAD_PLUS, 
  KEY_PAD_ENTER, 
  KEY_PAD_DECIMAL, 
  KEY_LBRACKET, 
  KEY_RBRACKET, 
  KEY_SEMICOLON, 
  KEY_APOSTROPHE, 
  KEY_BACKQUOTE, 
  KEY_COMMA, 
  KEY_PERIOD, 
  KEY_SLASH, 
  KEY_BACKSLASH, 
  KEY_MINUS, 
  KEY_EQUAL, 
  KEY_ENTER, 
  KEY_SPACE, 
  KEY_BACKSPACE, 
  KEY_TAB, 
  KEY_CAPSLOCK, 
  KEY_NUMLOCK, 
  KEY_ESCAPE, 
  KEY_SCROLLLOCK, 
  KEY_INSERT, 
  KEY_DELETE, 
  KEY_HOME, 
  KEY_END, 
  KEY_PAGEUP, 
  KEY_PAGEDOWN, 
  KEY_BREAK, 
  KEY_LSHIFT, 
  KEY_RSHIFT, 
  KEY_LALT, 
  KEY_RALT, 
  KEY_LCONTROL, 
  KEY_RCONTROL, 
  KEY_LWIN, 
  KEY_RWIN, 
  KEY_APP, 
  KEY_UP, 
  KEY_LEFT, 
  KEY_DOWN, 
  KEY_RIGHT, 
  KEY_F1, 
  KEY_F2, 
  KEY_F3, 
  KEY_F4, 
  KEY_F5, 
  KEY_F6, 
  KEY_F7, 
  KEY_F8, 
  KEY_F9, 
  KEY_F10, 
  KEY_F11, 
  KEY_F12, 
  KEY_CAPSLOCKTOGGLE, 
  KEY_NUMLOCKTOGGLE, 
  KEY_SCROLLLOCKTOGGLE, 
  
  KEY_LAST = KEY_SCROLLLOCKTOGGLE, 
  KEY_COUNT = KEY_LAST - KEY_FIRST + 1, 
  
  // Mouse
  MOUSE_FIRST = KEY_LAST + 1, 
  
  MOUSE_LEFT = MOUSE_FIRST, 
  MOUSE_RIGHT, 
  MOUSE_MIDDLE, 
  MOUSE_4, 
  MOUSE_5, 
  MOUSE_WHEEL_UP, 
  // A fake button which is 'pressed' and 'released' when the
  // wheel is moved up
  MOUSE_WHEEL_DOWN, 
  // A fake button which is 'pressed' and 'released' when the
  // wheel is moved down
  
  MOUSE_LAST = MOUSE_WHEEL_DOWN, 
  MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1, 
};

class CObjectTeleporter : public CObject {
 public:
  char *GetStateString() {
    switch( GetState() ) {
    case 1: {
      return "Idle";
    }
    
    case 2: {
      return "Active";
    }
    
    case 4: {
      return "Teleporting";
    }
    
    case 6: {
      return "Charging";
    }
    
    default:
      return "Unknown";
    }
  }
  
  float GetRechargeTime() {
    DYNVAR_RETURN( float, this, "DT_ObjectTeleporter", 
                   "m_flCurrentRechargeDuration" );
  }
  
  int GetYawToExit() {
    DYNVAR_RETURN( int, this, "DT_ObjectTeleporter", "m_flYawToExit" );
  }
};

class CBaseCombatWeapon : public CBaseEntity {
 public:
  int GetSlot() {
    typedef int( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 327 )( this );
  }
  
  char *GetName() {
    typedef char *( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 329 )( this );
  }
  
  char *GetPrintName() {
    typedef char *( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 330 )( this );
  }
  
  float GetChargeTime() {
    DYNVAR_RETURN( float, this, "DT_WeaponPipebombLauncher", "PipebombLauncherLocalData", "m_flChargeBeginTime" );
  }
  
  float GetChargeDamage() {
    DYNVAR_RETURN( float, this, "DT_TFSniperRifle", "SniperRifleLocalData", "m_flChargedDamage" );
  }
  
  weaponid GetItemDefinitionIndex() {
    DYNVAR_RETURN( weaponid, this, "DT_EconEntity", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex" );
  }
  
  float m_flLastFireTime() {
    DYNVAR_RETURN( float, this, "DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flLastFireTime" );
  }
  float GetSwingRange( CBaseEntity *pLocal ) {
    typedef int( __thiscall * OriginalFn )( CBaseEntity * );
    return  4.0f + ( float )getvfunc<OriginalFn>( this, 451 )( pLocal ) / 10.0f;
  }
  bool DoSwingTrace( CBaseEntity *pLocal, trace_t *trace ) {
    typedef int( __thiscall * OriginalFn )( CBaseEntity *, trace_t * );
    return getvfunc<OriginalFn>( this, 453 )( pLocal, trace );
  }
};

enum class classId : int {
  CAI_BaseNPC = 0, 
  CBaseAnimating = 1, 
  CBaseAnimatingOverlay = 2, 
  CBaseAttributableItem = 3, 
  CBaseCombatCharacter = 4, 
  CBaseCombatWeapon = 5, 
  CBaseDoor = 6, 
  CBaseEntity = 7, 
  CBaseFlex = 8, 
  CBaseGrenade = 9, 
  CBaseObject = 10, 
  CBaseObjectUpgrade = 11, 
  CBaseParticleEntity = 12, 
  CBasePlayer = 13, 
  CBaseProjectile = 14, 
  CBasePropDoor = 15, 
  CBaseTeamObjectiveResource = 16, 
  CBaseTempEntity = 17, 
  CBaseViewModel = 18, 
  CBeam = 19, 
  CBoneFollower = 20, 
  CBonusDuckPickup = 21, 
  CBonusPack = 22, 
  CBonusRoundLogic = 23, 
  CBreakableProp = 24, 
  CBreakableSurface = 25, 
  CCaptureFlag = 26, 
  CCaptureFlagReturnIcon = 27, 
  CCaptureZone = 28, 
  CColorCorrection = 29, 
  CColorCorrectionVolume = 30, 
  CCurrencyPack = 31, 
  CDynamicLight = 32, 
  CDynamicProp = 33, 
  CEconEntity = 34, 
  CEconWearable = 35, 
  CEmbers = 36, 
  CEntityDissolve = 37, 
  CEntityFlame = 38, 
  CEntityParticleTrail = 39, 
  CEnvDetailController = 40, 
  CEnvParticleScript = 41, 
  CEnvProjectedTexture = 42, 
  CEnvQuadraticBeam = 43, 
  CEnvScreenEffect = 44, 
  CEnvScreenOverlay = 45, 
  CEnvTonemapController = 46, 
  CEnvWind = 47, 
  CEyeballBoss = 48, 
  CFireSmoke = 49, 
  CFireTrail = 50, 
  CFish = 51, 
  CFogController = 52, 
  CFuncAreaPortalWindow = 55, 
  CFuncConveyor = 56, 
  CFuncForceField = 57, 
  CFuncLadder = 58, 
  CFuncMonitor = 59, 
  CFuncOccluder = 60, 
  CFuncPasstimeGoal = 61, 
  CFuncReflectiveGlass = 62, 
  CFuncRespawnRoom = 63, 
  CFuncRespawnRoomVisualizer = 64, 
  CFuncRotating = 65, 
  CFuncSmokeVolume = 66, 
  CFuncTrackTrain = 67, 
  CFunc_Dust = 53, 
  CFunc_LOD = 54, 
  CGameRulesProxy = 68, 
  CHalloweenGiftPickup = 69, 
  CHalloweenPickup = 70, 
  CHalloweenSoulPack = 71, 
  CHandleTest = 72, 
  CHeadlessHatman = 73, 
  CHightower_TeleportVortex = 74, 
  CInfoLadderDismount = 75, 
  CInfoLightingRelative = 76, 
  CInfoOverlayAccessor = 77, 
  CLaserDot = 78, 
  CLightGlow = 79, 
  CMannVsMachineStats = 80, 
  CMaterialModifyControl = 81, 
  CMerasmus = 82, 
  CMerasmusDancer = 83, 
  CMonsterResource = 84, 
  CObjectCartDispenser = 85, 
  CObjectDispenser = 86, 
  CObjectSapper = 87, 
  CObjectSentrygun = 88, 
  CObjectTeleporter = 89, 
  CParticleFire = 90, 
  CParticlePerformanceMonitor = 91, 
  CParticleSystem = 92, 
  CPasstimeBall = 93, 
  CPasstimeGun = 94, 
  CPhysBox = 95, 
  CPhysBoxMultiplayer = 96, 
  CPhysMagnet = 99, 
  CPhysicsProp = 97, 
  CPhysicsPropMultiplayer = 98, 
  CPlasma = 100, 
  CPlayerDestructionDispenser = 101, 
  CPlayerResource = 102, 
  CPointCamera = 103, 
  CPointCommentaryNode = 104, 
  CPoseController = 105, 
  CPrecipitation = 106, 
  CPropVehicleDriveable = 107, 
  CRagdollManager = 108, 
  CRagdollProp = 109, 
  CRagdollPropAttached = 110, 
  CRobotDispenser = 111, 
  CRopeKeyframe = 112, 
  CSceneEntity = 113, 
  CShadowControl = 114, 
  CSlideshowDisplay = 115, 
  CSmokeStack = 116, 
  CSniperDot = 117, 
  CSpotlightEnd = 118, 
  CSprite = 119, 
  CSpriteOriented = 120, 
  CSpriteTrail = 121, 
  CSteamJet = 122, 
  CSun = 123, 
  CTEArmorRicochet = 128, 
  CTEBSPDecal = 141, 
  CTEBaseBeam = 129, 
  CTEBeamEntPoint = 130, 
  CTEBeamEnts = 131, 
  CTEBeamFollow = 132, 
  CTEBeamLaser = 133, 
  CTEBeamPoints = 134, 
  CTEBeamRing = 135, 
  CTEBeamRingPoint = 136, 
  CTEBeamSpline = 137, 
  CTEBloodSprite = 138, 
  CTEBloodStream = 139, 
  CTEBreakModel = 140, 
  CTEBubbleTrail = 143, 
  CTEBubbles = 142, 
  CTEClientProjectile = 144, 
  CTEDecal = 145, 
  CTEDust = 146, 
  CTEDynamicLight = 147, 
  CTEEffectDispatch = 148, 
  CTEEnergySplash = 149, 
  CTEExplosion = 150, 
  CTEFireBullets = 151, 
  CTEFizz = 152, 
  CTEFootprintDecal = 153, 
  CTEGaussExplosion = 154, 
  CTEGlowSprite = 155, 
  CTEImpact = 156, 
  CTEKillPlayerAttachments = 157, 
  CTELargeFunnel = 158, 
  CTEMetalSparks = 160, 
  CTEMuzzleFlash = 161, 
  CTEParticleSystem = 162, 
  CTEPhysicsProp = 163, 
  CTEPlayerAnimEvent = 164, 
  CTEPlayerDecal = 165, 
  CTEProjectedDecal = 166, 
  CTEShatterSurface = 167, 
  CTEShowLine = 168, 
  CTESmoke = 170, 
  CTESparks = 171, 
  CTESprite = 172, 
  CTESpriteSpray = 173, 
  CTETFBlood = 176, 
  CTETFExplosion = 177, 
  CTETFParticleEffect = 178, 
  CTEWorldDecal = 179, 
  CTFAmmoPack = 180, 
  CTFBall_Ornament = 181, 
  CTFBaseBoss = 182, 
  CTFBaseProjectile = 183, 
  CTFBaseRocket = 184, 
  CTFBat = 185, 
  CTFBat_Fish = 186, 
  CTFBat_Giftwrap = 187, 
  CTFBat_Wood = 188, 
  CTFBonesaw = 189, 
  CTFBotHintEngineerNest = 190, 
  CTFBottle = 191, 
  CTFBreakableMelee = 192, 
  CTFBreakableSign = 193, 
  CTFBuffItem = 194, 
  CTFCannon = 195, 
  CTFChargedSMG = 196, 
  CTFCleaver = 197, 
  CTFClub = 198, 
  CTFCompoundBow = 199, 
  CTFCrossbow = 200, 
  CTFDRGPomson = 201, 
  CTFDroppedWeapon = 202, 
  CTFFireAxe = 203, 
  CTFFists = 204, 
  CTFFlameManager = 205, 
  CTFFlameRocket = 206, 
  CTFFlameThrower = 207, 
  CTFFlareGun = 208, 
  CTFFlareGun_Revenge = 209, 
  CTFGameRulesProxy = 210, 
  CTFGasManager = 211, 
  CTFGenericBomb = 212, 
  CTFGlow = 213, 
  CTFGrapplingHook = 214, 
  CTFGrenadeLauncher = 215, 
  CTFGrenadePipebombProjectile = 216, 
  CTFHalloweenMinigame = 217, 
  CTFHalloweenMinigame_FallingPlatforms = 218, 
  CTFHellZap = 219, 
  CTFItem = 220, 
  CTFJar = 221, 
  CTFJarGas = 222, 
  CTFJarMilk = 223, 
  CTFKatana = 224, 
  CTFKnife = 225, 
  CTFLaserPointer = 226, 
  CTFLunchBox = 227, 
  CTFLunchBox_Drink = 228, 
  CTFMechanicalArm = 229, 
  CTFMedigunShield = 230, 
  CTFMiniGame = 231, 
  CTFMinigameLogic = 232, 
  CTFMinigun = 233, 
  CTFObjectiveResource = 234, 
  CTFPEPBrawlerBlaster = 240, 
  CTFParachute = 235, 
  CTFParachute_Primary = 236, 
  CTFParachute_Secondary = 237, 
  CTFParticleCannon = 238, 
  CTFPasstimeLogic = 239, 
  CTFPipebombLauncher = 241, 
  CTFPistol = 242, 
  CTFPistol_Scout = 243, 
  CTFPistol_ScoutPrimary = 244, 
  CTFPistol_ScoutSecondary = 245, 
  CTFPlayer = 246, 
  CTFPlayerDestructionLogic = 247, 
  CTFPlayerResource = 248, 
  CTFPointManager = 249, 
  CTFPowerupBottle = 250, 
  CTFProjectile_Arrow = 251, 
  CTFProjectile_BallOfFire = 252, 
  CTFProjectile_Cleaver = 253, 
  CTFProjectile_EnergyBall = 254, 
  CTFProjectile_EnergyRing = 255, 
  CTFProjectile_Flare = 256, 
  CTFProjectile_GrapplingHook = 257, 
  CTFProjectile_HealingBolt = 258, 
  CTFProjectile_Jar = 259, 
  CTFProjectile_JarGas = 260, 
  CTFProjectile_JarMilk = 261, 
  CTFProjectile_MechanicalArmOrb = 262, 
  CTFProjectile_Rocket = 263, 
  CTFProjectile_SentryRocket = 264, 
  CTFProjectile_SpellBats = 265, 
  CTFProjectile_SpellFireball = 266, 
  CTFProjectile_SpellKartBats = 267, 
  CTFProjectile_SpellKartOrb = 268, 
  CTFProjectile_SpellLightningOrb = 269, 
  CTFProjectile_SpellMeteorShower = 270, 
  CTFProjectile_SpellMirv = 271, 
  CTFProjectile_SpellPumpkin = 272, 
  CTFProjectile_SpellSpawnBoss = 273, 
  CTFProjectile_SpellSpawnHorde = 274, 
  CTFProjectile_SpellSpawnZombie = 275, 
  CTFProjectile_SpellTransposeTeleport = 276, 
  CTFProjectile_Throwable = 277, 
  CTFProjectile_ThrowableBreadMonster = 278, 
  CTFProjectile_ThrowableBrick = 279, 
  CTFProjectile_ThrowableRepel = 280, 
  CTFPumpkinBomb = 281, 
  CTFRagdoll = 282, 
  CTFRaygun = 283, 
  CTFReviveMarker = 284, 
  CTFRevolver = 285, 
  CTFRobotArm = 286, 
  CTFRobotDestructionLogic = 290, 
  CTFRobotDestruction_Robot = 287, 
  CTFRobotDestruction_RobotGroup = 288, 
  CTFRobotDestruction_RobotSpawn = 289, 
  CTFRocketLauncher = 291, 
  CTFRocketLauncher_AirStrike = 292, 
  CTFRocketLauncher_DirectHit = 293, 
  CTFRocketLauncher_Mortar = 294, 
  CTFRocketPack = 295, 
  CTFSMG = 305, 
  CTFScatterGun = 296, 
  CTFShotgun = 297, 
  CTFShotgunBuildingRescue = 302, 
  CTFShotgun_HWG = 298, 
  CTFShotgun_Pyro = 299, 
  CTFShotgun_Revenge = 300, 
  CTFShotgun_Soldier = 301, 
  CTFShovel = 303, 
  CTFSlap = 304, 
  CTFSniperRifle = 306, 
  CTFSniperRifleClassic = 307, 
  CTFSniperRifleDecap = 308, 
  CTFSodaPopper = 309, 
  CTFSpellBook = 310, 
  CTFStickBomb = 311, 
  CTFStunBall = 312, 
  CTFSword = 313, 
  CTFSyringeGun = 314, 
  CTFTankBoss = 315, 
  CTFTauntProp = 316, 
  CTFTeam = 317, 
  CTFThrowable = 318, 
  CTFViewModel = 319, 
  CTFWeaponBase = 320, 
  CTFWeaponBaseGrenadeProj = 321, 
  CTFWeaponBaseGun = 322, 
  CTFWeaponBaseMelee = 323, 
  CTFWeaponBaseMerasmusGrenade = 324, 
  CTFWeaponBuilder = 325, 
  CTFWeaponFlameBall = 326, 
  CTFWeaponInvis = 327, 
  CTFWeaponPDA = 328, 
  CTFWeaponPDAExpansion_Dispenser = 332, 
  CTFWeaponPDAExpansion_Teleporter = 333, 
  CTFWeaponPDA_Engineer_Build = 329, 
  CTFWeaponPDA_Engineer_Destroy = 330, 
  CTFWeaponPDA_Spy = 331, 
  CTFWeaponSapper = 334, 
  CTFWearable = 335, 
  CTFWearableCampaignItem = 336, 
  CTFWearableDemoShield = 337, 
  CTFWearableItem = 338, 
  CTFWearableLevelableItem = 339, 
  CTFWearableRazorback = 340, 
  CTFWearableRobotArm = 341, 
  CTFWearableVM = 342, 
  CTFWrench = 343, 
  CTeam = 124, 
  CTeamRoundTimer = 126, 
  CTeamTrainWatcher = 127, 
  CTeamplayRoundBasedRulesProxy = 125, 
  CTeleportVortex = 159, 
  CTesla = 169, 
  CTestTraceline = 175, 
  CTest_ProxyToggle_Networkable = 174, 
  CVGuiScreen = 344, 
  CVoteController = 345, 
  CWaterBullet = 346, 
  CWaterLODControl = 347, 
  CWeaponIFMBase = 348, 
  CWeaponIFMBaseCamera = 349, 
  CWeaponIFMSteadyCam = 350, 
  CWeaponMedigun = 351, 
  CWorld = 352, 
  CZombie = 353, 
  DustTrail = 354, 
  MovieExplosion = 355, 
  NextBotCombatCharacter = 356, 
  ParticleSmokeGrenade = 357, 
  RocketTrail = 358, 
  SmokeTrail = 359, 
  SporeExplosion = 360, 
  SporeTrail = 361
};

class IGameEvent {
 public:
  virtual ~IGameEvent() {
  };
  virtual const char *GetName() const = 0; // get event name
  
  virtual bool IsReliable() const = 0; // if event handled reliable
  virtual bool IsLocal() const = 0; // if event is never networked
  virtual bool IsEmpty(const char *keyName = nullptr ) = 0; // check if data field exists
    
  // Data access
  virtual bool GetBool( const char *keyName = nullptr, bool defaultValue = false ) = 0;
  virtual int GetInt( const char *keyName = nullptr, int defaultValue = 0 ) = 0;
  virtual float GetFloat( const char *keyName = nullptr, float defaultValue = 0.0f ) = 0;
  virtual const char *GetString( const char *keyName = nullptr, const char *defaultValue = "" ) = 0;
                                 
  virtual void SetBool( const char *keyName, bool value ) = 0;
  virtual void SetInt( const char *keyName, int value ) = 0;
  virtual void SetFloat( const char *keyName, float value ) = 0;
  virtual void SetString( const char *keyName, const char *value ) = 0;
};

class IAppSystem {
 public:
  // Here's where the app systems get to learn about each other
  virtual bool Connect( CreateInterfaceFn factory ) = 0;
  virtual void Disconnect() = 0;
  
  // Here's where systems can access other interfaces implemented by this object
  // Returns NULL if it doesn't implement the requested interface
  virtual void *QueryInterface( const char *pInterfaceName ) = 0;
  
  // Init, shutdown
  virtual int Init() = 0;
  virtual void Shutdown( char *reason ) = 0;
};

enum AnalogCode_t {
  // needed but not rly
};

class IInputSystem : public IAppSystem {
 public:
  // Attach, detach input system from a particular window
  // This window should be the root window for the application
  // Only 1 window should be attached at any given time.
  virtual void AttachToWindow( void *hWnd ) = 0;
  virtual void DetachFromWindow() = 0;
  
  // Enables/disables input. PollInputState will not update current
  // button/analog states when it is called if the system is disabled.
  virtual void EnableInput( bool bEnable ) = 0;
  
  // Enables/disables the windows message pump. PollInputState will not
  // Peek/Dispatch messages if this is disabled
  virtual void EnableMessagePump( bool bEnable ) = 0;
  
  // Polls the current input state
  virtual void PollInputState() = 0;
  
  // Gets the time of the last polling in ms
  virtual int GetPollTick() const = 0;
  
  // Is a button down? "Buttons" are binary-state input devices (mouse buttons, 
  // keyboard keys)
  virtual bool IsButtonDown( ButtonCode_t code ) const = 0;
  
  // Returns the tick at which the button was pressed and released
  virtual int GetButtonPressedTick( ButtonCode_t code ) const = 0;
  virtual int GetButtonReleasedTick( ButtonCode_t code ) const = 0;
  
  // Gets the value of an analog input device this frame
  // Includes joysticks, mousewheel, mouse
  virtual int GetAnalogValue( AnalogCode_t code ) const = 0;
  
  // Gets the change in a particular analog input device this frame
  // Includes joysticks, mousewheel, mouse
  virtual int GetAnalogDelta( AnalogCode_t code ) const = 0;
  
  // Returns the input events since the last poll
  virtual int GetEventCount() const = 0;
  virtual const InputEvent_t *GetEventData() const = 0;
  
  // Posts a user-defined event into the event queue; this is expected
  // to be called in overridden wndprocs connected to the root panel.
  virtual void PostUserEvent( const InputEvent_t &event ) = 0;
  
  // Returns the number of joysticks
  virtual int GetJoystickCount() const = 0;
  
  // Enable/disable joystick, it has perf costs
  virtual void EnableJoystickInput( int nJoystick, bool bEnable ) = 0;
  
  // Enable/disable diagonal joystick POV (simultaneous POV buttons down)
  virtual void EnableJoystickDiagonalPOV( int nJoystick, bool bEnable ) = 0;
  
  // Sample the joystick and append events to the input queue
  virtual void SampleDevices( void ) = 0;
  
  virtual void SetRumble( float fLeftMotor, float fRightMotor, 
                          int userId = -1 ) = 0;
  virtual void StopRumble( void ) = 0;
  
  // Resets the input state
  virtual void ResetInputState() = 0;
  
  // Sets a player as the primary user - all other controllers will be ignored.
  virtual void SetPrimaryUserId( int userId ) = 0;
  
  // Convert back + forth between ButtonCode/AnalogCode + strings
  virtual const char *ButtonCodeToString( ButtonCode_t code ) const = 0;
  virtual const char *AnalogCodeToString( AnalogCode_t code ) const = 0;
  virtual ButtonCode_t StringToButtonCode( const char *pString ) const = 0;
  virtual AnalogCode_t StringToAnalogCode( const char *pString ) const = 0;
  
  // Sleeps until input happens. Pass a negative number to sleep infinitely
  virtual void SleepUntilInput( int nMaxSleepTimeMS = -1 ) = 0;
  
  // Convert back + forth between virtual codes + button codes
  // FIXME: This is a temporary piece of code
  virtual ButtonCode_t VirtualKeyToButtonCode( int nVirtualKey ) const = 0;
  virtual int ButtonCodeToVirtualKey( ButtonCode_t code ) const = 0;
  virtual ButtonCode_t ScanCodeToButtonCode( int lParam ) const = 0;
  
  // How many times have we called PollInputState?
  virtual int GetPollCount() const = 0;
  
  // Sets the cursor position
  virtual void SetCursorPosition( int x, int y ) = 0;
  
  // NVNT get address to haptics interface
  virtual void *GetHapticsInterfaceAddress() const = 0;
  
  virtual void SetNovintPure( bool bPure ) = 0;
  
  // read and clear accumulated raw input values
  virtual bool GetRawMouseAccumulators( int &accumX, int &accumY ) = 0;
  
  // tell the input system that we're not a game, we're console text mode.
  // this is used for dedicated servers to not initialize joystick system.
  // this needs to be called before CInputSystem::Init (e.g. in PreInit of
  // some system) if you want ot prevent the joystick system from ever
  // being initialized.
  virtual void SetConsoleTextMode( bool bConsoleTextMode ) = 0;
};

class ICvar : public IAppSystem {
 public:
  // Allocate a unique DLL identifier
  virtual int AllocateDLLIdentifier() = 0;
  
  // Register, unregister commands
  virtual void RegisterConCommand( ConCommandBase *pCommandBase ) = 0;
  virtual void UnregisterConCommand( ConCommandBase *pCommandBase ) = 0;
  virtual void UnregisterConCommands( int id ) = 0;
  
  // If there is a +<varname> <value> on the command line, this returns the
  // value. Otherwise, it returns NULL.
  virtual const char *GetCommandLineValue( const char *pVariableName ) = 0;
  
  // Try to find the cvar pointer by name
  virtual ConCommandBase *FindCommandBase( const char *name ) = 0;
  virtual const ConCommandBase *FindCommandBase( const char *name ) const = 0;
  virtual ConVar *FindVar( const char *var_name ) = 0;
  virtual const ConVar *FindVar( const char *var_name ) const = 0;
  virtual ConCommand *FindCommand( const char *name ) = 0;
  virtual const ConCommand *FindCommand( const char *name ) const = 0;
  
  // Get first ConCommandBase to allow iteration
  virtual ConCommandBase *GetCommands( void ) = 0;
  virtual const ConCommandBase *GetCommands( void ) const = 0;
  
  // Install a global change callback (to be called when any convar changes)
  virtual void InstallGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
  virtual void RemoveGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
  virtual void CallGlobalChangeCallbacks( ConVar *var, const char *pOldString, float flOldValue ) = 0;
                                          
  // Install a console printer
  virtual void InstallConsoleDisplayFunc( void *pDisplayFunc ) = 0;
  virtual void RemoveConsoleDisplayFunc( void *pDisplayFunc ) = 0;
  virtual void ConsoleColorPrintf( const Color &clr, const char *pFormat, ... ) const = 0;
  virtual void ConsolePrintf( const char *pFormat, ... ) const = 0;
  virtual void ConsoleDPrintf( const char *pFormat, ... ) const = 0;
  
  // Reverts cvars which contain a specific flag
  virtual void RevertFlaggedConVars( int nFlag ) = 0;
  
  // Method allowing the engine ICvarQuery interface to take over
  // A little hacky, owing to the fact the engine is loaded
  // well after ICVar, so we can't use the standard connect pattern
  virtual void InstallCVarQuery( void *pQuery ) = 0;
  
#if defined(_X360)
  virtual void PublishToVXConsole() = 0;
#endif
  virtual bool IsMaterialThreadSetAllowed() const = 0;
  virtual void QueueMaterialThreadSetValue( ConVar *pConVar, const char *pValue ) = 0;
  virtual void QueueMaterialThreadSetValue( ConVar *pConVar, int nValue ) = 0;
  virtual void QueueMaterialThreadSetValue( ConVar *pConVar, float flValue ) = 0;
  virtual bool HasQueuedMaterialThreadConVarSets() const = 0;
  virtual int ProcessQueuedMaterialThreadConVarSets() = 0;
  
 protected:
  class ICVarIteratorInternal;
  
 public:
  /// Iteration over all cvars.
  /// (THIS IS A SLOW OPERATION AND YOU SHOULD AVOID IT.)
  /// usage:
  /// { ICVar::Iterator iter(g_pCVar);
  ///   for ( iter.SetFirst() ; iter.IsValid() ; iter.Next() )
  ///   {
  ///       ConCommandBase *cmd = iter.Get();
  ///   }
  /// }
  /// The Iterator class actually wraps the internal factory methods
  /// so you don't need to worry about new/delete -- scope takes care
  ///  of it.
  /// We need an iterator like this because we can't simply return a
  /// pointer to the internal data type that contains the cvars --
  /// it's a custom, protected class with unusual semantics and is
  /// prone to change.
  class Iterator {
   public:
    inline Iterator( ICvar *icvar );
    inline ~Iterator( void );
    inline void SetFirst( void );
    inline void Next( void );
    inline bool IsValid( void );
    inline ConCommandBase *Get( void );
    
   private:
    ICVarIteratorInternal *m_pIter;
  };
  
 protected:
  // internals for  ICVarIterator
  class ICVarIteratorInternal {
   public:
    // warning: delete called on 'ICvar::ICVarIteratorInternal' that is abstract
    // but has non-virtual destructor [-Wdelete-non-virtual-dtor]
    virtual ~ICVarIteratorInternal() {
    }
    
    virtual void SetFirst( void ) = 0;
    virtual void Next( void ) = 0;
    virtual bool IsValid( void ) = 0;
    virtual ConCommandBase *Get( void ) = 0;
  };
  
  virtual ICVarIteratorInternal *FactoryInternalIterator( void ) = 0;
  friend class Iterator;
};

typedef struct netadr_s {
 public:
  netadr_s() {
    SetIP( 0 );
    SetPort( 0 );
    // SetType(NA_IP);
  }
  
  netadr_s( const char *pch ) {
    SetFromString( pch );
  }
  
  void Clear(); // invalids Address
  
  // void SetType(netadrtype_t type);
  void SetPort( unsigned short port );
  bool SetFromSockadr( const struct sockaddr *s );
  void SetIP(    unsigned int unIP ); // Sets IP.  unIP is in host order (little-endian)
  void SetIPAndPort( unsigned int unIP, unsigned short usPort ) {
    SetIP( unIP );
    SetPort( usPort );
  }
  
  void SetFromString( const char *pch, bool bUseDNS = false ); // if bUseDNS is true then do a DNS
// lookup if needed

  bool CompareAdr( const netadr_s &a, bool onlyBase = false ) const;
  bool CompareClassBAdr( const netadr_s &a ) const;
  bool CompareClassCAdr( const netadr_s &a ) const;
  
  // netadrtype_t GetType() const;
  unsigned short GetPort() const;
  const char *ToString(
    bool onlyBase = false ) const; // returns xxx.xxx.xxx.xxx:ppppp
  void ToSockadr( struct sockaddr *s ) const;
  unsigned int GetIP() const;
  
  bool IsLocalhost() const; // true, if this is the localhost IP
  bool IsLoopback() const; // true if engine loopback buffers are used
  bool IsReservedAdr() const; // true, if this is a private LAN IP
  bool IsValid() const; // ip & port != 0
  void SetFromSocket( int hSocket );
  // These function names are decorated because the Xbox360 defines macros for
  // ntohl and htonl
  unsigned long addr_ntohl() const;
  unsigned long addr_htonl() const;
  
  bool operator==( const netadr_s &netadr ) const {
    return ( CompareAdr( netadr ) );
  }
  
  bool operator<( const netadr_s &netadr ) const;
  
 public: // members are public to avoid to much changes
// netadrtype_t type;
  unsigned char ip[4];
  unsigned short port;
} netadr_t;

#define MAX_FLOWS 2  // in & out
#define MAX_STREAMS 2
#define MAX_OSPATH 260
#define SUBCHANNEL_FREE 0  // subchannel is free to use

class INetChannelInfo {
 public:
  enum {
    GENERIC = 0, 
    // must be first and is default group
    LOCALPLAYER, 
    // bytes for local player entity update
    OTHERPLAYERS, 
    // bytes for other players update
    ENTITIES, 
    // all other entity bytes
    SOUNDS, 
    // game sounds
    EVENTS, 
    // event messages
    TEMPENTS, 
    // temp entities
    USERMESSAGES, 
    // user messages
    ENTMESSAGES, 
    // entity messages
    VOICE, 
    // voice data
    STRINGTABLE, 
    // a stringtable update
    MOVE, 
    // client move cmds
    STRINGCMD, 
    // string command
    SIGNON, 
    // various signondata
    TOTAL, 
    // must be last and is not a real group
  };
  
  virtual const char *GetName( void ) const = 0; // get channel name
  virtual const char *GetAddress(    void ) const = 0; // get channel IP address as string
  virtual float GetTime( void ) const = 0; // current net time
  virtual float GetTimeConnected(    void ) const = 0; // get connection time in seconds
  virtual int GetBufferSize( void ) const = 0; // netchannel packet history size
  virtual int GetDataRate( void ) const = 0; // send data rate in byte/sec
  
  virtual bool IsLoopback( void ) const = 0; // true if loopback channel
  virtual bool IsTimingOut( void ) const = 0; // true if timing out
  virtual bool IsPlayback( void ) const = 0; // true if demo playback
  
  virtual float GetLatency( int flow )
  const = 0; // current latency (RTT), more accurate but jittering
  virtual float GetAvgLatency(    int flow ) const = 0; // average packet latency in seconds
  virtual float GetAvgLoss( int flow ) const = 0; // avg packet loss[0..1]
  virtual float GetAvgChoke( int flow ) const = 0; // avg packet choke[0..1]
  virtual float GetAvgData( int flow ) const = 0; // data flow in bytes/sec
  virtual float GetAvgPackets( int flow ) const = 0; // avg packets/sec
  virtual int GetTotalData( int flow ) const = 0; // total flow in/out in bytes
  virtual int GetTotalPackets( int flow ) const = 0;
  virtual int GetSequenceNr( int flow ) const = 0; // last send seq number
  virtual bool IsValidPacket( int flow, int frame_number )
  const = 0; // true if packet was not lost/dropped/chocked/flushed
  virtual float GetPacketTime(    int flow, int frame_number ) const = 0; // time when packet was send
  virtual int GetPacketBytes( int flow, int frame_number, int group ) const = 0; // group size of this packet
  virtual bool GetStreamProgress( int flow, int *received, int *total )
  const = 0; // TCP progress if transmitting
  virtual float GetTimeSinceLastReceived(    void ) const = 0; // get time since last recieved packet in seconds
  virtual float GetCommandInterpolationAmount( int flow, int frame_number ) const = 0;
  virtual void GetPacketResponseLatency( int flow, int frame_number, int *pnLatencyMsecs, int *pnChoke ) const = 0;
  virtual void GetRemoteFramerate(    float *pflFrameTime, float *pflFrameTimeStdDeviation, 
  float *pflFrameStartTimeStdDeviation ) const = 0;
    
  virtual float GetTimeoutSeconds() const = 0;
};

class INetChannel : public INetChannelInfo {
 public:
  virtual ~INetChannel( void ) {
  };
  
  virtual void SetDataRate( float rate ) = 0;
  virtual bool RegisterMessage( INetMessage *msg ) = 0;
  virtual bool StartStreaming( unsigned int challengeNr ) = 0;
  virtual void ResetStreaming( void ) = 0;
  virtual void SetTimeout( float seconds ) = 0;
  // virtual void SetDemoRecorder(IDemoRecorder *recorder) = 0;
  virtual void SetChallengeNr( unsigned int chnr ) = 0;
  
  virtual void Reset( void ) = 0;
  virtual void Clear( void ) = 0;
  virtual void Shutdown( const char *reason ) = 0;
  
  virtual void ProcessPlayback( void ) = 0;
  virtual bool ProcessStream( void ) = 0;
  virtual void ProcessPacket( struct netpacket_s *packet, bool bHasHeader ) = 0;
  
  virtual bool SendNetMsg( INetMessage &msg, bool bForceReliable = false, bool bVoice = false ) = 0;
  // virtual bool SendData(bf_write &msg, bool bReliable = true) = 0;
  virtual bool SendFile( const char *filename, unsigned int transferID ) = 0;
  virtual void DenyFile( const char *filename, unsigned int transferID ) = 0;
  virtual void RequestFile_OLD(    const char *filename, unsigned int    transferID ) = 0; // get rid of this function when we version the
  virtual void SetChoked( void ) = 0;
  virtual int SendDatagram( bf_write *data ) = 0;
  virtual bool Transmit( bool onlyReliable = false ) = 0;
  
  virtual const netadr_t &GetRemoteAddress( void ) const = 0;
  virtual INetChannelHandler *GetMsgHandler( void ) const = 0;
  virtual int GetDropNumber( void ) const = 0;
  virtual int GetSocket( void ) const = 0;
  virtual unsigned int GetChallengeNr( void ) const = 0;
  virtual void GetSequenceData( int &nOutSequenceNr, int &nInSequenceNr, int &nOutSequenceNrAck ) = 0;
  virtual void SetSequenceData( int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck ) = 0;
                                
  virtual void UpdateMessageStats( int msggroup, int bits ) = 0;
  virtual bool CanPacket( void ) const = 0;
  virtual bool IsOverflowed( void ) const = 0;
  virtual bool IsTimedOut( void ) const = 0;
  virtual bool HasPendingReliableData( void ) = 0;
  
  virtual void SetFileTransmissionMode( bool bBackgroundMode ) = 0;
  virtual void SetCompressionMode( bool bUseCompression ) = 0;
  virtual unsigned int RequestFile( const char *filename ) = 0;
  float GetTimeSinceLastReceived(
    void ) const override = 0; // get time since last received packet in seconds
    
  virtual void SetMaxBufferSize( bool bReliable, int nBYTEs, bool bVoice = false ) = 0;
                                 
  virtual bool IsNull() const = 0;
  virtual int GetNumBitsWritten( bool bReliable ) = 0;
  virtual void SetInterpolationAmount( float flInterpolationAmount ) = 0;
  virtual void SetRemoteFramerate( float flFrameTime, float flFrameTimeStdDeviation ) = 0;
                                   
  // Max # of payload BYTEs before we must split/fragment the packet
  virtual void SetMaxRoutablePayloadSize( int nSplitSize ) = 0;
  virtual int GetMaxRoutablePayloadSize() = 0;
  
  virtual int GetProtocolVersion() = 0;
};

class INetMessage {
 public:
  virtual ~INetMessage() {
  };
  
  // Use these to setup who can hear whose voice.
  // Pass in client indices (which are their ent indices - 1).
  
  virtual void SetNetChannel(    INetChannel *netchan ) = 0; // netchannel this message is from/for
  virtual void SetReliable(    bool state ) = 0; // set to true if it's a reliable message
    
  virtual bool Process(    void ) = 0; // calles the recently set handler to process this message
    
  virtual bool ReadFromBuffer(    uintptr_t &buffer ) = 0; // returns true if parsing was OK
  virtual bool WriteToBuffer(    uintptr_t &buffer ) = 0; // returns true if writing was OK
    
  virtual bool IsReliable(    void ) const = 0; // true, if message needs reliable handling
    
  virtual int GetType(    void ) const = 0; // returns module specific header tag eg svc_serverinfo
  virtual int GetGroup(    void ) const = 0; // returns net message group of this message
  virtual const char *GetName(    void ) const = 0; // returns network message name, eg "svc_serverinfo"
  virtual INetChannel *GetNetChannel( void ) const = 0;
  virtual const char *ToString(    void ) const = 0; // returns a human readable string about message content
};

class INetChannelHandler {
 public:
  virtual ~INetChannelHandler( void ) {
  };
  
  virtual void ConnectionStart(    INetChannel    * chan ) = 0; // called first time network channel is established
    
  virtual void ConnectionClosing(    const char    * reason ) = 0; // network channel is being closed by remote site
    
  virtual void ConnectionCrashed(    const char *reason ) = 0; // network error occured
    
  virtual void PacketStart(    int incoming_sequence, int outgoing_acknowledged ) = 0; // called each time a new packet arrived
    
  virtual void PacketEnd( void ) = 0; // all messages has been parsed
  
  virtual void FileRequested(    const char *fileName, unsigned int transferID ) = 0; // other side request a file for download
    
  virtual void FileReceived( const char *fileName, unsigned int transferID ) = 0; // we received a file
                             
  virtual void FileDenied(    const char *fileName, unsigned int transferID ) = 0; // a file request was denied by other side
    
  virtual void FileSent( const char *fileName, unsigned int transferID ) = 0; // we sent a file
};

class CNetChan : public INetChannel {
 public: // netchan structurs
  typedef struct dataFragments_s {
    FileHandle_t file; // open file handle
    char filename[MAX_OSPATH]; // filename
    char *buffer; // if NULL it's a file
    unsigned int BYTEs; // size in BYTEs
    unsigned int bits; // size in bits
    unsigned int transferID; // only for files
    bool isCompressed; // true if data is bzip compressed
    unsigned int nUncompressedSize; // full size in BYTEs
    bool asTCP; // send as TCP stream
    int numFragments; // number of total fragments
    int ackedFragments; // number of fragments send & acknowledged
    int pendingFragments; // number of fragments send, but not acknowledged yet
  } dataFragments_t;
  
  struct subChannel_s {
    int startFraggment[MAX_STREAMS];
    int numFragments[MAX_STREAMS];
    int sendSeqNr;
    int state; // 0 = free, 1 = scheduled to send, 2 = send & waiting, 3 =
    // dirty
    int index; // index in m_SubChannels[]
    
    void Free() {
      state = SUBCHANNEL_FREE;
      sendSeqNr = -1;
      
      for( int i = 0; i < MAX_STREAMS; i++ ) {
        numFragments[i] = 0;
        startFraggment[i] = -1;
      }
    }
  };
  
  // Client's now store the command they sent to the server and the entire
  // results set of
  //  that command.
  typedef struct netframe_s {
    // Data received from server
    float time; // net_time received/send
    int size; // total size in BYTEs
    float latency; // raw ping for this packet, not cleaned. set when
    // acknowledged otherwise -1.
    float avg_latency; // averaged ping for this packet
    bool valid; // false if dropped, lost, flushed
    int choked; // number of previously chocked packets
    int dropped;
    float m_flInterpolationAmount;
    unsigned short msggroups[TOTAL]; // received BYTEs for
    // each message group
  } netframe_t;
  
  typedef struct {
    float nextcompute; // Time when we should recompute k/sec data
    float avgBYTEspersec; // average BYTEs/sec
    float avgpacketspersec; // average packets/sec
    float avgloss; // average packet loss [0..1]
    float avgchoke; // average packet choke [0..1]
    float avglatency; // average ping, not cleaned
    float latency; // current ping, more accurate also more jittering
    int totalpackets; // total processed packets
    int totalBYTEs; // total processed BYTEs
    int currentindex; // current frame index
    netframe_t frames[64]; // frame history
    netframe_t *currentframe; // current frame
  } netflow_t;
  
 public:
  bool m_bProcessingMessages;
  bool m_bShouldDelete;
  
  // last send outgoing sequence number
  int m_nOutSequenceNr;
  // last received incoming sequnec number
  int m_nInSequenceNr;
  // last received acknowledge outgoing sequnce number
  int m_nOutSequenceNrAck;
  
  // state of outgoing reliable data (0/1) flip flop used for loss detection
  int m_nOutReliableState;
  // state of incoming reliable data
  int m_nInReliableState;
  
  int m_nChokedPackets; // number of choked packets
  
  // Reliable data buffer, send which each packet (or put in waiting list)
  // bf_write m_StreamReliable;
  // CUtlMemory<BYTE> m_ReliableDataBuffer;
  
  // unreliable message buffer, cleared which each packet
  // bf_write m_StreamUnreliable;
  // CUtlMemory<BYTE> m_UnreliableDataBuffer;
  
  // bf_write m_StreamVoice;
  // CUtlMemory<BYTE> m_VoiceDataBuffer;
  
  // don't use any vars below this (only in net_ws.cpp)
  
  int m_Socket; // NS_SERVER or NS_CLIENT index, depending on channel.
  int m_StreamSocket; // TCP socket handle
  
  unsigned int m_MaxReliablePayloadSize; // max size of reliable payload in a
  // single packet
  
  // Address this channel is talking to.
  netadr_t remote_address;
  
  // For timeouts.  Time last message was received.
  float last_received;
  // Time when channel was connected.
  double connect_time;
  
  // Bandwidth choke
  // BYTEs per second
  int m_Rate;
  // If realtime > cleartime, free to send next packet
  double m_fClearTime;
  
  // CUtlVector<dataFragments_t *> m_WaitingList[MAX_STREAMS]; // waiting list
  // for reliable data and file transfer
  dataFragments_t m_ReceiveList[MAX_STREAMS]; // receive buffers for streams
  subChannel_s m_SubChannels[8];
  
  unsigned int
  m_FileRequestCounter; // increasing counter with each file request
  bool
  m_bFileBackgroundTranmission; // if true, only send 1 fragment per packet
  bool m_bUseCompression; // if true, larger reliable data will be bzip
  // compressed
  
  // TCP stream state maschine:
  bool m_StreamActive; // true if TCP is active
  int m_SteamType; // STREAM_CMD_*
  int m_StreamSeqNr; // each blob send of TCP as an increasing ID
  int m_StreamLength; // total length of current stream blob
  int m_StreamReceived; // length of already received BYTEs
  // char m_SteamFile[MAX_OSPATH];  // if receiving file, this is it's name
  // CUtlMemory<BYTE> m_StreamData; // Here goes the stream data (if not file).
  // Only allocated if we're going to use it.
  
  // packet history
  netflow_t m_DataFlow[MAX_FLOWS];
  int m_MsgStats[TOTAL]; // total BYTEs for each message group
  
  int m_PacketDrop; // packets lost before getting last update (was global
  // net_drop)
  
  char m_Name[32]; // channel name
  
  unsigned int m_ChallengeNr; // unique, random challenge number
  
  float m_Timeout; // in seconds
  
  INetChannelHandler *m_MessageHandler; // who registers and processes messages
  // CUtlVector<INetMessage *> m_NetMessages; // list of registered message
  // IDemoRecorder *m_DemoRecorder;       // if != NULL points to a
  // recording/playback demo object
  int m_nQueuedPackets;
  
  float m_flInterpolationAmount;
  float m_flRemoteFrameTime;
  float m_flRemoteFrameTimeStdDeviation;
  int m_nMaxRoutablePayloadSize;
  
  int m_nSplitPacketSequence;
};

class EngineClient {
 public:
  int GetPlayerForUserID( int UserID ) {
    using Original = int( __thiscall * )( PVOID, int );
    return getvfunc<Original>( this, 9 )( this, UserID );
  }
  
  void GetScreenSize( int &width, int &height ) {
    typedef void( __thiscall * OriginalFn )( PVOID, int &, int & );
    return getvfunc<OriginalFn>( this, 5 )( this, width, height );
  }
  
  bool GetPlayerInfo( int ent_num, player_info_t *pinfo ) {
    typedef bool( __thiscall * OriginalFn )( PVOID, int, player_info_t * );
    return getvfunc<OriginalFn>( this, 8 )( this, ent_num, pinfo );
  }
  
  bool Con_IsVisible( void ) {
    typedef bool( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 11 )( this );
  }
  
  int GetLocalPlayer( void ) {
    typedef int( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 12 )( this );
  }
  
  float Time( void ) {
    typedef float( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 14 )( this );
  }
  
  void GetViewAngles( Vector &va ) {
    typedef void( __thiscall * OriginalFn )( PVOID, Vector & va );
    return getvfunc<OriginalFn>( this, 19 )( this, va );
  }
  
  void SetViewAngles( Vector &va ) {
    typedef void( __thiscall * OriginalFn )( PVOID, Vector & va );
    return getvfunc<OriginalFn>( this, 20 )( this, va );
  }
  
  int GetMaxClients( void ) {
    typedef int( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 21 )( this );
  }
  
  bool IsInGame( void ) {
    typedef bool( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 26 )( this );
  }
  
  bool IsConnected( void ) {
    typedef bool( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 27 )( this );
  }
  
  bool IsDrawingLoadingImage( void ) {
    typedef bool( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 28 )( this );
  }
  
  const matrix3x4 &WorldToScreenMatrix( void ) {
    typedef const matrix3x4&( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 36 )( this );
  }
  
  bool IsTakingScreenshot( void ) {
    typedef bool( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 85 )( this );
  }
 
  void ClientCmd( const char *szCommandString ) {
    typedef void( __thiscall * ClientCmdFn )( void *, const char * );
    return getvfunc<ClientCmdFn>( this, 7 )( this, szCommandString );
  }
  
  void ClientCmd_Unrestricted( const char *chCommandString ) {
    typedef void( __thiscall * OriginalFn )( PVOID, const char * );
    return getvfunc<OriginalFn>( this, 106 )( this, chCommandString );
  }
  
  void ServerCmdKeyValues( PVOID kv ) {
    typedef void( __thiscall * OriginalFn )( PVOID, PVOID );
    getvfunc<OriginalFn>( this, 127 )( this, kv );
  }
  
  CNetChan *GetNetChannelInfo( void ) {
    typedef CNetChan*( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 72 )( this );
  }
};

class IPanel {
 public:
  const char *GetName( unsigned int vguiPanel ) {
    typedef const char *( __thiscall * OriginalFn )( PVOID, unsigned int );
    return getvfunc<OriginalFn>( this, 36 )( this, vguiPanel );
  }
  
  void SetMouseInputEnabled( unsigned int panel, bool state ) {
    getvfunc<void( __thiscall * )( void *, int, bool )>( this, 32 )( this, panel, 
        state );
  }
  
  void SetTopmostPopup( unsigned int panel, bool state ) {
    getvfunc<void( __thiscall * )( void *, int, bool )>( this, 59 )( this, panel, 
        state );
  }
};

class ISurface {
 public:
  void DrawSetColor( int r, int g, int b, int a ) {
    typedef void( __thiscall * OriginalFn )( PVOID, int, int, int, int );
    getvfunc<OriginalFn>( this, 11 )( this, r, g, b, a );
  }
  
  void DrawFilledRect( int x0, int y0, int x1, int y1 ) {
    typedef void( __thiscall * OriginalFn )( PVOID, int, int, int, int );
    getvfunc<OriginalFn>( this, 12 )( this, x0, y0, x1, y1 );
  }
  
  void DrawOutlinedRect( int x0, int y0, int x1, int y1 ) {
    typedef void( __thiscall * OriginalFn )( PVOID, int, int, int, int );
    getvfunc<OriginalFn>( this, 14 )( this, x0, y0, x1, y1 );
  }
  
  int CreateNewTextureID( bool procedural = true ) {
    typedef int( __thiscall * OriginalFn )( void *, bool );
    return getvfunc<OriginalFn>( this, 37 )( this, procedural );
  }
  
  void DrawSetTextureRGBA( int id, unsigned char const *rgba, int wide, int tall, int hardwareFilter = 0, bool forceReload = false ) {
    typedef void( __thiscall * OriginalFn )( void *, int, unsigned char const *, int, int, int, bool );
    return getvfunc<OriginalFn>( this, 31 )( this, id, rgba, wide, tall, hardwareFilter, forceReload );
  }
  
  void DrawSetTexture( int id ) {
    typedef void( __thiscall * OriginalFn )( void *, int );
    return getvfunc<OriginalFn>( this, 32 )( this, id );
  }
  
  bool IsTextureIDValid( int id ) {
    typedef bool( __thiscall * OriginalFn )( void *, int );
    return getvfunc<OriginalFn>( this, 35 )( this, id );
  }
  
  void DrawTexturedRect( int x, int y, int width, int height ) {
    typedef void( __thiscall * OriginalFn )( void *, int, int, int, int );
    return getvfunc<OriginalFn>( this, 34 )( this, x, y, width, height );
  }
  
  void GetCursorPosition( int &x, int &y ) {
    return getvfunc<void( __thiscall * )( void *, int &, int & )>( this, 96 )( this, x, y );
  }
  
  void DrawLine( int x0, int y0, int x1, int y1 ) {
    typedef void( __thiscall * DrawLineFn )( void *, int, int, int, int );
    getvfunc<DrawLineFn>( this, 15 )( this, x0, y0, x1, y1 );
  }
  
  void DrawSetTextFont( unsigned long font ) {
    typedef void( __thiscall * OriginalFn )( PVOID, unsigned long );
    getvfunc<OriginalFn>( this, 17 )( this, font );
  }
  
  void DrawSetTextColor( int r, int g, int b, int a ) {
    typedef void( __thiscall * OriginalFn )( PVOID, int, int, int, int );
    getvfunc<OriginalFn>( this, 19 )( this, r, g, b, a );
  }
  
  void DrawSetTextPos( int x, int y ) {
    typedef void( __thiscall * OriginalFn )( PVOID, int, int );
    getvfunc<OriginalFn>( this, 20 )( this, x, y );
  }
  
  void DrawPrintText( const wchar_t *text, int textLen ) {
    typedef void( __thiscall * OriginalFn )( PVOID, const wchar_t *, int, int );
    return getvfunc<OriginalFn>( this, 22 )( this, text, textLen, 0 );
  }
  
  unsigned long CreateFont() {
    typedef unsigned int( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 66 )( this );
  }
  
  void SetFontGlyphSet( unsigned long &font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags ) {
    typedef void( __thiscall * OriginalFn )( PVOID, unsigned long, const char *, int, int, int, int, int, int, int );
    getvfunc<OriginalFn>( this, 67 )( this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0 );
  }
  
  void GetTextSize( unsigned long font, const wchar_t *text, int &wide, int &tall ) {
    typedef void( __thiscall * OriginalFn )( PVOID, unsigned long, const wchar_t *, int &, int & );
    getvfunc<OriginalFn>( this, 75 )( this, font, text, wide, tall );
  }
};

class CEntList {
 public:
  CBaseEntity *GetClientEntity( int entnum ) {
    typedef CBaseEntity*( __thiscall * OriginalFn )( PVOID, int );
    return getvfunc<OriginalFn>( this, 3 )( this, entnum );
  }
  
  CBaseEntity *GetClientEntityFromHandle( int hEnt ) {
    typedef CBaseEntity*( __thiscall * OriginalFn )( PVOID, int );
    return getvfunc<OriginalFn>( this, 4 )( this, hEnt );
  }
  
  int GetHighestEntityIndex( void ) {
    typedef int( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 6 )( this );
  }
};

class __declspec( align( 16 ) ) VectorAligned : public Vector {
 public:
  VectorAligned( void ) {
  };
  
  VectorAligned( float X, float Y, float Z ) {
    Init( X, Y, Z );
  }
  
  explicit VectorAligned( const Vector &vOther ) {
    Init( vOther.x, vOther.y, vOther.z );
  }
  
  VectorAligned &operator=( const Vector &vOther ) {
    Init( vOther.x, vOther.y, vOther.z );
    return *this;
  }
  
  float w;
};

struct Ray_t {
  VectorAligned m_Start;
  VectorAligned m_Delta;
  VectorAligned m_StartOffset;
  VectorAligned m_Extents;
  
  bool m_IsRay;
  bool m_IsSwept;
  
  void Init( Vector start, Vector end ) {
    m_Delta = end - start;
    m_IsSwept = ( m_Delta.LengthSqr() != 0 );
    m_Extents.Init();
    m_IsRay = true;
    m_StartOffset.Init();
    m_Start = start;
  }
  
  void Init( Vector &start, Vector &end, Vector &mins, Vector &maxs ) {
    m_Delta = end - start;
    m_IsSwept = ( m_Delta.LengthSqr() != 0 );
    m_Extents = maxs - mins;
    m_Extents *= 0.5f;
    m_IsRay = ( m_Extents.LengthSqr() < 1e-6 );
    m_StartOffset = mins + maxs;
    m_StartOffset *= 0.5f;
    m_Start = start - m_StartOffset;
    m_StartOffset *= -1.0f;
  }
};

struct cplane_t {
  Vector normal;
  float dist;
  BYTE type;
  BYTE signbits;
  BYTE pad[2];
};

struct csurface_t {
  const char *name;
  short surfaceProps;
  unsigned short flags;
};

enum SurfaceFlags_t {
  DISPSURF_FLAG_SURFACE = ( 1 << 0 ), 
  DISPSURF_FLAG_WALKABLE = ( 1 << 1 ), 
  DISPSURF_FLAG_BUILDABLE = ( 1 << 2 ), 
  DISPSURF_FLAG_SURFPROP1 = ( 1 << 3 ), 
  DISPSURF_FLAG_SURFPROP2 = ( 1 << 4 ), 
};

enum TraceType_t {
  TRACE_EVERYTHING = 0, 
  TRACE_WORLD_ONLY, 
  // NOTE: This does *not* test static props!!!
  TRACE_ENTITIES_ONLY, 
  // NOTE: This version will *not* test static props
  TRACE_EVERYTHING_FILTER_PROPS, 
  // NOTE: This version will pass the
  // IHandleEntity for props through the filter, 
  // unlike all other filters
};

class ITraceFilter {
 public:
  virtual bool ShouldHitEntity( void *pEntity, int contentsMask ) = 0;
  virtual TraceType_t GetTraceType() const = 0;
};

class CTraceFilter : public ITraceFilter {
 public:
  bool ShouldHitEntity( void *pEntityHandle, int contentsMask ) override {
    CBaseEntity *pEntity = ( CBaseEntity * )pEntityHandle;
    
    switch( ( classId )pEntity->GetClassId() ) {
    case classId::CFuncAreaPortalWindow:
    case classId::CFuncRespawnRoomVisualizer:
    case classId::CSniperDot:
    case classId::CTFMedigunShield:
      return false;
      break;
    }
    
    return !( pEntityHandle == pSkip );
  }
  
  TraceType_t GetTraceType() const override {
    return TRACE_EVERYTHING;
  }
  
  void *pSkip;
};

class CBaseTrace {
 public:
  bool IsDispSurface( void ) {
    return ( ( dispFlags & DISPSURF_FLAG_SURFACE ) != 0 );
  }
  
  bool IsDispSurfaceWalkable( void ) {
    return ( ( dispFlags & DISPSURF_FLAG_WALKABLE ) != 0 );
  }
  
  bool IsDispSurfaceBuildable( void ) {
    return ( ( dispFlags & DISPSURF_FLAG_BUILDABLE ) != 0 );
  }
  
  bool IsDispSurfaceProp1( void ) {
    return ( ( dispFlags & DISPSURF_FLAG_SURFPROP1 ) != 0 );
  }
  
  bool IsDispSurfaceProp2( void ) {
    return ( ( dispFlags & DISPSURF_FLAG_SURFPROP2 ) != 0 );
  }
  
 public:
  Vector startpos;
  Vector endpos;
  cplane_t plane;
  
  float fraction;
  
  int contents;
  unsigned short dispFlags;
  
  bool allsolid;
  bool startsolid;
  
  CBaseTrace() {
  }
  
 private:
  CBaseTrace( const CBaseTrace &vOther );
};

class CGameTrace : public CBaseTrace {
 public:
  bool DidHitWorld() const;
  
  bool DidHitNonWorldEntity() const;
  
  int GetEntityIndex() const;
  
  bool DidHit() const {
    return fraction < 1 || allsolid || startsolid;
  }
  
 public:
  float fractionleftsolid;
  csurface_t surface;
  
  int hitgroup;
  
  short physicsbone;
  
  CBaseEntity *m_pEnt;
  int hitbox;
  
  CGameTrace() {
  }
  
  CGameTrace( const CGameTrace &vOther );
};

class IEngineTrace {
 public: // We really only need this I guess...
  void TraceRay( const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace ) {
    // 5
    typedef void( __thiscall * TraceRayFn )( void *, const Ray_t &, unsigned int, ITraceFilter *, trace_t * );
    return getvfunc<TraceRayFn>( this, 4 )( this, ray, fMask, pTraceFilter, pTrace );
  }
};

class IVModelInfo {
 public:
  DWORD *GetModel( int index ) {
    typedef DWORD*( __thiscall * GetModelFn )( void *, int );
    return getvfunc<GetModelFn>( this, 1 )( this, index );
  }
  
  int GetModelIndex( const char *name ) {
    typedef int( __thiscall * GetModelIndexFn )( void *, const char * );
    return getvfunc<GetModelIndexFn>( this, 2 )( this, name );
  }
  
  const char *GetModelName( const DWORD *model ) {
    typedef const char *( __thiscall * GetModelNameFn )( void *, const DWORD * );
    return getvfunc<GetModelNameFn>( this, 3 )( this, model );
  }
  
  studiohdr_t *GetStudiomodel( const DWORD *mod ) {
    typedef studiohdr_t *( __thiscall * GetStudiomodelFn )( void *, const DWORD * );
    return getvfunc<GetStudiomodelFn>( this, 28 )( this, mod );
  }
};

enum playercontrols {
  IN_ATTACK = ( 1 << 0 ), 
  IN_JUMP = ( 1 << 1 ), 
  IN_DUCK = ( 1 << 2 ), 
  IN_FORWARD = ( 1 << 3 ), 
  IN_BACK = ( 1 << 4 ), 
  IN_USE = ( 1 << 5 ), 
  IN_CANCEL = ( 1 << 6 ), 
  IN_LEFT = ( 1 << 7 ), 
  IN_RIGHT = ( 1 << 8 ), 
  IN_MOVELEFT = ( 1 << 9 ), 
  IN_MOVERIGHT = ( 1 << 10 ), 
  IN_ATTACK2 = ( 1 << 11 ), 
  IN_RUN = ( 1 << 12 ), 
  IN_RELOAD = ( 1 << 13 ), 
  IN_ALT1 = ( 1 << 14 ), 
  IN_ALT2 = ( 1 << 15 ), 
  IN_SCORE = ( 1 << 16 ), 
  // Used by client.dll for when scoreboard is held down
  IN_SPEED = ( 1 << 17 ), 
  // Player is holding the speed key
  IN_WALK = ( 1 << 18 ), 
  // Player holding walk key
  IN_ZOOM = ( 1 << 19 ), 
  // Zoom key for HUD zoom
  IN_WEAPON1 = ( 1 << 20 ), 
  // weapon defines these bits
  IN_WEAPON2 = ( 1 << 21 ), 
  // weapon defines these bits
  IN_BULLRUSH = ( 1 << 22 ), 
};

enum tf_cond {
  TFCond_Slowed = ( 1 << 0 ), 
  // Toggled when a player is slowed down.
  TFCond_Zoomed = ( 1 << 1 ), 
  // Toggled when a player is zoomed.
  TFCond_Disguising = ( 1 << 2 ), 
  // Toggled when a Spy is disguising.
  TFCond_Disguised = ( 1 << 3 ), 
  // Toggled when a Spy is disguised.
  TFCond_Cloaked = ( 1 << 4 ), 
  // Toggled when a Spy is invisible.
  TFCond_Ubercharged = ( 1 << 5 ), 
  // Toggled when a player is ÜberCharged.
  TFCond_TeleportedGlow = ( 1 << 6 ), 
  // Toggled when someone leaves a teleporter
  // and has glow beneath their feet.
  TFCond_Taunting = ( 1 << 7 ), 
  // Toggled when a player is taunting.
  TFCond_UberchargeFading = ( 1 << 8 ), 
  // Toggled when the ÜberCharge is fading.
  TFCond_CloakFlicker =
    ( 1 << 9 ), 
  // Toggled when a Spy is visible during cloak.
  TFCond_Teleporting =
    ( 1 << 10 ), 
  // Only activates for a brief second when the player is being
  // teleported; not very useful.
  TFCond_Kritzkrieged =
    ( 1
      << 11 ), 
  // Toggled when a player is being crit buffed by the KritzKrieg.
  TFCond_TmpDamageBonus = ( 1 << 12 ), 
  // Unknown what this is for. Name taken
  // from the AlliedModders SDK.
  TFCond_DeadRingered = ( 1 << 13 ), 
  // Toggled when a player is taking reduced
  // damage from the Deadringer.
  TFCond_Bonked = ( 1 << 14 ), 
  // Toggled when a player is under the effects of
  // The Bonk! Atomic Punch.
  TFCond_Stunned = ( 1 << 15 ), 
  // Toggled when a player's speed is reduced from
  // airblast or a Sandman ball.
  TFCond_Buffed = ( 1 << 16 ), 
  // Toggled when a player is within range of an
  // activated Buff Banner.
  TFCond_Charging =
    ( 1 << 17 ), 
  // Toggled when a Demoman charges with the shield.
  TFCond_DemoBuff =
    ( 1 << 18 ), 
  // Toggled when a Demoman has heads from the Eyelander.
  TFCond_CritCola = ( 1 << 19 ), 
  // Toggled when the player is under the effect
  // of The Crit-a-Cola.
  TFCond_InHealRadius =
    ( 1 << 20 ), 
  // Unused condition, name taken from AlliedModders SDK.
  TFCond_Healing = ( 1 << 21 ), 
  // Toggled when someone is being healed by a
  // medic or a dispenser.
  TFCond_OnFire = ( 1 << 22 ), 
  // Toggled when a player is on fire.
  TFCond_Overhealed = ( 1 << 23 ), 
  // Toggled when a player has >100% health.
  TFCond_Jarated =
    ( 1 << 24 ), 
  // Toggled when a player is hit with a Sniper's Jarate.
  TFCond_Bleeding =
    ( 1 << 25 ), 
  // Toggled when a player is taking bleeding damage.
  TFCond_DefenseBuffed = ( 1 << 26 ), 
  // Toggled when a player is within range of
  // an activated Battalion's Backup.
  TFCond_Milked = ( 1 << 27 ), 
  // Player was hit with a jar of Mad Milk.
  TFCond_MegaHeal =
    ( 1 << 28 ), 
  // Player is under the effect of Quick-Fix charge.
  TFCond_RegenBuffed =
    ( 1 << 29 ), 
  // Toggled when a player is within a Concheror's range.
  TFCond_MarkedForDeath =
    ( 1 << 30 ), 
  // Player is marked for death by a Fan O'War hit. Effects are
  // similar to TFCond_Jarated.
  TFCond_NoHealingDamageBuff = ( 1 << 31 ), 
  // Unknown what this is used for.
  
  TFCondEx_SpeedBuffAlly =
    ( 1 << 0 ), 
  // Toggled when a player gets hit with the disciplinary action.
  TFCondEx_HalloweenCritCandy =
    ( 1 << 1 ), 
  // Only for Scream Fortress event maps that drop crit candy.
  TFCondEx_CritCanteen =
    ( 1 << 2 ), 
  // Player is getting a crit boost from a MVM canteen.
  TFCondEx_CritDemoCharge = ( 1 << 3 ), 
  // From demo's shield
  TFCondEx_CritHype = ( 1 << 4 ), 
  // Soda Popper crits.
  TFCondEx_CritOnFirstBlood = ( 1 << 5 ), 
  // Arena first blood crit buff.
  TFCondEx_CritOnWin = ( 1 << 6 ), 
  // End of round crits.
  TFCondEx_CritOnFlagCapture = ( 1 << 7 ), 
  // CTF intelligence capture crits.
  TFCondEx_CritOnKill = ( 1 << 8 ), 
  // Unknown what this is for.
  TFCondEx_RestrictToMelee = ( 1 << 9 ), 
  // Unknown what this is for.
  TFCondEx_DefenseBuffNoCritBlock = ( 1 << 10 ), 
  // MvM Buff.
  TFCondEx_Reprogrammed = ( 1 << 11 ), 
  // MvM Bot has been reprogrammed.
  TFCondEx_PyroCrits =
    ( 1 << 12 ), 
  // Player is getting crits from the Mmmph charge.
  TFCondEx_PyroHeal =
    ( 1 << 13 ), 
  // Player is being healed from the Mmmph charge.
  TFCondEx_FocusBuff = ( 1 << 14 ), 
  // Player is getting a focus buff.
  TFCondEx_DisguisedRemoved = ( 1 << 15 ), 
  // Disguised remove from a bot.
  TFCondEx_MarkedForDeathSilent =
    ( 1 << 16 ), 
  // Player is under the effects of the Escape Plan/Equalizer or
  // GRU.
  TFCondEx_DisguisedAsDispenser = ( 1 << 17 ), 
  // Bot is disguised as dispenser.
  TFCondEx_Sapped = ( 1 << 18 ), 
  // MvM bot is being sapped.
  TFCondEx_UberchargedHidden = ( 1 << 19 ), 
  // MvM Related
  TFCondEx_UberchargedCanteen =
    ( 1 << 20 ), 
  // Player is receiving ÜberCharge from a canteen.
  TFCondEx_HalloweenBombHead =
    ( 1 << 21 ), 
  // Player has a bomb on their head from Merasmus.
  TFCondEx_HalloweenThriller =
    ( 1 << 22 ), 
  // Players are forced to dance from Merasmus.
  TFCondEx_BulletCharge =
    ( 1 << 26 ), 
  // Player is receiving 75% reduced damage from bullets.
  TFCondEx_ExplosiveCharge =
    ( 1 << 27 ), 
  // Player is receiving 75% reduced damage from explosives.
  TFCondEx_FireCharge =
    ( 1 << 28 ), 
  // Player is receiving 75% reduced damage from fire.
  TFCondEx_BulletResistance =
    ( 1 << 29 ), 
  // Player is receiving 10% reduced damage from bullets.
  TFCondEx_ExplosiveResistance =
    ( 1 << 30 ), 
  // Player is receiving 10% reduced damage from explosives.
  TFCondEx_FireResistance =
    ( 1 << 31 ), 
  // Player is receiving 10% reduced damage from fire.
  
  TFCondEx2_Stealthed = ( 1 << 0 ), 
  TFCondEx2_MedigunDebuff = ( 1 << 1 ), 
  TFCondEx2_StealthedUserBuffFade = ( 1 << 2 ), 
  TFCondEx2_BulletImmune = ( 1 << 3 ), 
  TFCondEx2_BlastImmune = ( 1 << 4 ), 
  TFCondEx2_FireImmune = ( 1 << 5 ), 
  TFCondEx2_PreventDeath = ( 1 << 6 ), 
  TFCondEx2_MVMBotRadiowave = ( 1 << 7 ), 
  TFCondEx2_HalloweenSpeedBoost =
    ( 1 << 8 ), 
  // Wheel has granted player speed boost.
  TFCondEx2_HalloweenQuickHeal =
    ( 1 << 9 ), 
  // Wheel has granted player quick heal.
  TFCondEx2_HalloweenGiant = ( 1 << 10 ), 
  // Wheel has granted player giant mode.
  TFCondEx2_HalloweenTiny = ( 1 << 11 ), 
  // Wheel has granted player tiny mode.
  TFCondEx2_HalloweenInHell =
    ( 1 << 12 ), 
  // Wheel has granted player in hell mode.
  TFCondEx2_HalloweenGhostMode =
    ( 1 << 13 ), 
  // Wheel has granted player ghost mode.
  TFCondEx2_Parachute = ( 1 << 16 ), 
  // Player has deployed the BASE Jumper.
  TFCondEx2_BlastJumping = ( 1 << 17 ), 
  // Player has sticky or rocket jumped.
  
  TFCond_MiniCrits = ( TFCond_Buffed | TFCond_CritCola ), 
  TFCond_IgnoreStates = ( TFCond_Ubercharged | TFCond_Bonked ), 
  TFCondEx_IgnoreStates = ( TFCondEx_PyroHeal )
};

class IGameEventListener2 {
 public:
  virtual ~IGameEventListener2( void ) {
  };
  
  // FireEvent is called by EventManager if event just occured
  // KeyValue memory will be freed by manager if not needed anymore
  virtual void FireGameEvent( IGameEvent *event ) = 0;
};

class IGameEventManager2 {
 public:
  bool AddListener( IGameEventListener2 *listener, const char *name, bool bServerSide ) {
    typedef bool( __thiscall * OriginalFn )( PVOID, IGameEventListener2 *, const char *, bool );
    return getvfunc<OriginalFn>( this, 3 )( this, listener, name, bServerSide );
  }
};



enum source_lifestates {
  LIFE_ALIVE, 
  LIFE_DYING, 
  LIFE_DEAD, 
  LIFE_RESPAWNABLE, 
  LIFE_DISCARDBODY, 
};

class ClientModeShared {
 public:
  bool IsChatPanelOutOfFocus( void ) {
    typedef PVOID( __thiscall * OriginalFn )( PVOID );
    PVOID CHudChat = getvfunc<OriginalFn>( this, 19 )( this );
    
    if( CHudChat ) {
      return *( PFLOAT )( ( DWORD )CHudChat + 0xFC ) == 0;
    }
    
    return false;
  }
};

enum MaterialVarFlags_t {
  MATERIAL_VAR_DEBUG = ( 1 << 0 ), 
  MATERIAL_VAR_NO_DEBUG_OVERRIDE = ( 1 << 1 ), 
  MATERIAL_VAR_NO_DRAW = ( 1 << 2 ), 
  MATERIAL_VAR_USE_IN_FILLRATE_MODE = ( 1 << 3 ), 
  MATERIAL_VAR_VERTEXCOLOR = ( 1 << 4 ), 
  MATERIAL_VAR_VERTEXALPHA = ( 1 << 5 ), 
  MATERIAL_VAR_SELFILLUM = ( 1 << 6 ), 
  MATERIAL_VAR_ADDITIVE = ( 1 << 7 ), 
  MATERIAL_VAR_ALPHATEST = ( 1 << 8 ), 
  MATERIAL_VAR_ZNEARER = ( 1 << 10 ), 
  MATERIAL_VAR_MODEL = ( 1 << 11 ), 
  MATERIAL_VAR_FLAT = ( 1 << 12 ), 
  MATERIAL_VAR_NOCULL = ( 1 << 13 ), 
  MATERIAL_VAR_NOFOG = ( 1 << 14 ), 
  MATERIAL_VAR_IGNOREZ = ( 1 << 15 ), 
  MATERIAL_VAR_DECAL = ( 1 << 16 ), 
  MATERIAL_VAR_ENVMAPSPHERE = ( 1 << 17 ), 
  // OBSOLETE
  MATERIAL_VAR_ENVMAPCAMERASPACE = ( 1 << 19 ), 
  // OBSOLETE
  MATERIAL_VAR_BASEALPHAENVMAPMASK = ( 1 << 20 ), 
  MATERIAL_VAR_TRANSLUCENT = ( 1 << 21 ), 
  MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = ( 1 << 22 ), 
  MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = ( 1 << 23 ), 
  // OBSOLETE
  MATERIAL_VAR_OPAQUETEXTURE = ( 1 << 24 ), 
  MATERIAL_VAR_ENVMAPMODE = ( 1 << 25 ), 
  // OBSOLETE
  MATERIAL_VAR_SUPPRESS_DECALS = ( 1 << 26 ), 
  MATERIAL_VAR_HALFLAMBERT = ( 1 << 27 ), 
  MATERIAL_VAR_WIREFRAME = ( 1 << 28 ), 
  MATERIAL_VAR_ALLOWALPHATOCOVERAGE = ( 1 << 29 ), 
  MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = ( 1 << 30 ), 
  MATERIAL_VAR_VERTEXFOG = ( 1 << 31 ), 
};

enum MaterialPropertyTypes_t {
  MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0, 
  // bool
  MATERIAL_PROPERTY_OPACITY, 
  // int (enum MaterialPropertyOpacityTypes_t)
  MATERIAL_PROPERTY_REFLECTIVITY, 
  // vec3_t
  MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS // bool
};

enum OverrideType_t {
  OVERRIDE_NORMAL = 0, 
  OVERRIDE_BUILD_SHADOWS, 
  OVERRIDE_DEPTH_WRITE, 
  OVERRIDE_SSAO_DEPTH_WRITE, 
};

struct ModelRenderInfo_t {
  Vector origin;
  Vector angles;
  PVOID *pRenderable;
  const DWORD *pModel;
  const matrix3x4_t *pModelToWorld;
  const matrix3x4_t *pLightingOffset;
  const Vector *pLightingOrigin;
  int flags;
  int entity_index;
  int skin;
  int body;
  int hitboxset;
  ModelInstanceHandle_t instance;
  
  ModelRenderInfo_t() {
    pModelToWorld = nullptr;
    pLightingOffset = nullptr;
    pLightingOrigin = nullptr;
  }
};

class IMaterial {
 public:
  // Get the name of the material.  This is a full path to
  // the vmt file starting from "hl2/materials" (or equivalent) without
  // a file extension.
  virtual const char *GetName() const = 0;
  virtual const char *GetTextureGroupName() const = 0;
  
  // Get the preferred size/bitDepth of a preview image of a material.
  // This is the sort of image that you would use for a thumbnail view
  // of a material, or in WorldCraft until it uses materials to render.
  // separate this for the tools maybe
  virtual DWORD GetPreviewImageProperties( int *width, int *height, DWORD *imageFormat, bool *isTranslucent ) const = 0;
      
  // Get a preview image at the specified width/height and bitDepth.
  // Will do resampling if necessary.(not yet!!! :) )
  // Will do color format conversion. (works now.)
  virtual DWORD GetPreviewImage( unsigned char *data, int width, int height, DWORD imageFormat ) const = 0;
  //
  virtual int GetMappingWidth() = 0;
  virtual int GetMappingHeight() = 0;
  
  virtual int GetNumAnimationFrames() = 0;
  
  // For material subrects (material pages).  Offset(u, v) and scale(u, v) are
  // normalized to texture.
  virtual bool InMaterialPage( void ) = 0;
  virtual void GetMaterialOffset( float *pOffset ) = 0;
  virtual void GetMaterialScale( float *pScale ) = 0;
  virtual IMaterial *GetMaterialPage( void ) = 0;
  
  // find a vmt variable.
  // This is how game code affects how a material is rendered.
  // The game code must know about the params that are used by
  // the shader for the material that it is trying to affect.
  virtual IMaterialVar *FindVar( const char *varName, bool *found, bool complain = true ) = 0;
                                 
  // The user never allocates or deallocates materials.  Reference counting is
  // used instead.  Garbage collection is done upon a call to
  // IMaterialSystem::UncacheUnusedMaterials.
  virtual void IncrementReferenceCount( void ) = 0;
  virtual void DecrementReferenceCount( void ) = 0;
  
  void AddRef() {
    IncrementReferenceCount();
  }
  
  void Release() {
    DecrementReferenceCount();
  }
  
  // Each material is assigned a number that groups it with like materials
  // for sorting in the application.
  virtual int GetEnumerationID( void ) const = 0;
  
  virtual void GetLowResColorSample( float s, float t, float *color ) const = 0;
  
  // This computes the state snapshots for this material
  virtual void RecomputeStateSnapshots() = 0;
  
  // Are we translucent?
  virtual bool IsTranslucent() = 0;
  
  // Are we alphatested?
  virtual bool IsAlphaTested() = 0;
  
  // Are we vertex lit?
  virtual bool IsVertexLit() = 0;
  
  // Gets the vertex format
  virtual VertexFormat_t GetVertexFormat() const = 0;
  
  // returns true if this material uses a material proxy
  virtual bool HasProxy( void ) const = 0;
  
  virtual bool UsesEnvCubemap( void ) = 0;
  
  virtual bool NeedsTangentSpace( void ) = 0;
  
  virtual bool NeedsPowerOfTwoFrameBufferTexture(    bool bCheckSpecificToThisFrame = true ) = 0;
  virtual bool NeedsFullFrameBufferTexture(    bool bCheckSpecificToThisFrame = true ) = 0;
    
  // returns true if the shader doesn't do skinning itself and requires
  // the data that is sent to it to be preskinned.
  virtual bool NeedsSoftwareSkinning( void ) = 0;
  
  // Apply constant color or alpha modulation
  virtual void AlphaModulate( float alpha ) = 0;
  
  virtual void ColorModulate( float r, float g, float b ) = 0;
  
  // Material Var flags...
  virtual void SetMaterialVarFlag( MaterialVarFlags_t flag, bool on ) = 0;
  
  virtual bool GetMaterialVarFlag( MaterialVarFlags_t flag ) = 0;
  
  // Gets material reflectivity
  virtual void GetReflectivity( Vector &reflect ) = 0;
  
  // Gets material property flags
  virtual bool GetPropertyFlag( MaterialPropertyTypes_t type ) = 0;
  
  // Is the material visible from both sides?
  virtual bool IsTwoSided() = 0;
  
  // Sets the shader associated with the material
  virtual void SetShader( const char *pShaderName ) = 0;
  
  // Can't be const because the material might have to precache itself.
  virtual int GetNumPasses( void ) = 0;
  
  // Can't be const because the material might have to precache itself.
  virtual int GetTextureMemoryBytes( void ) = 0;
  
  // Meant to be used with materials created using CreateMaterial
  // It updates the materials to reflect the current values stored in the
  // material vars
  virtual void Refresh() = 0;
  
  // GR - returns true is material uses lightmap alpha for blending
  virtual bool NeedsLightmapBlendAlpha( void ) = 0;
  
  // returns true if the shader doesn't do lighting itself and requires
  // the data that is sent to it to be prelighted
  virtual bool NeedsSoftwareLighting( void ) = 0;
  
  // Gets at the shader parameters
  virtual int ShaderParamCount() const = 0;
  virtual IMaterialVar **GetShaderParams( void ) = 0;
  
  // Returns true if this is the error material you get back from
  // IMaterialSystem::FindMaterial if the material can't be found.
  virtual bool IsErrorMaterial() const = 0;
  
  virtual void Unused() = 0;
  
  // Gets the current alpha modulation
  virtual float GetAlphaModulation() = 0;
  virtual void GetColorModulation( float *r, float *g, float *b ) = 0;
  
  // Is this translucent given a particular alpha modulation?
  virtual bool IsTranslucentUnderModulation(    float fAlphaModulation = 1.0f ) const = 0;
    
  // fast find that stores the index of the found var in the string table in
  // local cache
  virtual IMaterialVar *FindVarFast( char const *pVarName, unsigned int *pToken ) = 0;
                                     
  // Sets new VMT shader parameters for the material
  virtual void SetShaderAndParams( void *pKeyValues ) = 0;
  virtual const char *GetShaderName() const = 0;
  
  virtual void DeleteIfUnreferenced() = 0;
  
  virtual bool IsSpriteCard() = 0;
  
  virtual void CallBindProxy( void *proxyData ) = 0;
  
  virtual void RefreshPreservingMaterialVars() = 0;
  
  virtual bool WasReloadedFromWhitelist() = 0;
  
  virtual bool SetTempExcluded( bool bSet, int nExcludedDimensionLimit ) = 0;
  
  virtual int GetReferenceCount() const = 0;
};

class CMaterialSystem {
 public:
  IMaterial *FindMaterial( char const *pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL ) {
    typedef IMaterial *( __thiscall * OriginalFn )( PVOID, const char *, const char *, bool, const char * );
    return getvfunc<OriginalFn>( this, 73 )( this, pMaterialName, pTextureGroupName, complain, pComplainPrefix );
  }
  
  IMaterial *CreateMaterial( char const *pMaterialName, KeyValues *kv ) {
    typedef IMaterial *( __thiscall * OriginalFn )( PVOID, const char *, KeyValues * );
    return getvfunc<OriginalFn>( this, 72 )( this, pMaterialName, kv );
  }
  
  IMaterial *GetMaterial( MaterialHandle_t h ) {
    typedef IMaterial *( __thiscall * OriginalFn )( PVOID, MaterialHandle_t );
    return getvfunc<OriginalFn>( this, 78 )( this, h );
  }
  
  MaterialHandle_t FirstMaterial() {
    typedef MaterialHandle_t( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 75 )( this );
  }
  
  MaterialHandle_t InvalidMaterial() {
    typedef MaterialHandle_t( __thiscall * OriginalFn )( PVOID );
    return getvfunc<OriginalFn>( this, 77 )( this );
  }
  
  MaterialHandle_t NextMaterial( MaterialHandle_t h ) {
    typedef MaterialHandle_t( __thiscall * OriginalFn )( PVOID, MaterialHandle_t );
    return getvfunc<OriginalFn>( this, 76 )( this, h );
  }
};

class CModelRender {
 public:
  void ForcedMaterialOverride(
    IMaterial *mat, OverrideType_t type = OVERRIDE_NORMAL ) {
    typedef void( __thiscall * OriginalFn )( PVOID, IMaterial *, OverrideType_t );
    return getvfunc<OriginalFn>( this, 1 )( this, mat, type );
  }
  
  void DrawModelExecute( void *state, ModelRenderInfo_t &pInfo, 
                         matrix3x4 *pCustomBoneToWorld ) {
    typedef void( __thiscall * OriginalFn )( PVOID, void *, ModelRenderInfo_t &, 
        matrix3x4 * );
    return getvfunc<OriginalFn>( this, 19 )( this, state, pInfo, 
           pCustomBoneToWorld );
  }
  
  void SuppressEngineLighting( bool t ) {
    typedef void( __thiscall * OriginalFn )( PVOID, bool );
    return getvfunc<OriginalFn>( this, 22 )( this, t );
  }
};

class CRenderView {
 public:
  // Draw normal brush model.
  // If pMaterialOverride is non-null, then all the faces of the bmodel will
  // set this material rather than their regular material.
  virtual void DrawBrushModel( CBaseEntity *baseentity, model_t *model, const Vector &origin, const Vector &angles, bool sort ) = 0;
                               
  // Draw brush model that has no origin/angles change ( uses identity transform
  // )
  // FIXME, Material proxy CBaseEntity *baseentity is unused right now, use
  // DrawBrushModel for brushes with
  //  proxies for now.
  virtual void DrawIdentityBrushModel( IWorldRenderList *pList, model_t *model ) = 0;
                                       
  // Mark this dynamic light as having changed this frame ( so light maps
  // affected will be recomputed )
  virtual void TouchLight( struct dlight_t *light ) = 0;
  // Draw 3D Overlays
  virtual void Draw3DDebugOverlays( void ) = 0;
  // Sets global blending fraction
  virtual void SetBlend( float blend ) = 0;
  virtual float GetBlend( void ) = 0;
  
  // Sets global color modulation
  virtual void SetColorModulation( float const *blend ) = 0;
  virtual void GetColorModulation( float *blend ) = 0;
  
  // Wrap entire scene drawing
  virtual void SceneBegin( void ) = 0;
  virtual void SceneEnd( void ) = 0;
  
  // Gets the fog volume for a particular point
  virtual void GetVisibleFogVolume( const Vector &eyePoint, VisibleFogVolumeInfo_t *pInfo ) = 0;
                                    
  // Wraps world drawing
  // If iForceViewLeaf is not -1, then it uses the specified leaf as your
  // starting area for setting up area portal culling. This is used by water
  // since your reflected view origin is often in solid space, but we still want
  // to treat it as though the first portal we're looking out of is a water
  // portal, so our view effectively originates under the water.
  virtual IWorldRenderList *CreateWorldList() = 0;
  
  virtual void BuildWorldLists( IWorldRenderList *pList, WorldListInfo_t *pInfo, int iForceFViewLeaf, const VisOverrideData_t *pVisData = nullptr, bool bShadowDepth = false, float *pReflectionWaterHeight = nullptr ) = 0;
  virtual void DrawWorldLists( IWorldRenderList *pList, unsigned long flags, float waterZAdjust ) = 0;
  virtual int GetNumIndicesForWorldLists( IWorldRenderList *pList, unsigned long nFlags ) = 0;
                                          
  // Optimization for top view
  virtual void DrawTopView( bool enable ) = 0;
  virtual void TopViewBounds( Vector const &mins, Vector const &maxs ) = 0;
  
  // Draw lights
  virtual void DrawLights( void ) = 0;
  
  virtual void DrawMaskEntities( void ) = 0;
  
  // Draw surfaces with alpha, don't call in shadow depth pass
  virtual void DrawTranslucentSurfaces( IWorldRenderList *pList, int *pSortList, int sortCount, unsigned long flags ) = 0;
                                        
  // Draw Particles ( just draws the linefine for debugging map leaks )
  virtual void DrawLineFile( void ) = 0;
  // Draw lightmaps
  virtual void DrawLightmaps( IWorldRenderList *pList, int pageId ) = 0;
  // Wraps view render sequence, sets up a view
  virtual void ViewSetupVis( bool novis, int numorigins, const Vector origin[] ) = 0;
                             
  // Return true if any of these leaves are visible in the current PVS.
  virtual bool AreAnyLeavesVisible( int *leafList, int nLeaves ) = 0;
  
  virtual void VguiPaint( void ) = 0;
  // Sets up view fade parameters
  virtual void ViewDrawFade( byte *color, IMaterial *pMaterial ) = 0;
  // Sets up the projection matrix for the specified field of view
  virtual void OLD_SetProjectionMatrix( float fov, float zNear, float zFar ) = 0;
  // Determine lighting at specified position
  virtual Color GetLightAtPoint( Vector &pos ) = 0;
  // Whose eyes are we looking through?
  virtual int GetViewEntity( void ) = 0;
  virtual bool IsViewEntity( int entindex ) = 0;
  // Get engine field of view setting
  virtual float GetFieldOfView( void ) = 0;
  // 1 == ducking, 0 == not
  virtual unsigned char **GetAreaBits( void ) = 0;
  
  // Set up fog for a particular leaf
  virtual void SetFogVolumeState( int nVisibleFogVolume, bool bUseHeightFog ) = 0;
  
  // Installs a brush surface draw override method, null means use normal
  // renderer
  virtual void InstallBrushSurfaceRenderer( IBrushRenderer *pBrushRenderer ) = 0;
  
  // Draw brush model shadow
  virtual void DrawBrushModelShadow( IClientRenderable *pRenderable ) = 0;
  
  // Does the leaf contain translucent surfaces?
  virtual bool LeafContainsTranslucentSurfaces( IWorldRenderList *pList, int sortIndex, unsigned long flags ) = 0;
      
  virtual bool DoesBoxIntersectWaterVolume( const Vector &mins, const Vector &maxs, int leafWaterDataID ) = 0;
      
  virtual void SetAreaState( unsigned char chAreaBits[69], unsigned char chAreaPortalBits[69] ) = 0;
                             
  // See i
  virtual void VGui_Paint( int mode ) = 0;
};

struct CIncomingSequence {
  CIncomingSequence( int instate, int outstate, int seqnr, float time ) {
    inreliablestate = instate;
    outreliablestate = outstate;
    sequencenr = seqnr;
    curtime = time;
  }
  int inreliablestate;
  int outreliablestate;
  int sequencenr;
  float curtime;
};

using CBaseHandle = unsigned int;

class CMoveData {
 public:
  bool m_bFirstRunOfFunctions : 1;
  bool m_bGameCodeMovedPlayer : 1;
  
  CBaseHandle m_nPlayerHandle; // edict index on server, client entity handle
  // on client
  
  int m_nImpulseCommand;     // Impulse command issued.
  Vector m_vecViewAngles;    // Command view angles (local space)
  Vector m_vecAbsViewAngles; // Command view angles (world space)
  int m_nButtons;            // Attack buttons.
  int m_nOldButtons;         // From host_client->oldbuttons;
  float m_flForwardMove;
  float m_flSideMove;
  float m_flUpMove;
  
  float m_flMaxSpeed;
  float m_flClientMaxSpeed;
  
  // Variables from the player edict (sv_player) or entvars on the client.
  // These are copied in here before calling and copied out after calling.
  Vector m_vecVelocity; // edict::velocity    // Current movement
  // direction.
  Vector m_vecAngles;   // edict::angles
  Vector m_vecOldAngles;
  
  // Output only
  float m_outStepHeight; // how much you climbed this move
  Vector m_outWishVel;   // This is where you tried
  Vector m_outJumpVel;   // This is your jump velocity
  
  // Movement constraints (radius 0 means no constraint)
  Vector m_vecConstraintCenter;
  float m_flConstraintRadius;
  float m_flConstraintWidth;
  float m_flConstraintSpeedFactor;
  
  void SetAbsOrigin( const Vector &vec );
  const Vector &GetAbsOrigin() const;
  
 private:
  Vector m_vecAbsOrigin; // edict::origin
};

class IGameMovement {
 public:
  virtual ~IGameMovement( void ) {
  }
  
  // Process the current movement command
  virtual void ProcessMovement( CBaseEntity *pPlayer, CMoveData *pMove ) = 0;
  virtual void StartTrackPredictionErrors( CBaseEntity *pPlayer ) = 0;
  virtual void FinishTrackPredictionErrors( CBaseEntity *pPlayer ) = 0;
  virtual void DiffPrint( char const *fmt, ... ) = 0;
  
  // Allows other parts of the engine to find out the normal and ducked player
  // bbox sizes
  virtual Vector GetPlayerMins( bool ducked ) const = 0;
  virtual Vector GetPlayerMaxs( bool ducked ) const = 0;
  virtual Vector GetPlayerViewOffset( bool ducked ) const = 0;
};

class IPrediction {
 public:
  void SetupMove( CBaseEntity *pLocal, CUserCmd *cmd, CMoveData *moveData ) {
    using oSetupMove = void( __thiscall * )( IPrediction *, CBaseEntity *, CUserCmd *, class IMoveHelper *, CMoveData * );
    getvfunc<oSetupMove>( this, 18 )( this, pLocal, cmd, nullptr, moveData );
  }
  void FinishMove( CBaseEntity *pLocal, CUserCmd *cmd, CMoveData *moveData ) {
    using oFinishMove = void( __thiscall * )( IPrediction *, CBaseEntity *, CUserCmd *, CMoveData * );
    getvfunc<oFinishMove>( this, 19 )( this, pLocal, cmd, moveData );
  }
};

enum ObserverModes_t
{
  OBS_MODE_NONE = 0,		// not in spectator mode
  OBS_MODE_DEATHCAM,		// special mode for death cam animation
  OBS_MODE_FREEZECAM,		// zooms to a target, and freeze-frames on them
  OBS_MODE_FIXED,			// view from a fixed camera position
  OBS_MODE_FIRSTPERSON,	// follow a player in first person view
  OBS_MODE_THIRDPERSON,	// follow a player in third person view
  OBS_MODE_ROAMING,		// free roaming
};

struct CInterfaces {
  CEntList *EntList;
  EngineClient *Engine;
  IPanel *Panels;
  ISurface *Surface;
  ClientModeShared *ClientMode;
  ICvar *cvar;
  CGlobals *globals;
  CHLClient *Client;
  IEngineTrace *EngineTrace;
  IVModelInfo *ModelInfo;
  CModelRender *MdlRender;
  CRenderView *RenderView;
  CMaterialSystem *MatSystem;
  IGameEventManager2 *EventManager;
  IInputSystem *InputSys;
  PVOID *ClientState;
  IGameMovement *GameMovement;
  IPrediction *Prediction;
};

using CreateMoveFn = bool( __thiscall * )( PVOID, float, CUserCmd * );
using FrameStageNotifyThinkFn = void( __fastcall * )( PVOID, void *, ClientFrameStage_t );
using DrawModelExecuteFn = void( __stdcall * )( void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld );
using PaintTraverseFn = void( __thiscall * )( PVOID, unsigned int, bool, bool );
using SendDatagramFn = int( __thiscall * )( CNetChan *, bf_write * );

struct CHooks {
  vmt_single<CreateMoveFn> CreateMove;
  vmt_single<DrawModelExecuteFn> DrawModelExecute;
  vmt_single<FrameStageNotifyThinkFn> FrameStageNotifyThink;
  vmt_single<PaintTraverseFn> PaintTraverse;
  vmt_single<SendDatagramFn> SendDatagram;
  
};

enum gOffsets {
  DrawModelExecute = 19, 
  CreateMove = 21, 
  FrameStageNotifyThink = 35, 
  PaintTraverse = 41, 
  SendDatagram = 46, 
};

extern CInterfaces gInts;
extern CGlobalVariables gCvars;
extern CHooks gHooks;
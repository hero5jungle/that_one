#pragma once
#include "../menu/base/controls.h"
#include "headers/vector.h"

bool checkExists( string file );
void SaveToJson();
void LoadFromJson();

struct CGlobalVariables {
  int aim_index = -1;
  int backtrack_tick = 0;
  int backtrack_arr = 0;
  bool spyalert = false;
  Vector aim;
  int hitbox = -1;
  
  Checkbox Aimbot_enable{ "Enable Aimbot", true };
  Slider   Aimbot_fov{ "Fov", 5, 1, 180, 1 };
  Checkbox Aimbot_proj{ "Loose fov for proj", true };
  Listbox  Aimbot_hitbox{ "Hitbox", { "nearest mouse", "first visible", "head", "body" }, 1, 130 };
  Checkbox Aimbot_silent{ "Silent", true };
  Slider   Aimbot_smooth{ "Smooting", 0, 0, 16, 1 };
  Checkbox Aimbot_range{ "range check", true };
  Slider   Aimbot_ranges{ "shotgun wpn range", 26, 1, 100, 1, 160 };
  Checkbox Backtrack{ "backtrack", true };
  Checkbox Aimbot_melee{ "melee aim", true };
  KeyBind  Aimbot_auto_aim{ "Aim key", VK_SHIFT, e_kbmode::always };
  KeyBind  Aimbot_auto_shoot{ "Shoot key", VK_SHIFT, e_kbmode::always };
  
  Tab Aimbot{ "Aimbot", {
      &Aimbot_enable,
      &Aimbot_fov, &Aimbot_proj, &Aimbot_hitbox,
      &Aimbot_silent, &Aimbot_smooth,
      &Aimbot_range, &Aimbot_ranges,
      &Backtrack,
      &Aimbot_melee,
      &Aimbot_auto_aim, &Aimbot_auto_shoot,
    }
  };
  
  Checkbox Pyro{ "--Pyro--", comment };
  Checkbox pyro_lazy{ "lazy pyro primary", true };
  Checkbox Airblast_enable{ "Enable airblast", true };
  Checkbox Airblast_silent{ "Silent aim", true };
  Checkbox Airblast_rage{ "Rage aim" };
  Checkbox Demoman{ "--Demoman--", comment };
  Checkbox demo_sticky{ "auto sticky", true };
  Checkbox Sniper{ "--Sniper--", comment };
  Checkbox sniper_zoomed{ "Zoomed only", true };
  Checkbox sniper_body{ "sniper bodyshot if kill", true };
  Checkbox sniper_delay{ "Wait for headshot", true };
  Checkbox sniper_nozoom{ "No zoom" };
  Checkbox sniper_noscope{ "No scope", true };
  
  Tab Class{ "Class", {
      &Pyro,
      &pyro_lazy,
      &Airblast_enable, &Airblast_silent, &Airblast_rage,
      &Demoman,
      &demo_sticky,
      &Sniper,
      &sniper_zoomed,
      &sniper_body,
      &sniper_delay,
      &sniper_nozoom,
      &sniper_noscope,
    }
  };
  
  Checkbox Ignore_A{ "--Aimbot--", comment };
  Checkbox Ignore_A_cloak{ "Aimbot ignores cloak", true };
  Checkbox Ignore_A_disguise{ "Aimbot ignores disguise", true };
  Checkbox Ignore_A_taunt{ "Aimbot ignores taunt" };
  Checkbox Ignore_E{ "--ESP--", comment };
  Checkbox Ignore_E_cloak{ "ESP ignores cloak", true };
  Checkbox Ignore_E_disguise{ "ESP ignores disguise" };
  
  Tab Ignore{ "Ignore", {
      &Ignore_A,
      &Ignore_A_cloak, &Ignore_A_disguise, &Ignore_A_taunt,
      
      &Ignore_E,
      &Ignore_E_cloak, &Ignore_E_disguise
    }
  };
  
  Checkbox ESP_enable{ "Enable ESP", true };
  Checkbox ESP_enemy{ "Enemy only" };
  Checkbox ESP_text{ "--text esp--", true };
  Checkbox ESP_building_text{ "building text", true };
  Checkbox ESP_item_text{ "pickup names", true };
  Checkbox ESP_cham{ "--chams--", true };
  Listbox  ESP_cham_mat{ "cham material", { "shaded", "glow", "shiny"}, 0 };
  Checkbox ESP_object_cham{ "Object cham", true };
  Checkbox ESP_building_cham{ "Building cham", true };
  Checkbox ESP_player_cham{ "Player cham", true };
  Checkbox ESP_backtrack{ "Backtrack cham", true };
  Checkbox ESP_proj_cham{ "projectile cham", true };
  Checkbox ESP_hat{ "No player items" };
  Listbox  ESP_hand{ "Hands", { "Normal", "None", "transparent", "mat" }, 0 };
  Checkbox ESP_misc{ "--misc--", true };
  Listbox  ESP_fov{ "Fov circle", {"none", "normal", "dotted" }, 2 };
  Listbox  ESP_target{ "target highlight", { "Off", "line", "color", "both" }, 1 };
  
  
  Tab ESP{ "ESP", {
      &ESP_enable,
      &ESP_enemy,
      
      &ESP_text,
      &ESP_building_text,
      &ESP_item_text,
      
      &ESP_cham,
      &ESP_cham_mat,
      &ESP_object_cham,
      &ESP_building_cham,
      &ESP_player_cham,
      &ESP_backtrack,
      &ESP_proj_cham,
      &ESP_hat,
      &ESP_hand,
      
      &ESP_misc,
      &ESP_fov,
      &ESP_target,
    }
  };
  
  ColorPicker color_pickup{ "pickups", Colors::Green };
  ColorPicker color_building{ "buildings", Colors::White };
  Listbox     color_type{ "cham color type", { "red/blue", "enemy/ally" } };
  ColorPicker color_red{ "red", Colors::Red };
  ColorPicker color_blue{ "blue", Colors::Blue };
  ColorPicker color_enemy{ "enemy", Colors::Orange };
  ColorPicker color_ally{ "ally", Colors::Indigo };
  ColorPicker color_cham_history{ "backtrack history", Colors::White };
  ColorPicker color_cham_tick{ "backtrack tick", Colors::Green };
  ColorPicker color_aim{ "target color", Colors::Violet };
  ColorPicker color_fov{ "fov circle", Colors::Yellow, true };
  Checkbox    world_enabled{ "world/sky color" };
  ColorPicker world_clr{ "World color", Color( 200 ) };
  ColorPicker sky_clr{ "Sky color", Color( 200 ) };
  
  Tab Colors{ "Color", {
      &color_pickup,
      &color_building,
      &color_type,
      &color_red,
      &color_blue,
      &color_enemy,
      &color_ally,
      &color_cham_history,
      &color_cham_tick,
      &color_aim,
      &color_fov,
      &world_enabled,
      &world_clr,
      &sky_clr
    }
  };
  
  
  Checkbox Movement{ "--Movement--", comment };
  Checkbox Bunnyhop{ "Bunny hop", true };
  Checkbox Autostrafe{ "Auto strafe", true };
  Checkbox Random{ "--Random--", comment };
  Checkbox Norecoil{ "No visual recoil", true };
  Checkbox NoPush{ "No push", true };
  Checkbox engine{ "--engine pred--", true };
  Checkbox engine_edge{ "edge jump", true };
  
  Tab Misc{ "Misc", {
      &Movement,
      &Bunnyhop, &Autostrafe,
      
      &Random,
      &Norecoil, &NoPush,
      
      &engine,
      &engine_edge
    }
  };
  
  Checkbox loadbyclass{ "load based on class" };
  func_switch savejson{ "save to json", SaveToJson };
  func_switch loadjson{ "load from json", LoadFromJson };
  
  Tab Config{ "Config", {
      &loadbyclass,
      &savejson,
      &loadjson
    }
  };
  
  Checkbox Sv_cheat{ "sv_cheat 1" };
  Checkbox Thirdperson{ "Third person" };
  Checkbox latency{ "latency test" };
  Slider latency_amount{ "added latency", 0, 0, 800, 10, 120 };
  Slider ping_diff{ "ping difference", 0, -100, 100, 10, 140 };
  Checkbox explanation1{ "200 ping for 400ms backtrack", comment };
  Checkbox explanation2{ "or 800 ping for the 1 sec backtrack", comment };
  
  Tab Testing{ "Testing", {
      &Sv_cheat,
      &Thirdperson,
      &latency,
      &latency_amount,
      &ping_diff,
      &explanation1,
      &explanation2
    }
  };
  
  TabGroup tf2{ {
      &Aimbot,
      &Class,
      &Ignore,
      &ESP,
      &Misc,
      &Colors,
      &Config,
      &Testing
    }
  };
  
};
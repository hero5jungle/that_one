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
  int last_cmd_number = 0;
  
  
  Checkbox Aimbot_enable{ "Enable aimbot", true };
  Slider   Aimbot_fov{ "Fov", 5, 1, 180, 1 };
  Checkbox Aimbot_proj_lazy{ "Lazy proj fov", true };
  Listbox  Aimbot_proj_mode{ "Proj mode", {"Velocity", "Engine"}, 1 };
  Listbox  Aimbot_hitbox{ "Hitbox", { "nearest mouse", "first visible", "head", "body" }, 1, 130 };
  Checkbox Aimbot_silent{ "Silent", true };
  Slider   Aimbot_smooth{ "Smooting", 0, 0, 16, 1 };
  Checkbox Aimbot_range{ "Range check", true };
  Slider   Aimbot_ranges{ "Shotgun range", 26, 1, 100, 1 };
  Checkbox Aimbot_melee{ "Melee aim", true };
  Checkbox Backtrack{ "Backtrack", true };
  KeyBind  Aimbot_auto_aim{ "Aim key", VK_SHIFT, e_kbmode::always };
  KeyBind  Aimbot_auto_shoot{ "Shoot key", VK_SHIFT, e_kbmode::always };
  
  Groupbox AIMBOT{ "--Aimbot--", {
      &Aimbot_enable,
      &Aimbot_fov,
      &Aimbot_proj_lazy,
      &Aimbot_proj_mode,
      &Aimbot_hitbox,
      &Aimbot_silent,
      &Aimbot_smooth,
      &Aimbot_range,
      &Aimbot_ranges,
      &Aimbot_melee,
      &Backtrack,
      &Aimbot_auto_aim,
      &Aimbot_auto_shoot
    }, 170
    
  };
  Tab Aimbot{ "Aimbot", {
      &AIMBOT
    }
  };
  
  
  Checkbox pyro_lazy{ "lazy pyro primary", true };
  Checkbox Airblast_enable{ "Enable airblast", true };
  Checkbox Airblast_silent{ "Silent aim", true };
  
  Checkbox demo_sticky{ "auto sticky", true };
  
  Checkbox sniper_zoomed{ "Zoomed only", true };
  Checkbox sniper_body{ "sniper bodyshot if kill", true };
  Checkbox sniper_delay{ "Wait for headshot", true };
  Checkbox sniper_nozoom{ "No zoom" };
  Checkbox sniper_noscope{ "No scope", true };
  
  Groupbox PYRO{ "--Pyro--", {
      &pyro_lazy,
      &Airblast_enable, &Airblast_silent
    }
  };
  
  Groupbox DEMOMAN{ "--Demoman--", {
      &demo_sticky
    }
  };
  
  Groupbox SNIPER{ "--Sniper--", {
      &sniper_zoomed,
      &sniper_body,
      &sniper_delay,
      &sniper_nozoom,
      &sniper_noscope
    }
  };
  
  Tab Class{ "Class", {
      &PYRO,
      &DEMOMAN,
      &SNIPER,
    }
  };
  
  Checkbox Ignore_A_cloak{ "Aimbot ignores cloak", true };
  Checkbox Ignore_A_disguise{ "Aimbot ignores disguise", true };
  Checkbox Ignore_A_taunt{ "Aimbot ignores taunt" };
  
  Groupbox IGNORE_A{ "--Aimbot--", {
      &Ignore_A_cloak,
      &Ignore_A_disguise,
      &Ignore_A_taunt
    }, 195
  };
  
  Checkbox Ignore_E_cloak{ "ESP ignores cloak", true };
  Checkbox Ignore_E_disguise{ "ESP ignores disguise" };
  Groupbox IGNORE_E{ "--ESP--", {&Ignore_E_cloak, &Ignore_E_disguise} };
  
  Tab Ignore{ "Ignore", {
      &IGNORE_A,
      &IGNORE_E,
    }
  };
  
  Checkbox ESP_enable{ "Enable ESP", true };
  Checkbox ESP_enemy{ "Enemy only" };
  
  Checkbox ESP_building_text{ "building text", true };
  Checkbox ESP_item_text{ "pickup names", true };
  Groupbox ESP_TEXT{ "--text esp--", {&ESP_building_text, &ESP_item_text} };
  
  Listbox  ESP_cham_mat{ "cham material", { "shaded", "glow"}, 0 };
  Checkbox ESP_object_cham{ "Object cham", true };
  Checkbox ESP_building_cham{ "Building cham", true };
  Checkbox ESP_player_cham{ "Player cham", true };
  Checkbox ESP_backtrack{ "Backtrack cham", true };
  Checkbox ESP_proj_cham{ "projectile cham", true };
  Checkbox ESP_hat{ "No player items" };
  Listbox  ESP_hand{ "Hands", { "Normal", "None", "transparent", "mat" }, 0 };
  
  Groupbox ESP_CHAM{ "--chams--", {
      &ESP_cham_mat,
      &ESP_object_cham,
      &ESP_building_cham,
      &ESP_player_cham,
      &ESP_backtrack,
      &ESP_proj_cham,
      &ESP_hat,
      &ESP_hand,
    } };
    
  Listbox  ESP_fov{ "Fov circle", {"none", "normal", "dotted" }, 2 };
  Checkbox ESP_around_fov{ "Out of fov esp", true };
  Listbox  ESP_target{ "target highlight", { "Off", "line", "color", "both" }, 1 };
  
  Groupbox ESP_MISC{ "--misc--", {
      &ESP_fov,
      &ESP_around_fov,
      &ESP_target,
    }
  };
  
  Tab ESP{ "ESP", {
      &ESP_enable,
      &ESP_enemy,
      &ESP_TEXT,
      &ESP_CHAM,
      &ESP_MISC,
    }
  };
  
  ColorPicker color_pickup{ "pickups", Colors::Green };
  ColorPicker color_building{ "buildings", Colors::White };
  
  Groupbox color_world_obj{ "--World obj cham--", {
      &color_pickup,
      &color_building
    }
  };
  
  Listbox     color_type{ "cham color type", { "red/blue", "enemy/ally" } };
  ColorPicker color_red{ "red", Colors::Red };
  ColorPicker color_blue{ "blue", Colors::Blue };
  ColorPicker color_enemy{ "enemy", Colors::Orange };
  ColorPicker color_ally{ "ally", Colors::Indigo };
  
  Groupbox color_cham{ "--Player cham--", {
      &color_type,
      &color_red,
      &color_blue,
      &color_enemy,
      &color_ally
    }
  };
  
  ColorPicker color_cham_history{ "backtrack history", Colors::White };
  ColorPicker color_cham_tick{ "backtrack tick", Colors::Green };
  
  Groupbox color_backtrack{ "--Backtrack cham--", {
      &color_cham_history,
      &color_cham_tick
    }
  };
  
  ColorPicker color_aim{ "target color", Colors::Violet };
  ColorPicker color_fov{ "fov circle", Colors::Yellow, true };
  
  Groupbox color_misc{ "--Misc color--", {
      &color_aim,
      &color_fov
    }
  };
  
  Checkbox    world_enabled{ "world/sky color" };
  ColorPicker world_clr{ "World color", Color( 200 ) };
  ColorPicker sky_clr{ "Sky color", Color( 200 ) };
  
  Groupbox color_world{ "--World color--", {
      &world_enabled,
      &world_clr,
      &sky_clr
    }
  };
  
  Tab Colors{ "Color", {
      &color_world_obj,
      &color_cham,
      &color_backtrack,
      &color_misc,
      &color_world
    }
  };
  
  Checkbox Bunnyhop{ "Bunny hop", true };
  Checkbox Autostrafe{ "Auto strafe", true };
  
  Groupbox MOVEMENT{ "--Movement--", {
      &Bunnyhop,
      &Autostrafe
    }
  };
  
  Checkbox NoRecoil{ "No visual recoil", true };
  Checkbox NoPush{ "No push", true };
  
  Groupbox MISC{ "--Misc--", {
      &NoRecoil,
      &NoPush
    }
  };
  
  Checkbox engine{ "engine prediction", true };
  Checkbox engine_edge{ "edge jump", true };
  Groupbox ENGINE{ "--Engine pred--", {
      &engine,
      &engine_edge
    }
  };
  
  Checkbox latency{ "latency" };
  Slider latency_amount{ "added latency", 0, 0, 800, 10, 120 };
  Slider ping_diff{ "ping difference", 0, -100, 100, 10, 140 };
  
  Groupbox LATENCY{ "--Latency--", {
      &latency,
      &latency_amount,
      &ping_diff,
      
    } };
  Checkbox explanation1{ "200 ping for 400ms backtrack", comment };
  Checkbox explanation2{ "or 800 ping for the 1 sec backtrack", comment };
  
  Tab Misc{ "Misc", {
      &MOVEMENT,
      &MISC,
      &ENGINE,
      &LATENCY,
      &explanation1,
      &explanation2
    }
  };
  
  Checkbox    loadbyclass{ "load based on class" };
  func_switch savejson{ "save to json", SaveToJson };
  func_switch loadjson{ "load from json", LoadFromJson };
  
  Groupbox CONFIG{ "--Config--", {
      &loadbyclass,
      &savejson,
      &loadjson
    }, 200
  };
  
  Tab Config{ "Config", {
      &CONFIG
    }
  };
  
  Checkbox Sv_cheat{ "sv_cheat 1" };
  Checkbox Thirdperson{ "Third person" };
  
  
  Tab Testing{ "Testing", {
      &Sv_cheat,
      &Thirdperson,
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
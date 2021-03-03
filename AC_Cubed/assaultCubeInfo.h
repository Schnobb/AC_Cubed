#pragma once

// Main process name
#define AC_MODULE_NAME L"ac_client.exe"

// Main player entity base static offset
#define AC_PLAYER_ENTITY_OFF 0x109B74

#define AC_PLAYER_GRENADES_OFF {0x158}
#define AC_PLAYER_HEALTH_OFF {0xF8}
#define AC_PLAYER_ARMOR_OFF {0xFC}
#define AC_PLAYER_AKIMBO_ENABLED_OFF {0x10C}
#define AC_PLAYER_AKIMBO_AMMO_MAG_OFF {0x15C}
#define AC_PLAYER_AKIMBO_AMMO_INV_OFF {0x134}

#define AC_AMMO_FUNCTION_OFFSET 0x637E9
#define AC_AMMO_FUNCTION_SIZE 2
#define AC_AMMO_FUNCTION_ORIGINAL "\xFF\x0E" // dec [esi]

#define AC_MAX_GRENADE 3
#define AC_MAX_HEALTH 100
#define AC_MAX_ARMOR 100
#define AC_MAX_AKIMBO_AMMO_MAG 20
#define AC_MAX_AKIMBO_AMMO_INV 100
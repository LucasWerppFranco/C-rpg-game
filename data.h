#ifndef DATA_H
#define DATA_H

typedef struct {
    char name[20];
    char slot[10];
    int attribute; 
    char damage_type[20];
    char ammo_used[20];
    int ammo_quantity;
    char die[10];
    int damage;
} Attack;

typedef struct {
    char name[20];
    char type[20];
    char slot[10];
    char description[256];
    Attack attacks[5];
    int attack_count;
} Weapon;

typedef struct {
    char name[20];
    char type[20];
    int size;
    int health;
    int vigor;
    int strength;
    int agility;
    int intelligence;
    int charisma;
    int power;
    int armor_class;
    char resistances[100];
    char immunities[100];
    char Weakness[100];
    char conditions[100];
    char immunity_conditions[100];
    char slots[3][30];
    Attack attacks[10];
    int attack_count;
    char abilities[100];
} Character;

// MAIN CHARACTER
static const Character player_character = {
    .name = "Hero",
    .type = "Humanoid",
    .size = 2,
    .vigor = 2,
    .strength = 5,
    .agility = 3,
    .intelligence = 4,
    .charisma = 2,
    .power = 1,
    .health = 20 + 2 * 2,  
    .armor_class = 3 * 5,  
    .resistances = "",
    .immunities = "",
    .Weakness = "",
    .conditions = "",
    .immunity_conditions = "",
    .slots = { "", "", "" },
    .attacks = {
        { "Assault", "1-2", 0, "Bludgeoning", "", 0, "1d6", 3 },
    },
    .attack_count = 1,
    .abilities = "Strong Attack"
};

// GOBLIN
static const Character goblin_monster = {
    .name = "Goblin",
    .type = "Humanoid",
    .size = 1,
    .vigor = 2,
    .strength = 3,
    .agility = 6,
    .intelligence = 2,
    .charisma = 1,
    .power = 1,
    .health = 10 + 2 * 2,
    .armor_class = 6 * 5,
    .resistances = "",
    .immunities = "",
    .Weakness = "",
    .conditions = "",
    .immunity_conditions = "",
    .slots = { "", "", "" },
    .attacks = {
        { "Assault", "1-2", 1, "Bludgeoning", "", 0, "1d4", 2 },
        { "Bite", "1-2", 1, "Piercing", "", 0, "1d6", 3 },
    },
    .attack_count = 2,
    .abilities = ""
};

// WEAPONS

static const Weapon longsword = {
    .name = "Longsword",
    .type = "Versatile",
    .slot = "1-2",
    .description = "Long sword for melee combat.",
    .attacks = {
        { "Slash", "1-2", 0, "Slashing", "", 0, "1d6", 6 },
        { "Pierce", "1-2", 0, "Piercing", "", 0, "1d8", 8 },
        { "Slash (Two-handed)", "3", 0, "Slashing", "", 0, "1d8", 8 },
        { "Pierce (Two-handed)", "3", 0, "Piercing", "", 0, "1d10", 10 }
    },
    .attack_count = 4
};

static const Weapon dagger = {
    .name = "Dagger",
    .type = "Light",
    .slot = "1-2",
    .description = "Small dagger for precise strikes.",
    .attacks = {
        { "Quick Slash", "1-2", 1, "Slashing", "", 0, "1d4", 4 },
        { "Stab", "1-2", 1, "Piercing", "", 0, "1d4", 5 }
    },
    .attack_count = 2
};

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "data.h"

// =================== UTILS ===================

#define HEALTH_BAR_WIDTH 20

void print_health_bar(int health, int max_health) {
    int filled_blocks = (health * HEALTH_BAR_WIDTH) / max_health;

    printf("Health: [");
    for (int i = 0; i < HEALTH_BAR_WIDTH; i++) {
        printf(i < filled_blocks ? "█" : " ");
    }
    printf("] %d/%d\n", health, max_health);
}

// =================== CHARACTER "MÉTODOS" ===================

int character_get_attribute(const Character *character, int attribute_index) {
    switch (attribute_index) {
        case 0: return character->strength;
        case 1: return character->agility;
        case 2: return character->vigor;
        default: return 0;
    }
}

int character_calculate_attack_damage(const Character *character, int attack_index) {
    if (attack_index >= character->attack_count) return 0;

    Attack attack = character->attacks[attack_index];
    int attribute_value = character_get_attribute(character, attack.attribute);
    int variation = (rand() % 3) - 1;

    return attack.damage + attribute_value + variation;
}

void character_take_damage(Character *character, int damage) {
    character->health -= damage;
    if (character->health < 0) character->health = 0;
}

// =================== EQUIPAMENTO DE ARMA ===================

void equip_weapon_attacks(Character *character, const Weapon *weapon) {
    for (int i = 0; i < weapon->attack_count; i++) {
        if (character->attack_count < 10) {  // limite de ataques
            character->attacks[character->attack_count++] = weapon->attacks[i];
        }
    }
}

void equip_weapon(Character *character, const Weapon *weapon, int slot_index) {
    strncpy(character->slots[slot_index], weapon->name, sizeof(character->slots[slot_index]));
    equip_weapon_attacks(character, weapon);
}

// =================== VERIFICAÇÃO DE SLOT ===================

int is_slot_equipped(const Character *character, const char *required_slot) {
    if (strcmp(required_slot, "1-2") == 0) {
        if (strlen(character->slots[0]) > 0 || strlen(character->slots[1]) > 0) {
            return 1;
        }
    } else if (strcmp(required_slot, "3") == 0) {
        if (strlen(character->slots[2]) > 0) {
            return 1;
        }
    }
    return 0;
}

// =================== COMBAT ===================

void start_combat(Character *player, Character *enemy) {
    int max_player_health = player->health;
    int max_enemy_health = enemy->health;

    printf("Iniciando combate entre %s e %s!\n", player->name, enemy->name);

    while (player->health > 0 && enemy->health > 0) {
        printf("\n== STATUS ==\n");
        printf("%s:\n", player->name);
        print_health_bar(player->health, max_player_health);

        printf("%s:\n", enemy->name);
        print_health_bar(enemy->health, max_enemy_health);

        printf("\nSua vez! Escolha o ataque:\n");
        for (int i = 0; i < player->attack_count; i++) {
            printf("%d - %s (Requer slot: %s)\n", i + 1, player->attacks[i].name, player->attacks[i].slot);
        }

        int escolha = 0;
        scanf("%d", &escolha);
        escolha = (escolha >= 1 && escolha <= player->attack_count) ? escolha - 1 : 0;

        const char *required_slot = player->attacks[escolha].slot;
        if (!is_slot_equipped(player, required_slot)) {
            printf("Você não pode usar esse ataque! Requer arma equipada no slot %s.\n", required_slot);
            continue;  // volta para nova escolha
        }

        int damage = character_calculate_attack_damage(player, escolha);
        printf("%s ataca %s com %s e causa %d de dano!\n",
               player->name, enemy->name, player->attacks[escolha].name, damage);
        character_take_damage(enemy, damage);

        if (enemy->health <= 0) {
            printf("%s foi derrotado!\n", enemy->name);
            break;
        }

        sleep(1);

        int enemy_attack_index = rand() % enemy->attack_count;
        int enemy_damage = character_calculate_attack_damage(enemy, enemy_attack_index);
        printf("%s ataca %s com %s e causa %d de dano!\n",
               enemy->name, player->name, enemy->attacks[enemy_attack_index].name, enemy_damage);
        character_take_damage(player, enemy_damage);

        if (player->health <= 0) {
            printf("%s foi derrotado!\n", player->name);
            break;
        }

        sleep(1);
    }
}

// =================== MAIN ===================

int main() {
    srand(time(NULL));

    Character player = player_character;
    Character enemy = goblin_monster;

    // Equipa a Dagger no slot 1
    equip_weapon(&player, &dagger, 0);

    // Equipa a Longsword no slot 3
    equip_weapon(&player, &longsword, 2);

    start_combat(&player, &enemy);

    printf("---------------[End]---------------\n");
    return 0;
}


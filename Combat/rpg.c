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

// =================== INVENTÁRIO DINÂMICO ===================

typedef struct {
    char name[20];
    char type[10];
    int quantity;
} InventoryItem;

typedef struct {
    InventoryItem* items;
    int size;
} Inventory;

Inventory player_inventory = {NULL, 0};

void save_inventory_to_file(Inventory* inv) {
    FILE* f = fopen("inventory.txt", "w");
    if (!f) {
        fprintf(stderr, "Erro ao abrir arquivo inventory.txt para salvar.\n");
        return;
    }
    for (int i = 0; i < inv->size; i++) {
        fprintf(f, "%s (%s): %d\n", inv->items[i].name, inv->items[i].type, inv->items[i].quantity);
    }
    fclose(f);
}

void add_to_inventory(Inventory* inv, const char* name, const char* type) {
    for (int i = 0; i < inv->size; i++) {
        if (strcmp(inv->items[i].name, name) == 0 && strcmp(inv->items[i].type, type) == 0) {
            inv->items[i].quantity++;
            save_inventory_to_file(inv);
            return;
        }
    }
    InventoryItem *temp = realloc(inv->items, (inv->size + 1) * sizeof(InventoryItem));
    if (temp == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para o inventário.\n");
        return;
    }
    inv->items = temp;
    strcpy(inv->items[inv->size].name, name);
    strcpy(inv->items[inv->size].type, type);
    inv->items[inv->size].quantity = 1;
    inv->size++;
    save_inventory_to_file(inv);
}

void decrease_inventory_item_quantity(Inventory* inv, const char* name, const char* type) {
    for (int i = 0; i < inv->size; i++) {
        if (strcmp(inv->items[i].name, name) == 0 && strcmp(inv->items[i].type, type) == 0) {
            if (inv->items[i].quantity > 0) {
                inv->items[i].quantity--;
                if (inv->items[i].quantity == 0) {
                }
                save_inventory_to_file(inv);
            }
            return;
        }
    }
}

int get_inventory_quantity(Inventory* inv, const char* name, const char* type) {
    for (int i = 0; i < inv->size; i++) {
        if (strcmp(inv->items[i].name, name) == 0 && strcmp(inv->items[i].type, type) == 0) {
            return inv->items[i].quantity;
        }
    }
    return 0;
}

void free_inventory(Inventory* inv) {
    free(inv->items);
    inv->items = NULL;
    inv->size = 0;
}

void load_inventory_from_file(Inventory* inv) {
    FILE* f = fopen("inventory.txt", "r");
    if (!f) {
        return;
    }
    char line[100];
    while (fgets(line, sizeof(line), f)) {
        char name[20];
        char type[10];
        int quantity;
        if (sscanf(line, " %19[^ (] (%9[^)]): %d", name, type, &quantity) == 3) {
            InventoryItem *temp = realloc(inv->items, (inv->size + 1) * sizeof(InventoryItem));
            if (temp == NULL) {
                fprintf(stderr, "Erro: Falha ao alocar memória ao carregar inventário.\n");
                fclose(f);
                return;
            }
            inv->items = temp;
            strncpy(inv->items[inv->size].name, name, sizeof(inv->items[inv->size].name)-1);
            inv->items[inv->size].name[sizeof(inv->items[inv->size].name)-1] = '\0';
            strncpy(inv->items[inv->size].type, type, sizeof(inv->items[inv->size].type)-1);
            inv->items[inv->size].type[sizeof(inv->items[inv->size].type)-1] = '\0';
            inv->items[inv->size].quantity = quantity;
            inv->size++;
        }
    }
    fclose(f);
}

// =================== SISTEMA DE ARMADURA ===================

Armor equipped_armor;

void equip_armor() {
    equipped_armor = leather_armor;
    printf("Armadura equipada: %s (Defesa: %d)\n", equipped_armor.name, equipped_armor.defense);
    add_to_inventory(&player_inventory, equipped_armor.name, "Armadura");
}

// =================== SISTEMA DE ITENS ===================

const Item* item_inventory[] = { &healing_potion };
const int item_inventory_size = sizeof(item_inventory) / sizeof(item_inventory[0]);

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void use_item(Character *character) {
    printf("\nItens disponíveis:\n");
    int usable_items_count = 0;
    for (int i = 0; i < item_inventory_size; i++) {
        int quantity = get_inventory_quantity(&player_inventory, item_inventory[i]->name, "Item");
        if (quantity > 0) {
            usable_items_count++;
            printf("%d - %s (Quantidade: %d): %s\n", usable_items_count, item_inventory[i]->name, quantity, item_inventory[i]->description);
        }
    }
    if (usable_items_count == 0) {
        printf("Você não possui itens utilizáveis no momento.\n");
        return;
    }
    printf("0 - Não usar item\n");

    int escolha = -1;
    while (escolha < 0 || escolha > usable_items_count) {
        printf("Escolha um item: ");
        if (scanf("%d", &escolha) != 1) {
            clear_input_buffer();
            escolha = -1;
            printf("Entrada inválida! Digite um número.\n");
        }
    }
    clear_input_buffer();

    if (escolha == 0) {
        printf("Nenhum item usado.\n");
    } else {
        int item_index = -1;
        int count = 0;
        for (int i = 0; i < item_inventory_size; i++) {
            int quantity = get_inventory_quantity(&player_inventory, item_inventory[i]->name, "Item");
            if (quantity > 0) {
                count++;
                if (count == escolha) {
                    item_index = i;
                    break;
                }
            }
        }
        if (item_index == -1) {
            printf("Erro: Item inválido.\n");
            return;
        }
        character->health += item_inventory[item_index]->healing;
        printf("Você usou %s e recuperou %d de vida!\n",
               item_inventory[item_index]->name, item_inventory[item_index]->healing);
        decrease_inventory_item_quantity(&player_inventory, item_inventory[item_index]->name, "Item");
    }
}

// =================== VERIFICAÇÃO DE ATAQUE ===================

int is_attack_usable(const Character *character, const char *required_slot) {
    if (strcmp(required_slot, "3") == 0) {
        return strlen(character->slots[2]) > 0;
    } else if (strcmp(required_slot, "1-2") == 0) {
        if (strlen(character->slots[2]) > 0) return 0;
        return strlen(character->slots[0]) > 0 || strlen(character->slots[1]) > 0;
    }
    return 0;
}

// =================== COMBATE ===================

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

void start_combat(Character *player, Character *enemy) {
    int max_player_health = player->health;
    int max_enemy_health = enemy->health;

    printf("\nIniciando combate entre %s e %s!\n", player->name, enemy->name);

    while (player->health > 0 && enemy->health > 0) {
        printf("\n== STATUS ==\n");
        printf("%s:\n", player->name);
        print_health_bar(player->health, max_player_health);
        printf("%s:\n", enemy->name);
        print_health_bar(enemy->health, max_enemy_health);

        printf("\nO que deseja fazer?\n1 - Atacar\n2 - Usar item\nEscolha: ");
        int acao;
        while (scanf("%d", &acao) != 1 || (acao != 1 && acao != 2)) {
            clear_input_buffer();
            printf("Entrada inválida! Digite 1 para Atacar ou 2 para Usar item.\n");
            printf("Escolha: ");
        }
        clear_input_buffer();

        if (acao == 2) {
            use_item(player);
            continue;
        }

        printf("\nSua vez! Escolha o ataque:\n");
        int valid_attacks[10], valid_count = 0;
        for (int i = 0; i < player->attack_count; i++) {
            if (is_attack_usable(player, player->attacks[i].slot)) {
                printf("%d - %s (Requer slot: %s)\n", valid_count + 1, player->attacks[i].name, player->attacks[i].slot);
                valid_attacks[valid_count++] = i;
            }
        }

        if (valid_count == 0) {
            printf("Você não pode realizar nenhum ataque!\n");
            return;
        }

        int escolha = -1;
        while (escolha < 1 || escolha > valid_count) {
            printf("Digite a opção: ");
            if (scanf("%d", &escolha) != 1) {
                clear_input_buffer();
                escolha = -1;
                printf("Entrada inválida! Digite um número válido.\n");
            }
        }
        clear_input_buffer();

        int selected_attack = valid_attacks[escolha - 1];
        int damage = character_calculate_attack_damage(player, selected_attack);
        printf("%s ataca %s com %s e causa %d de dano!\n",
               player->name, enemy->name, player->attacks[selected_attack].name, damage);
        character_take_damage(enemy, damage);

        if (enemy->health <= 0) {
            printf("%s foi derrotado!\n", enemy->name);
            break;
        }

        sleep(1);

        int enemy_attack_index = rand() % enemy->attack_count;
        int enemy_damage = character_calculate_attack_damage(enemy, enemy_attack_index) - equipped_armor.defense;
        if (enemy_damage < 0) enemy_damage = 0;

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

// =================== EQUIPAMENTO DE ARMAS ===================

const Weapon* inventory[] = { &longsword, &dagger };
const int inventory_size = sizeof(inventory) / sizeof(inventory[0]);

void equip_weapon_attacks(Character *character, const Weapon *weapon, int slot_index) {
    for (int i = 0; i < weapon->attack_count; i++) {
        if ((slot_index == 2 && strcmp(weapon->attacks[i].slot, "3") == 0) ||
            (slot_index < 2 && strcmp(weapon->attacks[i].slot, "1-2") == 0)) {
            if (character->attack_count < 10) {
                character->attacks[character->attack_count++] = weapon->attacks[i];
            }
        }
    }
}

void equip_weapon(Character *character, const Weapon *weapon, int slot_index) {
    strncpy(character->slots[slot_index], weapon->name, sizeof(character->slots[slot_index]) - 1);
    character->slots[slot_index][sizeof(character->slots[slot_index]) - 1] = '\0'; 
    equip_weapon_attacks(character, weapon, slot_index);
    add_to_inventory(&player_inventory, weapon->name, "Arma");
}

void unequip_weapon(Character *character, int slot_index) {
    character->slots[slot_index][0] = '\0';
    character->attack_count = 0;
    character->attacks[character->attack_count++] = (Attack){ "Assault", "1-2", 0, "Bludgeoning", "", 0, "1d6", 3 };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < inventory_size; j++) {
            if (strcmp(character->slots[i], inventory[j]->name) == 0) {
                equip_weapon_attacks(character, inventory[j], i);
            }
        }
    }
}

void choose_weapon_for_slot(Character *character, int slot_index) {
    printf("\nEscolha a arma para o slot %d:\n", slot_index + 1);
    for (int i = 0; i < inventory_size; i++) {
        printf("%d - %s (%s)\n", i + 1, inventory[i]->name, inventory[i]->description);
    }
    printf("0 - Desequipar\n");

    int escolha = -1;
    while (escolha < 0 || escolha > inventory_size) {
        printf("Digite a opção: ");
        if (scanf("%d", &escolha) != 1) {
            clear_input_buffer();
            escolha = -1;
            printf("Entrada inválida! Digite um número.\n");
        }
    }
    clear_input_buffer();

    if (escolha == 0) {
        unequip_weapon(character, slot_index);
        printf("Slot %d desequipado.\n", slot_index + 1);
    } else {
        equip_weapon(character, inventory[escolha - 1], slot_index);
        printf("Arma %s equipada no slot %d.\n", inventory[escolha - 1]->name, slot_index + 1);
    }
}

// =================== MAIN ===================

int main() {
    srand(time(NULL));

    Character player = player_character;
    Character enemy = goblin_monster;

    load_inventory_from_file(&player_inventory);

    printf("Configuração de armas antes do combate:\n");
    for (int i = 0; i < 3; i++) {
        choose_weapon_for_slot(&player, i);
    }

    equip_armor();

    printf("\nDeseja usar um item antes do combate?\n1 - Sim\n2 - Não\nEscolha: ");
    int escolha;
    while (scanf("%d", &escolha) != 1 || (escolha != 1 && escolha != 2)) {
        clear_input_buffer();
        printf("Entrada inválida! Digite 1 para Sim ou 2 para Não.\nEscolha: ");
    }
    clear_input_buffer();

    if (escolha == 1) {
        use_item(&player);
    }

    start_combat(&player, &enemy);

    printf("---------------[End]---------------\n");

    free_inventory(&player_inventory);

    return 0;
}

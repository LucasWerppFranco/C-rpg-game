#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>  // Para usar a função sleep
#include <stdbool.h> // Para usar bool
#include <dirent.h>  // Para manipular diretórios

#define MAX_HEALTH 50
#define HEALTH_BAR_WIDTH 20

// Estrutura para armazenar os ataques da arma
typedef struct {
    char name[20];          // Nome do ataque
    char slot[10];          // Slot necessário para usar o ataque
    char attribute[10];     // Atributo que influencia o ataque
    char damage_type[20];   // Tipo de dano
    char ammo_used[20];     // Munição utilizada (se aplicável)
    int ammo_quantity;      // Quantidade de munição utilizada
    char die[10];           // Dado de dano (ex: 1d6)
    int base_damage;        // Dano base
} Attack;

// Estrutura para armazenar as características da arma
typedef struct {
    char name[20];
    char type[20];          // Tipo da arma
    char slot[10];          // Slot onde deve ser equipada
    char description[256];  // Descrição da arma
    Attack attacks[4];      // Até 4 ataques
    int attack_count;       // Número de ataques
} Weapon;

// Estrutura para armazenar as características do personagem
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
    char conditions[100];
    char immunity_conditions[100];
    char slots[3][30];      // Equipamentos slots 1, 2 e 3
    char abilities[100];
} Character;

// Protótipos das funções
void print_health_bar(int health, int max_health);
void update_health(int *health, int change);
void load_character(const char *filename, Character *character);
void load_weapon(const char *filename, Weapon *weapon);
void start_combat(const char *enemy_file);
int calculate_attack_damage(Character *character, Weapon *weapon, int attack_index);
bool is_two_handed_slot_blocked(const Character *character);
bool are_one_handed_slots_blocked(const Character *character);
int inventory_select_weapon(char *selected_weapon_name, size_t name_size);

int main() {
    srand(time(NULL));

    char weapon_filename[256];
    if (inventory_select_weapon(weapon_filename, sizeof(weapon_filename)) == 0) {
        printf("Você escolheu a arma: %s\n", weapon_filename);
        // Para exemplo simples, você pode adicionar a arma escolhida a slot 0 do player em seu arquivo character_sheet.txt
    } else {
        printf("Nenhuma arma disponível para equipar.\n");
    }

    start_combat("goblin.txt");

    return 0;
}

void load_character(const char *filename, Character *character) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo: %s\n", filename);
        exit(1);
    }

    fscanf(file, "Nome: %s\n", character->name);
    fscanf(file, "Tipo: %s\n", character->type);
    fscanf(file, "Tamanho: %d\n", &character->size);
    int vida_base = 0;
    fscanf(file, "Vida: %d + %d\n", &vida_base, &character->vigor);
    character->health = vida_base + character->vigor;

    fscanf(file, "FOR: %d\n", &character->strength);
    fscanf(file, "AGI: %d\n", &character->agility);
    fscanf(file, "VIG: %d\n", &character->vigor);
    fscanf(file, "INT: %d\n", &character->intelligence);
    fscanf(file, "CAR: %d\n", &character->charisma);
    fscanf(file, "POD: %d\n", &character->power);

    fscanf(file, "Resistências: %[^\n]\n", character->resistances);
    fscanf(file, "Imunidades: %[^\n]\n", character->immunities);
    fscanf(file, "Fraquezas: %[^\n]\n", character->conditions);
    fscanf(file, "Imunidade a Condições: %[^\n]\n", character->immunity_conditions);

    character->armor_class = character->agility * 10;

    for (int i = 0; i < 3; i++) {
        fscanf(file, "Slot %d: %[^\n]\n", &i, character->slots[i]);
    }

    fscanf(file, "Habilidades: %[^\n]\n", character->abilities);

    fclose(file);
}

void load_weapon(const char *filename, Weapon *weapon) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo: %s\n", filename);
        exit(1);
    }

    fscanf(file, "Nome: %s\n", weapon->name);
    fscanf(file, "Slots: %s\n", weapon->slot);
    fscanf(file, "Tipo: %s\n", weapon->type);
    fscanf(file, "Descrição: %[^\n]\n", weapon->description);

    weapon->attack_count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Ataque", 6) == 0 && weapon->attack_count < 4) {
            Attack *attack = &weapon->attacks[weapon->attack_count];

            fscanf(file, "Nome: %s\n", attack->name);
            fscanf(file, "Slot: %s\n", attack->slot);
            fscanf(file, "Atributo: %s\n", attack->attribute);
            fscanf(file, "Tipo de Dano: %s\n", attack->damage_type);

            // Alguns ataques podem não ter munição, então tenta ler, mas é opcional
            if(fgets(line, sizeof(line), file) && strncmp(line, "Munição Utilizada:", 17) == 0) {
                sscanf(line, "Munição Utilizada: %s\n", attack->ammo_used);
                fscanf(file, "Quantidade %d\n", &attack->ammo_quantity);
            } else {
                attack->ammo_used[0] = '\0';
                attack->ammo_quantity = 0;
            }

            fscanf(file, "Dado: %s\n", attack->die);
            fscanf(file, "Dano: %d\n", &attack->base_damage);

            weapon->attack_count++;
        }
    }

    fclose(file);
}

void print_health_bar(int health, int max_health) {
    int total_blocks = HEALTH_BAR_WIDTH;
    int filled_blocks = (health * total_blocks) / max_health;
    int damaged_blocks = (max_health - health) / (max_health / 10);

    printf("########################\n");
    printf("##");
    for (int i = 0; i < filled_blocks; i++) {
        printf("█");
    }
    for (int i = 0; i < damaged_blocks; i++) {
        printf("▒");
    }
    for (int i = filled_blocks + damaged_blocks; i < total_blocks; i++) {
        printf(" ");
    }
    printf("##\n");
    printf("########################\n");
}

void update_health(int *health, int change) {
    *health += change;
    if (*health > MAX_HEALTH) *health = MAX_HEALTH;
    if (*health < 0) *health = 0;
}

bool is_two_handed_slot_blocked(const Character *character) {
    return (strlen(character->slots[0]) > 0) || (strlen(character->slots[1]) > 0);
}

bool are_one_handed_slots_blocked(const Character *character) {
    return strlen(character->slots[2]) > 0;
}

int calculate_attack_damage(Character *character, Weapon *weapon, int attack_index) {
    if (attack_index >= weapon->attack_count) return 0;

    Attack *attack = &weapon->attacks[attack_index];
    int base_damage = attack->base_damage;

    int attribute_value = 0;
    if (strcmp(attack->attribute, "FOR") == 0 || strcmp(attack->attribute, "Força") == 0)
        attribute_value = character->strength;
    else if (strcmp(attack->attribute, "AGI") == 0 || strcmp(attack->attribute, "Destreza") == 0)
        attribute_value = character->agility;
    else if(strcmp(attack->attribute, "VIG") == 0)
        attribute_value = character->vigor;

    int variation = rand() % 3;
    return base_damage + attribute_value + variation;
}

int inventory_select_weapon(char *selected_weapon_name, size_t name_size) {
    DIR *d;
    struct dirent *dir;
    char weapon_dir[] = "weapons";
    char weapon_files[100][256];
    char weapon_names[100][50];
    int count = 0;

    d = opendir(weapon_dir);
    if (!d) {
        perror("Erro ao abrir diretório weapons");
        return -1;
    }

    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG) {
            const char *ext = strrchr(dir->d_name, '.');
            if (ext && strcmp(ext, ".txt") == 0) {
                snprintf(weapon_files[count], sizeof(weapon_files[count]), "%s/%s", weapon_dir, dir->d_name);

                FILE *file = fopen(weapon_files[count], "r");
                if (file) {
                    char line[256];
                    while (fgets(line, sizeof(line), file)) {
                        if (strncmp(line, "Nome:", 5) == 0) {
                            char *name_start = line + 5;
                            while (*name_start == ' ' || *name_start == '\t') name_start++;
                            size_t len = strlen(name_start);
                            if(len > 0 && (name_start[len-1] == '\n' || name_start[len-1] == '\r'))
                                name_start[len-1] = '\0';
                            strncpy(weapon_names[count], name_start, sizeof(weapon_names[count]));
                            weapon_names[count][sizeof(weapon_names[count])-1] = '\0';
                            break;
                        }
                    }
                    fclose(file);
                } else {
                    strcpy(weapon_names[count], "Unknown Weapon");
                }

                count++;
                if (count >= 100) break;
            }
        }
    }
    closedir(d);

    if (count == 0) {
        printf("Nenhuma arma disponível no inventário.\n");
        return -1;
    }

    printf("Escolha uma arma para equipar:\n");
    for (int i = 0; i < count; i++) {
        printf("%d - %s\n", i+1, weapon_names[i]);
    }
    int escolha = 0;
    do {
        printf("Digite o número da arma desejada: ");
        if (scanf("%d", &escolha) != 1) {
            while (getchar() != '\n');
            escolha = 0;
            continue;
        }
    } while (escolha < 1 || escolha > count);

    const char *chosen_file = strchr(weapon_files[escolha-1], '/') + 1;
    strncpy(selected_weapon_name, chosen_file, name_size);
    selected_weapon_name[name_size - 1] = '\0';

    return 0;
}

void start_combat(const char *enemy_file) {
    Character player;
    Character enemy;
    Weapon player_weapon[3];
    Weapon enemy_weapon[3];

    load_character("char/character_sheet.txt", &player);
    load_character(enemy_file, &enemy);

    for (int i = 0; i < 3; i++) {
        if (strlen(player.slots[i]) > 0) {
            char weapon_path[256];
            snprintf(weapon_path, sizeof(weapon_path), "weapons/%s.txt", player.slots[i]);
            load_weapon(weapon_path, &player_weapon[i]);
        } else {
            memset(&player_weapon[i], 0, sizeof(Weapon));
        }
    }
    for (int i = 0; i < 3; i++) {
        if (strlen(enemy.slots[i]) > 0) {
            char weapon_path[256];
            snprintf(weapon_path, sizeof(weapon_path), "weapons/%s.txt", enemy.slots[i]);
            load_weapon(weapon_path, &enemy_weapon[i]);
        } else {
            memset(&enemy_weapon[i], 0, sizeof(Weapon));
        }
    }

    printf("Iniciando combate entre %s e %s!\n", player.name, enemy.name);

    while (player.health > 0 && enemy.health > 0) {
        system("clear");

        print_health_bar(player.health, MAX_HEALTH);
        print_health_bar(enemy.health, MAX_HEALTH);

        if (!are_one_handed_slots_blocked(&player)) {
            printf("\nSua vez! Escolha o ataque:\n");
            for (int i = 0; i < player_weapon->attack_count; i++) {
                printf("%d - %s\n", i + 1, player_weapon[0].attacks[i].name);
            }
            int escolha;
            scanf("%d", &escolha);
            escolha = (escolha >= 1 && escolha <= player_weapon[0].attack_count) ? escolha - 1 : 0;
            int damage = calculate_attack_damage(&player, &player_weapon[0], escolha);
            printf("%s ataca %s com %s e causa %d de dano!\n",
                    player.name, enemy.name, player_weapon[0].attacks[escolha].name, damage);
            update_health(&enemy.health, -damage);
        } else if (!is_two_handed_slot_blocked(&player)) {
            if (strlen(player.slots[2]) > 0) {
                printf("\nSua vez! Use ataque do slot 3 (duas mãos):\n");
                for (int i = 0; i < player_weapon[2].attack_count; i++) {
                    printf("%d - %s\n", i + 1, player_weapon[2].attacks[i].name);
                }
                int escolha;
                scanf("%d", &escolha);
                escolha = (escolha >= 1 && escolha <= player_weapon[2].attack_count) ? escolha - 1 : 0;
                int damage = calculate_attack_damage(&player, &player_weapon[2], escolha);
                printf("%s ataca %s com %s (duas mãos) e causa %d de dano!\n",
                        player.name, enemy.name, player_weapon[2].attacks[escolha].name, damage);
                update_health(&enemy.health, -damage);
            } else {
                printf("Você está sem arma para atacar!\n");
            }
        } else {
            printf("Mãos ocupadas, nenhum ataque disponível!\n");
        }
        if (enemy.health <= 0) {
            printf("%s foi derrotado!\n", enemy.name);
            break;
        }

        sleep(2);

        // Ataque do inimigo simplificado: sempre usa ataque principal do slot 0 se existir
        if (!are_one_handed_slots_blocked(&enemy)) {
            if (enemy_weapon[0].attack_count > 0) {
                int damage = calculate_attack_damage(&enemy, &enemy_weapon[0], 0);
                printf("%s ataca %s com %s e causa %d de dano!\n",
                        enemy.name, player.name, enemy_weapon[0].attacks[0].name, damage);
                update_health(&player.health, -damage);
            } else {
                printf("%s não tem ataques disponíveis.\n", enemy.name);
            }
        } else if (!is_two_handed_slot_blocked(&enemy)) {
            if (enemy_weapon[2].attack_count > 0) {
                int damage = calculate_attack_damage(&enemy, &enemy_weapon[2], 0);
                printf("%s ataca %s com %s (duas mãos) e causa %d de dano!\n",
                        enemy.name, player.name, enemy_weapon[2].attacks[0].name, damage);
                update_health(&player.health, -damage);
            } else {
                printf("%s não tem ataques disponíveis.\n", enemy.name);
            }
        } else {
            printf("%s não pode atacar devido a slots ocupados!\n", enemy.name);
        }
        if (player.health <= 0) {
            printf("%s foi derrotado!\n", player.name);
            break;
        }

        sleep(2);
    }
}

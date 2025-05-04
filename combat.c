#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>  // Para usar a função sleep
#include <stdbool.h> // Para usar bool
#include <dirent.h>  // Para manipular diretórios

#define MAX_HEALTH 50
#define HEALTH_BAR_WIDTH 20

// Estrutura para armazenar as características do personagem
typedef struct {
    char name[20];
    char type[20];          // Ex: Humanoide
    int size;               // Ex: Médio
    int health;             // Vida total
    int vigor;
    int strength;           // FOR
    int agility;            // AGI
    int intelligence;       // INT
    int charisma;           // CAR
    int power;              // POD
    int armor_class;        // AC calculada
    char resistances[100];  // Lista de resistências
    char immunities[100];   // Lista de imunidades
    char conditions[100];   // Condições atuais
    char immunity_conditions[100]; // Imunidade a condições
    char slots[3][30];      // Equipamento: slot 0 = mão esquerda, slot 1 = mão direita, slot 2 = duas mãos
    char abilities[100];    // Habilidades
} Character;

// Estrutura para armazenar as características da arma
typedef struct {
    char name[20];
    char type[20];          // leve, pesada, curto alcance, longo alcance, etc.
    char damage_type[20];   // Perfurante, Cortante, Contundente
    char attribute[20];     // Força, Destreza
    int main_attack;
    int secondary_attack;   // 0 se não houver
} Weapon;

// Protótipos de funções
void print_health_bar(int health, int max_health);
void update_health(int *health, int change);
void load_character(const char *filename, Character *character);
void load_weapon(const char *filename, Weapon *weapon);
void start_combat(const char *enemy_file);
int calculate_attack_damage(Character *character, Weapon *weapon, int attack_type); // 1 principal, 2 secundário
bool is_two_handed_slot_blocked(const Character *character);
bool are_one_handed_slots_blocked(const Character *character);
int inventory_select_weapon(char *selected_weapon_name, size_t name_size);

int main() {
    srand(time(NULL)); // Inicializa o gerador de números aleatórios

    // Inventory - escolhe arma
    char weapon_filename[256];
    if (inventory_select_weapon(weapon_filename, sizeof(weapon_filename)) == 0) {
        printf("Você escolheu a arma: %s\n", weapon_filename);
        // Aqui poderia atualizar a slot do jogador antes de começar combate
    } else {
        printf("Nenhuma arma disponível para equipar.\n");
    }

    // Ajuste conforme desejar para passar a arma escolhida ao carregar personagem
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
    int vida_base=0, vigor=0;
    fscanf(file, "Vida: %d + %d\n", &vida_base, &vigor);
    character->vigor = vigor;
    character->health = vida_base + vigor; // Vida total

    fscanf(file, "FOR: %d\n", &character->strength);
    fscanf(file, "AGI: %d\n", &character->agility);
    fscanf(file, "VIG: %d\n", &character->vigor);  // Atualiza vigor direto
    fscanf(file, "INT: %d\n", &character->intelligence);
    fscanf(file, "CAR: %d\n", &character->charisma);
    fscanf(file, "POD: %d\n", &character->power);

    fscanf(file, "Resistências: %[^\n]\n", character->resistances);
    fscanf(file, "Imunidades: %[^\n]\n", character->immunities);
    fscanf(file, "Fraquezas: %[^\n]\n", character->conditions);
    fscanf(file, "Imunidade a Condições: %[^\n]\n", character->immunity_conditions);

    character->armor_class = character->agility * 10;

    // Lê slots de equipamentos
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
    fscanf(file, "Tipo: %s\n", weapon->type);
    fscanf(file, "Tipo de Dano: %s\n", weapon->damage_type);
    fscanf(file, "Atributo: %s\n", weapon->attribute);
    fscanf(file, "Ataque Principal: %d\n", &weapon->main_attack);
    fscanf(file, "Ataque Secundário: %d\n", &weapon->secondary_attack);

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

int calculate_attack_damage(Character *character, Weapon *weapon, int attack_type) {
    int base_attack = 0;
    if (attack_type == 1) {
        base_attack = weapon->main_attack;
    } else if (attack_type == 2 && weapon->secondary_attack > 0) {
        base_attack = weapon->secondary_attack;
    } else {
        return 0;
    }
    int attribute_bonus = 0;
    if (strcmp(weapon->attribute, "Força") == 0) {
        attribute_bonus = character->strength;
    } else if (strcmp(weapon->attribute, "Destreza") == 0) {
        attribute_bonus = character->agility;
    }
    int variation = rand() % 3;
    return base_attack + attribute_bonus + variation;
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
            for (int i = 0; i < 2; i++) {
                if (strlen(player.slots[i]) > 0)
                    printf("%d - Ataque com %s\n", i + 1, player_weapon[i].name);
                else
                    printf("%d - Mão %d vazia\n", i + 1, i + 1);
            }
            int escolha;
            scanf("%d", &escolha);
            escolha = (escolha >= 1 && escolha <= 2) ? escolha - 1 : 0;
            if (strlen(player.slots[escolha]) == 0) {
                printf("Mão vazia, ataque nulo.\n");
            } else {
                int damage = calculate_attack_damage(&player, &player_weapon[escolha], 1);
                printf("%s ataca %s com %s e causa %d de dano!\n", player.name, enemy.name, player_weapon[escolha].name, damage);
                update_health(&enemy.health, -damage);
            }
        } else if (!is_two_handed_slot_blocked(&player)) {
            if (strlen(player.slots[2]) > 0) {
                int damage = calculate_attack_damage(&player, &player_weapon[2], 1);
                printf("%s ataca %s com %s (duas mãos) e causa %d de dano!\n", player.name, enemy.name, player_weapon[2].name, damage);
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

        if (!are_one_handed_slots_blocked(&enemy)) {
            int possiveis[2];
            int count = 0;
            for (int i = 0; i < 2; i++) {
                if (strlen(enemy.slots[i]) > 0) possiveis[count++] = i;
            }
            if (count > 0) {
                int escolha = possiveis[rand() % count];
                int damage = calculate_attack_damage(&enemy, &enemy_weapon[escolha], 1);
                printf("%s ataca %s com %s e causa %d de dano!\n", enemy.name, player.name, enemy_weapon[escolha].name, damage);
                update_health(&player.health, -damage);
            } else {
                printf("%s está sem armas para atacar!\n", enemy.name);
            }
        } else if (!is_two_handed_slot_blocked(&enemy)) {
            if (strlen(enemy.slots[2]) > 0) {
                int damage = calculate_attack_damage(&enemy, &enemy_weapon[2], 1);
                printf("%s ataca %s com %s (duas mãos) e causa %d de dano!\n", enemy.name, player.name, enemy_weapon[2].name, damage);
                update_health(&player.health, -damage);
            } else {
                printf("%s está sem armas para atacar!\n", enemy.name);
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

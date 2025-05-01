#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>  // Para usar a função sleep

#define MAX_HEALTH 50
#define HEALTH_BAR_WIDTH 20

// Estrutura para armazenar as características do personagem
typedef struct {
    char name[20];
    int health;
    int hit_chance; // Chance de acerto (0 a 5)
    int attack_min; // Ataque mínimo
    int attack_max; // Ataque máximo
} Character;

// Funções
void print_health_bar(int health, int max_health);
void update_health(int *health, int change);
void load_character(const char *filename, Character *character);
void start_combat(const char *enemy_file); // Protótipo da função start_combat

// Função principal
int main() {
    srand(time(NULL)); // Inicializa o gerador de números aleatórios

    // Inicia o combate com o goblin
    start_combat("goblin.txt");

    // Inicia o combate com o esqueleto
    // start_combat("esqueleto.txt");

    return 0;
}

// Função para carregar as características do personagem a partir de um arquivo
void load_character(const char *filename, Character *character) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", filename);
        exit(1);
    }

    fscanf(file, "Nome: %s\n", character->name);
    fscanf(file, "Vida: %d\n", &character->health);
    fscanf(file, "Dificuldade de Acerto: %d\n", &character->hit_chance);
    fscanf(file, "Ataque: %d-%d\n", &character->attack_min, &character->attack_max);

    fclose(file);
}

// Função para imprimir a barra de vida
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

// Função para atualizar a saúde
void update_health(int *health, int change) {
    *health += change;
    if (*health > MAX_HEALTH) {
        *health = MAX_HEALTH;
    } else if (*health < 0) {
        *health = 0;
    }
}

void start_combat(const char *enemy_file) {
    Character player;
    Character enemy;

    load_character("player.txt", &player);
    load_character(enemy_file, &enemy);

    printf("Iniciando combate entre %s e %s!\n", player.name, enemy.name);

    while (player.health > 0 && enemy.health > 0) {
        // Jogador ataca
        if (rand() % 6 < player.hit_chance) {
            int damage = rand() % (player.attack_max - player.attack_min + 1) + player.attack_min;
            printf("%s ataca %s e causa %d de dano!\n", player.name, enemy.name, damage);
            update_health(&enemy.health, -damage);
        } else {
            printf("%s erra o ataque!\n", player.name);
        }

        // Verifica se o inimigo foi derrotado
        if (enemy.health <= 0) {
            printf("%s foi derrotado!\n", enemy.name);
            break;
        }

        // Atraso de 2 segundos
        sleep(2); // Para Unix/Linux

        // Inimigo ataca
        if (rand() % 6 < enemy.hit_chance) {
            int damage = rand() % (enemy.attack_max - enemy.attack_min + 1) + enemy.attack_min;
            printf("%s ataca %s e causa %d de dano!\n", enemy.name, player.name, damage);
            update_health(&player.health, -damage);
        } else {
            printf("%s erra o ataque!\n", enemy.name);
        }

        // Verifica se o jogador foi derrotado
        if (player.health <= 0) {
            printf("%s foi derrotado!\n", player.name);
            break;
        }

        // Imprime a barra de vida
        print_health_bar(player.health, MAX_HEALTH);
        print_health_bar(enemy.health, MAX_HEALTH);
    }
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TERRITORIOS 5
#define MAX_NOME 100
#define MAX_COR 50

typedef struct {
    char nome[MAX_NOME];
    char cor_exercito[MAX_COR];
    int tropas;
} Territorio;

static void remover_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') s[len - 1] = '\0';
}

Territorio *criar_mapa(int n) {
    Territorio *m = calloc((size_t)n, sizeof *m);
    if (!m) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    return m;
}

void cadastrar_territorios(Territorio *mapa, int n) {
    for (int i = 0; i < n; ++i) {
        printf("Território %d\n", i + 1);

        printf("Nome: ");
        if (fgets(mapa[i].nome, sizeof(mapa[i].nome), stdin) == NULL) {
            mapa[i].nome[0] = '\0';
        } else {
            remover_newline(mapa[i].nome);
        }

        printf("Cor do Exército: ");
        if (fgets(mapa[i].cor_exercito, sizeof(mapa[i].cor_exercito), stdin) == NULL) {
            mapa[i].cor_exercito[0] = '\0';
        } else {
            remover_newline(mapa[i].cor_exercito);
        }

        printf("Número de Tropas: ");
        if (scanf("%d", &mapa[i].tropas) != 1) {
            mapa[i].tropas = 0;
        }
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }

        printf("\n");
    }
}

void mostrar_mapa(Territorio *mapa, int n) {
    printf("Estado atual do mapa:\n");
    printf("-------------------------------\n");
    for (int i = 0; i < n; ++i) {
        printf("Território %d:\n", i + 1);
        printf("  Nome: %s\n", mapa[i].nome);
        printf("  Cor do Exército: %s\n", mapa[i].cor_exercito);
        printf("  Tropas: %d\n", mapa[i].tropas);
        printf("-------------------------------\n");
    }
}

/* Retorna 1 se o ataque conquistou o território, 0 caso contrário */
int simular_ataque(Territorio *atk, Territorio *def) {
    int atk_roll = rand() % 6 + 1;
    int def_roll = rand() % 6 + 1;

    printf("Dados sorteados -> Atacante: %d  Defensor: %d\n", atk_roll, def_roll);

    if (atk_roll >= def_roll) { /* empates favorecem o atacante */
        def->tropas = def->tropas - 1;
        printf("Resultado: atacante vence. Defensor perde 1 tropa.\n");
        if (def->tropas <= 0) {
            printf("Território conquistado!\n");
            /* Transferência simples: defensor passa a ter 1 tropa do atacante */
            def->tropas = 1;
            strncpy(def->cor_exercito, atk->cor_exercito, MAX_COR - 1);
            def->cor_exercito[MAX_COR - 1] = '\0';
            if (atk->tropas > 1) atk->tropas -= 1;
            return 1;
        }
    } else {
        atk->tropas = atk->tropas - 1;
        if (atk->tropas < 0) atk->tropas = 0;
        printf("Resultado: defensor vence. Atacante perde 1 tropa.\n");
    }
    return 0;
}

int main(void) {
    srand((unsigned) time(NULL));

    Territorio *mapa = criar_mapa(MAX_TERRITORIOS);

    printf("Cadastro de %d territórios\n\n", MAX_TERRITORIOS);
    cadastrar_territorios(mapa, MAX_TERRITORIOS);

    while (1) {
        mostrar_mapa(mapa, MAX_TERRITORIOS);
        printf("Ações:\n");
        printf("  1) Atacar\n");
        printf("  2) Sair\n");
        printf("Escolha: ");
        int opc;
        if (scanf("%d", &opc) != 1) break;
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }

        if (opc == 2) break;
        if (opc != 1) {
            printf("Opção inválida.\n");
            continue;
        }

        int a, d;
        printf("Território atacante (1-%d): ", MAX_TERRITORIOS);
        if (scanf("%d", &a) != 1) break;
        printf("Território defensor (1-%d): ", MAX_TERRITORIOS);
        if (scanf("%d", &d) != 1) break;
        while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }

        if (a < 1 || a > MAX_TERRITORIOS || d < 1 || d > MAX_TERRITORIOS) {
            printf("Índices fora do intervalo.\n");
            continue;
        }
        if (a == d) {
            printf("Atacante e defensor não podem ser o mesmo território.\n");
            continue;
        }
        Territorio *atk = &mapa[a - 1];
        Territorio *def = &mapa[d - 1];

        if (atk->tropas <= 0) {
            printf("Território atacante não tem tropas suficientes para atacar.\n");
            continue;
        }

        simular_ataque(atk, def);
        printf("\n");
    }

    free(mapa);
    return 0;
}
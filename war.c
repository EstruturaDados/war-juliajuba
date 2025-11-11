#include <stdio.h>
#include <string.h>

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

int main(void) {
    Territorio mapa[MAX_TERRITORIOS];

    printf("Cadastro de %d territórios\n\n", MAX_TERRITORIOS);

    for (int i = 0; i < MAX_TERRITORIOS; ++i) {
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
        // consumir o '\n' restante do buffer após o scanf para a próxima fgets
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }

        printf("\n");
    }

    printf("Estado atual do mapa:\n");
    printf("-------------------------------\n");
    for (int i = 0; i < MAX_TERRITORIOS; ++i) {
        printf("Território %d:\n", i + 1);
        printf("  Nome: %s\n", mapa[i].nome);
        printf("  Cor do Exército: %s\n", mapa[i].cor_exercito);
        printf("  Tropas: %d\n", mapa[i].tropas);
        printf("-------------------------------\n");
    }

    return 0;
}
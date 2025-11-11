#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define MAX_TERRITORIOS 5
#define MAX_NOME 100
#define MAX_COR 50
#define MISSao_CONQUISTAS_NECESSARIAS 3

typedef struct {
    char nome[MAX_NOME];
    char cor_exercito[MAX_COR];
    int tropas;
} Territorio;

typedef enum {
    MISS_NONE = 0,
    MISS_DESTRUIR_VERDE,
    MISS_CONQUISTAR_3
} MissaoTipo;

typedef struct {
    MissaoTipo tipo;
    int progresso; /* usado para contar conquistas para MISS_CONQUISTAR_3 */
    int concluida;
} Missao;

static void remover_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') s[len - 1] = '\0';
}

/* Aloca mapa */
Territorio *criar_mapa(int n) {
    Territorio *m = calloc((size_t)n, sizeof *m);
    if (!m) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    return m;
}

/* Inicialização automática de territórios (nomes, cores e tropas aleatórias) */
void inicializar_mapa_auto(Territorio *mapa, int n) {
    const char *nomes[] = {
        "Acre", "Bravos", "Catar", "Delta", "Erebo",
        "Fronteira", "Garra", "Harpia", "Ilha", "Jade"
    };
    const char *cores[] = {
        "Verde", "Vermelho", "Azul", "Amarelo", "Preto"
    };
    int num_nomes = sizeof(nomes) / sizeof(nomes[0]);
    int num_cores = sizeof(cores) / sizeof(cores[0]);

    for (int i = 0; i < n; ++i) {
        strncpy(mapa[i].nome, nomes[rand() % num_nomes], MAX_NOME - 1);
        mapa[i].nome[MAX_NOME - 1] = '\0';

        /* garantir que pelo menos um território comece como Verde */
        if (i == 0) {
            strncpy(mapa[i].cor_exercito, "Verde", MAX_COR - 1);
        } else {
            strncpy(mapa[i].cor_exercito, cores[rand() % num_cores], MAX_COR - 1);
        }
        mapa[i].cor_exercito[MAX_COR - 1] = '\0';

        mapa[i].tropas = rand() % 5 + 1; /* 1..5 tropas */
    }
}

/* Mostra mapa */
void mostrar_mapa(const Territorio *mapa, int n) {
    puts("Estado atual do mapa:");
    puts("-------------------------------");
    for (int i = 0; i < n; ++i) {
        printf("Território %d:\n", i + 1);
        printf("  Nome: %s\n", mapa[i].nome);
        printf("  Cor do Exército: %s\n", mapa[i].cor_exercito);
        printf("  Tropas: %d\n", mapa[i].tropas);
        puts("-------------------------------");
    }
}

/* Simula um ataque; retorna 1 se conquistado, 0 caso contrário */
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

/* Conta quantos territórios pertencem à cor fornecida (case-insensitive) */
int contar_territorios_por_cor(const Territorio *mapa, int n, const char *cor) {
    int cnt = 0;
    for (int i = 0; i < n; ++i) {
        /* comparação case-insensitive simples */
        const char *a = mapa[i].cor_exercito;
        const char *b = cor;
        while (*a && *b && tolower((unsigned char)*a) == tolower((unsigned char)*b)) {
            ++a; ++b;
        }
        if (*a == '\0' && *b == '\0') ++cnt;
    }
    return cnt;
}

/* Atribui missão aleatória */
void atribuir_missao(Missao *m) {
    if (!m) return;
    m->concluida = 0;
    m->progresso = 0;
    if ((rand() % 2) == 0) {
        m->tipo = MISS_DESTRUIR_VERDE;
        puts("Missão atribuída: Destruir o exército Verde.");
    } else {
        m->tipo = MISS_CONQUISTAR_3;
        puts("Missão atribuída: Conquistar 3 territórios.");
    }
}

/* Verifica se missão foi cumprida; atualiza m->concluida e mostra status */
int verificar_missao(Missao *m, const Territorio *mapa, int n) {
    if (!m) return 0;
    if (m->concluida) {
        puts("Missão já concluída!");
        return 1;
    }
    if (m->tipo == MISS_DESTRUIR_VERDE) {
        int cnt_verde = contar_territorios_por_cor(mapa, n, "Verde");
        if (cnt_verde == 0) {
            puts("Parabéns! Você destruiu o exército Verde. Missão cumprida!");
            m->concluida = 1;
            return 1;
        } else {
            printf("Missão em andamento: ainda existem %d territórios verdes.\n", cnt_verde);
            return 0;
        }
    } else if (m->tipo == MISS_CONQUISTAR_3) {
        if (m->progresso >= MISSao_CONQUISTAS_NECESSARIAS) {
            puts("Parabéns! Você conquistou 3 territórios. Missão cumprida!");
            m->concluida = 1;
            return 1;
        } else {
            printf("Missão em andamento: conquistas %d/%d.\n", m->progresso, MISSao_CONQUISTAS_NECESSARIAS);
            return 0;
        }
    }
    puts("Nenhuma missão ativa.");
    return 0;
}

/* Fluxo de ataque interativo; atualiza missão se houver conquista */
void fluxo_ataque(Territorio *mapa, int n, Missao *missao) {
    int a, d, c;
    printf("Território atacante (1-%d): ", n);
    if (scanf("%d", &a) != 1) {
        while ((c = getchar()) != '\n' && c != EOF) {}
        puts("Entrada inválida.");
        return;
    }
    printf("Território defensor (1-%d): ", n);
    if (scanf("%d", &d) != 1) {
        while ((c = getchar()) != '\n' && c != EOF) {}
        puts("Entrada inválida.");
        return;
    }
    while ((c = getchar()) != '\n' && c != EOF) {}

    if (a < 1 || a > n || d < 1 || d > n) {
        puts("Índices fora do intervalo.");
        return;
    }
    if (a == d) {
        puts("Atacante e defensor não podem ser o mesmo território.");
        return;
    }

    Territorio *atk = &mapa[a - 1];
    Territorio *def = &mapa[d - 1];

    if (atk->tropas <= 0) {
        puts("Território atacante não tem tropas suficientes para atacar.");
        return;
    }

    int conquistado = simular_ataque(atk, def);
    if (conquistado && missao && missao->tipo == MISS_CONQUISTAR_3 && !missao->concluida) {
        missao->progresso += 1;
    }
}

/* Ler opção do menu de forma segura */
int ler_opcao(void) {
    int opc;
    int c;
    if (scanf("%d", &opc) != 1) {
        while ((c = getchar()) != '\n' && c != EOF) {}
        return -1;
    }
    while ((c = getchar()) != '\n' && c != EOF) {}
    return opc;
}

int main(void) {
    srand((unsigned) time(NULL));

    Territorio *mapa = criar_mapa(MAX_TERRITORIOS);
    Missao missao = { MISS_NONE, 0, 0 };

    puts("Inicializando mapa automaticamente...");
    inicializar_mapa_auto(mapa, MAX_TERRITORIOS);
    atribuir_missao(&missao);

    for (;;) {
        mostrar_mapa(mapa, MAX_TERRITORIOS);
        puts("Ações:");
        puts("  1) Atacar");
        puts("  2) Verificar Missão");
        puts("  0) Sair");
        printf("Escolha: ");
        int opc = ler_opcao();
        if (opc < 0) {
            puts("Entrada inválida. Saindo.");
            break;
        }
        if (opc == 0) break;
        if (opc == 1) {
            fluxo_ataque(mapa, MAX_TERRITORIOS, &missao);
        } else if (opc == 2) {
            verificar_missao(&missao, mapa, MAX_TERRITORIOS);
        } else {
            puts("Opção inválida.");
        }
        puts("");
        /* Se missão concluída, mostrar e encerrar com vitória */
        if (missao.concluida) {
            puts("Você cumpriu a missão! Fim de jogo.");
            break;
        }
    }

    free(mapa);
    return 0;
}
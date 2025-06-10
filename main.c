#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "expressao.h"

#define MAX 512

// Função própria para valor absoluto de float
float my_fabs(float x) {
    return x < 0 ? -x : x;
}

void imprimeResultado(float resultado) {
    if (my_fabs(resultado - (int)resultado) < 0.0001) {
        printf("%d\n", (int)resultado);
    } else {
        printf("%.2f\n", resultado);
    }
}

int main() {
    int opcao;
    float resultado;
    char continuar;
    char linha[MAX];
    char expressao[MAX];
    char opcao_str[16];  // buffer para entrada do menu

    while (1) {
        printf("\nEscolha o tipo de entrada:\n");
        printf("\n1 - Expressão Infixa\n");
        printf("2 - Expressão Pós-fixada\n");
        printf("3 - Sair\n");
        printf("\nOpcao: ");

        if (fgets(opcao_str, sizeof(opcao_str), stdin) == NULL) {
            // Em caso de erro ou EOF
            printf("\nErro na leitura da opção. Encerrando.\n");
            break;
        }

        // Remove '\n' final
        opcao_str[strcspn(opcao_str, "\n")] = '\0';

        // Verifica se a entrada é exatamente 1 caractere e dígito
        if (strlen(opcao_str) != 1 || !isdigit(opcao_str[0])) {
            printf("\nOpcao invalida! Digite 1, 2 ou 3.\n");
            continue;
        }

        opcao = opcao_str[0] - '0';

        if (opcao == 3) {
            printf("\nPrograma encerrado.\n");
            break;
        }

        if (opcao != 1 && opcao != 2) {
            printf("\nOpcao invalida! Tente novamente.\n");
            continue;
        }

        expressao[0] = '\0'; // limpa a expressão

        printf("\nDigite a expressão (um número ou operador por linha, ENTER vazio para finalizar):\n\n");
        while (1) {
            printf("> ");
            if (fgets(linha, MAX, stdin) == NULL) {
                printf("\nErro na leitura da expressão. Retornando ao menu.\n");
                break;
            }
            linha[strcspn(linha, "\n")] = '\0'; // remove o '\n'

            if (strlen(linha) == 0) break; // ENTER vazio finaliza

            strcat(expressao, linha);
            strcat(expressao, " ");
        }

        if (strlen(expressao) == 0) {
            printf("Expressão vazia. Retornando ao menu.\n");
            continue;
        }

        if (opcao == 1) {
            char posfixa[MAX];
            strcpy(posfixa, getFormaPosFixa(expressao));
            printf("\nA) Expressão em notação pós-fixada: %s\n", posfixa);
            printf("B) Expressão convertida de volta para infixa: %s\n", getFormaInFixa(posfixa));

            printf("C) Valor da expressão avaliada na forma pós-fixada: ");
            resultado = getValorPosFixa(posfixa);
            imprimeResultado(resultado);

            printf("D) Valor da expressão avaliada na forma infixa (original): ");
            resultado = getValorInFixa(expressao);
            imprimeResultado(resultado);
        } else if (opcao == 2) {
            printf("\nA) Expressão em notação pós-fixada: %s\n", expressao);
            printf("B) Expressão convertida de volta para infixa: %s\n", getFormaInFixa(expressao));

            printf("C) Valor da expressão avaliada na forma pós-fixada: ");
            resultado = getValorPosFixa(expressao);
            imprimeResultado(resultado);
        }

        printf("\nDeseja fazer outro cálculo? (s/n): ");
        scanf(" %c", &continuar);
        getchar(); // limpa o buffer

        if (continuar == 'n' || continuar == 'N') {
            printf("\nPrograma encerrado.\n");
            break;
        }
    }

    return 0;
}

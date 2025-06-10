#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "expressao.h"

#define MAX 512
#define PI 3.14159265f
#define LN_10 2.302585092994046f  // ln(10) aproximado

// Verifica se o caractere é operador
int isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^');
}

// Verifica se a string é uma função suportada
int isFunction(char *str) {
    return (!strcmp(str, "raiz") || !strcmp(str, "sen") || !strcmp(str, "cos") ||
            !strcmp(str, "tg") || !strcmp(str, "log"));
}

// Define precedência dos operadores
int precedence(char c) {
    if (c == '^') return 4;
    if (c == '*' || c == '/' || c == '%') return 3;
    if (c == '+' || c == '-') return 2;
    return 0;
}

// Pilha para caracteres
void push(char *stack, int *top, char elem) {
    stack[++(*top)] = elem;
}

// Remove elemento da pilha de caracteres
char pop(char *stack, int *top) {
    return stack[(*top)--];
}

// Extrai token (função ou número) da expressão
int extractToken(char *input, int start, char *token) {
    int i = start, j = 0;
    // Lê letras e ponto
    while (input[i] && (isalpha(input[i]) || input[i] == '.')) {
        token[j++] = input[i++];
    }
    // Lê dígitos e ponto
    while (input[i] && (isdigit(input[i]) || input[i] == '.')) {
        token[j++] = input[i++];
    }
    token[j] = '\0';
    return i;
}

// Converte expressão infixa para pós-fixa (notação polonesa reversa)
char *getFormaPosFixa(char *Str) {
    static char output[MAX];
    output[0] = '\0';
    char stack[MAX];
    int top = -1;
    int i = 0;

    while (i < (int)strlen(Str)) {
        if (Str[i] == ' ') {
            i++;
            continue;
        }
        if (isdigit(Str[i]) || Str[i] == '.') {
            char number[MAX];
            int k = 0;
            while (isdigit(Str[i]) || Str[i] == '.') {
                number[k++] = Str[i++];
            }
            number[k] = '\0';
            strcat(output, number);
            strcat(output, " ");
        } else if (isalpha(Str[i])) {
            char func[MAX];
            int next = extractToken(Str, i, func);
            i = next;
            strcat(output, func);
            strcat(output, " ");
        } else if (Str[i] == '(') {
            push(stack, &top, Str[i++]);
        } else if (Str[i] == ')') {
            while (top != -1 && stack[top] != '(') {
                char op[2] = {pop(stack, &top), '\0'};
                strcat(output, op);
                strcat(output, " ");
            }
            pop(stack, &top);
            i++;
        } else if (isOperator(Str[i])) {
            while (top != -1 && isOperator(stack[top]) &&
                   ((precedence(stack[top]) > precedence(Str[i])) ||
                    (precedence(stack[top]) == precedence(Str[i]) && Str[i] != '^'))) {
                char op[2] = {pop(stack, &top), '\0'};
                strcat(output, op);
                strcat(output, " ");
            }
            push(stack, &top, Str[i++]);
        } else {
            i++;
        }
    }

    while (top != -1) {
        char op[2] = {pop(stack, &top), '\0'};
        strcat(output, op);
        strcat(output, " ");
    }

    return output;
}

// Converte expressão pós-fixa para infixa com parênteses mínimos
char *getFormaInFixa(char *Str) {
    typedef struct {
        char expr[MAX];
        int precedence;
    } ExprNode;

    static ExprNode stack[MAX];
    int top = -1;
    char temp[MAX];
    strcpy(temp, Str);
    char *token = strtok(temp, " ");

    while (token) {
        if (isOperator(token[0]) && strlen(token) == 1) {
            ExprNode right = stack[top--];
            ExprNode left = stack[top--];
            int opPrec = precedence(token[0]);

            char leftExpr[MAX];
            if (left.precedence < opPrec) {
                snprintf(leftExpr, MAX, "(%s)", left.expr);
            } else {
                strcpy(leftExpr, left.expr);
            }

            char rightExpr[MAX];
            if (right.precedence < opPrec || (right.precedence == opPrec && token[0] != '^')) {
                snprintf(rightExpr, MAX, "(%s)", right.expr);
            } else {
                strcpy(rightExpr, right.expr);
            }

            snprintf(stack[++top].expr, MAX, "%s %s %s", leftExpr, token, rightExpr);
            stack[top].precedence = opPrec;
        } else if (isFunction(token)) {
            ExprNode a = stack[top--];
            snprintf(stack[++top].expr, MAX, "%s(%s)", token, a.expr);
            stack[top].precedence = 5; // precedência alta para função
        } else {
            strcpy(stack[++top].expr, token);
            stack[top].precedence = 6; // valor atômico com precedência máxima
        }
        token = strtok(NULL, " ");
    }
    return stack[top].expr;
}

// Converte graus para radianos
float toRadians(float degrees) {
    return degrees * (PI / 180.0f);
}

// Cálculo da raiz quadrada usando método de Newton
float my_sqrt(float x) {
    float guess = x / 2.0f, eps = 0.00001f;
    if (x < 0) return -1;
    while ((guess * guess - x) > eps || (x - guess * guess) > eps) {
        guess = (guess + x / guess) / 2.0f;
    }
    return guess;
}

// Potência para expoentes inteiros (simplificado)
float my_pow(float base, float exp) {
    float result = 1.0f;
    int e = (int)exp;
    int i;
    for (i = 0; i < abs(e); i++) {
        result *= base;
    }
    if (e < 0) result = 1.0f / result;
    return result;
}

// Seno usando série de Taylor
float my_sin(float x) {
    x = toRadians(x);
    float term = x, sum = x;
    for (int i = 1; i <= 5; i++) {
        term *= -1.0f * x * x / ((2 * i) * (2 * i + 1));
        sum += term;
    }
    return sum;
}

// Cosseno usando série de Taylor
float my_cos(float x) {
    x = toRadians(x);
    float term = 1.0f, sum = 1.0f;
    for (int i = 1; i <= 5; i++) {
        term *= -1.0f * x * x / ((2 * i - 1) * (2 * i));
        sum += term;
    }
    return sum;
}

// Tangente como razão entre seno e cosseno
float my_tan(float x) {
    return my_sin(x) / my_cos(x);
}

// Logaritmo natural usando série de Taylor
float my_ln(float x) {
    if (x <= 0) return -1;
    float y = (x - 1) / (x + 1);
    float result = 0.0f;
    for (int i = 1; i <= 9; i += 2) {
        result += (1.0f / i) * my_pow(y, (float)i);
    }
    return 2.0f * result;
}

// Logaritmo base 10 usando my_ln
float my_log10(float x) {
    float ln_x = my_ln(x);
    if (ln_x < 0) return -1;
    return ln_x / LN_10;
}

// Avalia expressão em notação pós-fixa e retorna valor float
float getValorPosFixa(char *StrPosFixa) {
    float stack[MAX];
    int top = -1;
    char temp[MAX];
    strcpy(temp, StrPosFixa);
    char *token = strtok(temp, " ");

    while (token) {
        if (isOperator(token[0]) && strlen(token) == 1) {
            float b = stack[top--];
            float a = stack[top--];
            switch (token[0]) {
                case '+': stack[++top] = a + b; break;
                case '-': stack[++top] = a - b; break;
                case '*': stack[++top] = a * b; break;
                case '/': stack[++top] = a / b; break;
                case '%': stack[++top] = (float)((int)a % (int)b); break;
                case '^': stack[++top] = my_pow(a, b); break;
            }
        } else if (isFunction(token)) {
            float a = stack[top--];
            if (!strcmp(token, "raiz")) {
                stack[++top] = my_sqrt(a);
            } else if (!strcmp(token, "sen")) {
                stack[++top] = my_sin(a);
            } else if (!strcmp(token, "cos")) {
                stack[++top] = my_cos(a);
            } else if (!strcmp(token, "tg")) {
                stack[++top] = my_tan(a);
            } else if (!strcmp(token, "log")) {
                stack[++top] = my_log10(a);
            }
        } else {
            stack[++top] = strtof(token, NULL);
        }
        token = strtok(NULL, " ");
    }
    return stack[top];
}

// Avalia expressão na forma infixa: converte para pós-fixa e chama getValorPosFixa
float getValorInFixa(char *StrInFixa) {
    char *posFixa = getFormaPosFixa(StrInFixa);
    return getValorPosFixa(posFixa);
}

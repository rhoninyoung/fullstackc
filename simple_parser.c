#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int pos = 0;
char *expression;

void error(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

int consume(int expected_token) {
    if (expression[pos] == expected_token) {
        return expression[pos++];
    }
    error("Unexpected token");
    return -1; // unreachable
}

// Forward declaration of expr function
int expr();

int factor() {
    int value = 0;
    if (isdigit(expression[pos])) {
        while (isdigit(expression[pos])) {
            value = value * 10 + (expression[pos++] - '0');
        }
        return value;
    } else if (expression[pos] == '(') {
        consume('(');
        int result = expr();
        consume(')');
        return result;
    } else {
        error("Expected number or '('");
        return -1; // unreachable
    }
}

int term() {
    int result = factor();
    while (expression[pos] == '*' || expression[pos] == '/') {
        if (expression[pos] == '*') {
            consume('*');
            result *= factor();
        } else if (expression[pos] == '/') {
            consume('/');
            int divisor = factor();
            if (divisor == 0) {
                error("Division by zero");
            }
            result /= divisor;
        }
    }
    return result;
}

int expr() {
    int result = term();
    while (expression[pos] == '+' || expression[pos] == '-') {
        if (expression[pos] == '+') {
            consume('+');
            result += term();
        } else if (expression[pos] == '-') {
            consume('-');
            result -= term();
        }
    }
    return result;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"expression\"\n", argv[0]);
        return EXIT_FAILURE;
    }

    expression = argv[1];
    int result = expr();

    printf("Result: %d\n", result);

    return EXIT_SUCCESS;
}




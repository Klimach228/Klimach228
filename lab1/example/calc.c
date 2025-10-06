#include <stdio.h>
#include "calc.h"

float calc(float a, float b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return (b != 0) ? a / b : 0; 
        default: 
            printf("Ошибка: неизвестная операция '%c'\n", op);
            return 0;
    }
}

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "calc.h"

int main() {
    assert(fabs(calc(5, 2, '+') - 7.0) < 1e-6); // числа в компьютере работают не так как в математике 
    assert(fabs(calc(5, 2, '-') - 3.0) < 1e-6); // поэтому мы сравниваем их с определённой точностью
    assert(fabs(calc(5, 2, '*') - 10.0) < 1e-6); // почитайте про это, там прикольно
    assert(fabs(calc(5, 2, '/') - 2.5) < 1e-6);

    // проверка деления на ноль
    assert(calc(5, 0, '/') == 0);

    // проверка неизвестной операции
    assert(calc(5, 2, '%') == 0);

    printf("Все тесты пройдены!\n");
    return 0;
}

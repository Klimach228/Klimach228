#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "generic.h"

// Вспомогательная функция для изменения размера
static bool needToResize(Vector *vector, bool *increase)
{
    // TODO - имплементировать
}

// Определяем увеличивать размер или уменьшать
static int resize(Vector *vector, bool increase)
{
    // TODO - имплементировать
}

Vector *createVector(size_t elem_size)
{
    // TODO - имплементировать
}

int appendVectorItem(Vector *vector, void *el)
{
    // TODO - имплементировать
}

void *getVectorItem(Vector *vector, size_t index)
{
    // TODO - имплементировать
}

int setVectorItem(Vector *vector, size_t index, void *value)
{
    // TODO - имплементировать
}

void *popVectorItem(Vector *vector, size_t index)
{
    // TODO - имплементировать
}

long int findVectorItem(Vector *vector, void *value, EqualsFunc cmp)
{
    // TODO - имплементировать
}

int vectorFree(Vector *vector)
{
    // TODO - имплементировать
}
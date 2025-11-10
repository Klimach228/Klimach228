#include "generic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GenericList *createList(size_t elem_size)
{
    // TODO - имплементировать
}

void appendItem(GenericList *list, void *data)
{
    // TODO - имплементировать
}

int findItem(GenericList *list, void *value, EqualsFunc cmp)
{
    // TODO - имплементировать
}

void *popItem(GenericList *list, size_t index)
{
    // TODO - имплементировать
}

void freeList(GenericList *list)
{
    // TODO - имплементировать
}

unsigned int listLength(GenericList *list)
{
    // TODO - имплементировать
}

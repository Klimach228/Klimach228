#include "generic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

int main() {
    printf("Quick hash table tests...\n\n");
    
    // 1. Создание
    printf("1. Create hash table: ");
    HashTable* table = createHashTable(sizeof(int), sizeof(int));
    assert(table != NULL);
    printf("OK\n");
    
    // 2. Вставка
    printf("2. Insert: ");
    int key = 5, value = 50;
    setItemHashTable(table, &key, &value, HashInt, CmpInt);
    assert(table->size == 1);
    printf("OK\n");
    
    // 3. Получение
    printf("3. Get: ");
    int* result = (int*)getItemHashTable(table, &key, HashInt, CmpInt);
    assert(result != NULL);
    assert(*result == 50);
    printf("OK\n");
    
    // 4. Обновление
    printf("4. Update: ");
    int new_value = 500;
    setItemHashTable(table, &key, &new_value, HashInt, CmpInt);
    result = (int*)getItemHashTable(table, &key, HashInt, CmpInt);
    assert(*result == 500);
    printf("OK\n");
    
    // 5. Удаление
    printf("5. Pop: ");
    int* popped = (int*)popItemHashTable(table, &key, HashInt, CmpInt);
    assert(popped != NULL);
    assert(*popped == 500);
    assert(table->size == 0);
    free(popped);
    printf("OK\n");
    
    // 6. Коллизии
    printf("6. Multiple inserts: ");
    for (int i = 0; i < 5; i++) {
        setItemHashTable(table, &i, &i, HashInt, CmpInt);
    }
    assert(table->size == 5);
    printf("OK\n");
    
    // 7. Коллизии подсчет
    printf("7. Collision count: ");
    unsigned long collisions = getCollisionCount(table, HashInt);
    printf("%lu collisions - OK\n", collisions);
    
    freeHashTable(table);
    
    printf("\n=== ALL TESTS PASSED ===\n");
    return 0;
}
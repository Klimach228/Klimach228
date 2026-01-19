#include "tasks.h"
#include "hash_table/generic.h"
#include "../lab3/vector/generic.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int HashStringPtr(const void *key)
{
    const char **str_ptr = (const char **)key;
    const char *str = *str_ptr;
    
    if (str == NULL) {
        return 0;
    }
    
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return (int)(hash % INT_MAX);
}

int CmpStringPtr(const void *a, const void *b)
{
    const char **str_a = (const char **)a;
    const char **str_b = (const char **)b;
    
    if (*str_a == NULL && *str_b == NULL) return 0;
    if (*str_a == NULL) return -1;
    if (*str_b == NULL) return 1;
    
    return strcmp(*str_a, *str_b);
}

Vector *removeDuplicates(Vector *strings)
{
    if (strings == NULL || vectorSize(strings) == 0) {
        Vector *empty = vectorCreate(sizeof(char*), 0);
        return empty;
    }
    
    // Создаем хэш-таблицу для отслеживания уникальных строк
    HashTable *seen = createHashTable(sizeof(char*), sizeof(int));
    if (seen == NULL) {
        return NULL;
    }
    
    // Создаем результирующий вектор
    Vector *result = vectorCreate(sizeof(char*), vectorSize(strings));
    if (result == NULL) {
        freeHashTable(seen);
        return NULL;
    }
    
    int marker = 1; // маркер для хранения в хеш таблице
    
    for (size_t i = 0; i < vectorSize(strings); i++) {
        char *str = *(char**)vectorGet(strings, i);
        
        // Проверяем, есть ли уже эта строка
        int *found = (int*)getItemHashTable(seen, &str, HashStringPtr, CmpStringPtr);
        
        if (found == NULL) {
            // Строка уникальна - добавляем в результат и в хэш-таблицу
            vectorPushBack(result, &str);
            setItemHashTable(seen, &str, &marker, HashStringPtr, CmpStringPtr);
        }
    }
    
    freeHashTable(seen);
    return result;
}

Vector *encodeStrings(Vector *strings)
{
    if (strings == NULL || vectorSize(strings) == 0) {
        Vector *empty = vectorCreate(sizeof(int), 0);
        return empty;
    }
    
    // Создаем хэш-таблицу для сопоставления строк с кодами
    HashTable *encoding = createHashTable(sizeof(char*), sizeof(int));
    if (encoding == NULL) {
        return NULL;
    }
    
    // Создаем результирующий вектор кодов
    Vector *result = vectorCreate(sizeof(int), vectorSize(strings));
    if (result == NULL) {
        freeHashTable(encoding);
        return NULL;
    }
    
    int next_code = 0;
    
    for (size_t i = 0; i < vectorSize(strings); i++) {
        char *str = *(char**)vectorGet(strings, i);
        
        // Пытаемся найти уже существующий код для этой строки
        int *existing_code = (int*)getItemHashTable(encoding, &str, HashStringPtr, CmpStringPtr);
        
        if (existing_code != NULL) {
            // Строка уже имеет код - используем его
            vectorPushBack(result, existing_code);
        } else {
            // Новая строка - присваиваем новый код
            vectorPushBack(result, &next_code);
            setItemHashTable(encoding, &str, &next_code, HashStringPtr, CmpStringPtr);
            next_code++;
        }
    }
    
    freeHashTable(encoding);
    return result;
}

HashTable *swapKeysValues(HashTable *table, HashFunc hash, CmpFunc cmp)
{
    if (table == NULL || hash == NULL || cmp == NULL) {
        return NULL;
    }
    
    // Проверяем, что ключи и значения имеют тип int
    if (table->key_size != sizeof(int) || table->val_size != sizeof(int)) {
        fprintf(stderr, "Error: swapKeysValues requires int keys and values\n");
        return NULL;
    }
    
    // Создаем новую таблицу (поменяв местами размеры ключа и значения)
    HashTable *swapped = createHashTable(table->val_size, table->key_size);
    if (swapped == NULL) {
        return NULL;
    }
    
    // Проходим по всем слотам исходной таблицы
    for (size_t i = 0; i < table->capacity; i++) {
        unsigned char state = *get_slot_state(table, i);
        
        if (state == SLOT_OCCUPIED) {
            // Получаем ключ и значение из исходной таблицы
            int *key = (int*)get_slot_key(table, i);
            int *value = (int*)get_slot_value(table, i);
            
            // Меняем местами: новое значение = старый ключ, новый ключ = старое значение
            setItemHashTable(swapped, value, key, hash, cmp);
        }
    }
    
    return swapped;
}

// Вспомогательные функции для работы со слотами (добавляем сюда, если они не доступны извне)
static unsigned char* get_slot_state(const HashTable* table, size_t index) {
    if (table == NULL || table->values == NULL) return NULL;
    
    void* slot = vectorGet(table->values, index);
    if (slot == NULL) return NULL;
    
    return (unsigned char*)slot;
}

static void* get_slot_key(const HashTable* table, size_t index) {
    if (table == NULL || table->values == NULL) return NULL;
    
    void* slot = vectorGet(table->values, index);
    if (slot == NULL) return NULL;
    
    return (char*)slot + 1; // Пропускаем состояние слота (1 байт)
}

static void* get_slot_value(const HashTable* table, size_t index) {
    if (table == NULL || table->values == NULL) return NULL;
    
    void* slot = vectorGet(table->values, index);
    if (slot == NULL) return NULL;
    
    return (char*)slot + 1 + table->key_size; // Пропускаем состояние и ключ
}
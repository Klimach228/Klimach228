#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../../lab3/vector/generic.h"
#include "generic.h"

// Константы
#define LOAD_FACTOR_THRESHOLD 0.5

// Структура слота: [состояние (1 байт)] + [ключ] + [значение]
#define SLOT_STATE_OFFSET 0
#define SLOT_KEY_OFFSET 1
#define SLOT_SIZE(table) (1 + (table)->key_size + (table)->val_size)

// Вспомогательные функции для работы со слотами
static unsigned char* get_slot_state(const HashTable* table, size_t index) {
    void* slot = vectorGet(table->values, index);
    return (unsigned char*)slot;
}

static void* get_slot_key(const HashTable* table, size_t index) {
    void* slot = vectorGet(table->values, index);
    return (char*)slot + SLOT_KEY_OFFSET;
}

static void* get_slot_value(const HashTable* table, size_t index) {
    void* slot = vectorGet(table->values, index);
    return (char*)slot + SLOT_KEY_OFFSET + table->key_size;
}

static void init_slot(HashTable* table, size_t index) {
    unsigned char* state = get_slot_state(table, index);
    *state = SLOT_EMPTY;
}

// Функция для вычисления следующего индекса при квадратичном пробировании
static size_t quadratic_probe(size_t start_index, size_t i, size_t capacity) {
    return (start_index + i * i) % capacity;
}

int HashInt(const void *key)
{
    const int* key_ptr = (const int *)key;
    int k = *key_ptr;
    
    // Простая хэш-функция для int
    unsigned int hash = (unsigned int)k;
    hash ^= (hash >> 20) ^ (hash >> 12);
    hash ^= (hash >> 7) ^ (hash >> 4);
    
    return (int)hash;
}

int HashString(const void *key)
{
    const unsigned char *str = (const unsigned char *)key;
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return (int)(hash % TABLE_MIN_SIZE);
}

HashTable *createHashTable(size_t key_size, size_t val_size)
{
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    if (table == NULL){
        return NULL;
    }
    
    table->key_size = key_size;
    table->val_size = val_size;
    table->size = 0;
    table->capacity = TABLE_MIN_SIZE;
    
    // Создаем вектор для хранения слотов
    size_t slot_size = SLOT_SIZE(table);
    table->values = vectorCreate(slot_size, TABLE_MIN_SIZE);
    
    if (table->values == NULL) {
        free(table);
        return NULL;
    }
    
    // Инициализируем все слоты как пустые
    for (size_t i = 0; i < TABLE_MIN_SIZE; i++) {
        vectorPushBack(table->values, NULL); // Добавляем пустой слот
        init_slot(table, i);
    }
    
    return table;
}

void setItemHashTable(HashTable *table, void *key, void *data, HashFunc hash, CmpFunc cmp)
{
    if (table == NULL || key == NULL || data == NULL || hash == NULL || cmp == NULL) {
        return;
    }
    
    // Проверяем, нужен ли rehash
    double load_factor = (double)table->size / table->capacity;
    if (load_factor > LOAD_FACTOR_THRESHOLD) {
        rehashHashTable(table, hash, cmp);
    }
    
    // Вычисляем начальный индекс
    int hash_value = hash(key);
    if (hash_value < 0) hash_value = -hash_value;
    size_t start_index = (size_t)hash_value % table->capacity;
    
    // Ищем место для вставки
    size_t index;
    size_t first_deleted = SIZE_MAX;
    
    for (size_t i = 0; i < table->capacity; i++) {
        index = quadratic_probe(start_index, i, table->capacity);
        unsigned char state = *get_slot_state(table, index);
        
        if (state == SLOT_EMPTY) {
            // Нашли пустой слот
            if (first_deleted != SIZE_MAX) {
                index = first_deleted; // Используем удаленный слот
            }
            break;
        } else if (state == SLOT_DELETED) {
            // Запоминаем первый удаленный слот
            if (first_deleted == SIZE_MAX) {
                first_deleted = index;
            }
            continue;
        } else if (state == SLOT_OCCUPIED) {
            // Проверяем, не тот же ли это ключ
            void* existing_key = get_slot_key(table, index);
            if (cmp(existing_key, key) == 0) {
                // Обновляем существующее значение
                memcpy(get_slot_value(table, index), data, table->val_size);
                return;
            }
        }
    }
    
    // Если нашли удаленный слот, используем его
    if (first_deleted != SIZE_MAX && *get_slot_state(table, index) != SLOT_EMPTY) {
        index = first_deleted;
    }
    
    // Вставляем ключ и значение
    unsigned char* state = get_slot_state(table, index);
    *state = SLOT_OCCUPIED;
    
    void* slot_key = get_slot_key(table, index);
    memcpy(slot_key, key, table->key_size);
    
    void* slot_value = get_slot_value(table, index);
    memcpy(slot_value, data, table->val_size);
    
    table->size++;
}

void *getItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp)
{
    if (table == NULL || key == NULL || hash == NULL || cmp == NULL) {
        return NULL;
    }
    
    // Вычисляем начальный индекс
    int hash_value = hash(key);
    if (hash_value < 0) hash_value = -hash_value;
    size_t start_index = (size_t)hash_value % table->capacity;
    
    // Ищем элемент с квадратичным пробированием
    for (size_t i = 0; i < table->capacity; i++) {
        size_t index = quadratic_probe(start_index, i, table->capacity);
        unsigned char state = *get_slot_state(table, index);
        
        if (state == SLOT_EMPTY) {
            // Достигли пустого слота - элемента нет
            return NULL;
        } else if (state == SLOT_OCCUPIED) {
            // Проверяем ключ
            void* existing_key = get_slot_key(table, index);
            if (cmp(existing_key, key) == 0) {
                // Нашли элемент
                return get_slot_value(table, index);
            }
        }
        // Для SLOT_DELETED продолжаем поиск
    }
    
    return NULL; // в случае, если не нашли
}

void *popItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp)
{
    if (table == NULL || key == NULL || hash == NULL || cmp == NULL) {
        return NULL;
    }
    
    // Находим элемент
    void* value = getItemHashTable(table, key, hash, cmp);
    if (value == NULL) {
        return NULL;
    }
    
    // Вычисляем начальный индекс
    int hash_value = hash(key);
    if (hash_value < 0) hash_value = -hash_value;
    size_t start_index = (size_t)hash_value % table->capacity;
    
    // Ищем элемент для удаления
    for (size_t i = 0; i < table->capacity; i++) {
        size_t index = quadratic_probe(start_index, i, table->capacity);
        unsigned char state = *get_slot_state(table, index);
        
        if (state == SLOT_OCCUPIED) {
            void* existing_key = get_slot_key(table, index);
            if (cmp(existing_key, key) == 0) {
                // Нашли элемент для удаления
                unsigned char* slot_state = get_slot_state(table, index);
                *slot_state = SLOT_DELETED;
                
                // Создаем копию значения для возврата
                void* value_copy = malloc(table->val_size);
                if (value_copy) {
                    memcpy(value_copy, value, table->val_size);
                }
                
                table->size--;
                return value_copy;
            }
        }
    }
    
    return NULL;
}

unsigned long int getCollisionCount(HashTable *table, HashFunc hash)
{
    if (table == NULL || hash == NULL) {
        return 0;
    }
    
    unsigned long int collisions = 0;
    
    for (size_t i = 0; i < table->capacity; i++) {
        unsigned char state = *get_slot_state(table, i);
        if (state == SLOT_OCCUPIED) {
            void* key = get_slot_key(table, i);
            int hash_value = hash(key);
            if (hash_value < 0) hash_value = -hash_value;
            size_t ideal_index = (size_t)hash_value % table->capacity;
            
            // Если элемент не на своем идеальном месте - это коллизия
            if (i != ideal_index) {
                // Проверяем, действительно ли это коллизия, а не результат пробирования
                collisions++;
            }
        }
    }
    
    return collisions;
}

void freeHashTable(HashTable *table)
{
    // TODO: реализовать
}
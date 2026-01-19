#include "tasks.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../lab3/vector/generic.h"

int main() {
    printf("Quick assert tests...\n\n");
    
    // 1. HashStringPtr
    printf("1. HashStringPtr: ");
    char* s = "test";
    int hash = HashStringPtr(&s);
    assert(hash != 0 || strcmp(s, "") == 0);
    printf("OK\n");
    
    // 2. CmpStringPtr
    printf("2. CmpStringPtr: ");
    char* a = "a";
    char* b = "b";
    assert(CmpStringPtr(&a, &b) < 0);
    assert(CmpStringPtr(&b, &a) > 0);
    assert(CmpStringPtr(&a, &a) == 0);
    printf("OK\n");
    
    // 3. removeDuplicates
    printf("3. removeDuplicates: ");
    Vector* vec = vectorCreate(sizeof(char*), 0);
    char* x = "x";
    char* y = "y";
    char* x2 = "x";
    vectorPushBack(vec, &x);
    vectorPushBack(vec, &y);
    vectorPushBack(vec, &x2);
    
    Vector* unique = removeDuplicates(vec);
    assert(vectorSize(unique) == 2);
    vectorFree(vec);
    vectorFree(unique);
    printf("OK\n");
    
    // 4. encodeStrings
    printf("4. encodeStrings: ");
    vec = vectorCreate(sizeof(char*), 0);
    vectorPushBack(vec, &x);
    vectorPushBack(vec, &y);
    
    Vector* codes = encodeStrings(vec);
    assert(vectorSize(codes) == 2);
    assert(*(int*)vectorGet(codes, 0) == 0);
    assert(*(int*)vectorGet(codes, 1) == 1);
    vectorFree(vec);
    vectorFree(codes);
    printf("OK\n");
    
    // 5. swapKeysValues
    printf("5. swapKeysValues: ");
    HashTable* table = createHashTable(sizeof(int), sizeof(int));
    int k = 5, v = 50;
    setItemHashTable(table, &k, &v, HashInt, CmpInt);
    
    HashTable* swapped = swapKeysValues(table, HashInt, CmpInt);
    assert(swapped != NULL);
    assert(swapped->size == 1);
    
    int* found = (int*)getItemHashTable(swapped, &v, HashInt, CmpInt);
    assert(found != NULL);
    assert(*found == k);
    
    freeHashTable(table);
    freeHashTable(swapped);
    printf("OK\n");
    
    printf("\n=== ALL ASSERT TESTS PASSED ===\n");
    return 0;
}
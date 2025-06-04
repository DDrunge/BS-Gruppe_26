#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "keyValStore.h"

#define MAX_ENTRIES 100
#define MAX_KEY 64
#define MAX_VALUE 256

typedef struct {
    char key[MAX_KEY];
    char value[MAX_VALUE];
} KeyValue;

static KeyValue store[MAX_ENTRIES];
static int store_count = 0;

int put(const char* key, const char* value) {
    for (int i = 0; i < store_count; i++) {
        if (strcmp(store[i].key, key) == 0) {
            strncpy(store[i].value, value, MAX_VALUE);
            return 1; // Überschrieben
        }
    }

    if (store_count >= MAX_ENTRIES) return -1;

    strncpy(store[store_count].key, key, MAX_KEY);
    strncpy(store[store_count].value, value, MAX_VALUE);
    store_count++;
    return 0; // Neu eingefügt
}

int get(const char* key, char* result) {
    for (int i = 0; i < store_count; i++) {
        if (strcmp(store[i].key, key) == 0) {
            strncpy(result, store[i].value, MAX_VALUE);
            return 0;
        }
    }
    return -1; // Nicht gefunden
}

int del(const char* key) {
    for (int i = 0; i < store_count; i++) {
        if (strcmp(store[i].key, key) == 0) {
            store[i] = store[store_count - 1]; // Ersetze durch letztes Element
            store_count--;
            return 0;
        }
    }
    return -1;
}

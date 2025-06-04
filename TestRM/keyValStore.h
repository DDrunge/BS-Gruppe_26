#ifndef KEYVALSTORE_H
#define KEYVALSTORE_H

int put(const char* key, const char* value);
int get(const char* key, char* result);
int del(const char* key);

#endif // KEYVALSTORE_H

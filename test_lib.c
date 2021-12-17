#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "test_lib.h"

void hello(char *message) {
    printf("Hello, I have got your message: %s\n", message);
}

int addSum(int a, int b) {
    return a + b;
}

void concatString(int count, ...) {
    va_list ap;
    va_start(ap, count);
    char *buf = va_arg(ap, char*);
    bool includes_concat_info = va_arg(ap, int);
    for (int i = 0; i < count - 2; i++) {
        char *str = va_arg(ap, char*);
        strcat(buf, str);
    }
    if (includes_concat_info) {
        char concat_info[50]= {0};
        sprintf(concat_info, "(concatenate from %d strings)", count - 2);
        strcat(buf, concat_info);
    }
    va_end(ap);
}

char* (*getValueByKey)(char* key);
void initImportedFunctions(void *func) {
    getValueByKey = func;
}

void showValue(char* key) {
    char *value = getValueByKey(key);
    if (value == NULL) {
        printf("Not found value for %s\n", key);
        return;
    }
    printf("The corresponding value of %s is: %s\n", key, value);
}
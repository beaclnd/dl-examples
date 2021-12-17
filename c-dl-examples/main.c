#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char *getValueByKey(char *key) {
    if(strcmp(key, "some") == 0) {
        char *value = "{\"a\": 123 \"b\": \"Hi!\"}";
        return value;
    }
    return NULL;
}

void dllinked() {
    #ifdef DLLINKED
    #include "../test_lib.h"

    char buf[1000] = {0};
    concatString(4, buf, true, "This is the string 1", " and this is the string 2");
    hello(buf);

    int sum = addSum(123, 789);
    sprintf(buf, "the sum is: %d", sum);
    hello(buf);

    initImportedFunctions(getValueByKey);
    showValue("some");
    showValue("none");
    #endif
}

void dlopened() {
    #ifdef DLOPENED
    #include <dlfcn.h>

    typedef void (*anyfunc)(int, ...);

    void *dlhandle = dlopen("./libtest_lib.so", RTLD_LAZY);
    void (*hello)(char*) = dlsym(dlhandle, "hello");

    // char* (*concatString)(char*, char*) = dlsym(dlhandle, "concatString");
    anyfunc concatString = dlsym(dlhandle, "concatString");
    char buf[1000] = {0};
    concatString(6, buf, true, "This is the string a;", " and this is the string b;", " Hah, this is the string c;", " this is the string d.");
    hello(buf);

    int (*addSum)(int, int) = dlsym(dlhandle, "addSum");
    int sum = addSum(123, 321);
    sprintf(buf, "the sum is: %d", sum);
    hello(buf);

    void (*initImportedFunctions)(void*) = dlsym(dlhandle, "initImportedFunctions");
    initImportedFunctions(getValueByKey);
    void (*showValue)(char*) = dlsym(dlhandle, "showValue");
    showValue("some");
    showValue("none");
    #endif
}

int main(int argc, char **argv) {
    dllinked();
    dlopened();

    return 0;
}
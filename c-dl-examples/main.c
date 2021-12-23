#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

// A struct to hold the cadidate value
typedef struct _Value {
    int id;
    char* name;
    int age;
    int votes;
} CandidateValue;
typedef struct _GetValueByKeyResult {
    bool exception; 
    void *value;
} GetValueByKeyResult;

// An array to mock the kv store
CandidateValue kvStore[] = {
    {0, "White", 49, 0}, {1, "Pink", 29, 0}
};

// The functions to be imported into the dl
void getValueByKey(int key, GetValueByKeyResult *result) {
    if (key > 1 || key < 0) {
        result->exception = true;
        result->value = NULL;
        return;
    }
    result->exception = false;
    CandidateValue *cv = result->value;
    cv->id = kvStore[key].id;
    cv->name = kvStore[key].name;
    cv->age = kvStore[key].age;
    cv->votes = kvStore[key].votes;
}
int setValueByKey(int key, void *value) {
    if (key > 1 || key < 0) {
        return -1;
    }
    CandidateValue *cv = value;
    kvStore[key].id = cv->id;
    kvStore[key].name = cv->name;
    kvStore[key].age = cv->age;
    kvStore[key].votes = cv->votes;
    return 0;
}
void myLog(char* message) {
    printf("%s\n", message);
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

    initImportedFunctions(getValueByKey, setValueByKey, myLog);
    vote(1, -1);
    vote(1, 0);
    vote(1, 0);
    vote(1, 1);
    printf("The candidate 0: (id: %d, name: %s, age: %d, votes: %d)\n", kvStore[0].id, kvStore[0].name, kvStore[0].age, kvStore[0].votes);
    printf("The candidate 1: (id: %d, name: %s, age: %d, votes: %d)\n", kvStore[1].id, kvStore[1].name, kvStore[1].age, kvStore[1].votes);
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

    void (*initImportedFunctions)(void*, void*, void*) = dlsym(dlhandle, "initImportedFunctions");
    initImportedFunctions(getValueByKey, setValueByKey, myLog);
    void (*vote)(int, int) = dlsym(dlhandle, "vote");
    vote(1, -1);
    vote(1, 0);
    vote(1, 0);
    vote(1, 1);
    printf("The candidate 0: (id: %d, name: %s, age: %d, votes: %d)\n", kvStore[0].id, kvStore[0].name, kvStore[0].age, kvStore[0].votes);
    printf("The candidate 1: (id: %d, name: %s, age: %d, votes: %d)\n", kvStore[1].id, kvStore[1].name, kvStore[1].age, kvStore[1].votes);
    #endif
}

int main(int argc, char **argv) {
    dllinked();
    dlopened();

    return 0;
}
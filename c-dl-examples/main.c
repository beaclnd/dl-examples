#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

// A struct to hold the cadidate value
typedef struct _Value
{
    char *id;
    char *name;
    int age;
    int votes;
} CandidateValue;
typedef struct _GetValueByKeyResult
{
    bool status;
    void *value;
} GetValueByKeyResult;
typedef struct _Args {
    char *id;
} VoteArgs;

// An array to mock the kv store
CandidateValue c1 = {"a123", "White", 49, 0};
CandidateValue c2 = {"b456", "Pink", 29, 0};

// A struct to hold the exported function result
typedef struct _Result
{
    char *code;
    char *message;
} FunctionResult;

// The functions to be imported into the dl
void getValueByKey(char *key, GetValueByKeyResult *result)
{
    if (strcmp(key, "a123") != 0 && strcmp(key, "b456") != 0)
    {
        result->status = false;
        result->value = NULL;
        return;
    }
    result->status = true;
    CandidateValue *cv = result->value;
    if (strcmp(key, "a123") == 0)
    {
        cv->id = c1.id;
        cv->name = c1.name;
        cv->age = c1.age;
        cv->votes = c1.votes;
    }
    else
    {
        cv->id = c2.id;
        cv->name = c2.name;
        cv->age = c2.age;
        cv->votes = c2.votes;
    }
}
bool setValueByKey(char *key, void *value)
{
    if (strcmp(key, "a123") != 0 && strcmp(key, "b456") != 0)
    {
        return false;
    }

    CandidateValue *cv = value;
    if (strcmp(key, "a123") == 0) {
        c1.id = cv->id;
        c1.name = cv->name;
        c1.age = cv->age;
        c1.votes = cv->votes;
    } else {
        c2.id = cv->id;
        c2.name = cv->name;
        c2.age = cv->age;
        c2.votes = cv->votes;
    }
    return true;
}
void myLog(char *message)
{
    printf("%s\n", message);
}

void dllinked()
{
#ifdef DLLINKED
#include "../test_lib.h"

    char buf[1000] = {0};
    concatString(4, buf, true, "This is the string 1", " and this is the string 2");
    hello(buf);

    int sum = addSum(123, 789);
    sprintf(buf, "the sum is: %d", sum);
    hello(buf);

    initImportedFunctions(getValueByKey, setValueByKey, myLog);
    FunctionResult fr;
    char msgBuf[100] = {0};
    fr.message = msgBuf;
    VoteArgs args0 = {"not-existed"};
    VoteArgs args1 = {"a123"};
    VoteArgs args2 = {"b456"};
    callMethod("not-existed", &args1, &fr);
    printf("Method result: %s\n", fr.message);
    callMethod("vote", &args0, &fr);
    printf("Method result: %s\n", fr.message);
    callMethod("vote", &args1, &fr);
    printf("Method result: %s\n", fr.message);
    callMethod("vote", &args1, &fr);
    printf("Method result: %s\n", fr.message);
    callMethod("vote", &args2, &fr);
    printf("Method result: %s\n", fr.message);
    printf("The candidate a123: (id: %s, name: %s, age: %d, votes: %d)\n", c1.id, c1.name, c1.age, c1.votes);
    printf("The candidate b456: (id: %s, name: %s, age: %d, votes: %d)\n", c2.id, c2.name, c2.age, c2.votes);
#endif
}

void dlopened()
{
#ifdef DLOPENED
#include <dlfcn.h>

    typedef void (*anyfunc)(int, ...);

    void *dlhandle = dlopen("./libtest_lib.so", RTLD_LAZY);
    void (*hello)(char *) = dlsym(dlhandle, "hello");

    // char* (*concatString)(char*, char*) = dlsym(dlhandle, "concatString");
    anyfunc concatString = dlsym(dlhandle, "concatString");
    char buf[1000] = {0};
    concatString(6, buf, true, "This is the string a;", " and this is the string b;", " Hah, this is the string c;", " this is the string d.");
    hello(buf);

    int (*addSum)(int, int) = dlsym(dlhandle, "addSum");
    int sum = addSum(123, 321);
    sprintf(buf, "the sum is: %d", sum);
    hello(buf);

    void (*initImportedFunctions)(void *, void *, void *) = dlsym(dlhandle, "initImportedFunctions");
    initImportedFunctions(getValueByKey, setValueByKey, myLog);
    void (*callMethod)(char *, void *, FunctionResult *, ...) = dlsym(dlhandle, "callMethod");
    FunctionResult fr;
    char msgBuf[100] = {0};
    fr.message = msgBuf;
    VoteArgs args0 = {"not-existed"};
    VoteArgs args1 = {"a123"};
    VoteArgs args2 = {"b456"};
    callMethod("not-existed", &args1, &fr);
    printf("Method result: %s\n", fr.message);
    callMethod("vote", &args0, &fr);
    printf("Method result: %s\n", fr.message);
    callMethod("vote", &args1, &fr);
    printf("Method result: %s\n", fr.message);
    callMethod("vote", &args1, &fr);
    printf("Method result: %s\n", fr.message);
    callMethod("vote", &args2, &fr);
    printf("Method result: %s\n", fr.message);
    printf("The candidate a123: (id: %s, name: %s, age: %d, votes: %d)\n", c1.id, c1.name, c1.age, c1.votes);
    printf("The candidate b456: (id: %s, name: %s, age: %d, votes: %d)\n", c2.id, c2.name, c2.age, c2.votes);
#endif
}

int main(int argc, char **argv)
{
    dllinked();
    dlopened();

    return 0;
}
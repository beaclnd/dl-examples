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

// void concatString(char* buf, bool includes_concat_info, int count, ...);
void concatString(int count, ...) {
    va_list ap;
    va_start(ap, count);
    char *buf = va_arg(ap, char*);
    // Type promotion
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

GetValueByKey getValueByKey;
SetValueByKey setValueByKey;
Log logSomething;
void initImportedFunctions(GetValueByKey getValueByKeyFunc, SetValueByKey setValueByKeyFunc, Log logFunc) {
    getValueByKey = getValueByKeyFunc;
    setValueByKey = setValueByKeyFunc;
    logSomething = logFunc;
}

// The inner of the function will call the callback functions
static FunctionResult vote(int id, FunctionResult *fr) {
    GetValueByKeyResult result; 
    CandidateValue cv;
    result.value = &cv;
    getValueByKey(id, &result);
    if (result.exception) {
        char buf[100] = {0};
        sprintf(buf, "Not found the candidate whose id is: %d", id);
        logSomething(buf);
        fr->code = -1;
        char *msgBuf = fr->message;
        sprintf(msgBuf, "Faied to found the candidate: id = %d", id);
        return *fr;
    }
    cv.votes += 1;
    int result_code = setValueByKey(id, &cv);
    if (result_code == -1) {
        fr->code = -1;
        char *msgBuf = fr->message;
        sprintf(msgBuf, "Faied to set the candidate: id = %d", id);
        return *fr;
    }
    fr->code = 0;
    char *msgBuf = fr->message;
    sprintf(msgBuf, "Success to vote");
    return *fr;
}

FunctionResult callMethod(char* name, FunctionResult *result, ...) {
    va_list ap;
    va_start(ap, result);
    if (strcmp(name, "vote") == 0)  {
        int id = va_arg(ap, int);
        return vote(id, result);
    } else {
        result->code = -1;
        char *buf = result->message; 
        snprintf(buf, 100, "Not found the function: %s", name);
    }
    va_end(ap);
    return *result;
}

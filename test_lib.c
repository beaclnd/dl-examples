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
static FunctionResult vote(VoteArgs *args, FunctionResult *fr) {
    GetValueByKeyResult get_result; 
    CandidateValue cv;
    get_result.value = &cv;
    char *id = args->id;
    getValueByKey(id, &get_result);
    if (!get_result.status) {
        char buf[100] = {0};
        sprintf(buf, "Not found the candidate: id = %s", id);
        logSomething(buf);
        fr->status = false;
        char *msgBuf = fr->message;
        sprintf(msgBuf, "Faied to found the candidate: id = %s", id);
        return *fr;
    }
    cv.votes += 1;
    bool set_result = setValueByKey(id, &cv);
    if (!set_result) {
        fr->status = false;
        char *msgBuf = fr->message;
        sprintf(msgBuf, "Faied to set the candidate: id = %s", id);
        return *fr;
    }
    fr->status = true;
    char *msgBuf = fr->message;
    sprintf(msgBuf, "Success to vote");
    return *fr;
}

FunctionResult callMethod(char* name, void *args, FunctionResult *result) {
    if (strcmp(name, "vote") == 0)  {
        return vote(args, result);
    } else {
        result->status = false;
        char *buf = result->message; 
        snprintf(buf, 100, "Not found the function: %s", name);
    }
    return *result;
}

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

GetValueByKey getValueByKey;
SetValueByKey setValueByKey;
Log logSomething;
void initImportedFunctions(GetValueByKey getValueByKeyFunc, SetValueByKey setValueByKeyFunc, Log logFunc) {
    getValueByKey = getValueByKeyFunc;
    setValueByKey = setValueByKeyFunc;
    logSomething = logFunc;
}

// @param count <int> variable parameter count except itself
// @param id <int> candidate id
FunctionResult vote(int count, ...) {
    va_list ap;
    va_start(ap, count);
    int id = va_arg(ap, int);
    // GetValueByKeyResult result;
    GetValueByKeyResult result; 
    CandidateValue cv;
    result.value = &cv;
    getValueByKey(id, &result);
    if (result.exception) {
        va_end(ap);
        logSomething("Not found the candidate whose id is: ", id);
        FunctionResult fr = {-1, "Faied to found the candidate"};
        return fr;
    }
    cv.votes += 1;
    int result_code = setValueByKey(id, &cv);
    if (result_code == -1) {
        va_end(ap);
        FunctionResult fr = {-1, "Faied to set the candidate"};
        return fr;
    }
    FunctionResult fr = {0, "Success to vote"};
    va_end(ap);
    return fr;
}

#ifndef TEST_LIB_H__
#define TEST_LIB_H__

extern void hello(char*);

extern int addSum(int, int);

// extern void concatString(char* buf, bool includes_concat_info, int count, ...);
extern void concatString(int, ...);

// A struct to hold the GetValueByKey result
typedef struct _GetValueByKeyResult {
    bool exception; 
    void *value;
} GetValueByKeyResult;

// The functions act as callback which should be imported from the outside environment

// Returning struct by value in JNA callback is not working, so insteadly only use pointer parameter to receive the returning result 
typedef void (*GetValueByKey) (int key, GetValueByKeyResult *result);
typedef int (*SetValueByKey)(int key, void *value);
// JNA does not support varargs in callback function
// typedef void (*Log)(char *message, ...);
typedef void (*Log)(char *message);

// To import the callback functions
extern void initImportedFunctions(GetValueByKey, SetValueByKey, Log);

// A struct to hold the candidate data
typedef struct _Value {
    int id;
    char* name;
    int age; 
    int votes;
} CandidateValue;

// A struct to hold the exported function result
typedef struct _Result {
    int code;
    char* message;
} FunctionResult;

// The exported to outside interface function which act as a proxy to call the inner static functions
// Return the struct just only to show the ability of JNA
extern FunctionResult callMethod(char* name, FunctionResult *result, ...);

#endif // TEST_LIB_H__
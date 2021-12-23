#ifndef TEST_LIB_H__
#define TEST_LIB_H__

extern void hello(char*);

extern int addSum(int, int);

extern void concatString(int, ...);

// A struct to hold the GetValueByKey result
typedef struct _GetValueByKeyResult {
    bool exception; 
    void *value;
} GetValueByKeyResult;

// The functions act as callback which should be imported from the outside environment
// Returning struct by value in JNA callback is not working, so insteadly use pointer parameter to receive the returning result 
typedef void (*GetValueByKey) (int key, GetValueByKeyResult *result);
typedef int (*SetValueByKey)(int key, void *value);
typedef void (*Log)(char *message, ...);

// To import the callback functions
extern void initImportedFunctions(GetValueByKey, SetValueByKey, Log);

// A struct to hold the exported function result
typedef struct _Result {
    int code;
    char* message;
} FunctionResult;

// A struct to hold the candidate data
typedef struct _Value {
    int id;
    char* name;
    int age; 
    int votes;
} CandidateValue;

// The inner of the function will call the callback functions
extern FunctionResult vote(int, ...);

#endif // TEST_LIB_H__
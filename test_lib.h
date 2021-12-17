#ifndef TEST_LIB_H__
#define TEST_LIB_H__

extern void hello(char*);

extern int addSum(int, int);

extern void concatString(int, ...);

// import callback function
extern void initImportedFunctions(void*);

// the function will call a callback function
extern void showValue(char*);

#endif // TEST_LIB_H__
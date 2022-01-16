package com.examples.jna;

public interface CDL extends DL {
    void hello(String message);
    int addSum(int a, int b);
    void concatString(int count, Object... args);

    // The vote method is static in the c native library
    // and could not be called directly if not through the callMethod function
    FunctionResult.ByValue vote(int id, FunctionResult result);
}

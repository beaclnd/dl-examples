package com.examples.jna;

import com.sun.jna.*;

public interface CDL extends Library {
    void hello(String message);
    int addSum(int a, int b);
    void concatString(int count, Object... args);
    void initImportedFunctions(GetValueByKey gvbk, SetValueByKey svbk, LogSomething ls);

    FunctionResult.ByValue callMethod(String name, Object args, FunctionResult result);
    // The vote method is static in the c native library
    // and could not be called directly not through the callMethod function
    FunctionResult.ByValue vote(int id, FunctionResult result);

    @Structure.FieldOrder({"status", "value"})
    public class GetValueByKeyResult extends Structure {
        // Use int type act as bool for c dylib,
        // because the boolean true value in java will write 0xff to the bool variable in c dylib,
        // which is not working when using the ! operator in the c dylib
        // 1 for true(ok), 0 for false(error)
        public int status;
        public Pointer value;

//        public GetValueByKeyResult() {}
        public GetValueByKeyResult(Pointer ptr) {
            super(ptr);
            read();
        }

//        public static class ByValue extends GetValueByKeyResult implements Structure.ByValue {}
//        public static class ByReference extends GetValueByKeyResult implements Structure.ByReference {}
    }

    @Structure.FieldOrder({"status", "message"})
    public class FunctionResult extends Structure {
           public boolean status;
           // To receive string data
           public Pointer message;

           public static class ByValue extends FunctionResult implements Structure.ByValue {}
    }

    // The callback functions
    public interface GetValueByKey extends Callback {
        // Returning structure by value in callback is not working well
        // public GetValueByKeyResult invoke(int key, Pointer result);
        public void invoke(String key, Pointer result);
    }
    public interface SetValueByKey extends Callback {
        public int invoke(String key, Pointer value);
    }
    public interface LogSomething extends Callback {
        // Varargs in callback is not supported?
        //public void invoke(String message, Object... args);
        public void invoke(String message);
    }
}

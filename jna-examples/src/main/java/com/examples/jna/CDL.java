package com.examples.jna;

import com.sun.jna.Callback;
import com.sun.jna.Library;
import com.sun.jna.Pointer;
import com.sun.jna.Structure;

public interface CDL extends Library {
    void hello(String message);
    int addSum(int a, int b);
    void concatString(int count, Object... args);
    FunctionResult.ByValue vote(int count, Object... args);
    void initImportedFunctions(GetValueByKey gvbk, SetValueByKey svbk, LogSomething ls);

    @Structure.FieldOrder({"exception", "value"})
    public class GetValueByKeyResult extends Structure{
        public boolean exception;
        public Pointer value;

        public GetValueByKeyResult() {}
        public GetValueByKeyResult(Pointer ptr) {
            super(ptr);
            read();
        }

//        public static class ByValue extends GetValueByKeyResult implements Structure.ByValue {}
//        public static class ByReference extends GetValueByKeyResult implements Structure.ByReference {}
    }

    @Structure.FieldOrder({"code", "message"})
    public class FunctionResult extends Structure {
           public int code;
           public String message;

           public static class ByValue extends FunctionResult implements Structure.ByValue {}
    }

    // The callback functions
    public interface GetValueByKey extends Callback {
        public void invoke(int key, Pointer result);
    }
    public interface SetValueByKey extends Callback {
        public int invoke(int key, Pointer value);
    }
    public interface LogSomething extends Callback {
        // Varargs in callback is not supported?
        //public void invoke(String message, Object... args);
        public void invoke(String message);
    }
}

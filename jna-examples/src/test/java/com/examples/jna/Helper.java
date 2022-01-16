package com.examples.jna;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;

import java.util.HashMap;

public class Helper {
    static public HashMap<String, CandidateValue> kvStore = new HashMap<String, CandidateValue>();
    static public class GetValueByKeyImp implements CDL.GetValueByKey {
        @Override
//        public CDL.GetValueByKeyResult.ByValue invoke(int key, Pointer resultPtr) {
        public void invoke(String key, Pointer resultPtr) {
            CandidateValue v = kvStore.get(key);
            CDL.GetValueByKeyResult result = new CDL.GetValueByKeyResult(resultPtr);
//            CDL.GetValueByKeyResult.ByValue retu = new CDL.GetValueByKeyResult.ByValue();
            if (v == null) {
                // Return NULL pointer to The native library not working
                // return null;

                // write to the native memory
                result.status = 0;
                result.write();
                return;
//                retu.exception = true;
//                retu.value = Pointer.NULL;
//                return retu;
            }
            result.status = 1;
            CandidateValue cv = new CandidateValue(result.value);
            cv.setAllFields(v);
            // write to the native memory
            cv.write();
            result.write();
//             retu.exception = true;
//             retu.value = result.value;
//             return retu;
        }
    }
    static public class SetValueByKeyImp implements CDL.SetValueByKey {
        @Override
        public int invoke(String key, Pointer value) {
            try {
                CandidateValue v = new CandidateValue(value);
                // Clone a new Structure instance to use the getPointer method
                kvStore.put(key, new CandidateValue(v));
            }
            catch(Error e) {
                return 0;
            }
            finally {
                return 1;
            }
        }
    }
    static public class LogSomethingImp implements CDL.LogSomething {
        @Override
        public void invoke(String message){
//        public void invoke(String message, int... args){
            System.out.println(message);
        }
    }

    @Structure.FieldOrder({"id", "name", "age", "votes"})
    static public class CandidateValue extends Structure {
        public String id;
        public String name;
        public int age;
        public int votes;

        public CandidateValue(CandidateValue cv) {
            id = cv.id;
            name = cv.name;
            age = cv.age;
            votes = cv.votes;
        }
        public CandidateValue(Pointer ptr) {
            super(ptr);
            read();
        }

        public CandidateValue(String id, String name, int age, int votes) {
            this.id = id;
            this.name = name;
            this.age = age;
            this.votes = votes;
        }

        // To copy all fields from another instance for writing data back to the native library
        public void setAllFields(CandidateValue cv) {
            id = cv.id;
            name = cv.name;
            age = cv.age;
            votes = cv.votes;
        }

    }

    @Structure.FieldOrder({"id"})
    static public class VoteArgs extends Structure {
        public String id;

        public VoteArgs(String id) {
            this.id = id;
        }
    }

}

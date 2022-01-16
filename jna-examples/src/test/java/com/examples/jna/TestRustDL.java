package com.examples.jna;

import com.sun.jna.*;
import org.junit.jupiter.api.*;
import com.examples.jna.Helper.*;

import java.util.HashMap;

@TestInstance(TestInstance.Lifecycle.PER_CLASS)
public class TestRustDL {
    RustDL rdl;
    interface CSTDDL extends Library {
        Pointer malloc(int size);
        void free(Pointer ptr);
    }
    CSTDDL cstddl;
    HashMap<String, CandidateValue> kvStore = Helper.kvStore;
    CandidateValue cv0, cv1;

    @BeforeAll
    public void setup() {
        String rdlPath = "src/test/resources";
        NativeLibrary.addSearchPath("test_rustlib", rdlPath);
        rdl = Native.load("test_rustlib", RustDL.class);
        cstddl = Native.load("c", CSTDDL.class);

        cv0 = new CandidateValue("a123", "White", 49, 0);
        cv1 = new CandidateValue("b456", "Pink", 29, 0);
        kvStore.put("a123", cv0);
        kvStore.put("b456", cv1);
    }

    @AfterAll
    public void teardown() {
        rdl = null;
        cstddl = null;
    }

    @Test
    @DisplayName("test to call the hello_with_res_in_arg function in the rust dynamic lib")
    public void callHelloWithResInArgInRdl() {
       String msg = "Hi! I am from java";
       int bufferLength = 100;
       Pointer res = cstddl.malloc(bufferLength);
       res.setMemory(0, bufferLength, (byte)0);

       rdl.hello_with_res_in_arg(msg, res);
       Assertions.assertEquals("I am in rust, and have got your message: " + msg, res.getString(0));

       cstddl.free(res);
    }

    @Test
    @DisplayName("test to call the hello_with_res_from_return function in the rust dynamic lib")
    public void callHelloWithResFromReturnInRdl() {
        String msg = "Hi! I am from java";
        Pointer res = rdl.hello_with_res_from_return(msg);
        Assertions.assertEquals("I am in rust, and have got your message: " + msg, res.getString(0));

        // must call the free function to take back the ownership of string res
        // into the rust dylib
        rdl.free_string(res);

        // After the free action in rust dylib
        Assertions.assertNotEquals("I am in rust, and have got your message: " + msg, res.getString(0));
    }


    @Test
    @DisplayName("test to call the vote function through callMethod in the rust dynamic lib")
    public void callVoteInRdl() {
        // Import the callback functions from java to the rust dylib
        GetValueByKeyImp gvbki = new GetValueByKeyImp();
        SetValueByKeyImp svbki = new SetValueByKeyImp();
        LogSomethingImp lsi = new LogSomethingImp();
        rdl.initImportedFunctions(gvbki, svbki, lsi);

        DL.FunctionResult fr = new DL.FunctionResult();
        fr.status = false;
        int msgBufLen = 100;
        Pointer msgPtr = cstddl.malloc(msgBufLen);
        msgPtr.setMemory(0, msgBufLen, (byte)0);
        fr.message = msgPtr;

        VoteArgs args0 = new VoteArgs("not-existed");
        VoteArgs args1 = new VoteArgs("a123");
        VoteArgs args2 = new VoteArgs("b456");
        rdl.callMethod("vote", args0, fr);
        Assertions.assertFalse(fr.status);
        Assertions.assertEquals(fr.message.getString(0), "Failed to found the candidate: id = not-existed");
        rdl.callMethod("vote", args1, fr);
        Assertions.assertTrue(fr.status);
        Assertions.assertEquals(fr.message.getString(0), "Success to vote");
        Assertions.assertEquals(kvStore.get("a123").votes, 1);
        rdl.callMethod("vote", args1, fr);
        Assertions.assertTrue(fr.status);
        Assertions.assertEquals(fr.message.getString(0), "Success to vote");
        Assertions.assertEquals(kvStore.get("a123").votes, 2);
        rdl.callMethod("vote", args2, fr);
        Assertions.assertTrue(fr.status);
        Assertions.assertEquals(fr.message.getString(0), "Success to vote");
        Assertions.assertEquals(kvStore.get("b456").votes, 1);
    }
}

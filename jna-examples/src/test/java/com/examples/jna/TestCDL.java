package com.examples.jna;

import com.sun.jna.*;
import org.junit.jupiter.api.*;

import com.examples.jna.Helper.*;

import java.util.HashMap;

@TestInstance(TestInstance.Lifecycle.PER_CLASS)
public class TestCDL {
    CDL cdl;
    interface CSTDDL extends Library {
        Pointer malloc(int size);
        void free(Pointer ptr);
    }
    CSTDDL cstddl;
    // A map to mock the kv store
    HashMap<String, CandidateValue> kvStore = Helper.kvStore;
    CandidateValue cv0, cv1;


    @BeforeAll
    public void setup() {
//        Native.setProtected(true);
        String cdlPath = "src/test/resources";
        NativeLibrary.addSearchPath("test_lib", cdlPath);
        cdl = Native.load("test_lib", CDL.class);
        cv0 = new CandidateValue("a123", "White", 49, 0);
        cv1 = new CandidateValue("b456", "Pink", 29, 0);
        kvStore.put("a123", cv0);
        kvStore.put("b456", cv1);

        cstddl = Native.load("c", CSTDDL.class);
    }

    @AfterAll
    public void teardown() {
        cdl = null;
        cstddl = null;
    }

    @Test
    @DisplayName("test to call the addSum function in the c dynamic library")
    public void callAddSumInCdl() {
        int a = 123;
        int b = 789;
        int sum = cdl.addSum(a, b);
        Assertions.assertEquals(sum, a + b);
    }

    @Test
    @DisplayName("test to call the hello function in the c dynamic library")
//    @Disabled
    public void callHelloInCdl() {
        String message = "Hello, can you print this message generated from java?";
        cdl.hello(message);
    }

    @Test
    @DisplayName("test to call the concatString function in the c dynamic library")
//    @Disabled
    public void callConcatStringInCdl() {
        String sp1 = "string a;";
        String sp2 = "string b;";
        String sp3 = "string c;";
        int bufferLength = sp1.length() + sp2.length() + sp3.length() + 100;
        Pointer ptr = cstddl.malloc(bufferLength);
        ptr.setMemory(0l, bufferLength, (byte)0);
        cdl.concatString(5, ptr, true, sp1, sp2, sp3);
        Assertions.assertEquals(
                sp1+sp2+sp3+"(concatenate from 3 strings)",
                ptr.getString(0)
        );
        cstddl.free(ptr);
    }


    @Test
    @DisplayName("test to call the vote function directly not through the callMethod function")
//    @Disabled
    public void callVoteDirectlyInCdl() {
        CDL.FunctionResult fr = new CDL.FunctionResult();
        Error err = Assertions.assertThrows(Error.class, () -> cdl.vote(0, fr));
        Assertions.assertTrue(err.getMessage().contains("undefined symbol: vote"));
    }

    @Test
    @DisplayName("test to call the vote function which will call the callbacks in the c dynamic library")
//    @Disabled
    public void callVoteInCdl() {
        // Generate and import the callback function instances
        GetValueByKeyImp gvbki = new GetValueByKeyImp();
        SetValueByKeyImp svbki = new SetValueByKeyImp();
        LogSomethingImp lsi = new LogSomethingImp();
        cdl.initImportedFunctions(gvbki, svbki, lsi);

        // Init the string buffer to receive data
        int msgBufLen = 100;
        Pointer msgPtr = cstddl.malloc(msgBufLen);
        msgPtr.setMemory(0, msgBufLen, (byte)0);
        CDL.FunctionResult fr = new CDL.FunctionResult();
        fr.message = msgPtr;

        VoteArgs args0 = new VoteArgs("not-existed");
        VoteArgs args1 = new VoteArgs("a123");
        VoteArgs args2 = new VoteArgs("b456");
        CDL.FunctionResult frOut = cdl.callMethod("vote", args0, fr);
        Assertions.assertEquals(fr.status, frOut.status);
        Assertions.assertEquals(fr.message, frOut.message);
        Assertions.assertFalse(fr.status);
        Assertions.assertEquals(fr.message.getString(0), "Faied to found the candidate: id = not-existed");
        cdl.callMethod("vote", args1, fr);
        Assertions.assertTrue(fr.status);
        Assertions.assertEquals(fr.message.getString(0), "Success to vote");
        Assertions.assertEquals(kvStore.get("a123").votes, 1);
        Assertions.assertEquals(kvStore.get("b456").votes, 0);
        cdl.callMethod("vote", args1, fr);
        Assertions.assertTrue(fr.status);
        Assertions.assertEquals(fr.message.getString(0), "Success to vote");
        Assertions.assertEquals(kvStore.get("a123").votes, 2);
        Assertions.assertEquals(kvStore.get("b456").votes, 0);
        cdl.callMethod("vote", args2, fr);
        Assertions.assertTrue(fr.status);
        Assertions.assertEquals(fr.message.getString(0), "Success to vote");
        Assertions.assertEquals(kvStore.get("a123").votes, 2);
        Assertions.assertEquals(kvStore.get("b456").votes, 1);

        cstddl.free(msgPtr);
    }
}

package com.examples.jna;

import com.sun.jna.Pointer;

public interface RustDL extends DL {
    void hello_with_res_in_arg(String msg, Pointer res);

    // Should return a Pointer instead of String otherwise we cannot free it in rust dylib
    Pointer hello_with_res_from_return(String msg);
    void free_string(Pointer sPtr);
}

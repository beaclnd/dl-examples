extern crate libc;

use libc::c_char;
use libc::c_void;
use std::ffi::{CStr, CString};
use std::ptr;

#[no_mangle]
pub extern "C" fn hello_with_res_in_arg(msg: *const c_char, res: *mut c_char) {
    let c_str = unsafe {
        assert!(!msg.is_null());
        CStr::from_ptr(msg)
    };
    let str_from_c = c_str.to_str().unwrap();
    let str_extended = String::from("I am in rust, and have got your message: ") + str_from_c;
    println!("{}", str_extended);
    let c_string = CString::new(str_extended).unwrap();

    let bytes = c_string.as_bytes_with_nul();
    for i in 0..bytes.len() {
        unsafe {
            *res.offset(i as isize) = bytes[i] as i8;
        }
    }
}

#[no_mangle]
pub extern "C" fn hello_with_res_from_return(msg: *const c_char) -> *mut c_char {
    let c_str = unsafe {
        assert!(!msg.is_null());
        CStr::from_ptr(msg)
    };
    let str_from_c = c_str.to_str().unwrap();
    let str_extended = String::from("I am in rust, and have got your message: ") + str_from_c;
    println!("{}", str_extended);
    let c_string = CString::new(str_extended).unwrap();

    c_string.into_raw()
}
#[no_mangle]
pub extern "C" fn free_string(s: *mut c_char) {
    unsafe {
        if s.is_null() {
            println!("Warning: free a null pointer");
            return;
        }
        drop(CString::from_raw(s));
    }
}

/* ------------------------------------------------------------------------------------------ */

type GetValueByKey = extern "C" fn(*const c_char, *mut GetValueByKeyResult);
type SetValueByKey = extern "C" fn(*const c_char, *const c_void) -> bool;
type LogSomething = extern "C" fn(*const c_char);
// Defined in the outside envrionment, to be called in rust dylib
struct Callbacks {
    get_value_by_key: GetValueByKey,
    set_value_by_key: SetValueByKey,
    log_something: LogSomething,
}
// Not thread safe, here just to be a simple example
static mut CALLBACKS_SINGLETON: Option<Callbacks> = None;
impl Callbacks {
    fn init(get: GetValueByKey, set: SetValueByKey, log: LogSomething) {
        unsafe {
            if CALLBACKS_SINGLETON.is_none() {
                CALLBACKS_SINGLETON = Some(Callbacks {
                    get_value_by_key: get,
                    set_value_by_key: set,
                    log_something: log,
                });
            } else {
                panic!("Already existed instance of Callbacks");
            }
        }
    }

    fn get() -> &'static Callbacks {
        unsafe {
            match &CALLBACKS_SINGLETON {
                Some(cs) => cs,
                None => panic!("Singleton must be inited before get"),
            }
        }
    }
}
#[no_mangle]
pub extern "C" fn initImportedFunctions(get: GetValueByKey, set: SetValueByKey, log: LogSomething) {
    Callbacks::init(get, set, log);
}

#[repr(C)]
pub struct GetValueByKeyResult {
    status: bool,
    value: *mut c_void,
}

#[repr(C)]
struct CandidateValue {
    id: *const c_char,
    name: *const c_char,
    age: u32,
    votes: u32,
}

#[repr(C)]
pub struct FunctionResult {
    status: bool,
    message: *mut c_char,
}

#[repr(C)]
struct VoteArgs {
    id: *const c_char,
}

fn vote(_args: *const VoteArgs, _result: *mut FunctionResult) {
    let callbacks: &Callbacks = Callbacks::get();
    let mut cv: CandidateValue = CandidateValue {
        id: ptr::null(),
        name: ptr::null(),
        age: 0,
        votes: 0,
    };
    let mut cv_ptr: *mut CandidateValue = &mut cv;
    let mut gvr: GetValueByKeyResult = GetValueByKeyResult {
        status: false,
        value: cv_ptr as *mut c_void,
    };
    let gvr_ptr: *mut GetValueByKeyResult = &mut gvr;
    let get_value_by_key: GetValueByKey = callbacks.get_value_by_key;
    let set_value_by_key: SetValueByKey = callbacks.set_value_by_key;
    let log: LogSomething = callbacks.log_something;
    unsafe {
        get_value_by_key((*_args).id, gvr_ptr);
        if !(*gvr_ptr).status {
            (*_result).status = false;
            let id = CStr::from_ptr((*_args).id).to_str().unwrap();
            let c_string =
                CString::new(format!("Failed to found the candidate: id = {}", id)).unwrap();
            let c_string_bytes = c_string.into_bytes_with_nul();
            for i in 0..c_string_bytes.len() {
                *((*_result).message.offset(i as isize)) = c_string_bytes[i] as i8;
            }
            let c_string = CString::new(format!("Failed to vote for id = {}", id)).unwrap();
            let c_string_ptr = c_string.as_ptr();
            log(c_string_ptr);
            return;
        }
        cv_ptr = (*gvr_ptr).value as *mut CandidateValue;
        (*cv_ptr).votes += 1;
        set_value_by_key((*_args).id, cv_ptr as *mut c_void);
        (*_result).status = true;
        let c_string = CString::new("Success to vote").unwrap();
        let c_string_bytes = c_string.into_bytes_with_nul();
        for i in 0..c_string_bytes.len() {
            *((*_result).message.offset(i as isize)) = c_string_bytes[i] as i8;
        }
    }
}

#[no_mangle]
pub extern "C" fn callMethod(
    name: *const c_char,
    args: *const c_void,
    result: *mut FunctionResult,
) {
    let name_str = unsafe {
        assert!(!name.is_null());
        CStr::from_ptr(name)
    }
    .to_str()
    .unwrap();

    if name_str == "vote" {
        vote(args as *const VoteArgs, result);
    } else {
        println!("Not found such function: {}", name_str);
    }
}

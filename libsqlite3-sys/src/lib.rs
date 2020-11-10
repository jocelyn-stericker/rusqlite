#![allow(non_snake_case, non_camel_case_types)]

pub use self::error::*;

use std::default::Default;
use std::mem;

mod error;

pub fn SQLITE_STATIC() -> sqlite3_destructor_type {
    None
}

pub fn SQLITE_TRANSIENT() -> sqlite3_destructor_type {
    Some(unsafe { mem::transmute(-1isize) })
}

/// Run-Time Limit Categories
#[repr(i32)]
#[non_exhaustive]
pub enum Limit {
    /// The maximum size of any string or BLOB or table row, in bytes.
    SQLITE_LIMIT_LENGTH = SQLITE_LIMIT_LENGTH,
    /// The maximum length of an SQL statement, in bytes.
    SQLITE_LIMIT_SQL_LENGTH = SQLITE_LIMIT_SQL_LENGTH,
    /// The maximum number of columns in a table definition or in the result set
    /// of a SELECT or the maximum number of columns in an index or in an
    /// ORDER BY or GROUP BY clause.
    SQLITE_LIMIT_COLUMN = SQLITE_LIMIT_COLUMN,
    /// The maximum depth of the parse tree on any expression.
    SQLITE_LIMIT_EXPR_DEPTH = SQLITE_LIMIT_EXPR_DEPTH,
    /// The maximum number of terms in a compound SELECT statement.
    SQLITE_LIMIT_COMPOUND_SELECT = SQLITE_LIMIT_COMPOUND_SELECT,
    /// The maximum number of instructions in a virtual machine program used to
    /// implement an SQL statement.
    SQLITE_LIMIT_VDBE_OP = SQLITE_LIMIT_VDBE_OP,
    /// The maximum number of arguments on a function.
    SQLITE_LIMIT_FUNCTION_ARG = SQLITE_LIMIT_FUNCTION_ARG,
    /// The maximum number of attached databases.
    SQLITE_LIMIT_ATTACHED = SQLITE_LIMIT_ATTACHED,
    /// The maximum length of the pattern argument to the LIKE or GLOB
    /// operators.
    SQLITE_LIMIT_LIKE_PATTERN_LENGTH = SQLITE_LIMIT_LIKE_PATTERN_LENGTH,
    /// The maximum index number of any parameter in an SQL statement.
    SQLITE_LIMIT_VARIABLE_NUMBER = SQLITE_LIMIT_VARIABLE_NUMBER,
    /// The maximum depth of recursion for triggers.
    SQLITE_LIMIT_TRIGGER_DEPTH = 10,
    /// The maximum number of auxiliary worker threads that a single prepared
    /// statement may start.
    SQLITE_LIMIT_WORKER_THREADS = 11,
}

#[allow(clippy::all)]
mod bindings {
    include!(concat!(env!("OUT_DIR"), "/bindgen.rs"));
}
pub use bindings::*;

pub type sqlite3_index_constraint = sqlite3_index_info_sqlite3_index_constraint;
pub type sqlite3_index_constraint_usage = sqlite3_index_info_sqlite3_index_constraint_usage;

impl Default for sqlite3_vtab {
    fn default() -> Self {
        unsafe { mem::zeroed() }
    }
}

impl Default for sqlite3_vtab_cursor {
    fn default() -> Self {
        unsafe { mem::zeroed() }
    }
}

#[cfg(feature = "wasm32-no-libc")]
pub mod wasm32_unknown_unknown_alloc {
    // https://github.com/fortanix/rust-sgx/blob/master/rs-libc/src/alloc.rs

    use std::alloc::*;
    use std::ffi::c_void;
    use std::mem;
    use std::ptr;

    #[allow(non_camel_case_types)]
    type size_t = usize;
    const ALIGN: usize = 8;

    #[no_mangle]
    pub unsafe extern "C" fn rusqlite_wasm32_unknown_unknown_malloc(size: size_t) -> *mut c_void {
        let ptr_size = mem::size_of::<*mut usize>();
        let alloc_size = size + ptr_size;
        let alloc_layout = Layout::from_size_align_unchecked(alloc_size, ALIGN);
        let ptr = System.alloc(alloc_layout) as *mut usize;
        if ptr == ptr::null_mut() {
            return ptr::null_mut();
        }
        ptr::write(ptr, alloc_size);
        ptr.offset(1) as *mut c_void
    }

    #[no_mangle]
    pub unsafe extern "C" fn rusqlite_wasm32_unknown_unknown_realloc(
        ptr: *mut c_void,
        size: size_t,
    ) -> *mut c_void {
        if ptr == ptr::null_mut() {
            return rusqlite_wasm32_unknown_unknown_malloc(size);
        } else if size == 0 {
            rusqlite_wasm32_unknown_unknown_free(ptr);
            return ptr::null_mut();
        }

        let ptr = (ptr as *mut usize).offset(-1);
        let ptr_size = mem::size_of::<*mut usize>();
        let old_alloc_layout = Layout::from_size_align_unchecked(ptr::read(ptr), ALIGN);
        let new_alloc_size = size + ptr_size;

        let ptr = System.realloc(ptr as _, old_alloc_layout, new_alloc_size) as *mut usize;
        if ptr == ptr::null_mut() {
            return ptr::null_mut();
        }

        ptr::write(ptr, new_alloc_size);
        ptr.offset(1) as *mut c_void
    }

    #[no_mangle]
    pub unsafe extern "C" fn rusqlite_wasm32_unknown_unknown_free(ptr: *mut c_void) {
        if ptr == ptr::null_mut() {
            return;
        }
        let ptr = (ptr as *mut usize).offset(-1);
        let alloc_layout = Layout::from_size_align_unchecked(ptr::read(ptr), ALIGN);
        System.dealloc(ptr as *mut u8, alloc_layout);
    }

    #[no_mangle]
    pub unsafe extern "C" fn rusqlite_wasm32_unknown_unknown_log(
        x: std::os::raw::c_double,
    ) -> std::os::raw::c_double {
        x.ln()
    }
}

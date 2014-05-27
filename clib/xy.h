#ifndef XY_LIB_H_INCLUDED
#define XY_LIB_H_INCLUDED

/**    xy-lang runtime library
 **  version 1.1.2
 **  last modified:  may 26, 2014
 **/


#include <stdbool.h>
#include <stdio.h>

#define XYLIB extern
#define XYLIBGC XYLIB


struct xy_value;
struct xy_closure;
struct xy_arg_list;
struct xy_gc;

typedef double xy_number_t;
typedef bool xy_bool_t;
typedef char* xy_err_string_t;
typedef struct xy_string xy_string_t;
typedef struct xy_list xy_list_t;
typedef struct xy_value xy_value_t;
typedef struct xy_closure xy_closure_t;
typedef struct xy_arg_list xy_arg_list_t;
typedef struct xy_gc xy_gc_t;

typedef void (*xy_gc_free_func_t)(void*);
typedef bool (*xy_func_ptr_t)(xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);



#define xy_true (true)
#define xy_false (false)
#define xy_err_none (NULL)


/** struct for managing any object that exists on the heap **/
struct xy_gc
{
	void* userdata;
	xy_gc_free_func_t free_func;
	
	int mark_info;
	xy_gc_t* next;
};

/** c string that exists on the heap **/
struct xy_string
{
	// TODO: c strings considered harmful...
	char* str;
	
	xy_gc_t gc;
};

/** argument list, typically for calling functions **/
struct xy_arg_list
{
	int size;
	xy_value_t* values;
};

/** keeps track of scope of a function **/
struct xy_closure
{
	int size;
	xy_value_t* values;
//	xy_arg_list_t a;
	xy_closure_t* parent;
	
	xy_gc_t gc;
};




/** for methods of lazy list storage **/
enum xy_list_type
{
	xy_list_type_basic = 0,
	xy_list_type_sublist = 1,
	xy_list_type_concat = 2,
	xy_list_type_range = 3
};

/** for list values **/
struct xy_list
{
	enum xy_list_type type;
	
	union
	{
		xy_arg_list_t basic;
		
		struct
		{
			int i;
			xy_list_t* a;
		} sublist;
		
		struct
		{
			xy_list_t* a;
			xy_list_t* b;
		} concat;
		
		struct
		{
			int start;
			int end;
		} range;
	};
	
	xy_gc_t gc;
};


enum xy_value_type
{
	xy_value_type_void = 		1,
	xy_value_type_number = 		2,
	xy_value_type_bool = 		4,
	xy_value_type_function =	8,
	xy_value_type_string = 		16,
	xy_value_type_list =		32,
	
	xy_value_type_int = -1,
	xy_value_type_orderable = -2, // supports '<', '>', '<=', '>='
	xy_value_type_iterable = -4   // supports '.', '..', 'hd', 'tl'
};

/** dynamically typed values **/
// TODO: implement lists!!
struct xy_value
{
	enum xy_value_type type;
	
	union
	{
		// for type = <number>
		xy_number_t num;
		
		// for type = <bool>
		xy_bool_t cond;
		
		// for type = <function>
		struct
		{
			xy_closure_t* closure;
			xy_func_ptr_t ptr;
		} func;
		
		// for type = <string>
		struct
		{
			bool constant;
			int subindex;
			union
			{
				const char* c_str;
				xy_string_t* xy_str;
			};
		} str;
		
		// for type = <list>
		xy_list_t* list;
	};
};








XYLIB bool xy_die (xy_err_string_t*, const char*);

// file may be NULL, if so error will not be printed
XYLIB void xy_dump_error (FILE*, xy_err_string_t*);


XYLIB void xy_clear_values (xy_value_t* a, int num_values);


XYLIBGC void xy_value_gc_mark (xy_value_t* value);
XYLIB void xy_value_set_void (xy_value_t* value);
XYLIB void xy_value_set_number (xy_value_t* value, xy_number_t n);
XYLIB void xy_value_set_bool (xy_value_t* value, xy_bool_t);
#define xy_value_set_bool_true(_v) xy_value_set_bool(_v, xy_true)
#define xy_value_set_bool_false(_v) xy_value_set_bool(_v, xy_false)
XYLIB void xy_value_set_function (xy_value_t* value, xy_func_ptr_t, xy_closure_t*);
XYLIB void xy_value_set_string (xy_value_t* value, const char*);
XYLIB void xy_value_set_string_c (xy_value_t* value, const char*);

XYLIB void xy_value_set_list_empty (xy_value_t* value);
XYLIB void xy_value_set_list_basic (xy_value_t* value, xy_value_t* a, int);
XYLIB void xy_value_set_list_concat (xy_value_t* value, xy_list_t*, xy_list_t*);
XYLIB void xy_value_set_list_sublist (xy_value_t* value, xy_list_t*, int);
XYLIB void xy_value_set_list_range (xy_value_t* value, int, int);

// copy value to 'out'
XYLIB void xy_value_set_value (xy_value_t* out, xy_value_t* in);

// use this function instead of 'value.type == ...'
//  supports <int>, <orderable> and <iterable> types
XYLIB bool xy_value_is_type (xy_value_t* value, enum xy_value_type);

// returns NULL if type != <string>
XYLIB const char* xy_value_get_string (xy_value_t*); 


XYLIB bool xy_value_is_function (xy_value_t* value, xy_func_ptr_t);


/*	           void = false
	       number 0 = false
	   bool 'false' = false
	      string "" = false
	everything else = true */
XYLIB xy_bool_t xy_value_condition (xy_value_t*);


// returns a string that may or not exist on the garbage heap
XYLIB const char* xy_value_to_string (xy_value_t*); 


XYLIBGC void xy_string_gc_mark (xy_string_t*);
XYLIB xy_string_t* xy_string (const char*);
XYLIB xy_string_t* xy_string_alloc (int);



XYLIBGC void xy_list_gc_mark (xy_list_t*);
XYLIB xy_list_t* xy_list_basic (xy_value_t*, int);
XYLIB xy_list_t* xy_list_concat (xy_list_t*, xy_list_t*);
XYLIB xy_list_t* xy_list_sublist (xy_list_t*, int);
XYLIB xy_list_t* xy_list_range (int, int);
XYLIB int xy_list_length (xy_list_t*);
XYLIB void xy_list_get (xy_value_t* out, xy_list_t*, int);


XYLIBGC void xy_closure_gc_mark (xy_closure_t*);
XYLIB xy_closure_t* xy_closure_create (int, xy_closure_t*);
XYLIB void xy_closure_get (xy_value_t*, xy_closure_t*, int i, int d);
XYLIB void xy_closure_copy_args (xy_closure_t*, xy_arg_list_t*);
XYLIB void xy_closure_free (xy_closure_t*);


XYLIB xy_string_t* xy_string (const char*);


// WARNING: not all operators are fully implemented yet

XYLIB bool xy_oper_pls (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*); // a + b
XYLIB bool xy_oper_min (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*); // a - b
XYLIB bool xy_oper_mul (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*); // a * b
XYLIB bool xy_oper_div (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*); // a / b
XYLIB bool xy_oper_exp (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*); // a ^ b
XYLIB bool xy_oper_gr (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*);  // a > b
XYLIB bool xy_oper_ls (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*);  // a < b
XYLIB bool xy_oper_dot (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*);  // a . b
XYLIB bool xy_oper_seq (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*); // a .. b
XYLIB bool xy_oper_eql (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*); // a == b
XYLIB bool xy_oper_neq (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*); // a != b
XYLIB bool xy_oper_gre (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*); // a >= b
XYLIB bool xy_oper_lse (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*); // a <= b
// TODO: lazy evaluate these in the compiler
XYLIB bool xy_oper_or (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*);  // a or b
XYLIB bool xy_oper_and (xy_value_t*, xy_value_t* a, xy_value_t* b, xy_err_string_t*); // a and b

XYLIB bool xy_u_oper_not (xy_value_t*, xy_value_t* x, xy_err_string_t*); // !x
XYLIB bool xy_u_oper_neg (xy_value_t*, xy_value_t* x, xy_err_string_t*); // -x
XYLIB bool xy_u_oper_hd (xy_value_t*, xy_value_t* x, xy_err_string_t*);  // hd x
XYLIB bool xy_u_oper_tl (xy_value_t*, xy_value_t* x, xy_err_string_t*);  // tl x


XYLIB bool xy_call (xy_value_t* out, xy_value_t* func, xy_arg_list_t*, xy_err_string_t*); // out = func(args)


XYLIB bool xy_oper_pls_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_min_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_mul_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_div_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_exp_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_gr_lambda  (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_ls_lambda  (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_dot_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_seq_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_eql_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_neq_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_gre_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_lse_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_or_lambda  (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);
XYLIB bool xy_oper_and_lambda (xy_value_t*, xy_arg_list_t*, xy_closure_t*, xy_err_string_t*);





/** static garbage collector **/

XYLIBGC void xy_gc_init ();
XYLIBGC void xy_gc_cleanse (); // frees EVERYTHING

XYLIBGC void xy_gc_add (xy_gc_t*, void* ud, void* free_func);
XYLIBGC void xy_gc_push_closure (xy_closure_t*, xy_value_t* accu, int);
XYLIBGC void xy_gc_pop_closure ();

XYLIBGC void xy_gc_begin (); // begin collection process
XYLIBGC void xy_gc_begin_force (); // forces collection to begin (see source code)
XYLIBGC bool xy_gc_mark (xy_gc_t*);
XYLIBGC void xy_gc_end ();   // finishes collecting

XYLIBGC void xy_gc_collect (); // { begin(), end(); }
XYLIBGC void xy_gc_collect_retain (xy_value_t*); // { begin(), mark(v), end(); }



#endif//XY_LIB_H_INCLUDED
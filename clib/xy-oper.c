#include <math.h>
#include <string.h>
#include "xy.h"
#include "xy-mem.inl"

#define NAME_LENGTH 8

static const char* value_type_name (enum xy_value_type t)
{
	switch (t)
	{
	case xy_value_type_void:		return "void";
	case xy_value_type_number:		return "number";
	case xy_value_type_bool:		return "bool";
	case xy_value_type_function:	return "function";
	case xy_value_type_string:		return "string";
	default:						return "??";
	}
}

#define mem_error() xy_die(e, "memory error");


enum compare
{
	compare_none = 0,
	compare_equal = 1,
	compare_less = 2,
	compare_greater = 0
};



static enum compare
compare_strings (const char* a, const char* b)
{
	int i;
	
	for (i = 0; ; i++)
		if (a[i] == b[i])
		{
			if (a[i] == '\0')
				return compare_equal;
		}
		else if (a[i] == '\0' || a[i] < b[i])
		{
			return compare_less;
		}
		else if (b[i] == '\0' || a[i] > b[i])
		{
			return compare_greater;
		}
}


static enum compare
compare (xy_value_t* a, xy_value_t* b)
{
	if (a->type != b->type)
		return compare_none;
	
	switch (a->type)
	{
	case xy_value_type_void:
		return compare_equal;
	
	case xy_value_type_number:
		if (a->num == b->num)
			return compare_equal;
		else if (a->num < b->num)
			return compare_less;
		else
			return compare_greater;
		
		
	case xy_value_type_bool:
		return (a->cond == b->cond) ? compare_equal : compare_none;
	
	
	case xy_value_type_function:
		return (a->func.ptr == b->func.ptr &&
					a->func.closure == b->func.closure) ? compare_equal : compare_none;
	
	
	case xy_value_type_string:
		return compare_strings(xy_value_get_string(a),
						xy_value_get_string(b));
		
	default:
		return compare_none;
	}
}






bool
xy_call (xy_value_t* o, xy_value_t* f, xy_arg_list_t* r, xy_err_string_t* e)
{
	if (o == NULL || f == NULL || r == NULL)
		return mem_error();
	
	if (f->type == xy_value_type_function)
		return f->func.ptr(o, r, f->func.closure, e);
	else if (r->size == 1)
	{
		if (f->type == xy_value_type_number)
			return xy_oper_mul(o, f, r->values, e);
	}
	
	char msg[43 + NAME_LENGTH];
	sprintf(msg, "cannot apply non-function value of type '%s'",
		value_type_name(f->type));
	return xy_die(e, msg);
}




#define NUMBER_OP(op) \
	if (a->type == xy_value_type_number && \
			b->type == xy_value_type_number) \
		return xy_value_set_number(o, a->num  op   b->num), true;

#define both_comparable() (\
	(a->type == xy_value_type_number && b->type == xy_value_type_number) || \
	(a->type == xy_value_type_string && b->type == xy_value_type_string) )


bool xy_oper_pls (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	NUMBER_OP( + )
	
	if (a->type == xy_value_type_string)
	{
		const char* a_str = xy_value_get_string(a);
		const char* b_str = xy_value_to_string(b);
		int a_size = strlen(a_str);
		int b_size = strlen(b_str);
		
		if (b_size == 0)
			xy_value_set_value(o, a);
		else if (a_size == 0)
		{
			if (b->type == xy_value_type_string)
				xy_value_set_value(o, b);
			else
				xy_value_set_string(o, b_str);
		}
		else
		{
			char* buf = xy_malloc(a_size + b_size + 1);
			strcpy(buf, a_str);
			strcpy(buf + a_size, b_str);
			xy_value_set_string(o, buf);
			xy_free(buf);
		}
		
		return true;
	}
	
	{
		char msg[56 + NAME_LENGTH + NAME_LENGTH];
		sprintf(msg, "cannot apply operator '+' to values of type '%s' and '%s'",
			value_type_name(a->type), value_type_name(b->type));
		return xy_die(e, msg);
	}
}

bool xy_oper_min (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	NUMBER_OP( - )
	
	{
		char msg[56 + NAME_LENGTH + NAME_LENGTH];
		sprintf(msg, "cannot apply operator '-' to values of type '%s' and '%s'",
			value_type_name(a->type), value_type_name(b->type));
		return xy_die(e, msg);
	}
}

bool xy_oper_mul (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	NUMBER_OP( * )
	
	{
		char msg[56 + NAME_LENGTH + NAME_LENGTH];
		sprintf(msg, "cannot apply operator '*' to values of type '%s' and '%s'",
			value_type_name(a->type), value_type_name(b->type));
		return xy_die(e, msg);
	}
}

bool xy_oper_div (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	if (a->type == xy_value_type_number &&
			b->type == xy_value_type_number)
	{
		if (b->num == 0)
			return xy_die(e, "cannot divide by zero");
		
		xy_value_set_number(o, a->num / b->num);
		return true;
	}
	
	{
		char msg[56 + NAME_LENGTH + NAME_LENGTH];
		sprintf(msg, "cannot apply operator '/' to values of type '%s' and '%s'",
			value_type_name(a->type), value_type_name(b->type));
		return xy_die(e, msg);
	}
}

bool xy_oper_exp (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	if (a->type == xy_value_type_number &&
			b->type == xy_value_type_number)
	{
		if (a->num == 0 && b->num < 0)
			return xy_die(e, "cannot divide by zero");
		
		xy_value_set_number(o, pow(a->num, b->num));
		return true;
	}
	
	{
		char msg[56 + NAME_LENGTH + NAME_LENGTH];
		sprintf(msg, "cannot apply operator '^' to values of type '%s' and '%s'",
			value_type_name(a->type), value_type_name(b->type));
		return xy_die(e, msg);
	}
}

bool xy_oper_gr (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	if (both_comparable())
	{
		// a > b   ~=   !(a <= b)
		xy_value_set_bool(o, !(compare(a, b) & (compare_less | compare_equal)));
		return true;
	}
	
	
	{
		char msg[56 + NAME_LENGTH + NAME_LENGTH];
		sprintf(msg, "cannot apply operator '>' to values of type '%s' and '%s'",
			value_type_name(a->type), value_type_name(b->type));
		return xy_die(e, msg);
	}
}

bool xy_oper_ls (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	if (both_comparable())
	{
		xy_value_set_bool(o, compare(a, b) & compare_less);
		return true;
	}
	
	{
		char msg[56 + NAME_LENGTH + NAME_LENGTH];
		sprintf(msg, "cannot apply operator '<' to values of type '%s' and '%s'",
			value_type_name(a->type), value_type_name(b->type));
		return xy_die(e, msg);
	}
}

bool xy_oper_seq (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	{
		char msg[56 + NAME_LENGTH + NAME_LENGTH];
		sprintf(msg, "cannot apply operator '..' to values of type '%s' and '%s'",
			value_type_name(a->type), value_type_name(b->type));
		return xy_die(e, msg);
	}
}

bool xy_oper_eql (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	xy_value_set_bool(o, compare(a, b) & compare_equal);
	return true;
}

bool xy_oper_neq (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	xy_value_set_bool(o, !(compare(a, b) & compare_equal));
	return true;
}

bool xy_oper_gre (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	if (both_comparable())
	{
		//    a >= b  ~=   !(a < b)
		xy_value_set_bool(o, !(compare(a, b) & compare_less));
		return true;
	}
	
	{
		char msg[56 + NAME_LENGTH + NAME_LENGTH];
		sprintf(msg, "cannot apply operator '>=' to values of type '%s' and '%s'",
			value_type_name(a->type), value_type_name(b->type));
		return xy_die(e, msg);
	}
}

bool xy_oper_lse (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	
	if (both_comparable())
	{
		xy_value_set_bool(o, compare(a, b) & (compare_less | compare_equal));
		return true;
	}
	
	
	{
		char msg[56 + NAME_LENGTH + NAME_LENGTH];
		sprintf(msg, "cannot apply operator '<=' to values of type '%s' and '%s'",
			value_type_name(a->type), value_type_name(b->type));
		return xy_die(e, msg);
	}
}

bool xy_oper_or (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	if (xy_value_condition(a))
		xy_value_set_value(o, a);
	else
		xy_value_set_value(o, b);
	
	return true;
}

bool xy_oper_and (xy_value_t* o, xy_value_t* a, xy_value_t* b, xy_err_string_t* e)
{
	if (a == NULL || b == NULL || o == NULL) return mem_error();
	
	if (xy_value_condition(a))
		xy_value_set_value(o, b);
	else
		xy_value_set_value(o, a);
	
	return true;
}






bool
xy_u_oper_not (xy_value_t* o, xy_value_t* a, xy_err_string_t* e)
{
	xy_value_set_bool(o, !xy_value_condition(a));
	return true;
}

bool
xy_u_oper_neg (xy_value_t* o, xy_value_t* a, xy_err_string_t* e)
{
	if (a == NULL || o == NULL) return mem_error();
	
	if (a->type == xy_value_type_number)
		return xy_value_set_number(o, -(a->num)), true;
	
	char msg[52 + NAME_LENGTH];
	sprintf(msg, "cannot apply unary operator '-' to value of type '%s'",
		value_type_name(a->type));
	return xy_die(e, msg);
}

static char constant_chars[256 * 2];

bool
xy_u_oper_hd (xy_value_t* o, xy_value_t* a, xy_err_string_t* e)
{
	if (a == NULL || o == NULL) return mem_error();
	
	if (a->type == xy_value_type_string)
	{
		const char* str = xy_value_get_string(a);
		
		if (str[0] == '\0')
			xy_value_set_string_c(o, "");
		else
		{
			int i = str[0];
			constant_chars[(i * 2)] = str[0];
			constant_chars[(i * 2) + 1] = '\0';
			xy_value_set_string_c(o, constant_chars + (i * 2));
		}
		return true;
	}
	
	char msg[53 + NAME_LENGTH];
	sprintf(msg, "cannot apply unary operator 'hd' to value of type '%s'",
		value_type_name(a->type));
	return xy_die(e, msg);
}

bool
xy_u_oper_tl (xy_value_t* o, xy_value_t* a, xy_err_string_t* e)
{
	if (a == NULL || o == NULL) return mem_error();
	
	if (a->type == xy_value_type_string)
	{
		const char* str = xy_value_get_string(a);
		
		xy_value_set_value(o, a);
		if (str[0] != '\0')
			o->str.subindex++;
		
		return true;
	}
	
	char msg[53 + NAME_LENGTH];
	sprintf(msg, "cannot apply unary operator 'tl' to value of type '%s'",
		value_type_name(a->type));
	return xy_die(e, msg);
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "xy.h"
#include "xy-mem.inl"

void
xy_value_set_void (xy_value_t* v)
{
	if (v == NULL) return;
	v->type = xy_value_type_void;
}


void
xy_value_set_number (xy_value_t* v, xy_number_t n)
{
	if (v == NULL) return;
	v->type = xy_value_type_number;
	v->num = n;
}


void
xy_value_set_bool (xy_value_t* v, xy_bool_t b)
{
	if (v == NULL) return;
	v->type = xy_value_type_bool;
	v->cond = b;
}


void
xy_value_set_function (xy_value_t* v, xy_func_ptr_t f, xy_closure_t* c)
{
	if (v == NULL) return;
	v->type = xy_value_type_function;
	v->func.ptr = f;
	v->func.closure = c;
}


void
xy_value_set_value (xy_value_t* v, xy_value_t* in)
{
	if (v == NULL) return;
	
	if (in == NULL)
		xy_value_set_void(v);
	else
		*v = *in;
}


xy_bool_t
xy_value_condition (xy_value_t* v)
{
	if (v == NULL) return false;
	
	switch (v->type)
	{
	case xy_value_type_number:
		return v->num != 0;
		
	case xy_value_type_bool:
		return v->cond;
		
	case xy_value_type_function:
		return xy_true;
		
	case xy_value_type_string:
		return xy_value_get_string(v)[0] != '\0';
		
	case xy_value_type_list:
		return v->list != NULL;
		
	default:
		return xy_false;
	}
}




bool
xy_value_is_function (xy_value_t* value, xy_func_ptr_t f)
{
	return value->type == xy_value_type_function &&
		value->func.ptr == f;
}


void
xy_value_gc_mark (xy_value_t* v)
{
	if (v == NULL) return;
	
	if (v->type == xy_value_type_function)
		xy_closure_gc_mark(v->func.closure);
	else if (v->type == xy_value_type_string)
	{
		if (!v->str.constant)
			xy_string_gc_mark(v->str.xy_str);
	}
	else if (v->type == xy_value_type_list)
		xy_list_gc_mark(v->list);
}




#define MAX_DIGITS_INT 128
#define MAX_DECIMALS 6

#define INFINITY_STRING "<inf>"

static const char*
number_to_string (double num)
{
	if (num == INFINITY)
		return INFINITY_STRING;
	else if (num == 0)
		return "0";
	
	char buf[MAX_DIGITS_INT + MAX_DECIMALS];
	if (num == (int)num)
		sprintf(buf, "%d", (int)num);
	else
	{
		int dec = 8;
		while (dec > 0)
		{
			sprintf(buf, "%.*f", dec, num);
			
			if (buf[strlen(buf) - 1] == '0')
				dec--;
			else
				break;
		}
		
		if (dec == 0)
			sprintf(buf, "%d", (int)num);
	}
	
	return xy_string(buf)->str;
}

static const char*
list_to_string (xy_list_t* list)
{
	int i, len = xy_list_length(list);
	xy_value_t v;
	if (len == 0)
		return "[ ]";
	
	// "[ X, X, X ]"
	//  ^^       ^^"
	//     ^^
	
	// string length = 4 + (n - 1) * 2 = 2 + 2 * n
	
	const char* items[len];
	int size = 0;
	for (i = 0; i < len; i++)
	{
		xy_list_get(&v, list, i);
		items[i] = xy_value_to_string(&v);
		size += strlen(items[i]);
	}
	
	char* start = xy_string_alloc(2 + 2 * len + size)->str;
	char* buf = start;
	
	buf += sprintf(buf, "[ ");
	
	for (i = 0; i < len; i++)
		buf += sprintf(buf, "%s%s",
					(i > 0) ? ", " : "",
					items[i]);
	
	sprintf(buf, " ]");
	return start;
}


const char*
xy_value_to_string (xy_value_t* v)
{
	if (v == NULL || v->type == xy_value_type_void) return "void";
	
	switch (v->type)
	{
	case xy_value_type_bool:
		return (v->cond) ? "true" : "false";
		
	case xy_value_type_number:
		return number_to_string(v->num);
		
	case xy_value_type_function:
		if (v->func.closure == NULL)
			return "<function>";
		else
			return "<lambda function>";
		
	case xy_value_type_string:
		return xy_value_get_string(v);
		
	case xy_value_type_list:
		return list_to_string(v->list);
	
	default:
		return "<unknown>";
	}
}


bool
xy_value_is_type (xy_value_t* v, enum xy_value_type t)
{
	if (v == NULL)
		return v->type == xy_value_type_void;
	
	switch (t)
	{
	case xy_value_type_int:
		return v->type == xy_value_type_number &&
			v->num == (int)(v->num);
	
	case xy_value_type_orderable:
		return v->type == xy_value_type_number ||
			v->type == xy_value_type_string;
	
	case xy_value_type_iterable:
		return v->type == xy_value_type_string ||
			v->type == xy_value_type_list;
			
	default:
		return v->type == t;
	}
}
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
	else/*
		switch (v->type = in->type)
		{
		case xy_value_type_number:
			xy_value_set_number(v, in->num);
			break;
			
		case xy_value_type_bool:
			xy_value_set_bool(v, in->cond);
			break;
			
		case xy_value_type_function:
			xy_value_set_function(v,
				in->func.ptr, in->func.closure);
			break;
			
		case xy_value_type_string:
		{
			if ((v->str.constant = in->str.constant))
				v->str.c_str = in->str.c_str;
			else
				v->str.xy_str = in->str.xy_str;
			
			v->str.subindex = in->str.subindex;
			break;
		}
			
		case xy_value_type_void:
		default:
			xy_value_set_void(v);
		}*/
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
		
	default:
		return xy_false;
	}
}




void
xy_value_set_string (xy_value_t* v, const char* s)
{
	if (v == NULL) return;
	
	if (s[0] == '\0')
	{
		xy_value_set_string_c(v, "");
		return;
	}
	
	v->type = xy_value_type_string;
	v->str.constant = false;
	v->str.subindex = 0;
	v->str.xy_str = xy_string(s);
}


void
xy_value_set_string_c (xy_value_t* v, const char* s)
{
	if (v == NULL) return;
	
	v->type = xy_value_type_string;
	v->str.constant = true;
	v->str.subindex = 0;
	v->str.c_str = s;
}

const char*
xy_value_get_string (xy_value_t* v)
{
	if (v == NULL || v->type != xy_value_type_string)
		return NULL;
		
	const char* s;
	if (v->str.constant)
		s = v->str.c_str;
	else
		s = v->str.xy_str->str;
	
	return s + v->str.subindex;
}


bool
xy_value_is_function (xy_value_t* value, xy_func_ptr_t f)
{
	return value->type == xy_value_type_function &&
		value->func.ptr == f;
}



xy_string_t*
xy_string (const char* s)
{
	if (s == NULL)
		return xy_string("");
	
	xy_string_t* xys =
		xy_malloc(sizeof(xy_string_t) +
			strlen(s) + 1);
	
	xys->str = (char*)(xys + 1);
	strcpy(xys->str, s);
	xy_gc_add(&xys->gc, xys, xy_free);
	
	return xys;
}


void
xy_string_gc_mark (xy_string_t* str)
{
	if (str != NULL)
		xy_gc_mark(&str->gc);
}


void
xy_value_gc_mark (xy_value_t* v)
{
	if (v == NULL) return;
	
	if (v->type == xy_value_type_function)
		xy_closure_gc_mark(v->func.closure);
	
	if (v->type == xy_value_type_string &&
			!v->str.constant)
		xy_string_gc_mark(v->str.xy_str);
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
		return v->type == xy_value_type_string;
			// || v->type == xy_value_type_list;
			
	default:
		return v->type == t;
	}
}
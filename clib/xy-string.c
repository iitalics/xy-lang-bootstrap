#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "xy.h"
#include "xy-mem.inl"


xy_string_t*
xy_string_alloc (int size)
{
	xy_string_t* xys = xy_malloc(sizeof(xy_string_t) + size + 1);
	xys->str = (char*)(xys + 1);
	xy_gc_add(&xys->gc, xys, xy_free);
	return xys;
}

xy_string_t*
xy_string (const char* s)
{
	if (s == NULL)
		return xy_string("");
	
	xy_string_t* xys = xy_string_alloc(strlen(s));
	strcpy(xys->str, s);
	
	return xys;
}


void
xy_string_gc_mark (xy_string_t* str)
{
	if (str != NULL)
	{
#ifdef XY_GC_MONITOR
		if (
#endif
		xy_gc_mark(&str->gc)
#ifdef XY_GC_MONITOR
			)
			printf("marking string: \"%s\" %p\n",
				str->str, str)
#endif
		; // this is a mess
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

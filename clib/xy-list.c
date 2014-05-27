#include <stdlib.h>
#include "xy.h"
#include "xy-mem.inl"


void
xy_value_set_list_empty (xy_value_t* value)
{
	if (value == NULL) return;
	
	value->type = xy_value_type_list;
	value->list = NULL;
}


void
xy_value_set_list_basic (xy_value_t* value, xy_value_t* a, int n)
{
	if (value == NULL) return;
	
	value->type = xy_value_type_list;
	value->list = xy_list_basic(a, n);
}


void
xy_value_set_list_concat (xy_value_t* value, xy_list_t* a, xy_list_t* b)
{
	if (value == NULL) return;
	
	value->type = xy_value_type_list;
	
	if (a == NULL)
		value->list = b;
	else if (b == NULL)
		value->list = a;
	else
		value->list = xy_list_concat(a, b);
}


void
xy_value_set_list_sublist (xy_value_t* value, xy_list_t* a, int n)
{
	if (value == NULL) return;
	
	value->type = xy_value_type_list;
	value->list =
		(n >= xy_list_length(a))
			? NULL
			: xy_list_sublist(a, n);
}


static inline xy_list_t*
xy_list_create_buf (enum xy_list_type t, int pad)
{
	xy_list_t* list = xy_malloc(sizeof(xy_list_t) + pad);
	list->type = t;
	xy_gc_add(&list->gc, list, xy_free);
	return list;
}

#define xy_list_create(t) \
	xy_list_create_buf(t, 0)


xy_list_t*
xy_list_basic (xy_value_t* a, int n)
{
	if (n <= 0 || a == NULL)
		return NULL;
	
	xy_list_t* list = xy_list_create_buf(xy_list_type_basic,
						sizeof(xy_value_t) * n);
	list->basic.size = n;
	list->basic.values = (xy_value_t*)(list + 1);
	
	while (n-- > 0)
		xy_value_set_value(list->basic.values + n, a + n);
	
	return list;
}


xy_list_t*
xy_list_concat (xy_list_t* a, xy_list_t* b)
{
	xy_list_t* list = xy_list_create(xy_list_type_concat);
	list->concat.a = a;
	list->concat.b = b;
	return list;
}


xy_list_t*
xy_list_sublist (xy_list_t* a, int i)
{
	xy_list_t* list = xy_list_create(xy_list_type_sublist);
	list->sublist.a = a;
	list->sublist.i = i;
	return list;
}


int
xy_list_length (xy_list_t* list)
{
	if (list == NULL)
		return 0;
	
	switch (list->type)
	{
	case xy_list_type_basic:
		return list->basic.size;
	
	case xy_list_type_sublist:
		return xy_list_length(list->sublist.a) - list->sublist.i;
	
	case xy_list_type_concat:
		return xy_list_length(list->concat.a) +
			xy_list_length(list->concat.b);
		
	default:	
		return 0;
	}
}


void
xy_list_get (xy_value_t* out, xy_list_t* list, int i)
{
	if (list == NULL || i < 0 || i >= xy_list_length(list))
		xy_value_set_void(out);
	else
		switch (list->type)
		{
		case xy_list_type_basic:
			xy_value_set_value(out, list->basic.values + i);
			break;
			
		case xy_list_type_sublist:
			xy_list_get(out, list->sublist.a, i + list->sublist.i);
			break;
			
		case xy_list_type_concat:
			{
				int len = xy_list_length(list->concat.a);
				if (i >= len)
					xy_list_get(out, list->concat.b, i - len);
				else
					xy_list_get(out, list->concat.a, i);
				break;
			}
		default: break;
		}
}


void
xy_list_gc_mark (xy_list_t* list)
{
	if (list == NULL) return;
	
	if (xy_gc_mark(&list->gc))
		switch (list->type)
		{
		case xy_list_type_basic:
			{
				int i;
				for (i = 0; i < list->basic.size; i++)
					xy_value_gc_mark(list->basic.values + i);
				break;
			}
			
		case xy_list_type_sublist:
			xy_list_gc_mark(list->sublist.a);
			break;
			
		case xy_list_type_concat:
			xy_list_gc_mark(list->concat.a);
			xy_list_gc_mark(list->concat.b);
			break;
		
		default: break;
		}
}

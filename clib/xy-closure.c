#include <stdlib.h>
#include <string.h>
#include "xy.h"
#include "xy-mem.inl"

xy_closure_t*
xy_closure_create (int size, xy_closure_t* parent)
{
	if (size < 0) return NULL;
	
	xy_closure_t* c = 
		xy_malloc(sizeof(xy_closure_t) + 
			size * sizeof(xy_value_t));
	
	if (c == NULL) return NULL;
	
	c->size = size;
	c->values = (xy_value_t*)(c + 1);
	c->parent = parent;
	c->a.size = 0;
	
	xy_gc_add(&c->gc, c, /*xy_closure_free*/xy_free);
	return c;
}

void
xy_closure_set_a (xy_closure_t* c, xy_value_t* a, int n)
{
	if (c == NULL) return;
	
	if ((c->a.size = n) > 0)
		c->a.values = a;
}

void
xy_closure_get (xy_value_t* v, xy_closure_t* c, int i, int d)
{
	if (v == NULL) return;
	
	if (c == NULL)
		xy_value_set_void(v);
	else if (d > 0)
		xy_closure_get(v, c->parent, i, d - 1);
	else if (i < 0 || i >= c->size)
		xy_value_set_void(v);
	else
		xy_value_set_value(v, c->values + i);
}


void
xy_closure_copy_args (xy_closure_t* c, xy_arg_list_t* r)
{
	if (c == NULL || r == NULL || c->size <= 0) return;
	
	int i;
	
	for (i = 0; i < r->size; i++)
		xy_value_set_value(c->values + i, r->values + i);
	
	for (; i < c->size; i++)
		xy_value_set_void(c->values + i);
		
	/*
	
	why doesn't this work?!
	
	memset(c->values, xy_value_type_void, n * sizeof(xy_value_t));
	memcpy(c->values, r->values, r->size * sizeof(xy_value_t));*/
}


void
xy_closure_free (xy_closure_t* c)
{
	xy_free(c);
}


void
xy_closure_gc_mark (xy_closure_t* c)
{
	if (c != NULL && xy_gc_mark(&c->gc))
	{
		int i;
		for (i = 0; i < c->size; i++)
			xy_value_gc_mark(c->values + i);
		
		for (i = 0; i < c->a.size; i++)
			xy_value_gc_mark(c->a.values + i);
		
		xy_closure_gc_mark(c->parent);
	}
}
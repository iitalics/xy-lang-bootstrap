#include <string.h>
#include <stdlib.h>
#include "xy.h"
#include "xy-mem.inl"


static xy_gc_t* garbage_heap = NULL;
static int prev_mark, next_mark;
static bool collecting = false;
static int collect_loop = 0;


// #define XY_GC_MONITOR
#define COLLECT_FREQ 10 // this could be much bigger actually

struct closure_node
{
	xy_closure_t* closure;
	xy_arg_list_t accu;
	
	struct closure_node* next;
};
static struct closure_node* closure_stack = NULL;






static inline void xy_gc_free (xy_gc_t* g)
{
	if (g != NULL && g->free_func != NULL)
		g->free_func(g->userdata);
}





void
xy_gc_init ()
{
	garbage_heap = NULL;
	
	prev_mark = 0;
	next_mark = 1;
	collecting = false;
	closure_stack = NULL;
	collect_loop = 0;
}


void
xy_gc_cleanse ()
{
	while (closure_stack != NULL)
		xy_gc_pop_closure();
	
	collect_loop = COLLECT_FREQ;
	xy_gc_collect();
	
	xy_gc_init();
}


void
xy_gc_add (xy_gc_t* g, void* ud, void* free_func)
{
	if (g == NULL) return;
	
	g->mark_info = next_mark;
	g->userdata = ud;
	g->free_func = free_func;
	g->next = garbage_heap;
	garbage_heap = g;
}


void
xy_gc_push_closure (xy_closure_t* c, xy_value_t* accu, int n)
{
	if (c == NULL) return; // ??
	
	struct closure_node* node = xy_malloc(sizeof(struct closure_node));
	
	if (node != NULL)
	{
		node->closure = c;
		node->accu.values = accu;
		node->accu.size = n;
		node->next = closure_stack;
		closure_stack = node;
	}
}


void
xy_gc_pop_closure ()
{
	if (closure_stack == NULL) return;
	
	struct closure_node* node = closure_stack;
	closure_stack = node->next;
	xy_free(node);
}


void
xy_gc_begin_force ()
{
	collect_loop = COLLECT_FREQ;
	xy_gc_begin();
}

void
xy_gc_begin ()
{
	if ((++collect_loop) >= COLLECT_FREQ)
	{
#ifdef XY_GC_MONITOR
		printf(" * gc collect begin!\n");
#endif
		
		collect_loop = 0;
		
		collecting = true;
		next_mark = prev_mark + 1; // integer overflows SHOULDNT be a problem
		
		struct closure_node* node;
		int i;
		
		for (node = closure_stack; node != NULL; node = node->next)
		{
			xy_closure_gc_mark(node->closure);
			
			for (i = 0; i < node->accu.size; i++)
				xy_value_gc_mark(node->accu.values + i);
		}
	}
}


bool
xy_gc_mark (xy_gc_t* g) // returns 'false' if collecting wasn't needed
{
	if (g == NULL || !collecting)
		return false;
	
	if (g->mark_info == prev_mark)
	{
		g->mark_info = next_mark;
		return true;
	}
	else
		return false;
}


void
xy_gc_end ()
{
	if (!collecting)
		return;
	
	xy_gc_t* prev = NULL;
	xy_gc_t* next;
	xy_gc_t* g;
	
	int total = 0, freed = 0;
	
	for (g = garbage_heap; g != NULL; g = next)
	{
		next = g->next;
		total++;
		
		if (g->mark_info == prev_mark)
		{
			if (prev == NULL)
				garbage_heap = next;
			else
				prev->next = next;
			
			xy_gc_free(g);
			freed++;
		}
		else
			prev = g;
	}
	
	collecting = false;
	prev_mark = next_mark;

#ifdef XY_GC_MONITOR
	fprintf(stderr, " * gc status: destroyed %d/%d objects in heap.\n",
		freed, total);
#endif
}


void
xy_gc_collect ()
{
	xy_gc_begin();
	xy_gc_end();
}


void
xy_gc_collect_retain (xy_value_t* v)
{
	xy_gc_begin();
	xy_value_gc_mark(v);
	xy_gc_end();
}
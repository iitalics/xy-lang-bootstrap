#include <string.h>
#include "xy.h"
#include "xy-mem.inl"



bool
xy_die (xy_err_string_t* e, const char* str)
{
	if (e != NULL)
		*e = xy_strdup(str);
	
	return false;
}


void
xy_dump_error (FILE* f, xy_err_string_t* e)
{
	if (e == NULL) return;
	
	if (f != NULL && (*e) != xy_err_none)
		fprintf(f, "xy error:\n"
		           "    %s\n",
			*e);
	
	xy_free(*e);
	*e = xy_err_none;
}



void
xy_clear_values (xy_value_t* a, int n)
{
	while (n-- > 0)
		xy_value_set_void(a + n);
}
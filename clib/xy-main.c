#include <string.h>
#include "xy.h"


extern bool
function_main (xy_value_t* o, xy_arg_list_t* r,
                xy_closure_t* p, xy_err_string_t* e);

int main (int argc, char** argv)
{
	xy_gc_init();
	xy_err_string_t err = xy_err_none;
	xy_value_t output;
	xy_arg_list_t args;
	
	args.size = 0;
	if (!function_main(&output, &args, NULL, &err))
		xy_dump_error(stderr, &err);
	else
	{
		const char* str = xy_value_to_string(&output);
		printf("main () =\n"
		       "  %s\n",
			str);
	}
	
	xy_gc_cleanse();
	
	return 0;
}
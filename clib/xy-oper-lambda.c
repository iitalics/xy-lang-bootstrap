#include "xy.h"


#define INIT_CODE \
	xy_value_t a[2]; \
	xy_closure_t c = { .size = 2, .values = a }; \
	xy_closure_copy_args(&c, r);


bool xy_oper_pls_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_pls(o, a, a + 1, e);
}
bool xy_oper_min_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_min(o, a, a + 1, e);
}
bool xy_oper_mul_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_mul(o, a, a + 1, e);
}
bool xy_oper_div_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_div(o, a, a + 1, e);
}
bool xy_oper_exp_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_exp(o, a, a + 1, e);
}
bool xy_oper_gr_lambda  (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_gr(o, a, a + 1, e);
}
bool xy_oper_ls_lambda  (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_ls(o, a, a + 1, e);
}
bool xy_oper_dot_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_dot(o, a, a + 1, e);
}
bool xy_oper_seq_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_seq(o, a, a + 1, e);
}
bool xy_oper_eql_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_eql(o, a, a + 1, e);
}
bool xy_oper_neq_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_neq(o, a, a + 1, e);
}
bool xy_oper_gre_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_gre(o, a, a + 1, e);
}
bool xy_oper_lse_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_lse(o, a, a + 1, e);
}
bool xy_oper_or_lambda  (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_or(o, a, a + 1, e);
}
bool xy_oper_and_lambda (xy_value_t* o, xy_arg_list_t* r, xy_closure_t* p, xy_err_string_t* e)
{
	INIT_CODE
	return xy_oper_and(o, a, a + 1, e);
}
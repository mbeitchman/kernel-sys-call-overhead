#include <linux/kernel.h>
#include <linux/module.h>

void myDelay(void)
{

	return;
}

void(*marc_fn_ptr)(void) = &myDelay;

EXPORT_SYMBOL(marc_fn_ptr);

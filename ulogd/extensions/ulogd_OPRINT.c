/* ulogd_MAC.c, Version $Revision: 1.2 $
 *
 * ulogd output target for logging to a file 
 *
 * (C) 2000 by Harald Welte <laforge@sunbeam.franken.de>
 * This software is released under the terms of GNU GPL
 *
 * $Id: ulogd_OPRINT.c,v 1.2 2000/08/14 08:28:24 laforge Exp $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ulogd.h"
#include "conffile.h"

#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

#define HIPQUAD(addr) \
        ((unsigned char *)&addr)[3], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[0]

static FILE *of = NULL;

int _output_print(ulog_iret_t *res)
{
	ulog_iret_t *ret;
	
	fprintf(of, "===>PACKET BOUNDARY\n");
	for (ret = res; ret; ret = ret->next) {
		fprintf(of,"%s=", ret->key);
		switch (ret->type) {
			case ULOGD_RET_STRING:
				fprintf(of, "%s\n", (char *) ret->value.ptr);
				break;
			case ULOGD_RET_BOOL:
			case ULOGD_RET_INT8:
				fprintf(of, "%d\n", ret->value.i8);
				break;
			case ULOGD_RET_INT16:
				fprintf(of, "%d\n", ret->value.i16);
				break;
			case ULOGD_RET_INT32:
				fprintf(of, "%ld\n", ret->value.i32);
				break;
			case ULOGD_RET_UINT8:
				fprintf(of, "%u\n", ret->value.ui8);
				break;
			case ULOGD_RET_UINT16:
				fprintf(of, "%u\n", ret->value.ui16);
				break;
			case ULOGD_RET_UINT32:
				fprintf(of, "%lu\n", ret->value.ui32);
				break;
			case ULOGD_RET_IPADDR:
				fprintf(of, "%u.%u.%u.%u\n", 
					HIPQUAD(ret->value.ui32));
				break;
			case ULOGD_RET_NONE:
				fprintf(of, "<none>");
				break;
		}
	}
	return 0;
}

static ulog_output_t base_op[] = {
	{ NULL, "print", &_output_print },
	{ NULL, "", NULL },
};


static void _base_reg_op(void)
{
	ulog_output_t *op = base_op;
	ulog_output_t *p;

	for (p = op; p->output; p++)
		register_output(p);
}

static config_entry_t outf_ce = { NULL, "dumpfile", CONFIG_TYPE_STRING, 
				  CONFIG_OPT_NONE, 0,
				  { string: "/var/log/ulogd.pktlog" } };
void _init(void)
{
#ifdef DEBUG
	of = stdout;
#else
	config_register_key(&outf_ce);
	config_parse_file(0);

	of = fopen(outf_ce.u.string, "a");
	if (!of) {
		ulogd_error("ulogd_OPRINT: can't open PKTLOG: %s\n", strerror(errno));
		exit(2);
	}		
#endif
		
	_base_reg_op();
}
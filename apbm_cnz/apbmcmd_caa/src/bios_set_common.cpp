/*
 * bios_set_common.cpp
 *
 *  Created on: Nov 25, 2011
 *      Author: xassore
 */




#ifdef BS_DEBUG
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "bios_set_common.h"


FILE  *_bslog_fp = NULL;


int  cmd_log::bios_set_log (const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	char buffer[8 * 1024];
	//int char_count;
        vsnprintf(buffer, BS_ARRAY_SIZE(buffer), format, argp);

    if(_bslog_fp == NULL) _bslog_fp = fopen(BS_LOG_FILE_NAME, "a");
    if(_bslog_fp != NULL){
		fprintf( _bslog_fp, buffer);
		fflush(_bslog_fp);
    }
	va_end(argp);
	return 0;
}


int  cmd_log::bios_set_openLogfile()
{
	_bslog_fp = fopen(BS_LOG_FILE_NAME, "a");
	fprintf( _bslog_fp, "BIOS_SET starting!!!\n\n");
	fflush(_bslog_fp);
	return 0;
}


void cmd_log::bios_set_closeLogfile(){
	fclose (_bslog_fp);

}

#endif

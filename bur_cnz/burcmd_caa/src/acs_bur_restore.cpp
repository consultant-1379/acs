/*
 * acs_bur_restore.cpp
 *
 *  Created on: 21 Mar 2012
 *      Author: egimarr
 */
#include "acs_bur_burrestore.h"

int main (int argc,char *argv[])
{
	acs_bur_burrestore restore(argc,argv);

	return (restore.execute());
}

/*
 * acs_bur_backup.cpp
 *
 *  Created on: 21 Mar 2012
 *      Author: egimarr
 */
#include "acs_bur_burbackup.h"
#include <boost/filesystem.hpp>
const char* const SWMVERSION="/cluster/storage/system/config/apos/swm_version";
bool is_swm_2_0 = true;
bool isSWM20();

int main (int argc,char *argv[])
{
	is_swm_2_0 = isSWM20();
	acs_bur_burbackup backup(argc,argv);

	return (backup.execute());
}

bool isSWM20()
{
    if((boost::filesystem::exists(boost::filesystem::path(SWMVERSION)))== true)
    {
        return true;
    }
    else {
        return false;
    }
}


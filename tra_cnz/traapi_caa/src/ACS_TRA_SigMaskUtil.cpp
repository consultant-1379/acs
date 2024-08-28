#include "ACS_TRA_SigMaskUtil.h"
#include "ACS_TRA_common.h"
#include "ACS_TRA_lib.h"
#include <vector>
#include <signal.h>


ACS_TRA_SigMaskUtil::ACS_TRA_SigMaskUtil(const std::vector<int> & siglist)
: _status(0)
{
	sigset_t sigset;
	sigemptyset(&sigset);
	for(std::vector<int>::const_iterator it = siglist.begin(); it != siglist.end(); ++it)
		ACE_OS::sigaddset(&sigset, *it);

	if((_err_num = ACE_OS::pthread_sigmask(SIG_BLOCK, &sigset, &_orig_sigmask)) != 0)
	{
		char buf[256];
		snprintf(buf, 256, "*** Warning: unable to set signal mask !");
		ACS_TRA_lib::trace_error(buf, 0);
		_status = -1;
	}
}

ACS_TRA_SigMaskUtil::~ACS_TRA_SigMaskUtil()
{
	if( (_status == 0) && ACE_OS::pthread_sigmask(SIG_SETMASK, &_orig_sigmask, NULL) != 0 )
	{
		char buf[256];
		snprintf(buf, 256, "*** Warning: unable to restore signal mask !");
		ACS_TRA_lib::trace_error(buf, 0);
	}
}


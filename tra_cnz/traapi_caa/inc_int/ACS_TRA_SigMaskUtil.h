#ifndef ACS_TRA_SIGMASKUTIL_HEADER
#define ACS_TRA_SIGMASKUTIL_HEADER

#include <vector>
#include <signal.h>

class ACS_TRA_SigMaskUtil
{
public :
	ACS_TRA_SigMaskUtil(const std::vector<int> & siglist);

	~ACS_TRA_SigMaskUtil();

	bool is_good() { return (_status == 0); }

	int err_num() { return _err_num; }

private:
	sigset_t _orig_sigmask;
	int _status;
	int _err_num;
};

#endif // ACS_TRA_SIGMASKUTIL_HEADER

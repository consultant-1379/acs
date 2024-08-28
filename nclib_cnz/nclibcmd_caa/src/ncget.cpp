/*#include "acs_nclib_session.h"
#include "acs_nclib_message.h"
#include "acs_nclib_hello.h"
#include "acs_nclib_rpc.h"
#include "acs_nclib_factory.h"
#include "acs_nclib_library.h"
#include "stdio.h"
#include "stdlib.h"
#include "nc_common.h"
#include <sstream>
#include <fstream>
*/
#include "acs_nclib_cmd.h"


int main (int argc, char * argv [])
{

	acs_nclib_cmd cmd(argc, argv,acs_nclib::OP_GET);

	int ret_val = cmd.parse();

	if (ret_val != nc_cmd_ns::NC_NO_ERROR)
	{
		cmd.error_msg(ret_val);
		return ret_val;
	}

	ret_val = cmd.execute();
	if (ret_val != nc_cmd_ns::NC_NO_ERROR)
	{
		cmd.error_msg(ret_val);
		return ret_val;
	}

	return 0;
}

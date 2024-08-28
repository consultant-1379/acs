#include "acs_nclib_session.h"
#include "acs_nclib_message.h"
#include "acs_nclib_hello.h"
#include "acs_nclib_rpc.h"
#include "acs_nclib_factory.h"
#include "acs_nclib_library.h"
#include "stdio.h"
#include "stdlib.h"

static const char FILTER_EXAMPLE[] = "<ManagedElement xmlns=\"urn:com:ericsson:ecim:ComTop\">"
"\n<managedElementId>1</managedElementId>"
"\n<SystemFunctions>"
"\n<systemFunctionsId>1</systemFunctionsId>"
"\n<Fm xmlns=\"urn:com:ericsson:ecim:ComFm\">"
"\n<fmId>1</fmId>"
 "\n<lastSequenceNo/>"
"\n</Fm>"
"\n</SystemFunctions>"
"\n</ManagedElement>\n";


static const char FILTER_MAC[] = "<ManagedElement xmlns=\"urn:com:ericsson:ecim:ComTop\">"
"\n<managedElementId>1</managedElementId>"
"\n<DmxcFunction>"
"\n<dmxcFunctionId>1</dmxcFunctionId>"
"\n<Eqm>"
"\n<eqmId>1</eqmId>"
"\n<VirtualEquipment>"
"\n<virtualEquipmentId>APZ</virtualEquipmentId>"
"\n<Blade>"
"\n<bladeId>0-3</bladeId>"
"\n<firstMacAddr/>"
"\n</Blade>"
"\n</VirtualEquipment>"
"\n</Eqm>"
"\n</DmxcFunction>"
"\n</ManagedElement>";


int main (int argc, char * argv []) {


	const char * pwd;
	const char * user;
	const char * addr;
	int iterations = 1;

	int it = 0;

	if (argc == 5)
	{
		addr = argv[1];
		user = argv[2];
		pwd = argv[3];
		iterations = atoi(argv[4]);
		iterations = (iterations < 0)? 0: iterations;
	}
	else if (argc == 4)
	{
		addr = argv[1];
		user = argv[2];
		pwd = argv[3];
	}
	else
	{
		std::cout << "Usage nc_client 127.0.0.1 cpadmin cpadmin [iteration]" << std::endl;
		return 1;
	}

	acs_nclib_library::init();

	do
	{
		acs_nclib_session session;
		session.setPassword(pwd);

		if (session.open(addr,830,acs_nclib::USER_AUTH_KBD_INTERACTIVE,user) == 0)
		{
			int sess_id = -1;
			if (session.getId(sess_id) == 0)
			{
				std::cout << "Session Open, ID: " << sess_id << std::endl;
			}

			std::vector<const char*> caps;
			if (session.getServerCapabilities(caps) == 0)
			{
				std::cout << "Capabilities: " << std::endl;
				for (unsigned int i = 0; i< caps.size(); i++)
				{
					std::cout << caps.at(i) << std::endl;
				}
			}
		}

		acs_nclib_rpc* rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_GET);
		acs_nclib_filter* filter = acs_nclib_factory::create_filter(acs_nclib::FILTER_SUBTREE);

		filter->set_filter_content(FILTER_MAC);

		rpc->add_filter(filter);
		//acs_nclib_rpc_reply* answer = 0;
		int result = 0;

		if ((result = session.send(rpc)) == 0)
		{
			std::cout << "\n\nRPC Get Message sent:" << rpc << std::endl;
			//sleep(1);

			acs_nclib_message* answer = 0;
			result = session.receive(answer);
			if (result == 0)
			{
				std::cout << "\n\nAnswer Received:\n" << answer << std::endl;
			}


			if (answer)
				acs_nclib_factory::dereference(answer);
		}
		else
		{
			std::cout << "Send Failed. RC = " << session.last_error_code() << " " << session.last_error_text() << std::endl;
		}



		if (rpc)
			acs_nclib_factory::dereference(rpc);

		if (filter)
			acs_nclib_factory::dereference(filter);

//		if (msg)
//			acs_nclib_factory::dereference(msg);

		if (session.close() == 0)
			std::cout << "Session Closed" << std::endl;

		it++;
	} while (it != iterations);

	acs_nclib_library::exit();
	return 0;
}

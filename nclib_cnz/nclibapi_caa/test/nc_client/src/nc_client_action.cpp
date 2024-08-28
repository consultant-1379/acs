/*
 * nc_client_action.cpp
 *
 *  Created on: Dec 5, 2012
 *      Author: eiolbel
 */


#include "acs_nclib_session.h"
#include "acs_nclib_message.h"
#include "acs_nclib_hello.h"
#include "acs_nclib_rpc.h"
#include "acs_nclib_factory.h"
#include "acs_nclib_library.h"
#include "stdio.h"
#include "stdlib.h"
#include <fstream>
/*
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


static const char ACTION_EXAMPLE[] = "\n<ManagedElement xmlns=\"http://www.ericsson.com/dmx\">"
				"\n<managedElementId>1</managedElementId>"
				"\n<Equipment>"
					"\n<equipmentId>1</equipmentId>"
					"\n<Shelf>"
						"\n<shelfId>0</shelfId>"
						"\n<Slot>"
							"\n<slotId>20</slotId>"
							"\n<Blade>"
								"\n<bladeId>1</bladeId>"
								"\n<getFirmwareData/>"
							"\n</Blade>"
						"\n</Slot>"
					"\n</Shelf>"
				"\n</Equipment>"
			"\n</ManagedElement>";
*/

int main (int argc, char * argv []) {


	const char * pwd;
	const char * user;
	const char * addr;
	const char * ns = 0;
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

		if (session.open(addr,2022,acs_nclib::USER_AUTH_PASSWORD,user) == 0)
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
					if (strstr(caps.at(i),"action"))
					{
						ns = caps.at(i);
					}

				}
			}
		}

		acs_nclib_rpc* rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_ACTION);

		if (ns)
		{
			//rpc->set_action_namespace("http://tail-f.com/ns/netconf/actions/1.0");
			rpc->set_action_namespace(ns);
		}

		//rpc->set_data(ACTION_EXAMPLE);

		char ch[3000] = {0};
		std::ifstream OpenFile("action.txt");
		while(!OpenFile.eof())
		{

			char line [512] = {0};

			OpenFile.getline(line,512);
			strcat(ch,line);


		}
		rpc->set_data(ch);

		if (session.send(rpc) == 0)
		{
			std::cout << "\n\nRPC Action Message sent:" << rpc << std::endl;

			acs_nclib_message* answer = 0;
			if (session.receive(answer) == 0)
			{
				std::cout << "\n\nAnswer Received:\n" << answer << std::endl;
			}

			if (answer)
				acs_nclib_factory::dereference(answer);
		}



		if (rpc)
			acs_nclib_factory::dereference(rpc);







		if (session.close() == 0)
			std::cout << "Session Closed" << std::endl;

		it++;
	} while (it != iterations);

	acs_nclib_library::exit();
	return 0;
}

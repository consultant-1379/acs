/*
 * acs_nclib_cmd.cpp
 *
 *  Created on: Jan 29, 2014
 *      Author: estevol
 */

#include "acs_nclib_cmd.h"
#include <arpa/inet.h>
#include <unistd.h>

acs_nclib_cmd::acs_nclib_cmd(int argc, char * argv [], acs_nclib::Operation op):
	_argc(argc),
	_argv(argv),
	authentication(acs_nclib::USER_AUTH_NONE),
	port(0),
	timeout(1),
	out_mode(nc_cmd_ns::OUT_SCREEN),
	operation(op)
{


}

acs_nclib_cmd::~acs_nclib_cmd() {

}

int acs_nclib_cmd::parse()
{
	int opt_c = 0; // Connection
	int opt_s = 0; // Authentication method
	int opt_a = 0; // Server IP address
	int opt_p = 0; // Server port
	int opt_u = 0; // user
	int opt_w = 0; // password
	int opt_k = 0; // private key
	int opt_x = 0; // xml string
	int opt_o = 0; // output file
	int opt_t = 0; // timeout
	int opt_n = 0; // action namespace
	int opt_i = 0;
	char *optvalue = NULL;
	int option;

	opterr = 0; // Preventing the print of a system error message

	while ( (option = getopt(_argc, _argv, CMD_OPTIONS)) != -1 ) {
		//analyze cmd line and set flags
		switch (option){
		case 'c':
		{
			if (opt_c)
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_c++;

			optvalue = optarg;

			if((strcmp(optvalue, "SSH") == 0))
			{
				connection = acs_nclib::SSH;
			}
			else if (strcmp(optvalue, "TCP") == 0)
			{
				connection = acs_nclib::TCP;
			}
			else
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INVALID_CONNECTION;
			}
		}
		break;

		case 's':
		{
			if (opt_s)
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_s++;

			optvalue = optarg;

			if((strcmp(optvalue, "PASSWORD") == 0))
			{
				authentication = acs_nclib::USER_AUTH_PASSWORD;
			}
			else if (strcmp(optvalue, "KBD_INT") == 0)
			{
				authentication = acs_nclib::USER_AUTH_KBD_INTERACTIVE;
			}
			else if (strcmp(optvalue, "PUBLIC_KEY") == 0)
			{
				authentication = acs_nclib::USER_AUTH_PUBLIC_KEY_FROM_FILE;
			}
			else
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INVALID_AUTHENTICATION;
			}
		}
		break;

		case 'a':
		{
			if (opt_a)
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_a++;

			optvalue = optarg;

			struct in_addr convaddr;

			if (inet_pton(AF_INET, optvalue, &convaddr) != 1)
				return nc_cmd_ns::NC_INVALID_IP;

			ipaddr = optvalue;

		}
		break;

		case 'p':
		{
			if (opt_p)
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_p++;

			optvalue = optarg;

			port = ::atoi(optvalue);

		}
		break;

		case 'u':
		{
			if (opt_u)
			{
				////std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_u++;

			optvalue = optarg;

			user = optvalue;
		}
		break;

		case 'w':
		{
			if (opt_w)
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_w++;

			optvalue = optarg;

			passwd = optvalue;
		}
		break;

		case 'x':
		{
			if (opt_x)
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_x++;

			optvalue = optarg;

			xml = optvalue;
		}
		break;

		case 'i':
		{
			if (opt_i)
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_i++;

			optvalue = optarg;

			std::ifstream file(optvalue);

			if (!file)
			{
				return nc_cmd_ns::NC_MISSING_INPUT_FILE;
			}

			std::stringstream buffer;
			buffer << file.rdbuf();

			xml = buffer.str();
		}
		break;

		case 'k':
		{
			if (opt_k)
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_k++;

			optvalue = optarg;

			key = optvalue;
		}
		break;

		case 'o':
		{
			if (opt_o)
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_o++;

			if (operation == acs_nclib::OP_EDITCONFIG)
			{
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}

			optvalue = optarg;

			out_mode = nc_cmd_ns::OUT_FILE;
			out = optvalue;
		}
		break;

		case 't':
		{
			if (opt_t)
			{
				//std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_t++;

			optvalue = optarg;

			timeout = atoi(optvalue);

		}
		break;

		case 'n':
		{
			if (opt_n)
			{
				////std::cout << __LINE__ << std::endl;
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}
			opt_n++;

			if (operation != acs_nclib::OP_ACTION)
			{
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			}

			optvalue = optarg;

			nspace = optvalue;
		}
		break;

		}
	}

	if((operation == acs_nclib::OP_ACTION && !opt_n) || (operation != acs_nclib::OP_ACTION && opt_n))
	{
		return nc_cmd_ns::NC_INCORRECT_USAGE;
	}



	if (!opt_c || !opt_a || !opt_p || !(opt_x || opt_i))
	{
		//std::cout << __LINE__ << std::endl;
		return nc_cmd_ns::NC_INCORRECT_USAGE;
	}

	if (opt_i && opt_x)
	{
		return nc_cmd_ns::NC_INCORRECT_USAGE;
	}

	if (connection == acs_nclib::SSH)
	{
		if (!opt_s || !opt_u)
		{
			//std::cout << __LINE__ << std::endl;
			return nc_cmd_ns::NC_INCORRECT_USAGE;
		}
		if (((authentication == acs_nclib::USER_AUTH_KBD_INTERACTIVE || authentication == acs_nclib::USER_AUTH_PASSWORD ) &&  !opt_w) ||
				(authentication == acs_nclib::USER_AUTH_PUBLIC_KEY_FROM_FILE && !opt_k) )
		{
			//std::cout << __LINE__ << std::endl;
			return nc_cmd_ns::NC_INCORRECT_USAGE;
		}
	}
	else if (connection == acs_nclib::TCP && (opt_s || opt_u || opt_w || opt_k))
	{
		//std::cout << __LINE__ << std::endl;
		return nc_cmd_ns::NC_INCORRECT_USAGE;
	}

	return nc_cmd_ns::NC_NO_ERROR;
}

int acs_nclib_cmd::execute()
{
	switch(operation) {
	case acs_nclib::OP_GET:
		return get();

	case acs_nclib::OP_EDITCONFIG:
		return editconfig();

	case acs_nclib::OP_ACTION:
		return action();

	default:
		return nc_cmd_ns::NC_UNKNOWN_OPERATION;
	}


}


int acs_nclib_cmd::get()
{

	acs_nclib_library::init();

	acs_nclib_session session(connection);

	if (authentication == acs_nclib::USER_AUTH_PASSWORD || authentication == acs_nclib::USER_AUTH_KBD_INTERACTIVE)
	{
		session.setPassword(passwd.c_str());
	}
	else if (authentication == acs_nclib::USER_AUTH_PUBLIC_KEY)
	{
		session.setPubicKey(key.c_str());
	}

	if (session.open(ipaddr.c_str(),port,authentication,user.c_str()) == 0)
	{

		acs_nclib_rpc* rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_GET);
		acs_nclib_filter* filter = acs_nclib_factory::create_filter(acs_nclib::FILTER_SUBTREE);

		filter->set_filter_content(xml.c_str());

		rpc->add_filter(filter);
		acs_nclib_rpc_reply* reply = 0;
		int result = 0;

		if ((result = session.send_sync(rpc, reply, timeout * 1000)) == 0)
		{
			//std::cout << "\n\nRPC Get Message sent:" << rpc << std::endl;
			//sleep(1);

			if (reply)
			{
				if (reply->get_reply_type() == acs_nclib::REPLY_DATA)
				{
					std::string data;
					reply->get_data(data);

					if (out_mode == nc_cmd_ns::OUT_SCREEN)
					{
						std::cout << "Command Executed. Data received:" << std::endl;
						std::cout << data << std::endl;
					}
					else
					{
						std::cout << "Command Executed. Data received saved in " << out << std::endl;
						std::ofstream out_file;

						out_file.open(out.c_str());
						out_file << data;

						out_file.close();
					}
				}
				else
				{

					std::vector<acs_nclib_rpc_error*> errors;
					reply->get_error_list(errors);
					std::cout << "Command execution failed: Server replied with an error " << std::endl;

					if (errors.size() > 0)
					{
						std::cout << "Error type " <<  errors[0]->get_error_type() << ". Error message: " << errors[0]->get_error_message() << "\n" << std::endl;

					}

					acs_nclib_factory::dereference(errors);
					return nc_cmd_ns::NC_SEND_RCV_FAILED;
				}

				acs_nclib_factory::dereference(reply);
			}
			else
			{
				std::cout << "Command execution failed: RC = " << session.last_error_code() << " " << session.last_error_text() << std::endl;
				return nc_cmd_ns::NC_SEND_RCV_FAILED;
			}
		}
		else
		{
			std::cout << "Command execution failed: RC = " << session.last_error_code() << " " << session.last_error_text() << std::endl;
			return nc_cmd_ns::NC_SEND_RCV_FAILED;
		}


		if (rpc)
			acs_nclib_factory::dereference(rpc);

		if (filter)
			acs_nclib_factory::dereference(filter);


		if (session.close() != 0)
		{
			return nc_cmd_ns::NC_SESSION_CLOSE_FAILED;
		}


	}
	else
	{
		//std::cout << "Session open failed" << std::endl;
		return nc_cmd_ns::NC_SESSION_OPEN_FAILED;
	}

	acs_nclib_library::exit();
	return nc_cmd_ns::NC_NO_ERROR;
}

int acs_nclib_cmd::editconfig()
{

	acs_nclib_library::init();

	acs_nclib_session session(connection);

	if (authentication == acs_nclib::USER_AUTH_PASSWORD || authentication == acs_nclib::USER_AUTH_KBD_INTERACTIVE)
	{
		session.setPassword(passwd.c_str());
	}
	else if (authentication == acs_nclib::USER_AUTH_PUBLIC_KEY)
	{
		session.setPubicKey(key.c_str());
	}

	if (session.open(ipaddr.c_str(),port,authentication,user.c_str()) == 0)
	{

		acs_nclib_rpc* rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_EDITCONFIG);
		rpc->set_config(xml.c_str());

		acs_nclib_rpc_reply* reply = 0;
		int result = 0;

		if ((result = session.send_sync(rpc, reply, timeout * 1000)) == 0)
		{
			//std::cout << "\n\nRPC Get Message sent:" << rpc << std::endl;
			//sleep(1);

			if (reply)
			{
				if (reply->get_reply_type() != acs_nclib::REPLY_OK)
				{
					std::vector<acs_nclib_rpc_error*> errors;
					reply->get_error_list(errors);
					std::cout << "Command execution failed: Server replied with an error " << std::endl;

					if (errors.size() > 0)
					{
						std::cout << "Error type " <<  errors[0]->get_error_type() << ". Error message: " << errors[0]->get_error_message() << "\n" << std::endl;
					}

					acs_nclib_factory::dereference(errors);
					return nc_cmd_ns::NC_SEND_RCV_FAILED;
				}

				acs_nclib_factory::dereference(reply);
			}
			else
			{
				std::cout << "Command execution failed: RC = " << session.last_error_code() << " " << session.last_error_text() << std::endl;
				return nc_cmd_ns::NC_SEND_RCV_FAILED;
			}
		}
		else
		{
			std::cout << "Command execution failed: RC = " << session.last_error_code() << " " << session.last_error_text() << std::endl;
			return nc_cmd_ns::NC_SEND_RCV_FAILED;
		}


		if (rpc)
			acs_nclib_factory::dereference(rpc);

		if (session.close() != 0)
		{
			return nc_cmd_ns::NC_SESSION_CLOSE_FAILED;
		} else
		{
			std::cout << "Command Executed" << std::endl;
		}


	}
	else
	{
		//std::cout << "Session open failed" << std::endl;
		return nc_cmd_ns::NC_SESSION_OPEN_FAILED;
	}

	acs_nclib_library::exit();
	return nc_cmd_ns::NC_NO_ERROR;
}

int acs_nclib_cmd::action()
{

	acs_nclib_library::init();

	acs_nclib_session session(connection);

	if (authentication == acs_nclib::USER_AUTH_PASSWORD || authentication == acs_nclib::USER_AUTH_KBD_INTERACTIVE)
	{
		session.setPassword(passwd.c_str());
	}
	else if (authentication == acs_nclib::USER_AUTH_PUBLIC_KEY)
	{
		session.setPubicKey(key.c_str());
	}

	if (session.open(ipaddr.c_str(),port,authentication,user.c_str()) == 0)
	{

		acs_nclib_rpc* rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_ACTION);

		rpc->set_data(xml.c_str());
		rpc->set_action_namespace(nspace.c_str());

		acs_nclib_rpc_reply* reply = 0;
		int result = 0;

		if ((result = session.send_sync(rpc, reply, timeout * 1000)) == 0)
		{
			//std::cout << "\n\nRPC Get Message sent:" << rpc << std::endl;
			//sleep(1);

			if (reply)
			{

				if (reply->get_reply_type() == acs_nclib::REPLY_OK)
				{
					std::cout << "Command Executed" << std::endl;
				}
				else if (reply->get_reply_type() == acs_nclib::REPLY_DATA)
				{
					std::string data;
					reply->get_data(data);

					if (out_mode == nc_cmd_ns::OUT_SCREEN)
					{
						std::cout << "Command Executed. Data received:" << std::endl;
						std::cout << data << std::endl;
					}
					else
					{
						std::cout << "Command Executed. Data received saved in " << out << std::endl;
						std::ofstream out_file;

						out_file.open(out.c_str());
						out_file << data;

						out_file.close();
					}
				}
				else
				{
					std::vector<acs_nclib_rpc_error*> errors;
					reply->get_error_list(errors);
					std::cout << "Command execution failed: Server replied with an error " << std::endl;

					if (errors.size() > 0)
					{
						std::cout << "Error type " <<  errors[0]->get_error_type() << ". Error message: " << errors[0]->get_error_message() << "\n" << std::endl;

					}

					acs_nclib_factory::dereference(errors);
					return nc_cmd_ns::NC_SEND_RCV_FAILED;
				}

				acs_nclib_factory::dereference(reply);
			}
			else
			{
				std::cout << "Command execution failed: RC = " << session.last_error_code() << " " << session.last_error_text() << std::endl;
				return nc_cmd_ns::NC_SEND_RCV_FAILED;
			}
		}
		else
		{
			std::cout << "Command execution failed: RC = " << session.last_error_code() << " " << session.last_error_text() << std::endl;
			return nc_cmd_ns::NC_SEND_RCV_FAILED;
		}


		if (rpc)
			acs_nclib_factory::dereference(rpc);


		if (session.close() != 0)
		{
			return nc_cmd_ns::NC_SESSION_CLOSE_FAILED;
		}


	}
	else
	{
		//std::cout << "Session open failed" << std::endl;
		return nc_cmd_ns::NC_SESSION_OPEN_FAILED;
	}

	acs_nclib_library::exit();
	return nc_cmd_ns::NC_NO_ERROR;
}

void acs_nclib_cmd::error_msg(int err_code)
{
	switch (err_code)
	{
	case nc_cmd_ns::NC_NO_ERROR:
	case nc_cmd_ns::NC_SEND_RCV_FAILED:  //Error string print in execute method
		break;
	case nc_cmd_ns::NC_INCORRECT_USAGE:
		print_usage();
		break;
	case nc_cmd_ns::NC_UNKNOWN_OPERATION:
		std::cout << "Command execution failed: Unknown operation" << std::endl;
		break;
	case nc_cmd_ns::NC_SESSION_OPEN_FAILED:
		std::cout << "Command execution failed: Session open failed" << std::endl;
		break;
	case nc_cmd_ns::NC_SESSION_CLOSE_FAILED:
		std::cout << "Command execution failed: Session close failed" << std::endl;
		break;
	case nc_cmd_ns::NC_INVALID_IP:
		std::cout << "Command syntax error: Invalid IP address" << std::endl;
		break;
	case nc_cmd_ns::NC_INVALID_CONNECTION:
		std::cout << "Command syntax error: Invalid connection type, valid values are [ TCP | SSH ]" << std::endl;
		break;
	case nc_cmd_ns::NC_INVALID_AUTHENTICATION:
		std::cout << "Command syntax error: Invalid authentication method, valid values are [ PASSWORD | KBD_INT | PUBLIC_KEY ]" << std::endl;
		break;
	case nc_cmd_ns::NC_MISSING_INPUT_FILE:
		std::cout << "Command execution failed: Input file not found" << std::endl;
		break;
	default:
		std::cout << "Command execution failed: Internal Error" << std::endl;
		break;
	}
}



void acs_nclib_cmd::print_usage()
{
	std::cout << "Incorrect Usage.\n" << std::endl;

	switch (operation)
	{
	case acs_nclib::OP_GET:
		std::cout << "ncget -c TCP -a <address> -p <port> [-x <filter> | -i <filter.xml>] [-o <output.xml>] [-t <timeout>]\n" << std::endl;
		std::cout << "ncget -c SSH -s [ PASSWORD | KBD_INT ] -a <address> -p <port>  -u <user>  -w <password> [-x <filter> | -i <filter.xml>] [-o <output.xml>] [-t <timeout>]\n" << std::endl;
		std::cout << "ncget -c SSH -s PUBLIC_KEY -a <address> -p <port>  -u <user>  -w <password> -k <key_file> [-x <filter> | -i <filter.xml>] [-o <output.xml>] [-t <timeout>]\n" << std::endl;
		break;
	case acs_nclib::OP_EDITCONFIG:
		std::cout << "ncedit-config -c TCP -a <address> -p <port> [-x <config> | -i <config.xml>] [-t <timeout>]\n" << std::endl;
		std::cout << "ncedit-config -c SSH -s [ PASSWORD | KBD_INT ] -a <address> -p <port>  -u <user>  -w <password> [-x <config> | -i <config.xml>] [-t <timeout>]\n" << std::endl;
		std::cout << "ncedit-config -c SSH -s PUBLIC_KEY -a <address> -p <port>  -u <user>  -w <password> -k <key_file> [-x <config> | -i <config.xml>] [-t <timeout>]\n" << std::endl;
		break;
	case acs_nclib::OP_ACTION:
		std::cout << "ncaction -c TCP -a <address> -p <port> -n <namespace> [-x <action> | -i <action.xml>] [-o <output.xml>] [-t <timeout>]\n" << std::endl;
		std::cout << "ncaction -c SSH  -s [ PASSWORD | KBD_INT ] -a <address> -p <port>  -u <user>  -w <password> -n <namespace> [-x <action> | -i <action.xml>] [-o <output.xml>] [-t <timeout>]\n" << std::endl;
		std::cout << "ncaction -c SSH -s PUBLIC_KEY -a <address> -p <port>  -u <user>  -w <password> -k <key_file> -n <namespace> [-x <action> | -i <action.xml>] [-o <output.xml>] [-t <timeout>]\n" << std::endl;
		break;
	default:
		break;

	}

}

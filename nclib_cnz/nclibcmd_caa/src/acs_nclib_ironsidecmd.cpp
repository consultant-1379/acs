#include "acs_nclib_ironsidecmd.h"
#include <arpa/inet.h>
#include <string>
#include <sstream>
#include <unistd.h>
#define CMD_SIZE 12
#define MSG_ID_SIZE 4

acs_nclib_ironsidecmd::acs_nclib_ironsidecmd(int argc, char * argv []):
        _argc(argc),
        _argv(argv),
        port(0),
        out_mode(nc_cmd_ns::OUT_SCREEN)
{


}

acs_nclib_ironsidecmd::~acs_nclib_ironsidecmd() {

}

int acs_nclib_ironsidecmd::parse()
{
	int opt_o = 0; 
        int opt_s = 0; 
        int opt_a = 0; 
        int opt_p = 0; 
        int opt_h = 0; 
	char *optvalue = NULL;
        int option;
	std::string strval;

        opterr = 0; // Preventing the print of a system error message

        while ( (option = getopt(_argc, _argv, IRONSIDE_CMD_OPTIONS)) != -1 ) {
                //analyze cmd line and set flags
                switch (option){

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
		case 's':
                {
                        if (opt_s)
                        {
                                //std::cout << __LINE__ << std::endl;
                                return nc_cmd_ns::NC_INCORRECT_USAGE;
                        }
                        opt_s++;

                        optvalue = optarg;
			
                        cmd = optvalue;
			if(strlen(cmd.c_str()) > CMD_SIZE )
			return nc_cmd_ns::NC_INVALID_QUERY; 
                }
                break;
		
		case 'h':
                {
                        if (opt_h)
                        {
                                //std::cout << __LINE__ << std::endl;
                                return nc_cmd_ns::NC_INCORRECT_USAGE;
                        }
                        opt_h++;

                        optvalue = optarg;
			strval = optvalue;
			
		//	std::cout << "optvalue=" << std::endl;
			
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

                        optvalue = optarg;

                        out_mode = nc_cmd_ns::OUT_FILE;
                        out = optvalue;
                }
		break;
		default:
			return nc_cmd_ns::NC_INCORRECT_USAGE;
			break;
		}

		}
	if (out_mode == nc_cmd_ns::OUT_FILE)
	{
		if(_argc > 9)
		{
			return nc_cmd_ns::NC_INCORRECT_USAGE;
		}
	}
	else 
	{	
		if(_argc > 7)
		{	
			return nc_cmd_ns::NC_INCORRECT_USAGE;
		}	
	}

	if(opt_s && opt_h)
		return nc_cmd_ns::NC_INCORRECT_USAGE;

	if (!opt_a || !opt_p || !(opt_s || opt_h))
        {
               	//std::cout << __LINE__ << std::endl;
	        return nc_cmd_ns::NC_INCORRECT_USAGE;
        }
	if(opt_h)
	{
		//int length = strlen(optvalue);
        //	std::cout<<"length =" << length;
		size_t pos = 0;
		unsigned char cmdvalue[CMD_SIZE]={0};
		int count=0;
		//std::cout<<"I am here....\n";
//		std::string strval(optvalue);
		std::string delimiter = ":";
		std::string token;
		while ((pos = strval.find(delimiter)) != std::string::npos) {
			token = strval.substr(0, pos);
		//	std::cout << token << std::endl;
			strval.erase(0, pos + delimiter.length());
			std::stringstream ss;
			int tmp=0;
			ss << std::hex << token;
			ss >> tmp;
			if((tmp < 0 ) || (tmp > 255 ))
				return nc_cmd_ns::NC_INCORRECT_USAGE;
			cmdvalue[count++]= (unsigned char)tmp;
		//	std::cout << cmdvalue[count - 1] << std::endl;
			if(count > (CMD_SIZE-1))
				return nc_cmd_ns::NC_INCORRECT_USAGE;
		}
		if(strval.length() > 0)
		{
			std::stringstream ss;
                        int tmp=0;
                        ss << std::hex << tmp;
                        ss >> tmp;
                        cmdvalue[count++]= (unsigned char)tmp;
		}
		else
			return nc_cmd_ns::NC_INCORRECT_USAGE;
		if(count != CMD_SIZE)
			return nc_cmd_ns::NC_INCORRECT_USAGE;
		std::string tempcmd(reinterpret_cast<const char *> (cmdvalue),
                     sizeof (cmdvalue) / sizeof (cmdvalue[0]));
		cmd = tempcmd;
	
	}
	return nc_cmd_ns::NC_NO_ERROR; 
		
}
		
int acs_nclib_ironsidecmd::execute()
{
		acs_nclib_library::init();
		acs_nclib_session session(acs_nclib::UDP);

                if (session.open(ipaddr.c_str(),port,acs_nclib::USER_AUTH_NONE,"") != acs_nclib::ERR_NO_ERRORS)
                {
                        std::cout << "Open Failed......." << std::endl;
			return nc_cmd_ns::NC_SESSION_OPEN_FAILED;
                }

                acs_nclib_udp* udp = acs_nclib_factory::create_udp(acs_nclib::OP_UDP_GET);
		acs_nclib_message* answer = 0;
                udp->set_cmd(cmd);

                //acs_nclib_rpc_reply* answer = 0;
                int result = 0;


                if ((result = session.send(udp)) == 0)
                {
                         std::cout << "\n\nUDP Get Message sent:" << udp << std::endl;

                         acs_nclib_udp_reply * reply=0;
                         if (session.receive(answer, 3000) == acs_nclib::ERR_NO_ERRORS)
                         {
			 	reply=dynamic_cast<acs_nclib_udp_reply*>(answer);
                                if((reply!=0) &&  (reply->get_message_id() == udp->get_message_id()))
                                {
                        	       reply->get_data(data);

				       if (out_mode == nc_cmd_ns::OUT_FILE)
                                       {
                                               	std::cout << "Command Executed. Data received saved in " << out << std::endl;
	                                        std::ofstream out_file;

        	                                out_file.open(out.c_str());
                	                        out_file << data;
	
        	                                out_file.close();
                                        }
					else 
					{
						std::cout << "\n\nAnswer Received:\n" << data.c_str() << std::endl;
					}	

                                 }
                                 else
                                 {
                                                std::cout << "DBG: Receive message have different id or reply error" << std::endl;
						if (answer)
                          				  acs_nclib_factory::dereference(answer);
						if (udp)
				                          acs_nclib_factory::dereference(udp);
						acs_nclib_library::exit();
						return nc_cmd_ns::NC_UNKNOWN_OPERATION;
						
                                 }
                          }
                          else
                          {
                                  std::cout << "DBG: Session Failed, error" << std::endl;
				  if (udp)
			                        acs_nclib_factory::dereference(udp);
				  acs_nclib_library::exit();
				  return nc_cmd_ns::NC_SESSION_OPEN_FAILED;  //Returning session open failed error to keep it backward compatible.
                          }

                }
                else
                {
                          std::cout << "Send Failed. RC = " << session.last_error_code() << " " << session.last_error_text() << std::endl;
			  if (udp)
                          	  acs_nclib_factory::dereference(udp);
			  acs_nclib_library::exit();
			  return nc_cmd_ns::NC_SEND_RCV_FAILED;
                }
		if (answer)
                          acs_nclib_factory::dereference(answer);
		if (udp)
                          acs_nclib_factory::dereference(udp);
	
                if (session.close() != 0)
                {
			  acs_nclib_library::exit();
                          return nc_cmd_ns::NC_SESSION_CLOSE_FAILED;
               	} 

		acs_nclib_library::exit();
		return nc_cmd_ns::NC_NO_ERROR;

}
void acs_nclib_ironsidecmd::error_msg(int err_code)
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
	case nc_cmd_ns::NC_INVALID_QUERY:
		std::cout << "Command syntax error: Invalid Query sent" << std::endl;
		break;
	default:
                std::cout << "Command execution failed: Internal Error" << std::endl;
                break;
        }
}
void acs_nclib_ironsidecmd::print_usage()
{
        std::cout << "Incorrect Usage.\n" << std::endl;
	std::cout << "ironsidecmd -a <address> -p <port> -s <query> [-o <output.log>] \n" << std::endl;
	std::cout << "ironsidecmd -a <address> -p <port> -h <hex_query> [-o <output.log>] \n" << std::endl;
}
int main (int argc, char * argv [])
{

        acs_nclib_ironsidecmd comnd(argc, argv);

        int ret_val = comnd.parse();

        if (ret_val != nc_cmd_ns::NC_NO_ERROR)
        {
                comnd.error_msg(ret_val);
                return ret_val;
        }

        ret_val = comnd.execute();
        if (ret_val != nc_cmd_ns::NC_NO_ERROR)
        {
                comnd.error_msg(ret_val);
                return ret_val;
        }

        return 0;
}


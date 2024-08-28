#ifndef ACS_NCLIB_IRONSIDECMD_H_
#define ACS_NCLIB_IRONSIDECMD_H_

#include "acs_nclib_session.h"
#include "acs_nclib_message.h"
#include "acs_nclib_udp.h"
#include "acs_nclib_udp_reply.h"
#include "acs_nclib_factory.h"
#include "acs_nclib_library.h"
#include "stdio.h"
#include "stdlib.h"
#include <fstream>
#include "nc_common.h"

class acs_nclib_ironsidecmd {
public:
        acs_nclib_ironsidecmd(int argc, char * argv []);
        virtual ~acs_nclib_ironsidecmd();

        int parse();
        int execute();

        void error_msg(int err_code);
        void print_usage();

private:
        int _argc;
        char ** _argv;
        std::string ipaddr;
        uint16_t port;
        std::string user;
        std::string cmd;
        std::string out;
	std::string data;
        std::string nspace;
        nc_cmd_ns::OutputMode out_mode;

};
#endif

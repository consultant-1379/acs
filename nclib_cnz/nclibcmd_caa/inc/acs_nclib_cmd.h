/*
 * acs_nclib_cmd.h
 *
 *  Created on: Jan 29, 2014
 *      Author: estevol
 */



#ifndef ACS_NCLIB_CMD_H_
#define ACS_NCLIB_CMD_H_

#include "acs_nclib_session.h"
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



class acs_nclib_cmd {
public:
	acs_nclib_cmd(int argc, char * argv [], acs_nclib::Operation);
	virtual ~acs_nclib_cmd();

	int parse();
	int execute();

	void error_msg(int err_code);
	void print_usage();

private:
	int get();
	int editconfig();
	int action();

	int _argc;
	char ** _argv;
	acs_nclib::SessionLayer connection;
	acs_nclib::UserAuthMethods authentication;
	std::string ipaddr;
	uint16_t port;
	std::string user;
	std::string passwd;
	std::string key;
	std::string xml;
	std::string out;
	int timeout;
	std::string nspace;
	nc_cmd_ns::OutputMode out_mode;
	acs_nclib::Operation operation;

};

#endif /* ACS_NCLIB_CMD_H_ */

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "acs_prc_api.h"
#include "acs_prc_dialogue.h"
#include "ace/OS.h"
#include <sys/wait.h>
#include "ACS_PRC_ispapi.h"
#include <ACS_CS_API.h>

bool o_opt = false;

using namespace std;

void myLog(const char* loginfo, int code);

/* define an alarm signal handler. */
void catch_alarm(int sig_num)
{
	ACE_UNUSED_ARG(sig_num);
    printf("Operation timed out. Exiting...\n\n");
    exit(1);
}

int doReboot(const char* user,bool /*decreaseCount*/,bool reboot,reasonType reason){
	ACS_PRC_API internal;

	string	message;

	cout<<endl;

	if ( !o_opt ){
		if (reboot)
			message = "REBOOT INITIATED!";
		else
			message = "SHUTDOWN INITIATED!";

		cout<<message<<endl<<endl;
	}

	if (internal.rebootNode(reason, user, reboot))
	{
		return 0;
	}

	cout << "Error " << ACE_OS::last_error() << endl;
	return 1;

}

bool askForConfirm(bool reboot){
	string question;
	if (reboot)
		question = "Do you want to REBOOT this node ?" ;
	else
		question = "Do you want to SHUTDOWN this node ?" ;

	bool answer = Dialogue::affirm(question);

	return answer;
}

int main(int argc, char* argv[])
{
	ACS_PRC_API internal;

	if (argc == 1){

		if (askForConfirm(true))
			doReboot("",true,true,manualRebootInitiated);

		return 0;
	}

	bool s_opt = false;
	bool f_opt = false;
	bool u_opt = false;
	int index = 1;
	bool ok = true;
	string argument = "";
	//string options = "ui=-";
	string options = "";
	string backGroundOpt = "";

	bool isVirtualized = false;

	ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;

	if(ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture) == ACS_CS_API_NS::Result_Success){
		if (nodeArchitecture == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
			isVirtualized =true;
	}

	while (index < argc){
		if ((strcmp(argv[index],"-s") == 0) && (!s_opt)){
			s_opt = true;
			options = options + "s";
			backGroundOpt = backGroundOpt + " -s";
		}
		else{
			if ((strcmp(argv[index],"-f") == 0) && (!f_opt)){
				f_opt = true;
				//options = options + "f";
				backGroundOpt = backGroundOpt + " -f";
			}
			else{
				if ((strcmp(argv[index],"-u") == 0) && (!u_opt)){
					u_opt = true;
					//options = options + "u";
					backGroundOpt = backGroundOpt + " -u";
				}
				else{
					if (strncmp(argv[index],"-",1) == 0){
						ok = false;
					}
					else{
						for (int i = index; i < argc; i++){
							argument = argument + argv[i];
							if ((i+1) < argc)
							{
								argument = argument + " ";
							}
							if (argument.length() > 32) break;
						}
					}
					index = argc;
				}
			}
		}
		index = index + 1;
	}

	if ( s_opt && isVirtualized )
		ok = false;

	if (!ok){
		cerr << "prcboot: Syntax error" << endl;

		if ( isVirtualized )
			cerr << "Usage: prcboot [-f] [-u] [user info]" << endl;
		else
			cerr << "Usage: prcboot [-s] [-f] [-u] [user info]" << endl;

		return 2;
	}

	if (!f_opt){
		if (!askForConfirm(!s_opt)){
			return 0;
		}
	}

	std::string cmd;

	//options = options + ";";
	if (argument.length() > 32)
	{
		argument = argument.substr(0,32);
	}

	options = options + argument;

	reasonType reason = manualRebootInitiated;

	if (u_opt)
	{
		reason = causedByError;
	}

	if (s_opt)
		doReboot(options.c_str(),true,false,reason);
	else
		doReboot(options.c_str(),true,true,reason);

	return 0;
} // End of main

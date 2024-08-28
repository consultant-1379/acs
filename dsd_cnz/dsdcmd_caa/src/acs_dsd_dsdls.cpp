/*
 *
 * NAME: acs_dsd_dsdls.cpp
 *
 * COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2010.
 * All rights reserved.
 *
 *  The Copyright to the computer program(s) herein
 *  is the property of Ericsson Telecom AB, Sweden.
 *  The program(s) may be used and/or copied only with
 *  the written permission from Ericsson Telecom AB or in
 *  accordance with the terms and conditions stipulated in the
 *  agreement/contract under which the program(s) have been
 *  supplied.
 *
 * .DESCRIPTION
 *  This is the main program for the dsdls command.
 *
 * DOCUMENT NO
 *	190 89-CAA 109 0122 Ux
 *
 * AUTHOR
 * 	2010-12-13 TEI/XSD  XGIOPAP
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1 	       	20101213	XGIOPAP		Impact due to IP:
 *	  									8/159 41-8/FCP 121 1642
 *	  									"AXE IO, DSD and APJTP in APG50"
 *
 */

// Module Include Files
#include "acs_dsd_command.h"

using namespace std;

// main program
int main(int argc, char* argv[])
{
	/* Description:
	 *   Implementation of the dsdls command.
	 *   This command is used to list and display the registred DSD services
	 *   in a Central Processor (CP) or Adjunct Processor (AP)
	 *   The following command formats are supported:
	 *   On a Single CP System/Multiple CP System:
	 *
	 *   Format 1: Default Services listing
	 *   	dsdls [-a] [-p]
	 *   	the command will display all registred DSD services running on the current node
	 *   	optins:
	 *   	using the option -p the command will display registred DSD services running on the
	 *   	current AP both active and passive node
	 *   	using the oprion -a on a single CP system the command will display all registred
	 *   	DSD services on the current AP (both active and passive node) and CP node
	 *   	using the option -a on a multiple CP system the command will display all registred
	 *   	DSD services on all nodes (when using the option -a, the option -p is ignored)
	 *
	 *   Format 2: Specific CP Services listing
	 *   	dsdls -cp cpname
	 *   	the command will display all registred DSD services running on the indicated CP
	 *
	 *   Format 3: Specific AP Services listing
	 *   	dsdls -ap apidentity [-p]
	 *   	the command will display all registred DSD services running on the indicated AP
	 *   	option:
	 *   	using the option -p the command will display all registred DSD services on the
	 *   	indicated AP both active and passive node
	 *
	 *   Format 4: Specific Service listing
	 *   	dsdls -s servicename -d domain [-p] [-a]
	 *   	the command will display registred DSD services with domain and service name indicated
	 *
	 *   Format 5: Network elemet listing
	 *   	dsdls -n
	 *   	the command will display the list of all nodes in the system
	*/

	DSDlsCommand cmd;
	bool bCpSystem;

	//check CP system (single or multiple)
	bCpSystem = cmd.MultipleCpCheck();

	//flags and flags index
	bool s_opt = false;
	bool a_opt = false;
	bool d_opt = false;
	bool p_opt = false;
	bool cp_opt = false;
	bool ap_opt = false;
	bool n_opt = false;
	ACE_INT32 a_ind = 0;
	ACE_INT32 s_ind = 0;
	ACE_INT32 d_ind = 0;
	ACE_INT32 p_ind = 0;
	ACE_INT32 cp_ind = 0;
	ACE_INT32 ap_ind = 0;
	ACE_INT32 n_ind = 0;
	ACE_INT32 ArgInd = 1;
	ACE_INT32 i = 0;
	string temp;

	//analyze cmd line and set flags
  	while(ArgInd < argc)
  	{
  		if(!ACE_OS::strcmp(argv[ArgInd],"-s"))
  		{
  			if(!s_opt)
  			{
  				s_opt = true;
  				s_ind = ArgInd;
  			}
  			else
  			{
  				cmd.usage(bCpSystem); //option already present
  				return (2);
  			}
  		}
  		else if(!ACE_OS::strcmp(argv[ArgInd],"-a"))
  		{
  			if(!a_opt)
  			{
  				a_opt = true;
  				a_ind = ArgInd;
  			}
  			else
  			{
				cmd.usage(bCpSystem); //option already present
				return (2);
			}
  		}
  		else if(!ACE_OS::strcmp(argv[ArgInd],"-d"))
  		{
  			if(!d_opt)
  			{
  				d_opt = true;
  				d_ind = ArgInd;
  			}
  			else
  			{
				cmd.usage(bCpSystem); //option already present
				return (2);
			}
  		}
  		else if(!ACE_OS::strcmp(argv[ArgInd],"-p"))
  		{
  			if(!p_opt)
  			{
  				p_opt = true;
  				p_ind = ArgInd;
  			}
  			else
  			{
				cmd.usage(bCpSystem); //option already present
				return (2);
			}
  		}
  		else if(!ACE_OS::strcmp(argv[ArgInd],"-cp"))
  		{
  			if(!cp_opt)
  			{
  				cp_opt = true;
  				cp_ind = ArgInd;
  			}
  			else
  			{
				cmd.usage(bCpSystem); //option already present
				return (2);
			}
  		}
  		else if(!ACE_OS::strcmp(argv[ArgInd],"-ap"))
  		{
  			if(!ap_opt)
  			{
  				ap_opt = true;
  				ap_ind = ArgInd;
  			}
  			else
  			{
				cmd.usage(bCpSystem); //option already present
				return (2);
			}
  		}
  		else if(!ACE_OS::strcmp(argv[ArgInd],"-n"))
  		{
  			if(!n_opt)
  			{
  				n_opt = true;
  				n_ind = ArgInd;
  			}
  			else
  			{
				cmd.usage(bCpSystem); //option already present
				return (2);
			}
  		}
  		ArgInd++;
  	}

  	//check the command Format:
	if(argc == 1) //cmd line ok
	{
		cmd.Format1 = true; //dsdls (single and multiple CP system)

		if(bCpSystem != true)
			cmd.CPSingle = true; //single CP System
	}
	else if((argc == 2) && (p_opt == true))
	{
		if(bCpSystem == true)
			cmd.Format2 = true; //dsdls -p (only multiple CP system)
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 2) && (a_opt == true))
	{
		cmd.Format3 = true; //dsdls -a (single and multiple CP system)

		if(bCpSystem != true)
			cmd.CPSingle = true; //single CP
	}
	else if((argc == 3) && (s_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format4 = true; //dsdls -s servicename (only multiple CP system)

			if(s_ind == (argc -1)) //check position argument -s
			{
				cmd.usage(bCpSystem); //last pos in cmd line:ERROR
				return (2);
			}

			if(cmd.CheckData(argv[s_ind + 1])) //check service name
				cmd.ServiceName = argv[s_ind + 1];//servicename ok
			else
			{
				cmd.usage(bCpSystem); //error in servicename
				return (2);
			}
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 4) && (s_opt == true) && (p_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format5 = true; //dsdls -s servicename -p (only multiple CP systen)

			//check errors
			if(s_ind == (argc -1))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			if(!cmd.CheckData(argv[s_ind + 1]))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			cmd.ServiceName = argv[s_ind + 1];
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 4) && (s_opt == true) && (a_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format6 = true; //dsdls -s servicename -a (only multiple CP systen)

			//check errors
			if(s_ind == (argc -1))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			if(!cmd.CheckData(argv[s_ind + 1]))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			cmd.ServiceName = argv[s_ind + 1];
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 3) && (d_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format7 = true; //dsdls -d domain (only multiple CP system)

			//check errors
			if(d_ind == (argc -1))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			if(!cmd.CheckData(argv[d_ind + 1]))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			cmd.Domain = argv[d_ind + 1];
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 4) && (d_opt == true) && (p_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format8 = true; //dsdls -d domain -p (only multiple CP system)

			//check errors
			if(d_ind == (argc -1))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			if(!cmd.CheckData(argv[d_ind + 1]))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			cmd.Domain = argv[d_ind + 1];
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 4) && (d_opt == true) && (a_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format9 = true; //dsdls -d domain -a (only multiple CP system)

			//check errors
			if(d_ind == (argc -1))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			if(!cmd.CheckData(argv[d_ind + 1]))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			cmd.Domain = argv[d_ind + 1];
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 5) && (s_opt == true) && (d_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format10 = true; //dsdls -s servicename -d domain (only multiple CP system)

			//check errors
			if((s_ind == (argc - 1)) || (d_ind == (argc - 1)))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			if((!cmd.CheckData(argv[s_ind + 1])) || (!cmd.CheckData(argv[d_ind + 1])))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			cmd.ServiceName = argv[s_ind + 1];
			cmd.Domain = argv[d_ind + 1];
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 6) && (s_opt == true) && (d_opt == true) && (p_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format11 = true; //dsdls -s servicename -d domain -p (only multiple CP system)

			//check errors
			if((s_ind == (argc - 1)) || (d_ind == (argc - 1)))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			if((!cmd.CheckData(argv[s_ind + 1])) || (!cmd.CheckData(argv[d_ind + 1])))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			cmd.ServiceName = argv[s_ind + 1];
			cmd.Domain = argv[d_ind + 1];
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 6) && (s_opt == true) && (d_opt == true) && (a_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format12 = true; //dsdls -s servicename -d domain -a (only multiple CP system)

			//check errors
			if((s_ind == (argc - 1)) || (d_ind == (argc - 1)))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			if((!cmd.CheckData(argv[s_ind + 1])) || (!cmd.CheckData(argv[d_ind + 1])))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			cmd.ServiceName = argv[s_ind + 1];
			cmd.Domain = argv[d_ind + 1];
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 3) && (ap_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format13 = true; //dsdls -ap apidentity (only multiple CP system)

			//check errors
			if(ap_ind == (argc -1))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			if(!cmd.CheckData(argv[ap_ind + 1]))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			cmd.APidentity = argv[ap_ind + 1];
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 4) && (ap_opt == true) && (p_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format14 = true; //dsdls -ap apidentity -p (only multiple CP system)

			//check errors
			if(ap_ind == (argc -1))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			if(!cmd.CheckData(argv[ap_ind + 1]))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			cmd.APidentity = argv[ap_ind + 1];
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 3) && (cp_opt == true))
	{
		if(bCpSystem == true)
		{
			cmd.Format15 = true; //dsdls -cp cpname (only multiple CP system)

			//check errors
			if(cp_ind == (argc -1))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			if(!cmd.CheckData(argv[cp_ind + 1]))
			{
				cmd.usage(bCpSystem);
				return (2);
			}

			cmd.CPname = argv[cp_ind + 1];
			//cmd.CPname = strupr(cmd.CPname);

			temp = cmd.CPname;

			while(temp[i])
			{
				temp[i] = toupper(temp[i]);
				i++;
			}

			strcpy(cmd.CPname, temp.c_str());
		}
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else if((argc == 2) && (n_opt == true))
	{
		if(bCpSystem == true)
			cmd.Format16 = true; //dsdls -n (only multiple CP system)
		else
		{
			ACE_OS::printf("%s\n", ERR116);
			return(116);
		}
	}
	else //error in command line
	{
		cmd.usage(bCpSystem);
		return (2);
	}

	/*
	 * The command line is ok, the check
	 * option is ok, then execute the command
	 */
	cmd.execute_cmd();

  	return 0;
} // end of main

/** @file hwiconf.cpp
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-05-30
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=====+============+==============+=====================================+
 *	| REV | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=====+============+==============+=====================================+
 *	| 001 | 2011-05-30 | xnicmut      | File created.                       |
 *	+=====+============+==============+=====================================+
 */

namespace {

}

int main (int /*argc*/, char * /*argv*/ []) {
	int cmd_exit_code = 0;

	return cmd_exit_code;
}









#ifdef APG43_WINDOWS_OLD_CODE

//******************************************************************************
//
//  NAME
//      hwiconf.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2010. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//
//  DOCUMENT NO
//	190 89-CAA xxx xxxx
//
//  SEE ALSO 
// 	-
//
//******************************************************************************

#pragma warning( disable : 4267; disable : 4244; disable : 4311)
#pragma warning( default : 4267; default : 4244; default : 4311)


#include <list>
#include <string>
#include <iostream>
#include <Get_opt.h>
#include <ACS_PRC_Process.h>
#include <ACS_ExceptionHandler.h>
#include <CommonDll.h>
#include <ACS_APBM_CmdCommon.h>
#include <ACS_APBM_CMDAPI.h>
#include <ACS_CS_API.h>
#include <ACS_TRA_trace.h>
#include <ACS_APBM_MsgTypes.h>
#include <ACS_APBM_Util.h>



ACS_TRA_trace hwiconf = ACS_TRA_DEF("ACS_APBM_HWI_HWICONF", "C300");

static void PrintUsage();

//command name
static char*  g_lpszName = "hwiconf";

#define __ std::cout<<"DBG: "<<__FUNCTION__<<"@"<<__LINE__<<std::endl;
#define CMD_OPTIONS  "m:s:r:n:x:y:ad"
//******************************************************************************
// PrintUsage
// Print the usage message to output stream
//******************************************************************************
void printUsage(std::string msg)
{
	std::cerr << msg.c_str() << "\n";
	std::cerr << "\n";
	std::cerr << "Usage: hwiconf -a -m magadd -r cabrow -n cabno -x xpos -y ypos" << std::endl;
	std::cerr << "       hwiconf -a -m magadd -r cabrow -n cabno" << std::endl;
	std::cerr << "       hwiconf -a -m magadd [-s slot] -x xpos -y ypos" << std::endl;
    std::cerr << "       hwiconf -d -m magadd [-s slot]" << std::endl;
} // End of PrintUsage


//******************************************************************************
// main process
//******************************************************************************
int main(int argc, char* argv[])
{
	AP_InitProcess(g_lpszName, AP_COMMAND);
	AP_SetCleanupAndCrashRoutine(g_lpszName, NULL);

	HANDLE semserver;
	semserver= CreateSemaphore(NULL,1,1, "acs_hwm_apbm_pipe_synHwiconf");


	std::string node = "";


	int opt;
	int nExitCode = 0;
	int ver = 0;

	int opt_a = 0; // add configuration
	int opt_d = 0; // remove configuration
	int opt_m = 0; // magazine
	int opt_s = 0; // slot
	int opt_r = 0; // rowNumber
	int opt_n = 0; // cabNumber
	int opt_x = 0; // xposition
    int opt_y = 0; // yposition
	
	//parameter value
	std::string magFilter = "";
	std::string slotFilter = "";
	std::string cabNumber = "";
	std::string cabRow = "";
	std::string xPos = "";
	std::string yPos = "";
	

	//Check if APG43
	ver = GetHWVersion(NULL, NULL);
	if (ver < 0)
	{
		std::cerr << "Failed to getHWVer" << std::endl;
		return nExitCode;
	}
	else if (ver != 3)
	{
		std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_WRONGPLATFORM, &nExitCode) << std::endl;
		return nExitCode;
	}
	// Check Node Architecture
	ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;
	if (ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture) != ACS_CS_API_NS::Result_Success)
	{
		std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_CSUNRECHEABLE, &nExitCode) << std::endl;
		return nExitCode;
	}

	if (nodeArchitecture == ACS_CS_API_CommonBasedArchitecture::NOT_CBA)
	{
		std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_CMDNOTSUPPORTED, &nExitCode) << std::endl;
		return nExitCode;
	}

	if(!ACS_APBM_CmdCommon::CheckIfActiveNode())
	{
		std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_NOTACTIVE, &nExitCode) << std::endl;
		return nExitCode;
	}

	//Parse command
	Get_opt getopt (argc, (char**)argv, CMD_OPTIONS);

	if ( argc < 2 ) 
	{
		printUsage("Incorrect usage");// Too few arguments
		return 2;
	}

	while ( ((opt = getopt()) != -1) && (opt != -2) ) 
	{
		switch (opt) 
		{
		case 'a':
			if( opt_a )
			{
				printUsage("Incorrect usage");
				return 2;
			}
			opt_a++;
			break;
		
		case 'd':
			if( opt_d )
			{
				printUsage("Incorrect usage");
				return 2;
			}
			opt_d++;
			break;
		
		case 'm':
			if( opt_m )
			{
				printUsage("Incorrect usage");
				return 2;
			}
			opt_m++;
			magFilter = getopt.optarg;
			break;

		case 's':
			if( opt_s )
			{
				printUsage("Incorrect usage");
				return 2;
			}	 
			opt_s++;
			slotFilter = getopt.optarg;
			break;
		
		case 'r':
			if( opt_r )
			{
				printUsage("Incorrect usage");
				return 2;
			}
			opt_r++;
			cabRow = getopt.optarg;
			break;
		
		case 'n':
			if( opt_n )
			{
				printUsage("Incorrect usage");
				return 2;
			}
			opt_n++;
			cabNumber = getopt.optarg;
			break;
		
		case 'x':
			if( opt_x )
			{
				printUsage("Incorrect usage");
				return 2;
			}
			opt_x++;
			xPos = getopt.optarg;
			break;
		
		case 'y':
			if( opt_y )
			{
				printUsage("Incorrect usage");
				return 2;
			}
			opt_y++;
			yPos = getopt.optarg;
			break;
		
		default: // Illegal option
			// Check if this is an known option
			if ( getopt.optind > 1 )
			{
				char* cp = (char*)argv[getopt.optind-1];
				if ( *cp == '-' )
					cp++;
				if ( ::strchr(CMD_OPTIONS,*cp) == NULL )
				{
					printUsage("Incorrect usage");
					return 2;
				}
			} 
			printUsage("Incorrect usage");
			return 2;
			break;
		} // end switch
	} // end while

	if ( opt_a + opt_d != 1)
	{
		printUsage("Incorrect usage");
		return 2;
	}
 
	if ( opt_a == 1)
	{
		if (opt_s == 1)
		{
			if (opt_r + opt_n > 0)
			{
				printUsage("Incorrect usage");
				return 2;
			}
			if (opt_m + opt_x + opt_y != 3 )
			{
				printUsage("Incorrect usage");
				return 2;
			}
		} 
		else
		{
			if ( opt_m + opt_x + opt_y + opt_r + opt_n != 5 )
			{
				if ( opt_m == 0 )
				{
					printUsage("Incorrect usage");
					return 2;
				}
				else if ( opt_x + opt_y == 2 )
				{
					if ( opt_r + opt_n > 0 )
					{
						printUsage("Incorrect usage");
						return 2;
					}
				}
				else if ( opt_r + opt_n == 2 )
				{
					if ( opt_x + opt_y > 0 )
					{
						printUsage("Incorrect usage");
						return 2;
					}
				}
				else if (( opt_x + opt_n == 2 ) || ( opt_x + opt_r == 2 ) || ( opt_y + opt_n == 2 ) || ( opt_y + opt_r == 2 ))
				{
					printUsage("Incorrect usage");
					return 2;
				}
				else if (( opt_x + opt_y + opt_n + opt_r < 2 ))
				{
					printUsage("Incorrect usage");
					return 2;
				}
			}
		}
	}
	
	if ( opt_d == 1)
	{
		if (opt_m != 1)
		{
			printUsage("Incorrect usage");
			return 2;
		}
		if (opt_x + opt_y + opt_r + opt_n > 0 )
		{
			printUsage("Incorrect usage");
			return 2;
		}
	}
	
	//avoid to manage '-' first caracter
	if (magFilter[0] == '-')
	{
		printUsage("Incorrect usage");
		return 2;
	}
    
	if (slotFilter[0] == '-')
	{
		printUsage("Incorrect usage");
		return 2;
	}
	
	if (cabNumber[0] == '-')
	{
		printUsage("Incorrect usage");
		return 2;
	}
	
	if (cabRow[0] == '-')
	{
		printUsage("Incorrect usage");
		return 2;
	}
	
	if (xPos[0] == '-')
	{
		printUsage("Incorrect usage");
		return 2;
	}
	
	if (yPos[0] == '-')
	{
		printUsage("Incorrect usage");
		return 2;
	}

	if (opt == -2)
	{
		printUsage("Incorrect usage");
		return 2;
	}

	//-----------------------------------------------------------
	// A R G U M E N T - C O N V E R S I O N   A N D   C H E C K
 	//-----------------------------------------------------------

	if ( getopt.optind < argc ) 
	{
		printUsage("Incorrect usage");
		return 2;// Too many arguments
	}

	unsigned long magaddr = 0;
	unsigned short slot = 0;
	
	if ( opt_m )
	{
		if (!APBM_Util::stringToUlongMagazine(magFilter, magaddr))
		{
			std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_INVALIDMAG, &nExitCode);
			return nExitCode;
		}
	}
	
	if ( opt_s )
	{
		if (APBM_Util::stringToUshort(slotFilter, slot))
		{
			if ((slot < 0) || (slot > 25))
			{
				std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_UNREASONABLEVALUE, &nExitCode);
				return nExitCode;
			}
		}
		else
		{
			std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_UNREASONABLEVALUE, &nExitCode);
			return nExitCode;
		}
	}

	if ( opt_r )
	{
	int cabRowInt;
		if (APBM_Util::stringToInt(cabRow, cabRowInt))
		{
			if ((cabRowInt < 1) || (cabRowInt > 999)) 
			{
				std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_UNREASONABLEVALUE, &nExitCode);
				return nExitCode;
			}
		}
		else
		{
			std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_UNREASONABLEVALUE, &nExitCode);
			return nExitCode;
		}
	}

	if ( opt_n )
	{
	int cabNumberInt;
		if (APBM_Util::stringToInt(cabNumber, cabNumberInt))
		{
			if ((cabNumberInt < 1) || (cabNumberInt > 99))
			{
				std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_UNREASONABLEVALUE, &nExitCode);
				return nExitCode;
			}
		}
		else
		{
			std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_UNREASONABLEVALUE, &nExitCode);
			return nExitCode;
		}
	}

	if ( opt_x )
	{
	int xPosInt;
		if (APBM_Util::stringToInt(xPos, xPosInt))
		{
			if ((xPosInt < 0) || (xPosInt > 255))
			{
				std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_UNREASONABLEVALUE, &nExitCode);
				return nExitCode;
			}
		}
		else
		{
			std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_UNREASONABLEVALUE, &nExitCode);
			return nExitCode;
		}
	}
	
	if ( opt_y )
	{
	int yPosInt;
		if (APBM_Util::stringToInt(yPos, yPosInt))
		{
			if ((yPosInt < 0) || (yPosInt > 255)) 
			{
				std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_UNREASONABLEVALUE, &nExitCode);
				return nExitCode;
			}
		}
		else
		{
			std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_UNREASONABLEVALUE, &nExitCode);
			return nExitCode;
		}
	}


	//--------------------------------------------
	//C O M M A N D - H A N D L I N G
	//--------------------------------------------
	int cmdTypeInput;
	ACS_APBM_CMDAPI* api = new ACS_APBM_CMDAPI();
	
	if (opt_a == 1)
	{
		if (opt_s == 1) 
		{
			cmdTypeInput = 27; 
		}
		else if ( opt_m + opt_r + opt_n + opt_x + opt_y == 5 )
		{
			cmdTypeInput = 28;
		}
		else if (opt_m + opt_x + opt_y == 3)
		{
			cmdTypeInput = 32;
		}
		else if (opt_m + opt_r + opt_n == 3)
		{
			cmdTypeInput = 31;
		}
	}
	
	if (opt_d ) 
	{
		if (opt_s == 1) cmdTypeInput = 29; 
		else cmdTypeInput = 30;
	}

	if (::WaitForSingleObject(semserver,2500) == WAIT_TIMEOUT)
	{
		//   Warning: semPersLog Semaphore not obtained. Continuing...
		ReleaseSemaphore(semserver, 1, NULL);
		std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(APBM_RC_APBMNOTRESPONDING, &nExitCode) << std::endl;
		exit(nExitCode);
	}

	int result = api->setHWInventoryConf(magaddr, slot, cabNumber, cabRow, xPos, yPos, cmdTypeInput);

	ReleaseSemaphore(semserver, 1, NULL); //PIPE SINCHRONIZATION

	if (result==0)
	{
		nExitCode = result;
	}
	else
	{
		std::cerr << ACS_APBM_CmdCommon::GetResultCodeText(result, &nExitCode);
	}
	return nExitCode;
}

#endif //APG43_WINDOWS_OLD_CODE

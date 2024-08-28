/*
 *
 * NAME: hwmscbls.cpp
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
 *  This is the main program for the hwmscbls command.
 *
 * DOCUMENT NO
 *
 *
 * AUTHOR
 * 	2011-12-2  XGIUFER
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION

 */



#include <hwmcommon.h>

//#include <string.h>
//using namespace std;

static void PrintUsage();


// Command name

//******************************************************************************
// PrintUsage
// Print the usage message to output stream
//******************************************************************************
void PrintUsage()
{
	::printf("Incorrect usage\n");
	::printf("Usage: hwmscbls \n");
} // End of PrintUsage


//******************************************************************************
// main process
//******************************************************************************
int main(int argc, char** /*argv[]*/)
{
   //Check if APG43
#ifdef DEBUG_PRINT
   cout << "Check if APG40 or APG43" << endl; //debug
#endif

  // int ver = 0;
   // TODO - RECUPERARE INFO DA APZFUNTIONS
 //   ver = GetHWVersion(NULL, NULL);
//
//   if (ver < 0)
//   {
//	   cerr << ACS_HWM_Common::GetResultCodeText(HWM_RC_EXECUTING, &nExitCode) << endl;
//	   return nExitCode;
//   }
//   else if (ver != 3)
//   {
//	   cerr << ACS_HWM_Common::GetResultCodeText(HWM_RC_WRONGPLATFORM, &nExitCode) << endl;
//	   return nExitCode;
//  }

//   int hw_version = -1;
//   hw_version = hwmcommon::check_hardware_version();
//   	if(hw_version != APG43 && hw_version != APG43_2)
//   	{
//            ::printf("%s",hwmcommon::get_last_error_text(ILLEGAL_SYSTEM_CONFIGURATION));
//            return ILLEGAL_SYSTEM_CONFIGURATION;
//   	}

   int node_architecture = -1;
   node_architecture = hwmcommon::check_node_architecture();

   if(node_architecture != EGEM_SCB_RP)
   {
       ::printf("%s",hwmcommon::get_last_error_text(ILLEGAL_SYSTEM_CONFIGURATION));
       return ILLEGAL_SYSTEM_CONFIGURATION;
   }


  if (argc > 1)
   {
      PrintUsage();
      return 2;
   }

#ifdef DEBUG_PRINT
  cout << "Fetch data..." << endl; //debug
#endif

   //Time to fetch data!
//	if ( /*!ACS_HWM_Common::CheckIfActiveNode() */ 1)
//	{
//
//	}


  //TODO - SERVER COMMUNICATION

    int result = -1;

    acs_apbm_api apbm_api;
    uint32_t magazine = 0;
    int32_t slot = -1;
    char ipna_str[16] = {0};
    char ipnb_str[16] = {0};
    acs_apbm::state_t shelf_mgr_state = acs_apbm::SWITCH_BOARD_STATE_UNINTT_STATUS;
    acs_apbm::neighbour_state_t neighbour_state = acs_apbm::SWITCH_BOARD_NEIGHBOUR_STATE_UNINTT;
    acs_apbm::fbn_t fbn = acs_apbm::SWITCH_BOARD_FBN_UNKNOWN;
    acs_apbm::architecture_type_t arch = acs_apbm::ARCHITECTURE_TYPE_NOT_CBA;
    bool print_header = true;
    for(;;)
    {
        result = apbm_api.get_switch_board_info(arch, magazine , slot, ipna_str, ipnb_str, shelf_mgr_state, neighbour_state, fbn);
        if (result == acs_apbm::ERR_INIT_CONNECTION_FAILED)
        {
            ::printf("%s",hwmcommon::get_last_error_text(SERVER_NOT_AVAILABLE));
             return SERVER_NOT_AVAILABLE;
        }
        if(result == acs_apbm::PCP_ERROR_CODE_INVALID_ARCHITECTURE)
        {
            //::printf("error = %d \n",result);
            ::printf("%s",hwmcommon::get_last_error_text(ILLEGAL_SYSTEM_CONFIGURATION));
            return ILLEGAL_SYSTEM_CONFIGURATION;
        }
        if(result < 0)
                {
                    //::printf("error = %d \n",result);
                    ::printf("%s",hwmcommon::get_last_error_text(GENERAL_FAULT));
                    return GENERAL_FAULT;
                }
        if(print_header && result >= 0) //to print header once
        {
            ::printf("\nSCB-RP/4s TABLE\n\n");
            ::printf("SLOT"	"    "   "IP ADDRESS"   "      "   "IP ADDRESS SEC"   "  "   "STATUS"    "  "    "NEIGHBOUR\n");
            print_header = false;
        }
        if(result == acs_apbm::PCP_ERROR_CODE_END_OF_DATA)
                break;
        ::printf("%-4d"  "    " "%-15s"          " "         "%-15s"            " "   "%-7s"      " "    "%-7s\n",
              slot,
              (*ipna_str != 0) ? ipna_str : " ",
              (*ipnb_str != 0) ? ipnb_str : " ",
              (shelf_mgr_state == acs_apbm::SWITCH_BOARD_STATE_MASTER) ? MASTER :
                       (shelf_mgr_state == acs_apbm::SWITCH_BOARD_STATE_PASSIVE) ? PASSIVE :
                      		 (shelf_mgr_state == acs_apbm::SWITCH_BOARD_STATE_IDLE) ? IDLE : UNINIT,
              (neighbour_state == acs_apbm::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT) ? PRESENT :
                      ((neighbour_state == acs_apbm::SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT) ? ABSENT : UNINIT));
    }
	//printf("\n OK!. result returned %d \n",result);
   //If we are here, all is fine!
   return 0;
}


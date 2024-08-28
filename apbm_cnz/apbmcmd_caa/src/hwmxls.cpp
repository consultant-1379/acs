///*
// *
// * NAME: hwmxls.cpp
// *
// * COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2010.
// * All rights reserved.
// *
// *  The Copyright to the computer program(s) herein
// *  is the property of Ericsson Telecom AB, Sweden.
// *  The program(s) may be used and/or copied only with
// *  the written permission from Ericsson Telecom AB or in
// *  accordance with the terms and conditions stipulated in the
// *  agreement/contract under which the program(s) have been
// *  supplied.
// *
// * .DESCRIPTION
// *  This is the main program for the hwmscbls command.
// *
// * DOCUMENT NO
// *
// *
// * AUTHOR
// * 	2011-12-2  XGIUFER
// *
// * REVISION
// *
// * CHANGES
// *
// * RELEASE REVISION HISTORY
// *
// * REV NO		DATE		NAME 		DESCRIPTION
//
// */


#include <hwmcommon.h>
#include <arpa/inet.h>
#include <set>

using namespace std;

static void PrintUsage(std::string);


// Command name
//static const char *  g_lpszName = "hwmxls";

//******************************************************************************
// PrintUsage
// Print the usage message to output stream
//******************************************************************************
void PrintUsage(std::string msg)
{
	//cerr << "Incorrect usage\n";
	cerr << msg.c_str() << "\n";
	cerr << "\n";
	cerr << "Usage: hwmxls [-m magazine]" << endl;
} // End of PrintUsage


//******************************************************************************
// main process
//******************************************************************************
int main(int argc, char* argv[])
{
	//PIPE SINCHRONIZATION


	string const digits = "0123456789";
	string slotstr = "";

   //Check if APG43
#ifdef DEBUG_PRINT
	cout << "Check if APG40 or APG43" << endl; //debug
#endif

//       int hw_version = -1;
//       hw_version = hwmcommon::check_hardware_version();
//        if(hw_version != 2 && hw_version != 1)
//        {
//            ::printf("%s",hwmcommon::get_last_error_text(ILLEGAL_SYSTEM_CONFIGURATION));
//            return ILLEGAL_SYSTEM_CONFIGURATION;
//        }

      int node_architecture = -1;
        node_architecture = hwmcommon::check_node_architecture();
        if(node_architecture != EGEM2_SCX_SA && node_architecture!= EGEM2_SMX_SA)
        {
            ::printf("%s",hwmcommon::get_last_error_text(ILLEGAL_SYSTEM_CONFIGURATION));
            return ILLEGAL_SYSTEM_CONFIGURATION;
        }

	std::string magFilter = "";
	if (argc > 3)
	{
		PrintUsage("Incorrect usage");
		return INVALID_USAGE;
	}
	else if (argc == 3)
	{
		std::string option = argv[1];
		if (option.compare("-m") == 0)
		{
			magFilter = argv[2];

			hwmcommon myUtil;
			if (myUtil.check_arg_mag(argv[2]))
			{
				magFilter = argv[2];
			}
			else
			{
				PrintUsage("Incorrect magazine address");
				return INVALID_USAGE;
			}

		}
		else
		{
			PrintUsage("Incorrect usage");
			return INVALID_USAGE;
		}
	}
	else if (argc == 2)
	{
		std::string argumnent = argv[1];
		size_t length = argumnent.length();
		std::string option = "";
		if (length > 2)
		{
			option = argumnent.substr(0,2);
		}
		else
		{
			PrintUsage("Incorrect usage");
			return INVALID_USAGE;
		}

		if (option.compare("-m") == 0)
		{
			magFilter = argumnent.substr(2);
		}
		else
		{
			PrintUsage("Incorrect usage");
			return INVALID_USAGE;
		}
	}
#ifdef DEBUG_PRINT
	cout << "Fetch data..." << endl; //debug
#endif

   int result = -1;


   //TODO - VERIFICA MAGAZINO

   int32_t slot = -1;
   char ipna_str[16] = {0};
   char ipnb_str[16] = {0};
   char magazine_str[16] = {0};
   acs_apbm::state_t shelf_mgr_state = acs_apbm::SWITCH_BOARD_STATE_UNINTT_STATUS;
   acs_apbm::neighbour_state_t neighbour_state = acs_apbm::SWITCH_BOARD_NEIGHBOUR_STATE_UNINTT;
   acs_apbm::fbn_t fbn = acs_apbm::SWITCH_BOARD_FBN_UNKNOWN;
   acs_apbm::architecture_type_t arch = acs_apbm::ARCHITECTURE_TYPE_CBA;
   bool print_header = true;
   set<uint32_t> magazine_list;
   if ( strcmp(magFilter.c_str(),"")==0 ){
        if (hwmcommon::get_all_magazines(magazine_list) !=0 ){
           ::printf("%s",hwmcommon::get_last_error_text(GENERAL_FAULT));
	   return GENERAL_FAULT;
   	}
   } else {
       uint32_t magazine = 0;
       if (hwmcommon::ip_format_to_uint32(magazine, magFilter.c_str()) != 0)
       {
           ::printf("%s",hwmcommon::get_last_error_text(GENERAL_FAULT));
           return GENERAL_FAULT;
       }
       magazine_list.insert(htonl(magazine));
   }
   set<uint32_t>::iterator it;
   for (it = magazine_list.begin(); it != magazine_list.end(); ++it) { // to sort magazine list
	   acs_apbm_api apbm_api;

	   std::set<int32_t> slots_to_print;
	   slots_to_print.insert(0);
	   slots_to_print.insert(25);
	

	   int failure_count = 0;
	   

	   for(;;)
	   {
		   result = apbm_api.get_switch_board_info(arch, ntohl(*it)/*magazine iterator*/ , slot, ipna_str, ipnb_str, shelf_mgr_state, neighbour_state, fbn);

		   if (result == acs_apbm::ERR_INIT_CONNECTION_FAILED)
		   {
			   ::printf("%s",hwmcommon::get_last_error_text(SERVER_NOT_AVAILABLE));
			   return SERVER_NOT_AVAILABLE;
		   }
		   if(result < 0)
		   {
			   //::printf("error = %d \n",result);
			   ::printf("%s",hwmcommon::get_last_error_text(GENERAL_FAULT));
			   return GENERAL_FAULT;
		   }
		   if(print_header && result >= 0) //to print header once
		   {
			   ::printf("\nSWITCH TABLE\n");
			   ::printf("MAG" "             " "SLOT" "   " "FBN" "   " "IP ADDRESS" "      " "IP ADDRESS SEC" "  " "STATUS" "  " "NEIGHBOUR\n");
			   print_header = false;
		   }

		   if(result == acs_apbm::PCP_ERROR_CODE_END_OF_DATA) //No more switch board for this magazine
		   {
			break;
		   }


		   if(slot!=-1)//slot=-1 in case of PCP_ERROR_CODE_DSD_REPLY_TIME_OUT, skipping the display in that case(tr hl97410)
		   {
			   slots_to_print.erase(slot);

			   ::printf("%-16s" "%-7d" "%-6s"  "" "%-16s"          ""         "%-16s"            ""   "%-8s"      ""    "%-9s\n",
					   (hwmcommon::uint32_to_ip_format(magazine_str, ntohl(*it))==0) ? magazine_str : " ",
							   slot,
							   (fbn == acs_apbm::SWITCH_BOARD_FBN_SCXB) ? FBN_SCXB : ((fbn == acs_apbm::SWITCH_BOARD_FBN_SMXB) ? FBN_SMXB : UNINIT),
									   (*ipna_str != 0) ? ipna_str : " ",
											   (*ipnb_str != 0) ? ipnb_str : " ",
													   /**/  (shelf_mgr_state == 3/*acs_apbm::SWITCH_BOARD_STATE_MASTER*/) ? MASTER :                        /*MASTER STATUS*/
															   (shelf_mgr_state == 1/*acs_apbm::SWITCH_BOARD_STATE_PASSIVE*/) ? PASSIVE :                  /*PASSIVE STATUS*/
																	   (shelf_mgr_state == 0/*acs_apbm::SWITCH_BOARD_STATE_IDLE*/) ? IDLE :                    /*IDLE STATUS*/
																			   (shelf_mgr_state == 2/*acs_apbm::SWITCH_BOARD_STATE_DEGRADED*/) ? DEGRADED : UNINIT,/*DEGRADED STATUS*/
																					   (neighbour_state == acs_apbm::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT) ? PRESENT :
																							   (neighbour_state == acs_apbm::SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT) ? ABSENT : UNINIT);
		   }
		   else if(result == acs_apbm::PCP_ERROR_CODE_DSD_REPLY_TIME_OUT)
		   {//Printing the status as UNINT in case of PCP_ERROR_CODE_DSD_REPLY_TIME_OUT  is repeated for 10 times
			   failure_count++;
			   if(failure_count>=4)
			   {
				   break;
			   }
			   else
			   {
				   continue;
			   }
		   }
       }

	if(result != acs_apbm::PCP_ERROR_CODE_END_OF_DATA)
	{
	   for (std::set<int32_t>::iterator slot_it = slots_to_print.begin(); slot_it != slots_to_print.end(); ++slot_it)
	   {
		   ::printf("%-16s" "%-7d" "%-6s"  "" "%-16s"          ""         "%-16s"            ""   "%-8s"      ""    "%-9s\n",
				   (hwmcommon::uint32_to_ip_format(magazine_str, ntohl(*it))==0) ? magazine_str : " ",
						   *slot_it,
						   UNINIT,
						   " ",
						   " ",
						   UNINIT,/*DEGRADED STATUS*/
						   UNINIT);
	   }
	}
   }
   //TODO - CHECK STATUS INSERTED 8 WHITE SPACE INSTEAD OF 7 AS IN DESIGN BASE
	/* SORT */
	return 0;
}

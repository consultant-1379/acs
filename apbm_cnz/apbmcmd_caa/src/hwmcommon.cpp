/*
 * hwmcommon.cpp
 *
 *  Created on: Dec 7, 2011
 *      Author: xgiufer
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_paramhandling.h>
#include <ACS_CC_Types.h>


#include <hwmcommon.h>

const char * __CLASS_NAME__::get_last_error_text(const int error) {

	switch (error)
	{
	case (GENERAL_FAULT):
		return "Error when executing ( general fault )\n";
		break;
//	case (INVALID_USAGE):
//		break;
	case (SERVER_NOT_AVAILABLE):
		return "Server not responding\n";
		break;
//	case (HWM_SERVER_NOT_AVAILABLE ):   //HWM SERVER NOT EXIST
//		return "HWM Server not responding\n";
//		break;
	case (APBM_SERVER_NOT_AVAILABLE):
			return "APBM Server not responding\n";
		break;
	case (ILLEGAL_SYSTEM_CONFIGURATION):
			return "Illegal command in this system configuration.\n";
		break;
        case (WRONG_PLATFORM):
                        return "The command is not valid for this platform.\n";
                break;
	default:
		return "Other error\n";
	}

}

void __CLASS_NAME__::reverse_dotted_dec_str(std::istringstream &istr)
{
	istr.clear();     // return to the beginning
	(void)istr.seekg(0);    //      of the stream
	std::istringstream tempIstr;
	std::string tempString = "";
	char c;

	do
	{
		c = istr.get();      //get char from istringstream

		if (c == '.' || c == EOF)
		{
			(void)tempIstr.seekg(0);

			if (c == '.')
				tempIstr.str(c + tempString + tempIstr.str());
			else
			tempIstr.str(tempString + tempIstr.str());

			tempString = "";
		}
		else
			tempString = tempString + c;
	} while (c != EOF);

	istr.str(tempIstr.str());

	istr.clear();     // return to the beginning
	(void)istr.seekg(0);    //      of the stream
}



unsigned short __CLASS_NAME__::num_of_char(std::istringstream &istr, char searchChar)
{
	istr.clear();        // return to the beginning
	(void)istr.seekg(0); //      of the stream
	char c;
	unsigned short numChar = 0;

	do {
		c = istr.peek();
		if (c == searchChar)
			numChar = numChar + 1;

		(void)istr.seekg((int)(istr.tellg()) + 1);
	} while (c != EOF);

	istr.clear();     // return to the beginning
	(void)istr.seekg(0);    //      of the stream
	return numChar;
}


bool __CLASS_NAME__::check_figures(std::istringstream &istr)
{
	istr.clear();     // return to the beginning
	(void)istr.seekg(0);    //      of the stream
	char c = istr.get(); //get first char in the istringstream

	while (c != EOF)
	{
		if ((c < '0') || (c > '9'))
			return false;

		c = istr.get();
	}

	istr.clear();     // return to the beginning
	(void)istr.seekg(0);    //      of the stream
	return true;
}

bool __CLASS_NAME__::dotted_dec_to_long(std::istringstream &istr, unsigned long &value,
                              unsigned long lowerLimit1, unsigned long upperLimit1,
                              unsigned long lowerLimit2, unsigned long upperLimit2,
                              unsigned long lowerLimit3, unsigned long upperLimit3,
                              unsigned long lowerLimit4, unsigned long upperLimit4)
{
	istr.clear();     // return to the beginning
	(void)istr.seekg(0);    //      of the stream
	char c;
	std::string strAddr = "";
	unsigned short dotNum = 0;
	value = 0;

	do {
		c = istr.get();
		if (c == '.' || c == EOF)
		{
			dotNum++;
			std::istringstream converter(strAddr);

			if (! check_figures(converter))   //checks that only figures (0-9) are used in the option arg.
			return false;

			strAddr = "";
			unsigned long plug;
			if (! (converter >> plug)) //check if string is possible to convert to long
			{
			value = 0;
			return false;
			}

			else
			{
			if (dotNum == 1)
			{
				if (! (((plug >= lowerLimit1) && (plug <= upperLimit1)) || (plug == 255)))  //CR GESB
				{
					value = 0;
					return false;
				}
			}

			else if (dotNum == 2)
			{
				if (! ((plug >= lowerLimit2) && (plug <= upperLimit2)))
				{
					value = 0;
					return false;
				}
			}

			else if (dotNum == 3)
			{
				if (! (((plug >= lowerLimit3) && (plug <= upperLimit3)) || (plug == 255)))  //CR GESB
				{
					value = 0;
					return false;
				}
			}

			else if (dotNum == 4)
			{
				if (! (((plug >= lowerLimit4) && (plug <= upperLimit4)) || (plug == 255)))  //CR GESB
				{
					value = 0;
					return false;
				}
			}

			value = plug | value;

			if (dotNum != 4)     // if not last byte, shift 1 byte to the left
				value = value << 8;
			}
		}

		else
			strAddr = strAddr + c;
	} while (c != EOF);

	return true;
}



bool __CLASS_NAME__::check_arg_mag(char* arg)
{
	std::istringstream istr(arg);

	reverse_dotted_dec_str(istr); //reverses the order of the dotted decimal string to match the mag addr format on the server

	if (num_of_char(istr,'.') != 3) // If not 3 dots, faulty format
		return false;

	unsigned long dummyMag;
	if (! dotted_dec_to_long(istr, dummyMag, 0, 15, 0, 0, 0, 15, 0, 15))
		return false;

	return true;
}

int __CLASS_NAME__::ip_format_to_uint32 (uint32_t & value, const char * value_ip) {

        in_addr addr;
        if (const int call_result = ::inet_pton(AF_INET, value_ip, &addr) <= 0) { // ERROR: in conversion
              return GENERAL_FAULT;
        }
        value = addr.s_addr;

        return 0;
}

int __CLASS_NAME__::uint32_to_ip_format (char (& value_ip) [16], uint32_t value) {
        // Converting value address into IP dotted-quad format "ddd.ddd.ddd.ddd"
        in_addr addr;
        addr.s_addr = value;
        errno = 0;
        if (!::inet_ntop(AF_INET, &addr, value_ip, ACS_APBM_ARRAY_SIZE(value_ip))) { // ERROR: in conversion
              ::printf("%s",hwmcommon::get_last_error_text(GENERAL_FAULT));
              return GENERAL_FAULT;
              value_ip[0] = 0;
        }
        return 0;
}
int __CLASS_NAME__::get_imm_class_instance_name(const char * class_name, string & class_instance_name){
	/* start connecting to IMM to get info*/
	OmHandler omManager;
	ACS_CC_ReturnType result;

	/*try initialize imm connection to IMM*/
	result = omManager.Init();

	if (result != ACS_CC_SUCCESS){
		::printf("%s",hwmcommon::get_last_error_text(GENERAL_FAULT));
		return GENERAL_FAULT;

	}/*exiting on IMM init failure: generic error*/


	/* get the root object instance of HardwareInventory class
	 * if no obj found return on Failure
	 * if more than one obj found returns error: just one instance hardwareInventoryId=1 must be present
	 */
	std::vector<std::string> p_dnList;
	result = omManager.getClassInstances(class_name, p_dnList);
	if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1) ) {
		/*exiting on error either an erroe occurred or more than one instance found */
		::printf("%s",hwmcommon::get_last_error_text(GENERAL_FAULT));
		return GENERAL_FAULT;
	}

	class_instance_name = p_dnList[0];
	omManager.Finalize();

	return 0;
}

int __CLASS_NAME__::check_hardware_version()
{

	string class_instance_name;

	if(get_imm_class_instance_name(IMM_ROOT_CLASS,class_instance_name) != 0)
	{
		//unable to get apHwVersion exiting with error 1
		::printf("%s",hwmcommon::get_last_error_text(GENERAL_FAULT));
		return GENERAL_FAULT;
	}

		//const std:: string ApzFunctionsId1		= "apzFunctionsId=1";
		const std:: string ap_hw_version_attr = "apHwVersion";
		acs_apgcc_paramhandling pha;
		ACS_CC_ReturnType imm_result;
		int hw_version;
		imm_result = pha.getParameter(class_instance_name.c_str(),ap_hw_version_attr, &hw_version);
		if (imm_result != ACS_CC_SUCCESS)
		{
			//unable to get apHwVersion exiting with error 1
			::printf("%s",hwmcommon::get_last_error_text(GENERAL_FAULT));
			return GENERAL_FAULT;
		}

	return hw_version;
}
int __CLASS_NAME__::check_node_architecture()
{

			string class_instance_name;

			if(get_imm_class_instance_name(IMM_ROOT_CLASS,class_instance_name) != 0)
			{
				//unable to get apHwVersion exiting with error 1
				::printf("%s",hwmcommon::get_last_error_text(GENERAL_FAULT));
				return GENERAL_FAULT;
			}
			const std:: string ap_node_architecture_attr = "apgShelfArchitecture";
			acs_apgcc_paramhandling pha;
			ACS_CC_ReturnType imm_result;
			int node_architecture = 0;
			imm_result = pha.getParameter(class_instance_name.c_str(),ap_node_architecture_attr, &node_architecture);
			if (imm_result != ACS_CC_SUCCESS)
			{
							//unable to get apHwVersion exiting with error 1
							::printf("%s",hwmcommon::get_last_error_text(GENERAL_FAULT));
							return GENERAL_FAULT;
			}
			return node_architecture;
}
int __CLASS_NAME__::get_all_magazines(set<uint32_t> & magazine_list){


   ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance();
   if (!hwc) { // ERROR: creating HWC CS instance
#ifdef DEBUG_PRINT
       ::printf("Call 'createHWCInstance' failed: cannot create the HWC instance to access CS configuration data");
#endif
       return GENERAL_FAULT;
   }
   ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
   if (!bs) {
#ifdef DEBUG_PRINT
       ::printf("Call 'createBoardSearchInstance' failed: cannot create a CS board seach instance to perform a configuration search");
#endif
      return GENERAL_FAULT;
   }
   // search criteria
   bs->reset();

   int node_architecture = -1;
   node_architecture = hwmcommon::check_node_architecture();
   if(node_architecture == EGEM2_SMX_SA)
   {
	   bs->setFBN(400  /* ACS_CS_API_HWC_NS::FBN_SMXB */);
   }
   else
   {
	   bs->setFBN(340  /* ACS_CS_API_HWC_NS::FBN_SCXB */);
   }

   // Now I can search
   ACS_CS_API_IdList board_list;
   ACS_CS_API_NS::CS_API_Result cs_call_result = hwc->getBoardIds(board_list, bs);
   if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
#ifdef DEBUG_PRINT
       ::printf("Call 'getBoardIds' failed: cannot get AP board ids from CS: cs_call_result == %d", cs_call_result);
#endif
       ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
       ACS_CS_API::deleteHWCInstance(hwc);
       return GENERAL_FAULT;
   }
   // Should there be at least one board
   if (board_list.size() <= 0) { // ERROR: no boards found
#ifdef DEBUG_PRINT
       ::printf("No AP boards found in the CS system configuration table");
#endif
       ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
       ACS_CS_API::deleteHWCInstance(hwc);
       return acs_apbm::ERR_CS_NO_BOARD_FOUND;
   }

   uint32_t magazine_x;

   for (size_t i = 0; i < board_list.size(); ++i) {
       BoardID board_id = board_list[i];
       //... my magazine address ...
       if ((cs_call_result = hwc->getMagazine(magazine_x, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting magazine info
#ifdef DEBUG_PRINT
        ::printf("Call 'getMagazine' failed: cannot retrieve my AP magazine address from CS: cs_call_result == %d", cs_call_result);
#endif
        ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
        ACS_CS_API::deleteHWCInstance(hwc);
        return acs_apbm::ERR_CS_GET_MAGAZINE;
       }

#ifdef DEBUG_PRINT
   char magc[16] = {0};
   hwmcommon::uint32_to_ip_format(magc, magazine_x);
   ::printf("magazine found in CS search : %d  string: %s \n\n", magazine_x, magc);
#endif

   //insert single value and ordered
   //magazine_list.insert(magazine_x);
   magazine_list.insert(htonl(magazine_x));
   }
#ifdef DEBUG_PRINT
   set<uint32_t>::iterator it;
   char magc[16] = {0};
   char magh[16] = {0};
   for (it=magazine_list.begin(); it!=magazine_list.end(); ++it) {
       hwmcommon::uint32_to_ip_format(magc, *it);
       hwmcommon::uint32_to_ip_format(magh, ntohl(*it));
       ::printf("set list value: %d   string: %s \n",*it, magc);
       ::printf("set list value: %d   string: %s reverse\n\n",ntohl(*it), magh);
   }
#endif
   ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
   return 0;
}

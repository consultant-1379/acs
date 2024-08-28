#include "ACS_DSD_ServicePrimitive.h"
#include <string>


std::string ACS_DSD_ServicePrimitive::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	sprintf(mess,"PRIMITIVE_ID = %d,  VERSION = %d, PROTOCOL_ID = %d", _primitive_id, _protocol_version, _protocol_id);
	return mess;
}



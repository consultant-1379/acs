#ifndef ACS_CS_API_NETWORKTABLECHANGE_R1_H_
#define ACS_CS_API_NETWORKTABLECHANGE_R1_H_


struct ACS_CS_API_NetworkTable_R1
{
	BoardID boardId;
	std::string interfaceId;
	std::string netwName;
	std::string mac;
	std::string adminState;
	std::string extNetwId;
	std::string intNetwId;
};

#endif

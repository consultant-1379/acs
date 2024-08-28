/*
 * ACS_CS_API_HWC_R2.h
 *
 *  Created on: May 18, 2012
 *      Author: estevol
 */

#ifndef ACS_CS_API_HWC_R2_H_
#define ACS_CS_API_HWC_R2_H_


#include "ACS_CS_API_HWC_R1.h"

class ACS_CS_API_HWC_R2: public ACS_CS_API_HWC_R1 {
public:

	virtual ACS_CS_API_NS::CS_API_Result getContainerPackage (std::string &container, BoardID boardId) = 0;

	virtual ACS_CS_API_NS::CS_API_Result getBgciIPEthA (uint32_t &ip, BoardID boardId) = 0;

	virtual ACS_CS_API_NS::CS_API_Result getBgciIPEthB (uint32_t &ip, BoardID boardId) = 0;

	virtual ACS_CS_API_NS::CS_API_Result getSoftwareVersionType (uint16_t &version, BoardID boardId) = 0;

	virtual ACS_CS_API_NS::CS_API_Result getUuid (std::string &uuid, BoardID boardId) = 0;

	static ACS_CS_API_BoardSearch_R2 * createBoardSearchInstance ();

	virtual ACS_CS_API_NS::CS_API_Result getSolIPEthA (uint32_t &ip, BoardID boardId) = 0;

	virtual ACS_CS_API_NS::CS_API_Result getSolIPEthB (uint32_t &ip, BoardID boardId) = 0;

};

#endif /* ACS_CS_API_HWC_R2_H_ */

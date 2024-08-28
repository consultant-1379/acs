#ifndef ACS_USA_IMM_H_
#define ACS_USA_IMM_H_

#include <string>
#include <stdint.h>

namespace imm
{
/**
 * @namespace AxeAdditionalInfo
 * 
 * @brief
 * Contains the names of all attributes of MOC AxeAdditionalInfo.
 *
 */
namespace AxeAdditionalInfo
{
	const std::string rDN = "AxeAdditionalInfoId";
	
	const std::string status = "status";
	
	const std::string userName = "attribute2";

	const std::string className = "AxeAdditionalInfo";

}

/**
 * @namespace AxeAdditionalInterface
 *
 * @brief
 * Contains the names of all attributes of MOC AxeAdditionalInterface.
 *
 */
namespace AxeAdditionalInterface
{
	const std::string rDN = "AxeAdditionalInterfaceId";

	const std::string className = "AxeAdditionalInterface";

}

}

#endif /* ACS_USA_IMM_H_ */


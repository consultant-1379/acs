/*
 * ACS_TRAPDS_API_R2.h
 *
 *  Created on: Mar 26, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_API_R2_H_
#define ACS_TRAPDS_API_R2_H_

#include "ACS_TRAPDS_API_R1.h"


class ACS_TRAPDS_API_R2 : public ACS_TRAPDS_API_R1
{

public:

	/** @brief Unsubscribe method
	 *
	 *	@return Return result of unsubscription as a ACS TRAPDS API Return Codes
	 */
	ACS_TRAPDS::ACS_TRAPDS_API_Result unsubscribe();


	ACS_TRAPDS_API_R2(int ){};
	ACS_TRAPDS_API_R2(){};
	virtual ~ACS_TRAPDS_API_R2(){};

protected:

private:

};

#endif /* ACS_TRAPDS_API_R2_H_ */

#ifndef ACS_APGCC_OIHANDLER_V4_H_
#define ACS_APGCC_OIHANDLER_V4_H_

#include "acs_apgcc_oihandler_V3.h"

class acs_apgcc_oihandler_V4_impl;

class acs_apgcc_oihandler_V4: public acs_apgcc_oihandler_V3 {
public:

	//==============//
	// Constructors //
	//==============//

	/** @brief acs_apgcc_oihandler_V4 Default constructor
	 *
	 *	acs_apgcc_oihandler_V4 Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_oihandler_V4();


	/** @brief acs_apgcc_oihandler_V4 copy constructor
	 *
	 *	acs_apgcc_oihandler_V4 Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_oihandler_V4(acs_apgcc_oihandler_V4 &oi);


	/** @brief acs_apgcc_oihandler_V4 Destructor
	 *
	 *	acs_apgcc_oihandler_V4 Destructor of class
	 *
	 *	@remarks Remarks
	 */

	~acs_apgcc_oihandler_V4();

	//===========//
	// Functions //
	//===========//

	/**
	 * getInternalLastError method: this method returns the error code of the last error occurred
	 *
	 * @return int. the error code of the last error occurred
	 */
	int getInternalLastError();


	/**
	 * getInternalLastErrorText method: this method returns the message error of the last error occurred
	 *
	 * @return char*. the message error of the last error occurred
	 */
	char* getInternalLastErrorText();

private:

	acs_apgcc_oihandler_V4& operator=(const acs_apgcc_oihandler_V4 &oiHandler);
	acs_apgcc_oihandler_V4_impl *oiHandlerimpl;

};



#endif /* acs_apgcc_oihandler_V4_H_ */

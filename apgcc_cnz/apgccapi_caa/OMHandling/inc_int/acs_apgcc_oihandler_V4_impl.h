#ifndef ACS_APGCC_OIHANDLER_V4_IMPL_H_
#define ACS_APGCC_OIHANDLER_V4_IMPL_H_
#include "acs_apgcc_oihandler_V3_impl.h"

class acs_apgcc_oihandler_V4_impl: public acs_apgcc_oihandler_V3_impl {
public:
	//==============//
	// Constructors //
	//==============//

	/** @brief acs_apgcc_oihandler_V3_impl Default constructor
	 *
	 *	ACS_APGCC_OiHandler Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_oihandler_V4_impl();


	/** @brief acs_apgcc_oihandler_V3_impl Default constructor
	 *
	 *	ACS_APGCC_OiHandler Copy Constructor of class
	 *
	 *	@remarks Remarks
	 */
	acs_apgcc_oihandler_V4_impl(acs_apgcc_oihandler_V4_impl *oiHandlerImp);


	/** @brief acs_apgcc_oihandler_V3_impl Destructor
	 *
	 *	ACS_APGCC_OiHandler Destructor of class
	 *
	 *	@remarks Remarks
	 */
	~acs_apgcc_oihandler_V4_impl();

};
#endif /* ACS_APGCC_OIHANDLER_V4_IMPL_H_ */

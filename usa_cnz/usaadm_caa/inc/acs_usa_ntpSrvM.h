/*
 * ACS_USA_NtpSrvM.h
 *
 *  Created on: 
 *      Author: 
 */

#ifndef ACS_USA_NTPSRVM_H_
#define ACS_USA_NTPSRVM_H_

#include <unistd.h>
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperationtypes.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_APGCC_Util.H"
#include "ACS_TRA_Logging.h"
#include "acs_usa_types.h"
#include "acs_usa_tratrace.h"
#include <acs_apgcc_oihandler_V3.h>


class ACS_USA_NtpSrvM: public ACE_Event_Handler, public acs_apgcc_objectimplementerinterface_V3 {
public:

	ACS_USA_NtpSrvM();

	virtual ~ACS_USA_NtpSrvM();

	/*the callback*/

	/** @brief create method
	 *
	 *	This method will be called as a callback when an Object is created as instance of a Class NtpServer
	 *	All input parameters are input provided by IMMSV Application and have to be used by the implementer to perform
	 *	proper actions.
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle in which the creation of the Object is contained.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param className: the name of the class. When an object is created as instance of this class this method is
	 *	called if the application has registered as class implementer. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param parentname: the name of the parent object for the object now creating.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param attr: a pointer to a null terminated array of ACS_APGCC_AttrValues element pointers each one containing
	 *	the info about the attributes belonging to the now creating class.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 */
	virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	/** @brief deleted method
	 *
	 *	This method will be called as a callback when deleting an NtpServer Object
	 *	Object Implementer. All input parameters are input provided by IMMSV Application and have to be used by
	 *	the implementer to perform proper actions.
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parametr provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle in which the deletion of the Object is contained.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param objName: the Distinguished name of the object that has to be deleted.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	/** @brief modify method
	 *
	 *	This method will be called as a callback when modifying an NtpServer Object
	 *	All input parameters are input provided by IMMSV Application and have to be used by the implementer to perform
	 *	proper actions.
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle in which the modify of the Object is contained.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param objName: the Distinguished name of the object that has to be modified.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param attrMods: a NULL terminated array of pointers to ACS_APGCC_AttrModification elements containing
	 *	the information about the modify to perform. This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	/** @brief complete method
	 *
	 *	This method will be called as a callback when a Configuration Change Bundle is complete and can be applied
	 *	regarding  an NtpServer Object
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle in which the modify of the Object is contained.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/** @brief abort method
	 *
	 *	This method will be called as a callback when a Configuration Change Bundle, regarding  an NtpServer Object,
	 *	has aborted. This method is called only if at least one complete method failed.
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
	 *	registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/** @brief apply method
	 *
	 *	This method will be called as a callback when a Configuration Change Bundle, regarding  an NtpServer Object, is complete and can be applied.
	 *	This method is called only if all the complete method have been successfully executed.
	 *
	 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
	 *	registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/** @brief updateRuntime method
	 *
	 * 	This method will be called as a callback when modifying a runtime not-cached attribute of a configuration Object
	 * 	for which the Application has registered as Object Implementer.
	 * 	All input parameters are input provided by IMMSV Application and have to be used by the implementer
	 * 	to perform proper actions.
	 *
	 *  @param p_objName: the Distinguished name of the object that has to be modified.
	 *  				  This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @param p_attrName: the name of attribute that has to be modified.
	 *  				   This is an Input Parameter provided by IMMSV Application.
	 *
	 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 *	@remarks Remarks
	 */
	virtual ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);

	/**
	 * @brief adminOperationCallback method
	 * adminOperationCallback method: This method will be called as a callback to manage an administrative operation invoked, on the
	 * object.
	 *
	 * @param  p_objName:	the distinguished name of the object for which the administrative operation
	 * has to be managed.
	 *
	 * @param  oiHandle : input parameter,ACS_APGCC_OiHandle this value has to be used for returning the callback
	 *  result to IMM.
	 *
	 * @param invocation: input parameter,the invocation id used to match the invocation of the callback with the invocation
	 * of result function
	 *
	 * @param  p_objName: input parameter,the name of the object
	 *
	 * @param  operationId: input parameter, the administrative operation identifier
	 *
	 * @param paramList: a null terminated array of pointers to operation params elements. each element of the list
	 * is a pointer toACS_APGCC_AdminOperationParamType element holding the params provided to the Administretive operation..
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList);



	/** @brief getClassName method
	 *
	 * 	This method return the name of the IMM class.
	 *
	 */
	const char* getIMMClassName() const {return m_ImmClassName.c_str();};

	int stop();

	virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);
	int handle_close(ACE_HANDLE, ACE_Reactor_Mask mask);
	int handle_timeout(const ACE_Time_Value&, const void* );
	int registerNtpObserver();
	bool removeNtpObserver();
	int monitorNtpServer();

private:
	enum operationCallBackType
        {
                createT = 1,
                deleteT = 2,
                modifyT = 3
        };

	ACS_USA_NtpSrvM::operationCallBackType m_callback;	
	std::string m_ImmClassName;
	int m_timerid;


	void reportProblem() const;
        int raiseNtpServerAlarm();
	int ceaseNtpServerAlarm();

};

#endif /* ACS_USA_NTPSRVM_H_ */

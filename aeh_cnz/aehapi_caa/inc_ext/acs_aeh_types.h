/*
 * acs_aeh_types.h
 *
 *  Created on: May 9, 2011
 *      Author: xfabron
 */

//========================================================================================
// The allowed values for perceived severity
//========================================================================================

#ifndef ACS_AEH_PERCEIVED_SEVERITY_A1
#define ACS_AEH_PERCEIVED_SEVERITY_A1	"A1"
#endif

#ifndef ACS_AEH_PERCEIVED_SEVERITY_A2
#define ACS_AEH_PERCEIVED_SEVERITY_A2	"A2"
#endif

#ifndef ACS_AEH_PERCEIVED_SEVERITY_A3
#define ACS_AEH_PERCEIVED_SEVERITY_A3	"A3"
#endif

#ifndef ACS_AEH_PERCEIVED_SEVERITY_O1
#define ACS_AEH_PERCEIVED_SEVERITY_O1	"O1"
#endif

#ifndef ACS_AEH_PERCEIVED_SEVERITY_O2
#define ACS_AEH_PERCEIVED_SEVERITY_O2	"O2"
#endif

#ifndef ACS_AEH_PERCEIVED_SEVERITY_CEASING
#define ACS_AEH_PERCEIVED_SEVERITY_CEASING	"CEASING"
#endif

#ifndef ACS_AEH_PERCEIVED_SEVERITY_EVENT
#define ACS_AEH_PERCEIVED_SEVERITY_EVENT	"EVENT"
#endif

//========================================================================================
// The allowed values for object class of reference
//========================================================================================

#ifndef ACS_AEH_OBJ_CLASS_REFERENZE_APZ
#define ACS_AEH_OBJ_CLASS_REFERENZE_APZ		"APZ"
#endif

#ifndef ACS_AEH_OBJ_CLASS_REFERENZE_APT
#define ACS_AEH_OBJ_CLASS_REFERENZE_APT		"APT"
#endif

#ifndef ACS_AEH_OBJ_CLASS_REFERENZE_PWR
#define ACS_AEH_OBJ_CLASS_REFERENZE_PWR		"PWR"
#endif

#ifndef ACS_AEH_OBJ_CLASS_REFERENZE_EXT
#define ACS_AEH_OBJ_CLASS_REFERENZE_EXT		"EXT"
#endif

#ifndef ACS_AEH_OBJ_CLASS_REFERENZE_EVENT
#define ACS_AEH_OBJ_CLASS_REFERENZE_EVENT	"EVENT"
#endif



#ifndef ACS_AEH_TYPES_H_
#define ACS_AEH_TYPES_H_

//========================================================================================
//	Size declarations for parameters.
//========================================================================================

#define PROCESS_NAME_MAX_LEN 		32
#define PERC_SEVERITY_MAX_LEN 		8
#define PROBABLE_CAUSE_MAX_LEN		128
#define OBJ_CLASS_OF_REF_MAX_LEN	8
#define OBJECT_OF_REF_MAX_LEN		64
#define PROBLEM_DATA_MAX_LEN		1024
#define PROBLEM_TEXT_MAX_LEN		1024

//========================================================================================
//	Type declarations for return type and error type.
//========================================================================================

typedef enum {

	ACS_AEH_ok 		= 0,	// Ok return code from sendEventMessage method
	ACS_AEH_error 	= 1		// Error return code from the sendEventMessage method

}ACS_AEH_ReturnType;


//========================================================================================
//	Constants to be retrieved by the getErrorText method.
//========================================================================================

const char ACS_AEH_noError[] = "No Error";		// The initial string set before first setErrorText call.

//========================================================================================
//	Constant error types to be retrieved by the getError method.
//========================================================================================

typedef enum {

	ACS_AEH_noErrorType 			= 0,
	ACS_AEH_eventDeliveryFailure 	= -1,
	ACS_AEH_syntaxError  			= -2,
	ACS_AEH_genericError  			= -3

}ACS_AEH_ErrorType;



//========================================================================================
//	Type declarations for parameters
//========================================================================================

typedef const char *acs_aeh_processName;
typedef const long acs_aeh_specificProblem;
typedef const char *acs_aeh_percSeverity;
typedef const char *acs_aeh_probableCause;
typedef const char *acs_aeh_objClassOfReference;
typedef const char *acs_aeh_objectOfReference;
typedef const char *acs_aeh_problemData;
typedef const char *acs_aeh_problemText;


#endif /* ACS_AEH_TYPES_H_ */

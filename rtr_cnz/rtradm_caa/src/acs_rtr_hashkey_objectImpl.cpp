/**
   @file acs_rtr_hashkey_ObjectImpl.cpp

   Class method implementation for RTR module.

   This module contains the implementation of class declared in
   the RTR Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012     XHARBAV       Initial Release
   N/A       05/12/2013   QVINCON   Re-factoring
*/

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_rtr_hashkey_objectImpl.h"
#include "acs_rtr_server.h"
#include "acs_rtr_global.h"
#include "acs_rtr_tracer.h"

#include "acs_apgcc_omhandler.h"

#include <crypto_status.h>
#include <crypto_api.h>

ACS_RTR_TRACE_DEFINE(ACS_RTR_HashKey_ObjectImpl);

namespace hashKeyClass{
    const char ImmImplementerName[] = "RTR_OI_HashKey";
}


/*===================================================================
   ROUTINE: ACS_RTR_HashKey_ObjectImpl
=================================================================== */
ACS_RTR_HashKey_ObjectImpl::ACS_RTR_HashKey_ObjectImpl()
: acs_apgcc_objectimplementereventhandler_V3(hashKeyClass::ImmImplementerName),
  m_ImmClassName(rtr_imm::EcimPasswordClassName)
{
	ACS_RTR_TRACE_MESSAGE("In");

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: ~ACS_RTR_HashKey_ObjectImpl
=================================================================== */
ACS_RTR_HashKey_ObjectImpl::~ACS_RTR_HashKey_ObjectImpl()
{
	ACS_RTR_TRACE_MESSAGE("In");
	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType ACS_RTR_HashKey_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	ACS_RTR_TRACE_MESSAGE("In, create a HashKey object MS DN:<%s>", parentName);
	// to avoid warning msg
	UNUSED(ccbId);
	UNUSED(oiHandle);
	UNUSED(className);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	std::string hashKeyVal;
	// extract the attributes
	for(size_t idx = 0; attr[idx] != NULL ; ++idx)
	{
		// check if RDN attribute
		if( 0 == ACE_OS::strcmp(rtr_imm::PasswordAttribute, attr[idx]->attrName) )
		{
			hashKeyVal.assign(reinterpret_cast<char *>(attr[idx]->attrValues[0]));
			if(!validateHashKey(hashKeyVal))
			{
				ACS_RTR_TRACE_MESSAGE("%s","Validation for HashKey failed");
				result = ACS_CC_FAILURE;
			}
			break;

		}
	}
	ACS_RTR_TRACE_MESSAGE("Out");

	return result;
}

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType ACS_RTR_HashKey_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	ACS_RTR_TRACE_MESSAGE("In, delete the HashKey object<%s>", objName);
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	ACS_RTR_TRACE_MESSAGE("Out");

	return result;
}

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType ACS_RTR_HashKey_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	ACS_RTR_TRACE_MESSAGE("In, modify the HashKey object<%s>", objName);
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

			// extract the attributes
			string hashKeyVal;
			for(size_t idx = 0; attrMods[idx] != NULL ; ++idx)
			{
				ACS_APGCC_AttrValues modAttribute = attrMods[idx]->modAttr;

				// Check for TQ parameter change
				if( 0 == ACE_OS::strcmp(rtr_imm::PasswordAttribute, modAttribute.attrName) )
				{
					hashKeyVal.assign(reinterpret_cast<char *>(modAttribute.attrValues[0]));

					if(!validateHashKey(hashKeyVal))
					{
						result = ACS_CC_FAILURE;
					}
					break;

				}
			}

	ACS_RTR_TRACE_MESSAGE("Out");

	return result;
}

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType ACS_RTR_HashKey_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_RTR_TRACE_MESSAGE("In");
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);

	ACS_RTR_TRACE_MESSAGE("Out");
	return ACS_CC_SUCCESS;
}


/*===================================================================
   ROUTINE: abort
=================================================================== */
void ACS_RTR_HashKey_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_RTR_TRACE_MESSAGE("In");
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: apply
=================================================================== */
void ACS_RTR_HashKey_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_RTR_TRACE_MESSAGE("In");
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType ACS_RTR_HashKey_ObjectImpl::updateRuntime(const char *objName, const char **attrName)
{
	ACS_RTR_TRACE_MESSAGE("In");
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	// to avoid warning msg
	UNUSED(objName);
	UNUSED(attrName);

	ACS_RTR_TRACE_MESSAGE("Out");
	return result;
}

/*===================================================================
ROUTINE: adminOperationCallback
=================================================================== */
void ACS_RTR_HashKey_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType** paramList)
{
	ACS_RTR_TRACE_MESSAGE("In, File Job DN:<%s> action ID:<%d>", p_objName, static_cast<int>(operationId) );

	// to avoid warning msg
	UNUSED(paramList);
	UNUSED(invocation);
	UNUSED(oiHandle);

	ACS_RTR_TRACE_MESSAGE("OUT");
}

/*===================================================================
ROUTINE: validateHashKey 
=================================================================== */
bool ACS_RTR_HashKey_ObjectImpl::validateHashKey(const std::string& hashKeyVal)
{
	bool result = true;

	char* passwordDecrypt = NULL;
	SecCryptoStatus decryptResult = sec_crypto_decrypt_ecimpassword(&passwordDecrypt, hashKeyVal.c_str());
	if( SEC_CRYPTO_OK != decryptResult )
	{
		ACS_RTR_TRACE_MESSAGE("%s","Failed in decrypt hashKey value");
	}
	else
	{
		int length = strlen(passwordDecrypt);
		if(hashKeyLength != length)
		{
			result = false;
			setExitCode(UNREASVALUE, "Unreasonable Value");

		}
		for (int i=0;i<length;i++)
		{
			if (!isdigit(passwordDecrypt[i]) && (passwordDecrypt[i]<'A' || passwordDecrypt[i]>'F') &&
					(passwordDecrypt[i]<'a' || passwordDecrypt[i]>'f'))
			{
				result = false;
				setExitCode(UNREASVALUE, "Unreasonable Value");
			}
		}

	}

	if(NULL != passwordDecrypt )
		free(passwordDecrypt);

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );
	return result;
}


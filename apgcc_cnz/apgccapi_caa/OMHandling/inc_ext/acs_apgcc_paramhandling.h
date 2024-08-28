/*
 * acs_apgcc_paramhandling.h
 *
 *  Created on: Aug 12, 2011
 *      Author: xlucpet
 */

#ifndef ACS_APGCC_PARAMHANDLING_H_
#define ACS_APGCC_PARAMHANDLING_H_

#include "ACS_CC_Types.h"
#include <string.h>
#include <typeinfo>

using namespace std;

class acs_apgcc_paramhandling {
public:
	acs_apgcc_paramhandling();
	virtual ~acs_apgcc_paramhandling();

	int getInternalLastError();

	char* getInternalLastErrorText();

	template <size_t size>
	ACS_CC_ReturnType getParameter ( string m_objectName, string m_attributeName, char (&value)[size] ){

		SaVersionT immVersion = { 'A', 2, 1 };

		SaNameT objectNode;
		SaImmAttrValuesT_2 **attributes = 0;
		SaImmAttrValuesT_2 *attr = 0;
		SaImmAttrNameT attributeNames[2] = {const_cast<char *>(m_attributeName.c_str()), 0};

		SaAisErrorT error = saImmOmInitialize(&immHandle, NULL, &immVersion);

		if (error != SA_AIS_OK) {
			return ACS_CC_FAILURE;
		}

		strncpy((char*)objectNode.value ,m_objectName.c_str(),SA_MAX_NAME_LENGTH);
		objectNode.length = strlen((char *)objectNode.value);

		SaImmAccessorHandleT accessorHandle;

	    error = saImmOmAccessorInitialize(immHandle, &accessorHandle);
	    if (error != SA_AIS_OK) {

	    	errorCode = error;

			error = saImmOmFinalize(immHandle);
			if (error != SA_AIS_OK) {
				return ACS_CC_FAILURE;
			}

			return ACS_CC_FAILURE;
	    }

		error = saImmOmAccessorGet_2(accessorHandle, &objectNode, attributeNames, &attributes);
		if (error != SA_AIS_OK)	{

	    	errorCode = error;

			error = saImmOmAccessorFinalize(accessorHandle);
			if (error != SA_AIS_OK) {
				return ACS_CC_FAILURE;
			}

			error = saImmOmFinalize(immHandle);
			if (error != SA_AIS_OK) {
				return ACS_CC_FAILURE;
			}

			return ACS_CC_FAILURE;
		}

		attr = attributes[0];

	    switch (attr->attrValueType) {
		case SA_IMM_ATTR_SAINT32T:
		case SA_IMM_ATTR_SAINT64T:
		case SA_IMM_ATTR_SAUINT32T:
		case SA_IMM_ATTR_SAUINT64T:
		case SA_IMM_ATTR_SAFLOATT:
		case SA_IMM_ATTR_SADOUBLET:
		case SA_IMM_ATTR_SATIMET:{

			error = saImmOmAccessorFinalize(accessorHandle);
			if (error != SA_AIS_OK) {
				errorCode = error;
				return ACS_CC_FAILURE;
			}

			error = saImmOmFinalize(immHandle);

			if (error != SA_AIS_OK) {
				errorCode = error;
				return ACS_CC_FAILURE;
			}

			errorCode = 7; // Invalid Parameter
			return ACS_CC_FAILURE;

			break;
			}
		case SA_IMM_ATTR_SANAMET:{
			if ( attr->attrValuesNumber > 0 ){
				SaNameT* myNameT = reinterpret_cast<SaNameT *>((*attributes)->attrValues[0]);
				if ( myNameT->length <= size ){
					strncpy( value, (char*)myNameT->value, size);
				}
				else{
					error = saImmOmAccessorFinalize(accessorHandle);
					if (error != SA_AIS_OK) {
						errorCode = error;
						return ACS_CC_FAILURE;
					}

					error = saImmOmFinalize(immHandle);

					if (error != SA_AIS_OK) {
						errorCode = error;
						return ACS_CC_FAILURE;
					}

					errorCode = 7; // Invalid Parameter
					return ACS_CC_FAILURE;
				}
			}

			break;
			}
		case SA_IMM_ATTR_SASTRINGT:{
			if ( attr->attrValuesNumber > 0 ){
				SaStringT tmp = *reinterpret_cast<SaStringT *>((*attributes)->attrValues[0]);

				if ( strlen(tmp) <= size ){
					strncpy( value, tmp, size );
				}
				else {
					error = saImmOmAccessorFinalize(accessorHandle);
					if (error != SA_AIS_OK) {
						errorCode = error;
						return ACS_CC_FAILURE;
					}

					error = saImmOmFinalize(immHandle);

					if (error != SA_AIS_OK) {
						errorCode = error;
						return ACS_CC_FAILURE;
					}

					errorCode = 7; // Invalid Parameter
					return ACS_CC_FAILURE;

				}
			}
			break;
			}
		default:
	        break;
	    }

	    error = saImmOmAccessorFinalize(accessorHandle);
	    if (error != SA_AIS_OK) {

	    	errorCode = error;

			error = saImmOmFinalize(immHandle);
			if (error != SA_AIS_OK) {
				return ACS_CC_FAILURE;
			}

			return ACS_CC_FAILURE;
	    }

		error = saImmOmFinalize(immHandle);

		if (error != SA_AIS_OK) {
			errorCode = error;
			return ACS_CC_FAILURE;
		}

		return ACS_CC_SUCCESS;

	}

	template<typename value_t>
	ACS_CC_ReturnType getParameter ( string m_objectName, string m_attributeName, value_t* value ){

		SaVersionT immVersion = { 'A', 2, 1 };

		SaNameT objectNode;
		SaImmAttrValuesT_2 **attributes = 0;
		SaImmAttrValuesT_2 *attr = 0;
		SaImmAttrNameT attributeNames[2] = {const_cast<char *>(m_attributeName.c_str()), 0};

		SaAisErrorT error = saImmOmInitialize(&immHandle, NULL, &immVersion);

		if (error != SA_AIS_OK) {
			return ACS_CC_FAILURE;
		}

		strncpy((char*)objectNode.value ,m_objectName.c_str(),SA_MAX_NAME_LENGTH);
		objectNode.length = strlen((char *)objectNode.value);

		SaImmAccessorHandleT accessorHandle;

	    error = saImmOmAccessorInitialize(immHandle, &accessorHandle);
	    if (error != SA_AIS_OK) {

	    	errorCode = error;

			error = saImmOmFinalize(immHandle);
			if (error != SA_AIS_OK) {
				return ACS_CC_FAILURE;
			}

			return ACS_CC_FAILURE;
	    }

		error = saImmOmAccessorGet_2(accessorHandle, &objectNode, attributeNames, &attributes);
		if (error != SA_AIS_OK)	{

	    	errorCode = error;

			error = saImmOmAccessorFinalize(accessorHandle);
			if (error != SA_AIS_OK) {
				return ACS_CC_FAILURE;
			}

			error = saImmOmFinalize(immHandle);
			if (error != SA_AIS_OK) {
				return ACS_CC_FAILURE;
			}

			return ACS_CC_FAILURE;
		}

		attr = attributes[0];

	    switch (attr->attrValueType) {
		case SA_IMM_ATTR_SAINT32T:
			if ( typeid(value_t) != typeid(int) ){

				error = saImmOmAccessorFinalize(accessorHandle);
				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				error = saImmOmFinalize(immHandle);

				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				errorCode = 7; // Invalid Parameter

				return ACS_CC_FAILURE;
			}

				break;
		case SA_IMM_ATTR_SAUINT32T:
			if ( typeid(value_t) != typeid(unsigned int) ){

				error = saImmOmAccessorFinalize(accessorHandle);
				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				error = saImmOmFinalize(immHandle);

				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				errorCode = 7; // Invalid Parameter
				return ACS_CC_FAILURE;
			}

				break;
		case SA_IMM_ATTR_SAINT64T:
				if ( typeid(value_t) != typeid(long long) ){

					error = saImmOmAccessorFinalize(accessorHandle);
					if (error != SA_AIS_OK) {
						errorCode = error;
						return ACS_CC_FAILURE;
					}

					error = saImmOmFinalize(immHandle);

					if (error != SA_AIS_OK) {
						errorCode = error;
						return ACS_CC_FAILURE;
					}

					errorCode = 7; // Invalid Parameter
					return ACS_CC_FAILURE;
				}

				break;
		case SA_IMM_ATTR_SAUINT64T:
			if ( typeid(value_t) != typeid(unsigned long long) ){

				error = saImmOmAccessorFinalize(accessorHandle);
				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				error = saImmOmFinalize(immHandle);

				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				errorCode = 7; // Invalid Parameter
				return ACS_CC_FAILURE;
			}

				break;
		case SA_IMM_ATTR_SAFLOATT:
			if ( typeid(value_t) != typeid(float) ){

				error = saImmOmAccessorFinalize(accessorHandle);
				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				error = saImmOmFinalize(immHandle);

				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				errorCode = 7; // Invalid Parameter
				return ACS_CC_FAILURE;
			}

				break;
		case SA_IMM_ATTR_SADOUBLET:
			if ( typeid(value_t) != typeid(double) ){

				error = saImmOmAccessorFinalize(accessorHandle);
				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				error = saImmOmFinalize(immHandle);

				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				errorCode = 7; // Invalid Parameter
				return ACS_CC_FAILURE;
			}

				break;
		default:{
				error = saImmOmAccessorFinalize(accessorHandle);
				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				error = saImmOmFinalize(immHandle);

				if (error != SA_AIS_OK) {
					errorCode = error;
					return ACS_CC_FAILURE;
				}

				errorCode = 7; // Invalid Parameter
				return ACS_CC_FAILURE;
			}
	    }

	    if ( attr->attrValuesNumber > 0 ){
	    	value_t tmp = 0;
	    	tmp = *reinterpret_cast<value_t *>((*attributes)->attrValues[0]);
			*value = tmp;
	    }

	    error = saImmOmAccessorFinalize(accessorHandle);
	    if (error != SA_AIS_OK) {

	    	errorCode = error;

			error = saImmOmFinalize(immHandle);
			if (error != SA_AIS_OK) {
				return ACS_CC_FAILURE;
			}

			return ACS_CC_FAILURE;
	    }

		error = saImmOmFinalize(immHandle);

		if (error != SA_AIS_OK) {
			errorCode = error;
			return ACS_CC_FAILURE;
		}

		return ACS_CC_SUCCESS;
	}

private:

	SaImmHandleT immHandle;

	void setInternalError ( int p_errorCode );
	int errorCode;

};

#endif /* ACS_APGCC_PARAMHANDLING_H_ */

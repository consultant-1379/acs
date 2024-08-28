/*
 * ACS_TRAPDS_IMM_TRAPDS_Util.cpp
 *
 *  Created on: Jan 23, 2012
 *      Author: eanform
 */

#include "ACS_TRAPDS_Imm_Util.h"

bool IMM_TRAPDS_Util::getImmAttributeString (std::string object, std::string attribute, std::string &value)
{
	bool res = true;
	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
		res = false;
	}
	else
	{
		ACS_CC_ImmParameter Param;
		char *name_attrPath = const_cast<char*>(attribute.c_str());
		Param.attrName = name_attrPath;
		result = omHandler.getAttribute(object.c_str(), &Param );
		if ( result != ACS_CC_SUCCESS ){	cout << "ERROR: Param " << attribute.c_str()<<" FAILURE!!!\n"; res = false; }
		else value = (char*)Param.attrValues[0];

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
			res = false;
		}
	}

	return res;
}


bool IMM_TRAPDS_Util::getImmAttributeInt(std::string object, std::string attribute, int &value)
{
 	bool res = true;
 	ACS_CC_ReturnType result;
 	OmHandler omHandler;

 	result = omHandler.Init();
 	if (result != ACS_CC_SUCCESS)
 	{
 		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
 		res = false;
 	}
 	else
 	{
 		ACS_CC_ImmParameter Param;
 		//Param.ACS_APGCC_IMMFreeMemory(1);
 		char *name_attrPath = const_cast<char*>(attribute.c_str());
 		Param.attrName = name_attrPath;
 		result = omHandler.getAttribute(object.c_str(), &Param );
 		if ( result != ACS_CC_SUCCESS ){	cout << "ERROR: Param " << attribute.c_str()<<" FAILURE!!!\n"; res = false; }
 		else value = (*(int*)Param.attrValues[0]);

 		result = omHandler.Finalize();
 		if (result != ACS_CC_SUCCESS)
 		{
 			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
 			res = false;
 		}
 	}

 	return res;
}

bool IMM_TRAPDS_Util::getChildrenObject(std::string dn, std::vector<std::string> & list )
{
	ACS_CC_ReturnType result;
	bool res = true;
	OmHandler omHandler;

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
		res = false;
	}
	else
	{

		result = omHandler.getChildren(dn.c_str(),ACS_APGCC_SUBLEVEL, &list);
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error getChildren" << std::endl;
			res = false;
		}

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Finalize()" << std::endl;
			res = false;
		}
	}

	return res;
}

bool IMM_TRAPDS_Util::getClassObjectsList(std::string className, std::vector<std::string> & list )
{
	ACS_CC_ReturnType result;
	bool res = true;
	OmHandler omHandler;

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
		res = false;
	}
	else
	{

		result = omHandler.getClassInstances(className.c_str(),list);
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error getClassIstance" << std::endl;
			res = false;
		}

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Finalize()" << std::endl;
			res = false;
		}
	}

	return res;
}

bool IMM_TRAPDS_Util::deleteImmObject(std::string dn)
{
	ACS_CC_ReturnType result;
	bool res = true;
	OmHandler omHandler;

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
		res = false;
	}
	else
	{

		result = omHandler.deleteObject(dn.c_str());
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error deleteObject" << std::endl;
			res = false;
		}

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Finalize()" << std::endl;
			res = false;
		}
	}

	return res;
}

bool IMM_TRAPDS_Util::getObject(std::string dn, ACS_APGCC_ImmObject *object)
{
	ACS_CC_ReturnType result;
	bool res = true;
	OmHandler omHandler;
	char *nameObject =  const_cast<char*>(dn.c_str());
	object->objName = nameObject;


	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
		res = false;
	}
	else
	{
		result = omHandler.getObject(object);
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error getObject()" << std::endl;
			res = false;
		}

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Finalize()" << std::endl;
			res = false;
		}
	}

	return res;
}

bool IMM_TRAPDS_Util::getDnParent (std::string object_dn, std::string &parent)
{
	bool result = false;
	if (!object_dn.empty())
	{
		parent = ACS_APGCC::after(object_dn,",");
		result = true;
	}
	return result;
}

bool IMM_TRAPDS_Util::getRdnAttribute (std::string object_dn, std::string &rdn)
{
	bool result = false;
	if (!object_dn.empty())
	{
		rdn = ACS_APGCC::before(object_dn,"=");
		result = true;
	}
	return result;
}

bool IMM_TRAPDS_Util::getRdnObject (std::string object_dn, std::string &rdn)
{
	bool result = false;
	if (!object_dn.empty())
	{
		rdn = ACS_APGCC::before(object_dn,",");
		result = true;
	}
	return result;
}

ACS_CC_ValuesDefinitionType IMM_TRAPDS_Util::defineAttributeString(const char* attributeName, ACS_CC_AttrValueType type, const char *value, unsigned int numValue)
{
	ACS_CC_ValuesDefinitionType attribute;
	char* stringValue = const_cast<char *>(value);
	attribute.attrName = const_cast<char*>(attributeName);
	attribute.attrType = type;
	attribute.attrValuesNum = numValue;

	if (attribute.attrValuesNum == 0)	attribute.attrValues = 0;
	else
	{
		attribute.attrValues=new void*[attribute.attrValuesNum];
		attribute.attrValues[0] =reinterpret_cast<void*>(stringValue);
	}

	return attribute;
}

ACS_CC_ValuesDefinitionType IMM_TRAPDS_Util::defineAttributeInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue)
{
	ACS_CC_ValuesDefinitionType attribute;

	attribute.attrName = const_cast<char*>(attributeName);
	attribute.attrType = type;
	attribute.attrValuesNum = numValue;

	if (attribute.attrValuesNum == 0)	attribute.attrValues = 0;
	else
	{
		attribute.attrValues=new void*[attribute.attrValuesNum];
		attribute.attrValues[0] =reinterpret_cast<void*>(value);
	}


	return attribute;
}

bool IMM_TRAPDS_Util::modify_OM_ImmAttr(const char *object, ACS_CC_ImmParameter parameter)
{
	ACS_CC_ReturnType result;
	bool res = true;
	OmHandler omHandler;

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
		res = false;
	}
	else
	{
		result = omHandler.modifyAttribute(object,&parameter);
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error getObject()" << std::endl;
			res = false;
		}

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Finalize()" << std::endl;
			res = false;
		}
	}

	return res;
}

ACS_CC_ImmParameter IMM_TRAPDS_Util::defineParameterInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue)
{
	ACS_CC_ImmParameter parToModify;

	char *name_attrUnsInt32 = const_cast<char*>(attributeName);
	parToModify.attrName = name_attrUnsInt32;
	parToModify.attrType = type;
	parToModify.attrValuesNum = numValue;

	if (parToModify.attrValuesNum == 0)	parToModify.attrValues = 0;
	else
	{
		parToModify.attrValues=new void*[parToModify.attrValuesNum];
		parToModify.attrValues[0] =reinterpret_cast<void*>(value);
	}

return parToModify;

}

ACS_CC_ImmParameter IMM_TRAPDS_Util::defineParameterString(const char* attributeName, ACS_CC_AttrValueType type, const char* value, unsigned int numValue)
{
	ACS_CC_ImmParameter parToModify;

	char* stringValue = const_cast<char *>(value);
	char *name_attrString = const_cast<char*>(attributeName);
	parToModify.attrName = name_attrString;
	parToModify.attrType = type;
	parToModify.attrValuesNum = numValue;

	if (parToModify.attrValuesNum == 0)	parToModify.attrValues = 0;
	else
	{
		parToModify.attrValues=new void*[parToModify.attrValuesNum];
		parToModify.attrValues[0] =reinterpret_cast<void*>(stringValue);
	}


return parToModify;

}

void IMM_TRAPDS_Util::printDebugObject (std::string dn)
{
	ACS_APGCC_ImmObject paramList;

	std::cout<<"\n--------------------------------------------------------------------------------"<< std::endl;
	std::cout<<"    OBJECT : " << dn.c_str() << std::endl;
	std::cout<<"--------------------------------------------------------------------------------"<< std::endl;

	//get Obj
	getObject(dn,&paramList);


	for (unsigned i=0; i<paramList.pointers.size(); i++)
	{
		ACS_APGCC_ImmAttribute att;
		att = paramList.attributes[i];


		switch ( att.attrType )
		{
			case ATTR_INT32T:
				cout << att.attrName.c_str() << "  =  " <<*reinterpret_cast<int *>(att.attrValues[0]) << endl;
				break;
			case ATTR_UINT32T:
				cout << att.attrName.c_str() << "  =  " <<*reinterpret_cast<unsigned int *>(att.attrValues[0]) << endl;
				break;
			case ATTR_INT64T:
				cout << att.attrName.c_str() << "  =  " <<*reinterpret_cast<long long *>(att.attrValues[0]) << endl;
				break;
			case ATTR_UINT64T:
				cout << att.attrName.c_str() << "  =  " <<*reinterpret_cast<unsigned long long *>(att.attrValues[0]) << endl;
				break;
			case ATTR_FLOATT:
				cout << att.attrName.c_str() << "  =  " <<*reinterpret_cast<float *>(att.attrValues[0]) << endl;
				break;
			case ATTR_DOUBLET:
				cout << att.attrName.c_str() << "  =  " <<*reinterpret_cast<double *>(att.attrValues[0]) << endl;
				break;
			case ATTR_NAMET:
				cout << att.attrName.c_str() << "  =  " << reinterpret_cast<char *>(att.attrValues[0]) << endl;
				break;
			case ATTR_STRINGT:
				cout << att.attrName.c_str() << "  =  " << reinterpret_cast<char *>(att.attrValues[0]) << endl;
				break;
			default:
				break;

		}//switch
	}//for

	std::cout << std::endl;
}

void IMM_TRAPDS_Util::printDebugAllObjectClass (std::string className)
{
	std::vector<std::string> list;

	std::cout<<"\n-----------------------------------------------------------------------------------"<< std::endl;
	std::cout<<"    CLASS NAME : " << className.c_str() << std::endl;

	//get objects
	getClassObjectsList(className,list);

	std::cout<<"    # of instances : " << list.size() << std::endl;
	std::cout<<"----------------------------------------------------------------------------------"<< std::endl;

	//loop of istance
	for (unsigned i=0; i<list.size(); i++)
	{
		printDebugObject(list[i]);
	}


}

bool IMM_TRAPDS_Util::createClassTrapSubscriber()
{
	bool res = true;

	ACS_CC_ReturnType result;
	OmHandler immHandler;

//	/* Name of the class to be defined*/
	char *nameClassToBeDefined = ((char*)classTrapSubscriber);//const_cast<char*>("Test_config");

	//	char nameClassToBeDefined[30] = "Test_config";

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;

	ACS_CC_AttrDefinitionType attributeRDN = {const_cast<char *>(ATT_TRAPDS_KEY),ATTR_NAMET,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType attributeI32 = {const_cast<char *>(ATT_TRAPDS_PORT),ATTR_INT32T,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {const_cast<char *>(ATT_TRAPDS_NAME),ATTR_STRINGT,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE,0};

	classAttributes.push_back(attributeRDN);
	classAttributes.push_back(attributeI32);
	classAttributes.push_back(attributeSTRING);

	result=immHandler.Init();
	if(result == ACS_CC_SUCCESS)
	{
		int id = -1;
		char* text;

		result=immHandler.defineClass( nameClassToBeDefined,categoryClassToBeDefined,classAttributes );
		id = immHandler.getInternalLastError();
		text = immHandler.getInternalLastErrorText();

		if(result  == ACS_CC_SUCCESS)
		{
			cout<<"\n######################################################" << endl;
			cout<<"   	CLASS "<<classTrapSubscriber<<" DEFINED !!! 	   " << endl;
			cout<<"######################################################\n" << endl;
		}
		else
		{
			if (id == -14) //SA_AIS_ERR_EXIST
			{
				cout<<"\n######################################################" << endl;
				cout<<"   	CLASS "<<classTrapSubscriber<<" ...ALREADY.. DEFINED !!!" << endl;
				cout<<"######################################################\n" << endl;
			}
			else
			{
				cout<<"\n######################################################" << endl;
				cout<<"   	CLASS "<<classTrapSubscriber<<" ...NOT... DEFINED !!!" << endl;
				cout<<"   	ErrorCode: "<< id <<" TEXT: "<< text  << endl;
				cout<<"######################################################\n" << endl;
				res = false;
			}
		}

		result = immHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "...Error Finalize()" << std::endl;
			res = false;
		}

	}
	else
	{
		cout << "...ERROR: init() FAILURE !!!\n "<< endl;
		res = false;
	}

	return res;
}

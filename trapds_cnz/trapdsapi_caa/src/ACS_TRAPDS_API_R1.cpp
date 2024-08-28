/*
 * ACS_TRAPDS_API_R1.cpp
 *
 *  Created on: 18/gen/2012
 *      Author: renato
 */

#include "ACS_TRAPDS_API_R1.h"
//#include <boost/archive/archive_exception.hpp>
namespace IMM_Util_Api_TRAPD
{

	//CLASS NAME
	static const char classTrapSubscriber[] = "TrapSubscriber";
	static const char classTrapDispatcherService[] = "TrapDispatcherService";

	//CLASS IMPLEMENTERS
	static const char IMPLEMENTER_TRAPSUBSCRIBER[] = "ACS_TRAPDS";

	//TrapDispatcherService
	static const char ATT_TRAPDS_PARENT_RDN[] = "trapDispatcherServiceId=1";
	static const char ATT_TRAPDS_PARENT_STATUS[] = "serviceState";

	//TrapSubscriber CLASS
	static const char ATT_TRAPDS_KEY[] = "trapSubscriberId";
	static const char ATT_TRAPDS_NAME[] = "name";
	static const char ATT_TRAPDS_PORT[] = "port";
	static const char ATT_TRAPDS_RDN[] = "trapSubscriberId=";

	//METHODS
	bool getImmAttributeString (std::string object, std::string attribute, std::string &value);
	bool getImmAttributeInt(std::string object, std::string attribute, int &value);
	bool getChildrenObject(std::string dn, std::vector<std::string> & list );
	bool getClassObjectsList(std::string className, std::vector<std::string> & list );
	bool deleteImmObject(std::string dn);
	bool getObject(std::string dn, ACS_APGCC_ImmObject *object);
	void printDebugAllObjectClass (std::string className);
	void printDebugObject (std::string dn);
	bool getDnParent (std::string object_dn, std::string &parent);
	bool getRdnObject (std::string object_dn, std::string &rdn);
	bool getRdnAttribute (std::string object_dn, std::string &rdn);
	ACS_CC_ValuesDefinitionType defineAttributeString(const char* attributeName, ACS_CC_AttrValueType type, const char *value, unsigned int numValue);
	ACS_CC_ValuesDefinitionType defineAttributeInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue);

	ACS_CC_ImmParameter defineParameterInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue);
	ACS_CC_ImmParameter defineParameterString(const char* attributeName, ACS_CC_AttrValueType type, const char* value, unsigned int numValue);
	bool modify_OM_ImmAttr(const char *object, ACS_CC_ImmParameter parameter);
	bool createClassTrapSubscriber();
};

ACS_TRAPDS_API_R1::ACS_TRAPDS_API_R1()
	:bExit(true),remote_addr_(NULL),local_addr_(NULL),local_(NULL)
{
	remote_addr_=new ACE_INET_Addr();
	local_addr_=new ACE_INET_Addr();
}


ACS_TRAPDS_API_R1::ACS_TRAPDS_API_R1(int l)
	:bExit(true),local_port(l)
{
	remote_addr_=new ACE_INET_Addr();
	local_addr_=new ACE_INET_Addr(l,"127.0.0.1");
	local_=new ACE_SOCK_Dgram(*local_addr_);
}

ACS_TRAPDS_API_R1::~ACS_TRAPDS_API_R1()
{
	// TODO Auto-generated destructor stub
//	close();

	if (remote_addr_) {
		delete(remote_addr_);
		remote_addr_ = NULL;
	}

	if(local_addr_) {
		delete(local_addr_);
		local_addr_ = NULL;
	}

	if(local_) {
		delete(local_);
		local_ = NULL;
	}
}


int ACS_TRAPDS_API_R1::svc(void)
{
	data_buf=(char*)malloc(sizeof(char)*SIZE_DATA);

	//remote_addr_.set(6622,std::string("192.168.169.1").c_str());
	ACE_Time_Value timeout (1);

	while(!bExit)
	{
		memset(data_buf,0,SIZE_DATA);

		int byte_count=0;

		//byte_count=local_->recv(data_buf,SIZE_DATA,*remote_addr_);
		byte_count=local_->recv(data_buf,SIZE_DATA,*remote_addr_,0,&timeout);

		if(byte_count!=-1 && !bExit)
		{
			data_buf[byte_count]=0;

			std::string out(data_buf,byte_count);	// TR HW34181 - specify size in string constructor as data_buf may contain null chars

			setQueue(out);

			/* ACS_TRAPDS_StructVariable v2(0,"");
			 try {
				 std::stringstream dataStringStream(std::stringstream::in | std::stringstream::out);
				 dataStringStream<<out;
				 boost::archive::text_iarchive ia(dataStringStream);

				 ia >> v2;
				 handleTrap(v2);
			 }
			catch (boost::archive::archive_exception& e)
			{
				std::cout << "Boost Serialization Exception Occurred " << e.what() << std::endl;
			}
			catch (boost::exception& )
			{
				std::cout << "Boost Exception Occurred!" << std::endl;
			}*/
		}

	}
	free(data_buf);
	return 0;
}

int ACS_TRAPDS_API_R1::open ()
{
	bExit= false;
	activate();
	activateThread();
	return 0;
}

int ACS_TRAPDS_API_R1::close ()
{
	bExit= true;
//	char b = 'a';
//	char * data_buf = &b;
//	int data_size = 1;
//	local_->send(data_buf, data_size, *local_addr_);
	sleep(1);
	if (local_)
		local_->close();
	return 0;
}

ACS_TRAPDS::ACS_TRAPDS_API_Result ACS_TRAPDS_API_R1::subscribe(std::string serviceName )
{
	ACS_TRAPDS::ACS_TRAPDS_API_Result result = ACS_TRAPDS::Result_Failure;

	int res = 0;
	int state = 0;

	IMM_Util_Api_TRAPD::getImmAttributeInt(IMM_Util_Api_TRAPD::ATT_TRAPDS_PARENT_RDN,IMM_Util_Api_TRAPD::ATT_TRAPDS_PARENT_STATUS,state);
	if (state!=1) return ACS_TRAPDS::Result_Failure;

	//check object into imm
	res = checkSubscriberObject(serviceName);

	if (res == 1)
	{
		//assign new port
		res = assignLocalPort();

		if (res == ACS_TRAPDS::Result_Success)
		{
			res = checkSubscriberPort(local_port);

			//create object into imm
			res = createSubscriberObject(serviceName);
		}
		else
		{
			return ACS_TRAPDS::Result_NoFreePort;
		}
	}

	if (res == ACS_TRAPDS::Result_Success)
	{
		//activate thread
		this->open();
		result = ACS_TRAPDS::Result_Success;
	}

	return result;
}


int ACS_TRAPDS_API_R1::checkLocalPort(int l)
{
	int res = 0;

//	remote_addr_=new ACE_INET_Addr();
//	local_addr_=new ACE_INET_Addr();

	res = local_addr_->set(l,"127.0.0.1");

	if (res == 0)
	{
		local_=new ACE_SOCK_Dgram();

		res = local_->open(*local_addr_);

		if (res != 0 )
		{
		 	delete(local_);
		 	local_=NULL;

			res = 1;
		}
	}

	return res;
}

int ACS_TRAPDS_API_R1::assignLocalPort()
{
	int res = 0;

	bool set = false;
	int lport = MIN_PORT;

	while ((lport<= MAX_PORT) && (!set))
	{
		if (checkLocalPort(lport) == 0)
		{
			set = true;
			setLocalPort(lport);
		}
		else lport++;
	}

	if (!set) res = ACS_TRAPDS::Result_NoFreePort;

	return res;
}

void ACS_TRAPDS_API_R1::setLocalPort(int lp)
{
	local_port=lp;
}

std::string ACS_TRAPDS_API_R1::intToString (int ivalue)
{
	std::stringstream ss_value("");
	ss_value << ivalue; //try to convert operand to value number
	return ss_value.str();
}
int ACS_TRAPDS_API_R1::checkSubscriberObject(std::string serviceName)
{
	int res = 0;
	bool found = false;

	std::vector<std::string> subscribList;
	if (!IMM_Util_Api_TRAPD::getClassObjectsList(IMM_Util_Api_TRAPD::classTrapSubscriber,subscribList)) return 1;
	else
	{
		for (unsigned i=0; i<subscribList.size();i++)
		{
			std::string dn_subcribe("");
			dn_subcribe = subscribList[i].c_str();

			std::string name("");
			IMM_Util_Api_TRAPD::getImmAttributeString(dn_subcribe,IMM_Util_Api_TRAPD::ATT_TRAPDS_NAME,name);

			if (strcmp(serviceName.c_str(),name.c_str()) == 0)
			{
				found = true;
				int port = 0;
				IMM_Util_Api_TRAPD::getImmAttributeInt(dn_subcribe,IMM_Util_Api_TRAPD::ATT_TRAPDS_PORT,port);

				//bind port
				if (checkLocalPort(port) == 0)
				{
					//set local port
					setLocalPort(port);
				}
				else
				{
					res = assignLocalPort();
					if (res == ACS_TRAPDS::Result_Success)
					{
						checkSubscriberPort(local_port);
						//create object into imm
						res = createSubscriberObject(serviceName);
					}
				}


				break;
			}
		}
		//if not found it must be create
		if (!found) return 1;
	}

	return res;
}

int ACS_TRAPDS_API_R1::createSubscriberObject(std::string serviceName )
{
	int result = 0;

	ACS_CC_ReturnType res = ACS_CC_SUCCESS;
	OmHandler immHandler;

	//The vector of attributes
	vector<ACS_CC_ValuesDefinitionType> AttrList;

	std::string lport = intToString(local_port);
	std::string rdn_sub = string(IMM_Util_Api_TRAPD::ATT_TRAPDS_RDN).append(lport);

	//the attributes of PFM class
	ACS_CC_ValuesDefinitionType TrapRDN = IMM_Util_Api_TRAPD::defineAttributeString(IMM_Util_Api_TRAPD::ATT_TRAPDS_KEY,ATTR_STRINGT,rdn_sub.c_str(),1);
	ACS_CC_ValuesDefinitionType TrapPort = IMM_Util_Api_TRAPD::defineAttributeInt(IMM_Util_Api_TRAPD::ATT_TRAPDS_PORT,ATTR_INT32T,&local_port,1);
	ACS_CC_ValuesDefinitionType TrapName = IMM_Util_Api_TRAPD::defineAttributeString(IMM_Util_Api_TRAPD::ATT_TRAPDS_NAME,ATTR_STRINGT,serviceName.c_str(),1);

	char* nameClasse = const_cast<char*>(IMM_Util_Api_TRAPD::classTrapSubscriber);
	char* parentName = const_cast<char*>(IMM_Util_Api_TRAPD::ATT_TRAPDS_PARENT_RDN);

	AttrList.push_back(TrapRDN);
	AttrList.push_back(TrapPort);
	AttrList.push_back(TrapName);

	res = immHandler.Init();
	if (res == ACS_CC_SUCCESS)
	{
		if (immHandler.createObject(nameClasse,parentName,AttrList) != ACS_CC_SUCCESS)
		{
			result = 1;
			int id = -1;
			std::string text("");
			id = immHandler.getInternalLastError();
			text = immHandler.getInternalLastErrorText();

			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "CREATION object FAILED" << std::endl;
			std::cout << "DBG: name Service: " << serviceName.c_str() << " port: " << local_port << std::endl;
			std::cout << "DBG: text: " << text.c_str() << " errorCode: " << id << std::endl;

		}
		res = immHandler.Finalize();
	}
	else
	{
		std::cout << "DBG: " << "ERROR : Init() ...CREATION object FAILED" << std::endl;
		result = 1;
	}


	//free memory
	delete[] TrapRDN.attrValues;
	TrapRDN.attrValues = NULL;
	delete[] TrapPort.attrValues;
	TrapPort.attrValues = NULL;
	delete[] TrapName.attrValues;
	TrapName.attrValues = NULL;


	return result;

}

int ACS_TRAPDS_API_R1::checkSubscriberPort(int lport )
{
	int res = 0;
	bool found = false;

	std::vector<std::string> subscribList;
	if (!IMM_Util_Api_TRAPD::getClassObjectsList(IMM_Util_Api_TRAPD::classTrapSubscriber,subscribList)) return res;
	else
	{
		for (unsigned i=0; i<subscribList.size();i++)
		{
			std::string dn_subcribe("");
			dn_subcribe = subscribList[i].c_str();

			int port = 0;
			IMM_Util_Api_TRAPD::getImmAttributeInt(dn_subcribe,IMM_Util_Api_TRAPD::ATT_TRAPDS_PORT,port);

			if (port == lport)
			{
				found = true;
				IMM_Util_Api_TRAPD::deleteImmObject(dn_subcribe);
				break;
			}
		}
	}

	return res;
}


bool IMM_Util_Api_TRAPD::getImmAttributeString (std::string object, std::string attribute, std::string &value)
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
		if ( result != ACS_CC_SUCCESS )
		{
			cout << "ERROR: Param " << attribute.c_str()<<" FAILURE!!!\n";
			res = false;
		}
		else if (Param.attrValuesNum > 0)
		{
			value = (char*)Param.attrValues[0];
		}
		else res = false;


		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
			res = false;
		}
	}

	return res;
}


bool IMM_Util_Api_TRAPD::getImmAttributeInt(std::string object, std::string attribute, int &value)
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
 		if ( result != ACS_CC_SUCCESS )
 		{
 			cout << "ERROR: Param " << attribute.c_str()<<" FAILURE!!!\n";
 			res = false;
 		}
 		else if (Param.attrValuesNum > 0)
 		{
 			value = (*(int*)Param.attrValues[0]);
 		}
 		else res = false;

 		result = omHandler.Finalize();
 		if (result != ACS_CC_SUCCESS)
 		{
 			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
 			res = false;
 		}
 	}

 	return res;
}

bool IMM_Util_Api_TRAPD::getChildrenObject(std::string dn, std::vector<std::string> & list )
{
	ACS_CC_ReturnType result;
	bool res = true;
	OmHandler omHandler;

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		res = false;
	}
	else
	{

		result = omHandler.getChildren(dn.c_str(),ACS_APGCC_SUBLEVEL, &list);
		if (result != ACS_CC_SUCCESS)
		{
			res = false;
		}

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			res = false;
		}
	}

	return res;
}

bool IMM_Util_Api_TRAPD::getClassObjectsList(std::string className, std::vector<std::string> & list )
{
	ACS_CC_ReturnType result;
	bool res = true;
	OmHandler omHandler;

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		res = false;
	}
	else
	{

		result = omHandler.getClassInstances(className.c_str(),list);
		if (result != ACS_CC_SUCCESS)
		{
			res = false;
		}

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			res = false;
		}
	}

	return res;
}

bool IMM_Util_Api_TRAPD::deleteImmObject(std::string dn)
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

bool IMM_Util_Api_TRAPD::getObject(std::string dn, ACS_APGCC_ImmObject *object)
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

bool IMM_Util_Api_TRAPD::getDnParent (std::string object_dn, std::string &parent)
{
	bool result = false;
	if (!object_dn.empty())
	{
		parent = ACS_APGCC::after(object_dn,",");
		result = true;
	}
	return result;
}

bool IMM_Util_Api_TRAPD::getRdnAttribute (std::string object_dn, std::string &rdn)
{
	bool result = false;
	if (!object_dn.empty())
	{
		rdn = ACS_APGCC::before(object_dn,"=");
		result = true;
	}
	return result;
}

bool IMM_Util_Api_TRAPD::getRdnObject (std::string object_dn, std::string &rdn)
{
	bool result = false;
	if (!object_dn.empty())
	{
		rdn = ACS_APGCC::before(object_dn,",");
		result = true;
	}
	return result;
}

ACS_CC_ValuesDefinitionType IMM_Util_Api_TRAPD::defineAttributeString(const char* attributeName, ACS_CC_AttrValueType type, const char *value, unsigned int numValue)
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

ACS_CC_ValuesDefinitionType IMM_Util_Api_TRAPD::defineAttributeInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue)
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

bool IMM_Util_Api_TRAPD::modify_OM_ImmAttr(const char *object, ACS_CC_ImmParameter parameter)
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

ACS_CC_ImmParameter IMM_Util_Api_TRAPD::defineParameterInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue)
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

ACS_CC_ImmParameter IMM_Util_Api_TRAPD::defineParameterString(const char* attributeName, ACS_CC_AttrValueType type, const char* value, unsigned int numValue)
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

void IMM_Util_Api_TRAPD::printDebugObject (std::string dn)
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

void IMM_Util_Api_TRAPD::printDebugAllObjectClass (std::string className)
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

bool IMM_Util_Api_TRAPD::createClassTrapSubscriber()
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
void ACS_TRAPDS_API_R1::setQueue(std::string strBuff)
{
	theLoadMgrMutex.acquire();
	messageQueue.push_back(strBuff);
	theLoadMgrMutex.release();
}
ACE_THR_FUNC_RETURN svc_thr(void *ptr){
	ACS_TRAPDS_API_R1 *loadObj = (ACS_TRAPDS_API_R1*) ptr;
	loadObj->svc_load();
        return 0;
}
int ACS_TRAPDS_API_R1::activateThread()
{
	const ACE_TCHAR* thread_name = "LoadManager";
	ACE_HANDLE threadHandle = ACE_Thread_Manager::instance()->spawn(&svc_thr,
																   (void *)this ,
																    THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
																    &thread_id,
																    0,
																    ACE_DEFAULT_THREAD_PRIORITY,                                                                        -1,
																    0,
																    ACE_DEFAULT_THREAD_STACKSIZE,
																    &thread_name);
	if (threadHandle == -1){
		return -1;
	}

	/* check if the thread is spawned */
	ACE_UINT32 tState;
	ACE_Thread_Manager::instance()->thr_state(thread_id, tState);

	if (tState == THR_RUNNING || tState == THR_SPAWNED) {
		syslog(LOG_INFO, "TRAPDS Load Balancer Thread state [%u]", tState);
		return 0;
	}
	else
		syslog(LOG_INFO, "TRAPDS Load Balancer Thread state [%u]", tState);

	return 0;
}
int ACS_TRAPDS_API_R1::svc_load(){
	bool sz = false;
	while(!bExit)
	{
		theLoadMgrMutex.acquire();
		sz = messageQueue.empty();
		theLoadMgrMutex.release();

		if(!sz){
			theLoadMgrMutex.acquire();
			std::string outBuff(messageQueue.front());
			theLoadMgrMutex.release();

			try {
				ACS_TRAPDS_StructVariable v2(0,"");
				std::stringstream dataStringStream(std::stringstream::in | std::stringstream::out);
				dataStringStream<<outBuff;
				boost::archive::text_iarchive ia(dataStringStream);

				ia >> v2;
				handleTrap(v2);
				theLoadMgrMutex.acquire();
				messageQueue.pop_front();
				theLoadMgrMutex.release();
			}
			catch (boost::archive::archive_exception& e)
			{
				std::cout << "Boost Serialization Exception Occurred " << e.what() << std::endl;
			}
			catch (boost::exception& )
			{
				std::cout << "Boost Exception Occurred!" << std::endl;
			}
		}

		while ( sz && !bExit) {
			usleep(500000);
			theLoadMgrMutex.acquire();
			sz = messageQueue.empty();
			theLoadMgrMutex.release();
		}
	}
return 0;
}

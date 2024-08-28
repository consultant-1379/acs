#ifndef ACS_APBM_Hwi_Xml_Maker_H_
#define ACS_APBM_Hwi_Xml_Maker_H_

//#include <xercesc/util/PlatformUtils.hpp>
//#include <xercesc/util/Platforms/Linux/LinuxDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
//#include "ACS_APBM_HwiData.h"
//#include "ACS_APBM_Parameter.h"
#include "acs_apbm_hwidata.h"


#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
//#include "ACS_APBM_HwiXStr.h"
#include <acs_apbm_hwixstr.h>

//#include <stdio.h>
//#include <stdlib.h>

//XERCES_CPP_NAMsssESPACE_USE

#include <string>
#include <sstream>
#include <iostream>


//no typedef map<int,BoardDataType> boardDataMapType;


class ACS_APBM_Hwi_Xml_Maker {

public:

	ACS_APBM_Hwi_Xml_Maker();
	virtual ~ACS_APBM_Hwi_Xml_Maker();

	int insertMagazine(ACS_APBM_HWIData* hwidata, std::string magazine);

//	int getXml(std::string &output);
	std::string getStringXmlDocument();

	std::map<std::string,ACS_APBM_HWIData*> cab;

private:

	//XERCES_CPP_NAMESPACE::DOMElement**	getBoardData(ACS_APBM_HWIData &hwi,std::string magazine,int &len);
	XERCES_CPP_NAMESPACE::DOMElement**	getBoardData(ACS_APBM_HWIData* hwi,std::string magazine,int &len);

	//XERCES_CPP_NAMESPACE::DOMElement* 	getPfmDataLower(ACS_APBM_HWIData &hwi);
	XERCES_CPP_NAMESPACE::DOMElement* 	getPfmDataLower(ACS_APBM_HWIData* hwi);

	//XERCES_CPP_NAMESPACE::DOMElement* 	getPfmDataUpper(ACS_APBM_HWIData &hwi);
	XERCES_CPP_NAMESPACE::DOMElement* 	getPfmDataUpper(ACS_APBM_HWIData* hwi);

	//XERCES_CPP_NAMESPACE::DOMElement* 	getBackPlane(ACS_APBM_HWIData &hwi);
	XERCES_CPP_NAMESPACE::DOMElement* 	getBackPlane(ACS_APBM_HWIData* hwi);

	//XERCES_CPP_NAMESPACE::DOMElement* 	getSubRack(ACS_APBM_HWIData &hwi,std::string magazine);
	XERCES_CPP_NAMESPACE::DOMElement* 	getSubRack(ACS_APBM_HWIData* hwi,std::string magazine);

	std::string DoOutput2Stream(XERCES_CPP_NAMESPACE::DOMDocument* pmyDOMDocument);
	char *			getDate();
//XERCES_CPP_NAMESPACE::
	XERCES_CPP_NAMESPACE::DOMDocument 		* pDOMDocument;
	XERCES_CPP_NAMESPACE::DOMImplementation 	* pDOMImplementation;
	//original std::map<std::string,ACS_APBM_HWIData> cab;


};

#endif /* XMLHWIMAKER_H_ */

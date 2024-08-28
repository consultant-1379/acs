//#include "ACS_APBM_Hwi_Xml_Maker.h"
#include <acs_apbm_hwixmlmaker.h>
#include <stdio.h>
#include<stdlib.h>
#include <time.h>
#include <string>
//#include "AES_GCC_Log.h"
//#include "ACS_CS_API.h"

//GCC_TDEF(ACS_APBM_HWI_XML);



using namespace std;

ACS_APBM_Hwi_Xml_Maker::ACS_APBM_Hwi_Xml_Maker():pDOMDocument(0),pDOMImplementation(0) {
	// TODO Auto-generated constructor stub

	XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();

	ACS_APBM_HwiXStr core("core");
	pDOMImplementation = XERCES_CPP_NAMESPACE::DOMImplementationRegistry::getDOMImplementation(core.unicodeForm());

}

ACS_APBM_Hwi_Xml_Maker::~ACS_APBM_Hwi_Xml_Maker() {
	// TODO Auto-generated destructor stub
	//delete pDOMDocument; the server hangs with this instruction!!

//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- destructor \n"));

	XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();

//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- destructor after terminate\n"));
}


string ACS_APBM_Hwi_Xml_Maker::DoOutput2Stream(XERCES_CPP_NAMESPACE::DOMDocument* pmyDOMDocument)

{


	/***************/
	//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- DoOutput2Stream \n"));

		XERCES_CPP_NAMESPACE::DOMImplementation *impl = NULL;

		ACS_APBM_HwiXStr x("LS");

		impl = XERCES_CPP_NAMESPACE::DOMImplementationRegistry::getDOMImplementation(x.unicodeForm());

		/*to be removed
		XERCES_CPP_NAMESPACE::DOMImplementation *impl = NULL;

		ACS_APBM_HwiXStr x("LS");

		impl = XERCES_CPP_NAMESPACE::DOMImplementationRegistry::getDOMImplementation(x.unicodeForm());

		XERCES_CPP_NAMESPACE::DOMWriter* myWriter = ((XERCES_CPP_NAMESPACE::DOMImplementationLS*)impl)->createDOMWriter();


		XERCES_CPP_NAMESPACE::XMLFormatTarget *myFormatTarget = new XERCES_CPP_NAMESPACE::StdOutFormatTarget();

		myWriter->writeNode(myFormatTarget, *pRootElement);

		myWriter->release();
		*/





		XERCES_CPP_NAMESPACE::DOMLSSerializer* writer=((XERCES_CPP_NAMESPACE::DOMImplementationLS*)impl)->createLSSerializer();

		if (writer->getDomConfig()->canSetParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTXercesPrettyPrint, true))
		{
			writer->getDomConfig()->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTFormatPrettyPrint, true);
		}

		string theOutput;


		XMLCh* toTranscode = writer->writeToString(pmyDOMDocument);

		char* xmlChar = XERCES_CPP_NAMESPACE::XMLString::transcode(toTranscode);

		std::string transcodedStr;
		transcodedStr = string(xmlChar);

		XERCES_CPP_NAMESPACE::XMLString::release(&xmlChar);
		XERCES_CPP_NAMESPACE::XMLString::release(&toTranscode);
		writer->release();

		string utf16("UTF-16");
		string utf8("UTF-8");
		size_t j;

		for ( ; (j = transcodedStr.find( utf16 )) != string::npos ; )
		{
			transcodedStr.replace( j, utf16.length(), utf8 );
		}

	//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- DoOutput2Stream  return \n"));
		/*addedby me*/
//		cout <<"*******************************************"<<endl;
//		cout <<"*******************************************"<<endl;
//		cout <<"*******************************************"<<endl;
//		cout <<"transcodedStr"<<transcodedStr<<endl;
		cout <<transcodedStr<<endl;
		return transcodedStr;

/*******************/
////	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- DoOutput2Stream \n"));
//
//	XERCES_CPP_NAMESPACE::DOMImplementation *impl = NULL;
//
//	ACS_APBM_HwiXStr x("LS");
//
//	impl = XERCES_CPP_NAMESPACE::DOMImplementationRegistry::getDOMImplementation(x.unicodeForm());
//
//	XERCES_CPP_NAMESPACE::DOMLSSerializer* writer=((XERCES_CPP_NAMESPACE::DOMImplementationLS*)impl)->createLSSerializer();
//
//	if (writer->getDomConfig()->canSetParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTXercesPrettyPrint, true))
//	{
//		writer->getDomConfig()->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTFormatPrettyPrint, true);
//	}
//
//	string theOutput;
//
//
//	XMLCh* toTranscode = writer->writeToString(pmyDOMDocument);
//
//	char* xmlChar = XERCES_CPP_NAMESPACE::XMLString::transcode(toTranscode);
//
//	std::string transcodedStr;
//	transcodedStr = string(xmlChar);
//
//	XERCES_CPP_NAMESPACE::XMLString::release(&xmlChar);
//	XERCES_CPP_NAMESPACE::XMLString::release(&toTranscode);
//	writer->release();
//
//	string utf16("UTF-16");
//	string utf8("UTF-8");
//	size_t j;
//
//	for ( ; (j = transcodedStr.find( utf16 )) != string::npos ; )
//	{
//		transcodedStr.replace( j, utf16.length(), utf8 );
//	}
//
////	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- DoOutput2Stream  return \n"));
//	return transcodedStr;
//
}

//XERCES_CPP_NAMESPACE::DOMElement** ACS_APBM_Hwi_Xml_Maker::getBoardData(ACS_APBM_HWIData &hwi,string magazine,int &len)
XERCES_CPP_NAMESPACE::DOMElement** ACS_APBM_Hwi_Xml_Maker::getBoardData(ACS_APBM_HWIData* hwi, string /*magazine*/,int &len)
{
/****/

	XERCES_CPP_NAMESPACE::DOMElement *productData_tag={0};
//	XERCES_CPP_NAMESPACE::DOMElement *board_tag={0};

//	boardDataMapType boards;
//	BoardData bd;

	std::vector<int> boardList;


	//hwi.getBoardData(boards);


//	staticSlotMapType staticSlots;
//	hwi.getStaticSlotdata(staticSlots);

	//hwi.getBoardList(boardList);
	hwi->getBoardList(boardList);
	if (boardList.size() == 0)/*no board found*/
		return NULL;
	

//	len=0;
//
//	if(boards.size()==0)
//		return NULL;


	XERCES_CPP_NAMESPACE::DOMElement **pDatatElement = NULL;

	//pDatatElement=(XERCES_CPP_NAMESPACE::DOMElement**)malloc(sizeof(XERCES_CPP_NAMESPACE::DOMElement)*boards.size());
	pDatatElement=(XERCES_CPP_NAMESPACE::DOMElement**)malloc(sizeof(XERCES_CPP_NAMESPACE::DOMElement)*boardList.size());

//	int slot=0;

	ACS_APBM_HwiXStr strFirst("");
	ACS_APBM_HwiXStr strSecond("");

	//boardDataMapType::const_iterator boardIt;
	std::vector<int>::iterator boardIt;

	int index=0;

	char slot_char[3]={0};

	for(boardIt = boardList.begin();boardIt!=boardList.end();++boardIt)
	{

		/*for each board get the boards info and xy position data then put info in xml doc*/

//		slot=boardIt->first;
//		bd=boardIt->second;


		std::string productNumber;
		std::string productName;
		std::string productRevision;
		std::string serialNo;
		std::string manufactureDate;
		std::string supplier;
		std::string busType;

		//orig hwi.getBoardData((*boardIt), productNumber, productName,productRevision,serialNo,manufactureDate,supplier,busType);
		hwi->getBoardData((*boardIt), productNumber, productName,productRevision,serialNo,manufactureDate,supplier,busType);

		int xPos;
		int yPos;
		//orig hwi.getSlotPosData((*boardIt), xPos, yPos );
		hwi->getSlotPosData((*boardIt), xPos, yPos );

		//sprintf(slot_char,"%d\0",slot);
		//sprintf(slot_char,"%d\0",(*boardIt));
		sprintf(slot_char,"%d",(*boardIt));

		//GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getBoardData for magazine %s slot %s \n",magazine.c_str(),slot_char));

		strFirst.setCharString("Board");

		pDatatElement[index] = pDOMDocument->createElement(strFirst.unicodeForm());

		strFirst.setCharString("slotPosition");
		strSecond.setCharString(slot_char);
		pDatatElement[index]->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

		char stir[6]={0};

		if(xPos!=-1)
		{
			strFirst.setCharString("xPos");
			sprintf( stir, "%d", xPos);
			//itoa(xPos,stir,10);

			strSecond.setCharString(stir);

			pDatatElement[index]->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
		}

		if(yPos!=-1)
		{
			strFirst.setCharString("yPos");

			//itoa(yPos,stir,10);
			sprintf( stir, "%d", yPos);

			strSecond.setCharString(stir);

			pDatatElement[index]->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
		}

		/*strFirst.setCharString("type");
		strSecond.setCharString("CP");
		pDatatElement[index]->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());*/

		strFirst.setCharString("busType");
		strSecond.setCharString(busType.c_str());
		pDatatElement[index]->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

		strFirst.setCharString("ProductData");

		productData_tag = pDOMDocument->createElement(strFirst.unicodeForm());

		strFirst.setCharString("productName");
		strSecond.setCharString(productName.c_str());
		productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

		strFirst.setCharString("productNo");
		strSecond.setCharString(productNumber.c_str());
		productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

		strFirst.setCharString("productRev");
		strSecond.setCharString(productRevision.c_str());
		productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

		strFirst.setCharString("serialNo");
		strSecond.setCharString(serialNo.c_str());
		//cout<<"TO DELETE serialNo.size"<<serialNo.size()<<endl;;
		productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

		strFirst.setCharString("manDate");
		//if (!bd.manufactureDate.empty())
		if (!manufactureDate.empty())
		{
			strSecond.setCharString(manufactureDate.c_str());
			productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
		}
		else
		{
			strSecond.setCharString("00000");
			//productData_tag->setAttribute(strFirst.unicodeForm(), L"00000");
			productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
		}

		strFirst.setCharString("supplier");
		strSecond.setCharString(supplier.c_str());
		//check optional attributes		
		if(!supplier.empty())
			productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

		pDatatElement[index]->appendChild(productData_tag);

		index++;
	}

	len=index;

	//GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getBoardData return\n"));

	return pDatatElement;

/*****/
//	XERCES_CPP_NAMESPACE::DOMElement *productData_tag={0};
//	XERCES_CPP_NAMESPACE::DOMElement *board_tag={0};
//
//	boardDataMapType boards;
//	BoardData bd;
//
//	hwi.getBoardData(boards);
//
//	staticSlotMapType staticSlots;
//	hwi.getStaticSlotdata(staticSlots);
//
//	len=0;
//
//	if(boards.size()==0)
//		return NULL;
//
//
//	XERCES_CPP_NAMESPACE::DOMElement **pDatatElement = NULL;
//
//	pDatatElement=(XERCES_CPP_NAMESPACE::DOMElement**)malloc(sizeof(XERCES_CPP_NAMESPACE::DOMElement)*boards.size());
//
//	int slot=0;
//
//	ACS_APBM_HwiXStr strFirst("");
//	ACS_APBM_HwiXStr strSecond("");
//
//	boardDataMapType::const_iterator boardIt;
//
//	int index=0;
//
//	char slot_char[3]={0};
//
//	for(boardIt = boards.begin();boardIt!=boards.end();boardIt++)
//	{
//
//
//
//		slot=boardIt->first;
//		bd=boardIt->second;
//
//		sprintf(slot_char,"%d\0",slot);
//
//		GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getBoardData for magazine %s slot %s \n",magazine.c_str(),slot_char));
//
//		strFirst.setCharString("Board");
//
//		pDatatElement[index] = pDOMDocument->createElement(strFirst.unicodeForm());
//
//		strFirst.setCharString("slotPosition");
//		strSecond.setCharString(slot_char);
//		pDatatElement[index]->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//		char stir[6]={0};
//
//		if(staticSlots[slot].xpos!=-1)
//		{
//			strFirst.setCharString("xPos");
//
//			itoa(staticSlots[slot].xpos,stir,10);
//
//			strSecond.setCharString(stir);
//
//			pDatatElement[index]->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//		}
//
//		if(staticSlots[slot].ypos!=-1)
//		{
//			strFirst.setCharString("yPos");
//
//			itoa(staticSlots[slot].ypos,stir,10);
//
//			strSecond.setCharString(stir);
//
//			pDatatElement[index]->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//		}
//
//		/*strFirst.setCharString("type");
//		strSecond.setCharString("CP");
//		pDatatElement[index]->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());*/
//
//		strFirst.setCharString("busType");
//		strSecond.setCharString(bd.busType.c_str());
//		pDatatElement[index]->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//		strFirst.setCharString("ProductData");
//
//		productData_tag = pDOMDocument->createElement(strFirst.unicodeForm());
//
//		strFirst.setCharString("productName");
//		strSecond.setCharString(bd.productName.c_str());
//		productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//		strFirst.setCharString("productNo");
//		strSecond.setCharString(bd.productNumber.c_str());
//		productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//		strFirst.setCharString("productRev");
//		strSecond.setCharString(bd.productRevision.c_str());
//		productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//		strFirst.setCharString("serialNo");
//		strSecond.setCharString(bd.serialNo.c_str());
//		productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//		strFirst.setCharString("manDate");
//		if (!bd.manufactureDate.empty())
//		{
//			strSecond.setCharString(bd.manufactureDate.c_str());
//			productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//		}
//		else
//		{
//			productData_tag->setAttribute(strFirst.unicodeForm(), L"00000");
//		}
//
//		strFirst.setCharString("supplier");
//		strSecond.setCharString(bd.supplier.c_str());
//		//check optional attributes
//		if(!bd.supplier.empty())
//			productData_tag->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//		pDatatElement[index]->appendChild(productData_tag);
//
//		index++;
//	}
//
//	len=index;
//
//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getBoardData return\n"));
//
//	return pDatatElement;
}


//XERCES_CPP_NAMESPACE::DOMElement* ACS_APBM_Hwi_Xml_Maker::getPfmDataLower(ACS_APBM_HWIData &hwi)
XERCES_CPP_NAMESPACE::DOMElement* ACS_APBM_Hwi_Xml_Maker::getPfmDataLower(ACS_APBM_HWIData* hwi)
{
	XERCES_CPP_NAMESPACE::DOMElement *pfm_dom_element={0};
	XERCES_CPP_NAMESPACE::DOMElement *pfm_prodata_element={0};

	//PfmData pfm;

	std::string pfmProductName, pfmProductNumber, pfmProductRevision, pfmHwVersion, pfmSerialNo, pfmDeviceType, pfmManufactureDate;
	

	//hwi.getPfmdataLower(pfm);
	//orig .
	hwi->getPFMUpperLowerData(pfmProductName,
							pfmProductNumber,
							pfmProductRevision,
							pfmHwVersion,
							pfmSerialNo,
							pfmDeviceType,
							pfmManufactureDate,
							false);

	ACS_APBM_HwiXStr strFirst("");
	ACS_APBM_HwiXStr strSecond("");

	strFirst.setCharString("PFM");
	pfm_dom_element = pDOMDocument->createElement(strFirst.unicodeForm());

	strFirst.setCharString("instance");
	strSecond.setCharString("lower");
	pfm_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("hwVersion");
	strSecond.setCharString(pfmHwVersion.c_str());
	pfm_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("deviceType");//TODO check se gia' presente
	strSecond.setCharString(pfmDeviceType.c_str());
//	if (pfmDeviceType == "1")
//		strSecond.setCharString("hod");
//	else if (pfmDeviceType == "0")
//		strSecond.setCharString("lod");
//	else
//		strSecond.setCharString("");
	pfm_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("ProductData");

	pfm_prodata_element = pDOMDocument->createElement(strFirst.unicodeForm());

	strFirst.setCharString("productName");

	strSecond.setCharString(pfmProductName.c_str());
	pfm_prodata_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("productNo");
	strSecond.setCharString(pfmProductNumber.c_str());
	pfm_prodata_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("productRev");
	strSecond.setCharString(pfmProductRevision.c_str());
	pfm_prodata_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("serialNo");
	strSecond.setCharString(pfmSerialNo.c_str());
	pfm_prodata_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("manDate");
	if (!pfmManufactureDate.empty())
	{
		strSecond.setCharString(pfmManufactureDate.c_str());
		pfm_prodata_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
	}
	else{
		strSecond.setCharString("00000");
		pfm_prodata_element->setAttribute(strFirst.unicodeForm(),strSecond.unicodeForm());
	}
		//pfm_prodata_element->setAttribute(strFirst.unicodeForm(), L"00000");

	pfm_dom_element->appendChild(pfm_prodata_element);
	return pfm_dom_element;

}

//XERCES_CPP_NAMESPACE::DOMElement* ACS_APBM_Hwi_Xml_Maker::getPfmDataUpper(ACS_APBM_HWIData &hwi)
XERCES_CPP_NAMESPACE::DOMElement* ACS_APBM_Hwi_Xml_Maker::getPfmDataUpper(ACS_APBM_HWIData* hwi)
{
	XERCES_CPP_NAMESPACE::DOMElement *pfm_dom_element={0};
	XERCES_CPP_NAMESPACE::DOMElement *pfm_prodata_element={0};

//	PfmData pfm;
//
//	hwi.getPfmdataUpper(pfm);

	std::string pfmProductName, pfmProductNumber, pfmProductRevision, pfmHwVersion, pfmSerialNo, pfmDeviceType, pfmManufactureDate;
	//orig .
	hwi->getPFMUpperLowerData(pfmProductName,
								pfmProductNumber,
								pfmProductRevision,
								pfmHwVersion,
								pfmSerialNo,
								pfmDeviceType,
								pfmManufactureDate,
								true);

	ACS_APBM_HwiXStr strFirst("");
	ACS_APBM_HwiXStr strSecond("");

	strFirst.setCharString("PFM");
	pfm_dom_element = pDOMDocument->createElement(strFirst.unicodeForm());

	strFirst.setCharString("instance");
	strSecond.setCharString("upper");
	pfm_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("hwVersion");
	strSecond.setCharString(pfmHwVersion.c_str());
	pfm_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("deviceType");
	strSecond.setCharString(pfmDeviceType.c_str());
//	if (pfmDeviceType == "1")
//		strSecond.setCharString("hod");
//	else if (pfmDeviceType == "0")
//		strSecond.setCharString("lod");
//	else
//		strSecond.setCharString("");

	pfm_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("ProductData");

	pfm_prodata_element = pDOMDocument->createElement(strFirst.unicodeForm());

	strFirst.setCharString("productName");

	strSecond.setCharString(pfmProductName.c_str());
	pfm_prodata_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("productNo");
	strSecond.setCharString(pfmProductNumber.c_str());
	pfm_prodata_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("productRev");
	strSecond.setCharString(pfmProductRevision.c_str());
	pfm_prodata_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("serialNo");
	strSecond.setCharString(pfmSerialNo.c_str());
	pfm_prodata_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("manDate");
	if (!pfmManufactureDate.empty())
	{
		strSecond.setCharString(pfmManufactureDate.c_str());
		pfm_prodata_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
	}
	else
	{
		//pfm_prodata_element->setAttribute(strFirst.unicodeForm(), L"00000");
		strSecond.setCharString("00000");
		pfm_prodata_element->setAttribute(strFirst.unicodeForm(),strSecond.unicodeForm());
	}

	pfm_dom_element->appendChild(pfm_prodata_element);
	return pfm_dom_element;

}


//orig XERCES_CPP_NAMESPACE::DOMElement*	ACS_APBM_Hwi_Xml_Maker::getBackPlane(ACS_APBM_HWIData &hwi)
XERCES_CPP_NAMESPACE::DOMElement*	ACS_APBM_Hwi_Xml_Maker::getBackPlane(ACS_APBM_HWIData* hwi)
{
//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getBackPlane\n"));

	XERCES_CPP_NAMESPACE::DOMElement *back_plane_dom_element={0};
	XERCES_CPP_NAMESPACE::DOMElement *back_plane_proData_element={0};

//	bkPlane bkp;
//
//	hwi.getBackplaneData(bkp);
	//string ACS_APBM_Hwi_Xml_Maker::DoOutput2Stream(XERCES_CPP_NAMESPACE::DOMDocument* pmyDOMDocument)
	//{
	//
	////	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- DoOutput2Stream \n"));
	//
	//	XERCES_CPP_NAMESPACE::DOMImplementation *impl = NULL;
	//
	//	ACS_APBM_HwiXStr x("LS");
	//
	//	impl = XERCES_CPP_NAMESPACE::DOMImplementationRegistry::getDOMImplementation(x.unicodeForm());
	//
	//	XERCES_CPP_NAMESPACE::DOMLSSerializer* writer=((XERCES_CPP_NAMESPACE::DOMImplementationLS*)impl)->createLSSerializer();
	//
	//	if (writer->getDomConfig()->canSetParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTXercesPrettyPrint, true))
	//	{
	//		writer->getDomConfig()->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTFormatPrettyPrint, true);
	//	}
	//
	//	string theOutput;
	//
	//
	//	XMLCh* toTranscode = writer->writeToString(pmyDOMDocument);
	//
	//	char* xmlChar = XERCES_CPP_NAMESPACE::XMLString::transcode(toTranscode);
	//
	//	std::string transcodedStr;
	//	transcodedStr = string(xmlChar);
	//
	//	XERCES_CPP_NAMESPACE::XMLString::release(&xmlChar);
	//	XERCES_CPP_NAMESPACE::XMLString::release(&toTranscode);
	//	writer->release();
	//
	//	string utf16("UTF-16");
	//	string utf8("UTF-8");
	//	size_t j;
	//
	//	for ( ; (j = transcodedStr.find( utf16 )) != string::npos ; )
	//	{
	//		transcodedStr.replace( j, utf16.length(), utf8 );
	//	}
	//
	////	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- DoOutput2Stream  return \n"));
	//	return transcodedStr;
	//
	//}
	std::string prodName;
	std::string prodNo;
	std::string prodRev;
	std::string prodSerialNo;
	std::string prodSupplier;
	std::string prodManufactureDate;

	// orig hwi.getBackplaneData(prodName, prodNo, prodRev, prodSerialNo, prodSupplier, prodManufactureDate);
	hwi->getBackplaneData(prodName, prodNo, prodRev, prodSerialNo, prodSupplier, prodManufactureDate);

	ACS_APBM_HwiXStr strFirst("");
	ACS_APBM_HwiXStr strSecond("");

	strFirst.setCharString("Backplane");

	back_plane_dom_element = pDOMDocument->createElement(strFirst.unicodeForm());

	/****************/
	strFirst.setCharString("ProductData");
	back_plane_proData_element = pDOMDocument->createElement(strFirst.unicodeForm());

	back_plane_dom_element->appendChild(back_plane_proData_element);

	strFirst.setCharString("productName");
	strSecond.setCharString(prodName.c_str());
	back_plane_proData_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("productNo");
	strSecond.setCharString(prodNo.c_str());
	back_plane_proData_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("productRev");
	strSecond.setCharString(prodRev.c_str());
	back_plane_proData_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("serialNo");
	strSecond.setCharString(prodSerialNo.c_str());
	back_plane_proData_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("manDate");

	if (!prodManufactureDate.empty())
	{
		strSecond.setCharString(prodManufactureDate.c_str());
		back_plane_proData_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
	} 
	else
	{
		strSecond.setCharString("00000");
		back_plane_proData_element->setAttribute(strFirst.unicodeForm(),strSecond.unicodeForm());
	}

	
//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getBackPlane return \n"));


	return back_plane_dom_element;
/**************/


//	strFirst.setCharString("ProductData");
//	back_plane_proData_element = pDOMDocument->createElement(strFirst.unicodeForm());
//
//	back_plane_dom_element->appendChild(back_plane_proData_element);
//
//	strFirst.setCharString("productName");
//	strSecond.setCharString(bkp.bpProductName.c_str());
//	back_plane_proData_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//	strFirst.setCharString("productNo");
//	strSecond.setCharString(bkp.bpProductNumber.c_str());
//	back_plane_proData_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//	strFirst.setCharString("productRev");
//	strSecond.setCharString(bkp.bpProductRevision.c_str());
//	back_plane_proData_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//	strFirst.setCharString("serialNo");
//	strSecond.setCharString(bkp.bpSerialNo.c_str());
//	back_plane_proData_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//	strFirst.setCharString("manDate");
//
//	if (!bkp.bpManufactureDate.empty())
//	{
//		strSecond.setCharString(bkp.bpManufactureDate.c_str());
//		back_plane_proData_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//	}
//	else
//	{
//		back_plane_proData_element->setAttribute(strFirst.unicodeForm(), L"00000");
//	}
//
//
////	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getBackPlane return \n"));
//
//
//	return back_plane_dom_element;

}

string ACS_APBM_Hwi_Xml_Maker::getStringXmlDocument()
{
	string xmlString("");



	std::string aptType;
//	ACS_CS_API_NS::CS_API_Result res;
//	res = ACS_CS_API_NetworkElement::getAPTType(aptType);
//	if (res != ACS_CS_API_NS::Result_Success)
//	{
		//Error
		aptType = "";
////		GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getStringXmlDocument - getAPTType ERROR \n"));
//	}

//	ACS_APBM_Parameter param;

	string infrastructure("");
	string nodeRelease("");
	string EquipmentBuildingPractice("");


	XERCES_CPP_NAMESPACE::DOMElement *Node_element;
	XERCES_CPP_NAMESPACE::DOMElement *equipment_element;
	XERCES_CPP_NAMESPACE::DOMElement *sub_rack_element;
	XERCES_CPP_NAMESPACE::DOMElement *pRootElement;
	XERCES_CPP_NAMESPACE::DOMElement* pfmL;
	XERCES_CPP_NAMESPACE::DOMElement* pfmU;
	XERCES_CPP_NAMESPACE::DOMElement* pBkp;

	char *date={0};
	date=getDate();

	ACS_APBM_HwiXStr xmlstring("");
	ACS_APBM_HwiXStr xmlstringSecond("");

	xmlstring.setCharString("HardwareInventory");

	pDOMDocument = pDOMImplementation->createDocument(0,xmlstring.unicodeForm(),0);

	pRootElement = pDOMDocument->getDocumentElement();

	xmlstring.setCharString("inventoryTime");
	xmlstringSecond.setCharString(date);
	pRootElement->setAttribute(xmlstring.unicodeForm(), xmlstringSecond.unicodeForm());

	xmlstring.setCharString("xsi:noNamespaceSchemaLocation");
	xmlstringSecond.setCharString("HWI_EGEM2_R1.xsd");
	pRootElement->setAttribute(xmlstring.unicodeForm(),xmlstringSecond.unicodeForm());

	xmlstring.setCharString("xmlns:xsi");
	xmlstringSecond.setCharString("http://www.w3.org/2001/XMLSchema-instance");
	pRootElement->setAttribute(xmlstring.unicodeForm(),xmlstringSecond.unicodeForm());

	xmlstring.setCharString("Node");
	Node_element= pDOMDocument->createElement(xmlstring.unicodeForm());

	xmlstring.setCharString("infrastructure");

//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getStringXmlDocument - APTType: %s \n", aptType.c_str()));

//	if (aptType.compare("MSC") == 0)
//	{
//		infrastructure = param.getGenericNodeInfrastructure();
//		infrastructure += ", " + aptType;
//	}
//	else if (aptType.compare("HLR") == 0)
//	{
//		infrastructure = param.getGenericNodeInfrastructure();
//		infrastructure += ", " + aptType;
//	}
//	else if (aptType.compare("BSC") == 0)
//	{	//BSC
//		infrastructure= param.getNodeInfrastructure();
//		infrastructure += "/" + aptType;
//	}
//	else
//	{	//Assume BSC !
//		infrastructure= param.getNodeInfrastructure();
//		infrastructure += "/BSC";
//	}

//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- infrastructure : %s \n", infrastructure.c_str()));

	if(!infrastructure.empty())
	{
		xmlstringSecond.setCharString(infrastructure.c_str());
	}
	else
	{
		xmlstringSecond.setCharString("");
	}

	Node_element->setAttribute(xmlstring.unicodeForm(),xmlstringSecond.unicodeForm());

	xmlstring.setCharString("release");

//	nodeRelease=param.getNodeRelease();

	if(!nodeRelease.empty())
	{
		xmlstringSecond.setCharString(nodeRelease.c_str());
		Node_element->setAttribute(xmlstring.unicodeForm(),xmlstringSecond.unicodeForm());
	}

	pRootElement->appendChild(Node_element);


	xmlstring.setCharString("Equipment");
	equipment_element= pDOMDocument->createElement(xmlstring.unicodeForm());

	xmlstring.setCharString("buildingPractice");

	//EquipmentBuildingPractice=param.getEquipmentBuildingPractice();

	if(!EquipmentBuildingPractice.empty())
	{
		xmlstringSecond.setCharString(EquipmentBuildingPractice.c_str());
	}
	else
	{
		xmlstringSecond.setCharString("");
	}


	equipment_element->setAttribute(xmlstring.unicodeForm(),xmlstringSecond.unicodeForm());

	Node_element->appendChild(equipment_element);

	//map<string,acs_apbm_HwiData>::iterator cabinet;
	//original map<string,ACS_APBM_HWIData>::iterator cabinet;
	/*new */map<string,ACS_APBM_HWIData*>::iterator cabinet;

	//orACS_APBM_HWIData hw;
	ACS_APBM_HWIData* hw;
	string magaz;
	XERCES_CPP_NAMESPACE::DOMElement **boards;
	int len=0;

//	Backplane bp;
//	PfmData pfm;
//	PfmData pfml;
//	boardDataMapType boardMap;
	for(cabinet=cab.begin();cabinet!=cab.end();++cabinet)
	{
		hw=cabinet->second;
		magaz=cabinet->first;

//		if (!hw.isEmpty())
//		{
			boards=getBoardData(hw,magaz,len);
			sub_rack_element=getSubRack(hw,magaz);
			equipment_element->appendChild(sub_rack_element);

			pBkp=getBackPlane(hw);
			pfmU=getPfmDataUpper(hw);
			pfmL=getPfmDataLower(hw);

			sub_rack_element->appendChild(pBkp);
			//if (hw.getPfmdataUpperPresence() == 1)
			if (hw->isPFMUpperPresent() == 1)
			sub_rack_element->appendChild(pfmU);
			//if (hw.getPfmdataLowerPresence() == 1)
			if (hw->isPFMLowerPresent() == 1)
			sub_rack_element->appendChild(pfmL);

			for(int i=0;i<len;i++)
			{
				sub_rack_element->appendChild(boards[i]);
			}

			free(boards);
//		}
	}



	/*Paola .... start
	XERCES_CPP_NAMESPACE::DOMImplementation *impl = NULL;

	ACS_APBM_HwiXStr x("LS");

	impl = XERCES_CPP_NAMESPACE::DOMImplementationRegistry::getDOMImplementation(x.unicodeForm());

	XERCES_CPP_NAMESPACE::DOMWriter* myWriter = ((XERCES_CPP_NAMESPACE::DOMImplementationLS*)impl)->createDOMWriter();


	XERCES_CPP_NAMESPACE::XMLFormatTarget *myFormatTarget = new XERCES_CPP_NAMESPACE::StdOutFormatTarget();

	myWriter->writeNode(myFormatTarget, *pRootElement);

	myWriter->release();

	Paola .....end*/
	xmlString = DoOutput2Stream(pDOMDocument);

	pDOMDocument->release();
	//XMLPlatformUtils::Terminate();


	free(date);

	return xmlString;
}



int ACS_APBM_Hwi_Xml_Maker::insertMagazine(ACS_APBM_HWIData *hwidata,string magazine)
{
	//cout<<"insertMagazine cab.insert(make_pair(magazine)1 "<<endl;

	//cab.insert (make_pair(magazine , hwidata) );
	cab.insert (pair<std::string,ACS_APBM_HWIData*>(magazine , hwidata) );

	//cout<<"insertMagazine cab.insert(make_pair(magazine)2 "<<endl;
	return 0;
}

//int ACS_APBM_Hwi_Xml_Maker::getXml(string &output)
//{
//	return 0;
//}


char *ACS_APBM_Hwi_Xml_Maker::getDate()
{
	time_t     now;
	struct tm  *ts;
	char buf[20],*bufRet;

	bufRet=(char*)malloc(sizeof(char)*20);
	memset(buf,0,20);

	/* Get the current time */
	now = time(NULL);

	/* Format and print the time, "ddd yyyy-mm-dd hh:mm:ss zzz" */
	ts = localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", ts);

	sprintf(bufRet,"%s",buf);

	return bufRet;
}


//XERCES_CPP_NAMESPACE::DOMElement*	ACS_APBM_Hwi_Xml_Maker::getSubRack(ACS_APBM_HWIData &hwi,string magazine)
XERCES_CPP_NAMESPACE::DOMElement*	ACS_APBM_Hwi_Xml_Maker::getSubRack(ACS_APBM_HWIData* hwi,string magazine)
{

//	SubRackData sbData;
//	Backplane bpData;
//
//
//	hwi.getSubrackdata(sbData);
//	hwi.getBackplaneData(bpData);


	int magRow;
	int magNo;
	int magXpos;
	int magYpos;
	std::string subRackName;

	// orig hwi.getMagazinePositionData(magRow, magNo, magXpos, magYpos, subRackName);
	hwi->getMagazinePositionData(magRow, magNo, magXpos, magYpos, subRackName);
	
	/***************************************/
	XERCES_CPP_NAMESPACE::DOMElement *sub_rack_dom_element={0};

	ACS_APBM_HwiXStr strFirst("");
	ACS_APBM_HwiXStr strSecond("");

	strFirst.setCharString("Subrack");
	sub_rack_dom_element = pDOMDocument->createElement(strFirst.unicodeForm());

	strFirst.setCharString("name");
	strSecond.setCharString(subRackName.c_str());


	sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());

	strFirst.setCharString("address");
	strSecond.setCharString(magazine.c_str());
	sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());


	char stri[20]={0};

	if(magRow!=-1)
	{
		memset(stri,0,20);
		strFirst.setCharString("cabRow");
		sprintf( stri, "%d", magRow);
		//itoa(sbData.cabRow,stri,10);
		strSecond.setCharString(stri);
		sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
	}

	if(magNo!=-1)
	{
		memset(stri,0,20);
		strFirst.setCharString("cabNo");
		//itoa(sbData.cabNo,stri,10);
		sprintf( stri, "%d", magNo);
		strSecond.setCharString(stri);
		sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
	}

	if(magXpos!=-1)
	{
		memset(stri,0,20);
		strFirst.setCharString("xPos");
		//itoa(sbData.cabXpos,stri,10);
		sprintf( stri, "%d", magXpos);
		strSecond.setCharString(stri);
		sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
	}

	if(magYpos!=-1)
	{
		memset(stri,0,20);
		strFirst.setCharString("yPos");
		//itoa(sbData.cabYpos,stri,10);
		sprintf( stri, "%d", magYpos);
		strSecond.setCharString(stri);
		sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
	}

//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- sbData.cabYpos=%d sbData.cabXpos=%d sbData.cabNo=%d sbData.cabRow=%d\n",sbData.cabXpos,sbData.cabYpos,sbData.cabNo,sbData.cabRow));
//
//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getSubRack for magazine %s return \n",magazine.c_str()));

	return sub_rack_dom_element;
	/****************************************/


//	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getSubRack for magazine %s\n",magazine.c_str()));

//	XERCES_CPP_NAMESPACE::DOMElement *sub_rack_dom_element={0};
//
//	ACS_APBM_HwiXStr strFirst("");
//	ACS_APBM_HwiXStr strSecond("");
//
//	strFirst.setCharString("Subrack");
//	sub_rack_dom_element = pDOMDocument->createElement(strFirst.unicodeForm());
//
//	strFirst.setCharString("name");
//	strSecond.setCharString(subRackName.c_str());
//
//
//	sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//	strFirst.setCharString("address");
//	strSecond.setCharString(magazine.c_str());
//	sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//
//
//	char stri[20]={0};
//
//	if(sbData.cabRow!=-1)
//	{
//		memset(stri,0,20);
//		strFirst.setCharString("cabRow");
//		itoa(sbData.cabRow,stri,10);
//		strSecond.setCharString(stri);
//		sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//	}
//
//	if(sbData.cabNo!=-1)
//	{
//		memset(stri,0,20);
//		strFirst.setCharString("cabNo");
//		itoa(sbData.cabNo,stri,10);
//		strSecond.setCharString(stri);
//		sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//	}
//
//	if(sbData.cabXpos!=-1)
//	{
//		memset(stri,0,20);
//		strFirst.setCharString("xPos");
//		itoa(sbData.cabXpos,stri,10);
//		strSecond.setCharString(stri);
//		sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//	}
//
//	if(sbData.cabYpos!=-1)
//	{
//		memset(stri,0,20);
//		strFirst.setCharString("yPos");
//		itoa(sbData.cabYpos,stri,10);
//		strSecond.setCharString(stri);
//		sub_rack_dom_element->setAttribute(strFirst.unicodeForm(), strSecond.unicodeForm());
//	}
//
////	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- sbData.cabYpos=%d sbData.cabXpos=%d sbData.cabNo=%d sbData.cabRow=%d\n",sbData.cabXpos,sbData.cabYpos,sbData.cabNo,sbData.cabRow));
////
////	GCC_TRACE((ACS_APBM_HWI_XML,"(%t) +-+- getSubRack for magazine %s return \n",magazine.c_str()));
//
//	return sub_rack_dom_element;

}

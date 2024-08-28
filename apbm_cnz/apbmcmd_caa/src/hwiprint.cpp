//============================================================================
// Name        : hwiprint.cpp
// Author      : 
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <acs_apgcc_omhandler.h>
#include <acs_prc_api.h>
#include <acs_apgcc_paramhandling.h>
//#include "ACS_APBM_Hwi_Xml_Maker.h"
#include <acs_apbm_hwixmlmaker.h>
#include "acs_apbm_hwidata.h"

/*Define Section*/
#define EXIT_CODE_1 1
#define EXIT_CODE_1_TEXT "Error when executing (general fault)"
#define EXIT_CODE_6 6
#define EXIT_CODE_6_TEXT "HWI Data not available"
#define EXIT_CODE_18 18
#define EXIT_CODE_18_TEXT "Invalid magazine value"
#define EXIT_CODE_31 31
#define EXIT_CODE_31_TEXT "The command can be executed on the active node only"
#define EXIT_CODE_32 32
#define EXIT_CODE_32_TEXT "Unable to check node architecture"
#define EXIT_CODE_115 115
#define EXIT_CODE_115_TEXT "Illegal command in this system configuration"

/*************** NODE ARCHITECTURE **************/
#define EGEM2_SCX       1
#define EGEM2_SMX       4

/*const definition Section*/
//const std:: string rootBaseObj 			= "hardwareInventoryId=1,safApp=safImmService";/*TODO modify it*/
const std:: string ApzFunctionsId1		= "apzFunctionsId=1";
const std:: string nodeArchitectureAttr = "nodeArchitecture";
// 7const std:: string baseClassName		= "HardwareInventory";
const std:: string baseClassName		= "AxeHardwareInventoryHardwareInventoryM";

/*Attributes names for shelf class*/
//const std::string SHELF_ID_ATT_NAME 		="hwiShelfId";
//newModel const std::string SHELF_ID_ATT_NAME 		="shelfId";
const std::string SHELF_ID_ATT_NAME 		="shelfInfoId";
//newModel const std::string SHELF_ROW_ATT_NAME 		="row";
const std::string SHELF_ROW_ATT_NAME 		="cabinetRow";
//newModel const std::string SHELF_NUMBER_ATT_NAME		="number";
const std::string SHELF_NUMBER_ATT_NAME		="cabinetNumber";

const std::string SHELF_XPOS_ATT_NAME 		="xPosition";
const std::string SHELF_YPOS_ATT_NAME 		="yPosition";
//newModel const std::string SHELF_PROD_NAME_ATT_NAME	="productName";
//newModel const std::string SHELF_PROD_NUM_ATT_NAME 	="productNumber";
//newModel const std::string SHELF_PROD_REV_ATT_NAME 	="productRevision";
const std::string SHELF_SERIAL_NUM_ATT_NAME	="serialNumber";
const std::string SHELF_SUPPLIER_ATT_NAME	="supplier";
const std::string SHELF_MAN_DATE_ATT_NAME 	="manufacturingDate";
const std::string SHELF_NAME_ATT_NAME 		="name";


/*Attributes names for PFM class*/
//newModel const std::string PFM_ID_ATT_NAME 			="powerFanModuleId";
const std::string PFM_ID_ATT_NAME 			="powerFanModuleInfoId";
const std::string PFM_PROD_NAME_ATT_NAME	="productName";
const std::string PFM_PROD_NUM_ATT_NAME 	="productNumber";
const std::string PFM_PROD_REV_ATT_NAME 	="productRevision";
const std::string PFM_SERIAL_NUM_ATT_NAME	="productSerialNumber";
const std::string PFM_DEVICE_TYPE_ATT_NAME	="deviceType";
const std::string PFM_HW_VERSION_ATT_NAME 	="hardwareVersion";
const std::string PFM_MAN_DATE_ATT_NAME 	="manufacturingDate";
//newModel added attributes
const std::string PFM_XPOS_ATT_NAME 	    ="xPosition";
const std::string PFM_YPOS_ATT_NAME 		="yPosition";

/*Attributes names for BOARD class*/
//newModel const std::string BOARD_ID_ATT_NAME 		="boardId";
const std::string BOARD_ID_ATT_NAME 		="bladeInfoId";
const std::string BOARD_XPOS_ATT_NAME 		="xPosition";
const std::string BOARD_YPOS_ATT_NAME 		="yPosition";
//newModel const std::string BOARD_PROD_NAME_ATT_NAME	="productName";
//newModel const std::string BOARD_PROD_NUM_ATT_NAME 	="productNumber";
//newModel const std::string BOARD_PROD_REV_ATT_NAME 	="productRevision";
const std::string BOARD_SERIAL_NUM_ATT_NAME	="serialNumber";
const std::string BOARD_SUPPLIER_ATT_NAME	="supplier";
const std::string BOARD_BUS_TYPE_ATT_NAME 	="busType";
const std::string BOARD_MAN_DATE_ATT_NAME 	="manufacturingDate";

/* newModel ADDED for ProductIdentity Object it take the place of
BOARD_PROD_NAME_ATT_NAME
BOARD_PROD_NUM_ATT_NAME
BOARD_PROD_REV_ATT_NAME
SHELF_PROD_NAME_ATT_NAME
SHELF_PROD_NUM_ATT_NAME
SHELF_PROD_REV_ATT_NAME
*/
const std::string PRODUCTIDENTITY_DESIGNATION_ATT_NAME	="productDesignation"; //name
const std::string PRODUCTIDENTITY_PROD_NUM_ATT_NAME 	="productNumber";      //number
const std::string PRODUCTIDENTITY_PROD_REV_ATT_NAME 	="productRevision";    //revision

const char BLADE_INFO_CLASS_NAME[] = "AxeHardwareInventoryBladeInfo";

using namespace std;


void getboardDNList(const std::string &dnMagazine,OmHandler objectManager, std::vector<string>& boardDNList);
void getboardDNListOrdered (const std::string &dnMagazine,OmHandler objectManager, std::vector<string>& boardDNList);

int retrieveXmlSingleBoardInfo(ACS_APBM_HWIData &p_hwi, const std::string &dnBoard,const std:: string &magazineName, OmHandler objectManager );
/*added for product identity mnagement
 * this method retrieve the productDesignation, productRev, product number of the productIdentity element
 * it is assumed that the dn of the parent object is provided as input and that it has just one product Identity element child whose name is 1,DNParent
 * this is a convention since product identity objects are created by the server*/
int  getProductIdentityDataFromIMM(OmHandler objectManager, const std::string &dn, std::string &productDesignation, std::string &productNumber, std::string &productRevision ){


		ACS_APGCC_ImmAttribute attribute_prodDesignation; 		/*productDesignation*/
		ACS_APGCC_ImmAttribute attribute_prodNum;				/*productNumber*/
		ACS_APGCC_ImmAttribute attribute_prodRev;				/*productRevision*/

		attribute_prodDesignation.attrName = PRODUCTIDENTITY_DESIGNATION_ATT_NAME.c_str();
		attribute_prodNum.attrName         =PRODUCTIDENTITY_PROD_NUM_ATT_NAME.c_str();
		attribute_prodRev.attrName         =PRODUCTIDENTITY_PROD_REV_ATT_NAME.c_str();

		std::vector<ACS_APGCC_ImmAttribute *> attributes;

		/*push back on vector*/
		attributes.push_back(&attribute_prodDesignation);
		attributes.push_back(&attribute_prodNum);
		attributes.push_back(&attribute_prodRev);

		/*get values*/
		ACS_CC_ReturnType result;
		std::string genericProductIdName ="id=prodId,"; //all Product Identity elements are named id=1,dnparent
		//4std::string dnProductIdentityObj = "1,"+ dn;
		std::string dnProductIdentityObj = genericProductIdName + dn;
		//cout << "getProductIdentityDataFromIMM dnProductIdentityObj = " << dnProductIdentityObj << endl;
		result = objectManager.getAttribute(dnProductIdentityObj.c_str(), attributes);
		if (result != ACS_CC_SUCCESS){
			//cout << "getProductIdentityDataFromIMM  objectManager.getAttribute :"  << "FAILURE"<<endl;
			//cout << "getProductIdentityDataFromIMM  error num :"  << objectManager.getInternalLastError()<<endl;
			//cout << "getProductIdentityDataFromIMM  error text :"  << objectManager.getInternalLastErrorText()<<endl;
			return -1;
		}

		/* TR HQ23444: the following if conditions line have been substituted by the next one*/
		/* To solve the Tr due to mandatory attributes for ProductIdentity object, when no data is retrieved
		 * via snmp the server sets to " " blanck space. In hwiprint command in particular for -x option when a
		 * value " " is found in IMM it is reinterpreted as empty value, giving the " " string the snmp value  */

		//if ( attribute_prodDesignation.attrValuesNum >0){
		if ( attribute_prodDesignation.attrValuesNum >0 && strcmp(reinterpret_cast<const char*>(*(attribute_prodDesignation.attrValues)) ," ")!= 0)
			productDesignation = ( reinterpret_cast<const char*>(*(attribute_prodDesignation.attrValues)) );

		/* TR HQ23444: the following if conditions line have been substituted by the next one*/
		//if ( attribute_prodNum.attrValuesNum >0){
		if ( attribute_prodNum.attrValuesNum >0 && strcmp(reinterpret_cast<const char*>(*(attribute_prodNum.attrValues)) ," ")!= 0)
			productNumber = ( reinterpret_cast<const char*>(*(attribute_prodNum.attrValues)) );

		/* TR HQ23444: the following if conditions line have been substituted by the next one*/
		//if ( attribute_prodRev.attrValuesNum >0){
		if ( attribute_prodRev.attrValuesNum >0 && strcmp(reinterpret_cast<const char*>(*(attribute_prodRev.attrValues)) ," ")!= 0)
			productRevision = ( reinterpret_cast<const char*>(*(attribute_prodRev.attrValues)) );

		/*otherwhise the three values are not modified*/


	return 0;

}
/*end added for product identity mnagement*/

/*getNamefromRDN: retrieve the Shelf name from rdn
 *
 */
std::string  getNamefromRDN(const std::string &dn){

	size_t in,fin;


//	in = dn.find_first_of("=");
//
//	fin = dn.find_first_of(",");
//
//	std::string MagAddr = dn.substr(in+1 , 7);
	in = dn.find_first_of("=");
	fin = dn.find_first_of(",");
	int len = fin-in-1;
	std::string MagAddr = "" ;

	if (len >0)//DN is shelfID=1.2.0.4,safApp=ImmService
		MagAddr = dn.substr(in+1 , len);
	else //DN is 1.2.0.4,safApp=ImmService
		MagAddr = dn.substr(0 , fin);


	//cout<<"getNamefromRDN :MagAddr = "<< MagAddr<<endl;

	return MagAddr;

}


/*
 * boardSlot = getBoardSlot(dnBoard);
 * retrieve the board id from the dn
 */

std::string getBoardSlot(std::string dnBoard){

	size_t in,fin;


//	in = dnBoard.find_first_of("=");
//
//	fin = dnBoard.find_first_of(",");
//	int len = fin-in-1;
//
//	std::string boardSlot = dnBoard.substr(in+1, len);

	in = dnBoard.find_first_of("=");
	fin = dnBoard.find_first_of(",");
	int len = fin-in-1;
	std::string boardSlot ="";
	if (len >0)//DN is boardId=23,shelfID=1.2.0.4,safApp=ImmService
		boardSlot = dnBoard.substr(in+1 , len);
	else //DN is 1.2.0.4,safApp=ImmService
		boardSlot = dnBoard.substr(0 , fin);

	return boardSlot;

}
/**
 * findInputMagazine : returns -1 when not found
 * or a value>=0 representing the position of found element in the shelf list
 */

 int findInputMagazine(std::string magName, std::vector<std::string> &shelfList ){



	int index=0;

	for (std::vector<std::string>::iterator it = shelfList.begin() ; it != shelfList.end() ; ++it ){

		/*check if the magName is preset in the list*/

		// cout << "findInputMagazine magName = " << magName << endl;
		// cout << "find Hw inventry Dn (*it)   = " << (*it) << endl;
		//dn contains shelfInfoId=<magazine>,AxeHardwareInventoryhardwareInventoryMId=1,
		// So extract magazine string from it
		size_t found;
		std::string dn = *it;
		dn = dn.substr(0,dn.find_first_of(",")); // fetch sub string shelfInfoId=<magazine>
		std::string mag_in_dn = dn.substr(dn.find("=")+1); // fetch sub string <magazine>

		if (mag_in_dn.compare(magName) == 0){
		  // cout << "found returning index =" <<index<< endl;
		  //elemFound = true;
		  return index;
		}

		index++;
		// cout << " NOT FOUND ..." << endl;

	}/*end for*/

	return -1;/*not found*/


}



 /*****************************************XML*****************************/
 /*retrieve info to generate xml file
  * return -1 if information not found */
 int retrieveXmlShelfInfo(ACS_APBM_HWIData &p_hwi, const std::string &dnMagazine, OmHandler objectManager){

 	ACS_CC_ReturnType result;
 	//std::cout<<"!!!TD retrieveXmlShelfInfo"<<std::endl;

 	/*get data HWI SHELF  for given magazine*/
 	ACS_APGCC_ImmAttribute attribute_rdn;			/*hwiShelfId*/
 	ACS_APGCC_ImmAttribute attribute_row; 			/*row*/
 	ACS_APGCC_ImmAttribute attribute_num; 			/*number*/
 	ACS_APGCC_ImmAttribute attribute_xPos; 			/*xPosition*/
 	ACS_APGCC_ImmAttribute attribute_yPos; 			/*yPosition*/
// 	ACS_APGCC_ImmAttribute attribute_prodName; 		/*productName*/
// 	ACS_APGCC_ImmAttribute attribute_prodNum;		/*productNumber*/
// 	ACS_APGCC_ImmAttribute attribute_prodRev;		/*productRevision*/
 	ACS_APGCC_ImmAttribute attribute_serNum;		/*serialNumber*/
 	ACS_APGCC_ImmAttribute attribute_supplier;		/*supplier*/
 	ACS_APGCC_ImmAttribute attribute_manufactDate;	/*manufactDate*/
 	ACS_APGCC_ImmAttribute attribute_name		;	/*name*/


 	/*set the att name*/
 	attribute_rdn.attrName = SHELF_ID_ATT_NAME.c_str();
 	attribute_row.attrName = SHELF_ROW_ATT_NAME.c_str();
 	attribute_num.attrName = SHELF_NUMBER_ATT_NAME.c_str();
 	attribute_xPos.attrName = SHELF_XPOS_ATT_NAME.c_str();
 	attribute_yPos.attrName = SHELF_YPOS_ATT_NAME.c_str();
// 	attribute_prodName.attrName = SHELF_PROD_NAME_ATT_NAME.c_str();
// 	attribute_prodNum.attrName =SHELF_PROD_NUM_ATT_NAME.c_str();
// 	attribute_prodRev.attrName =SHELF_PROD_REV_ATT_NAME.c_str();
 	attribute_serNum.attrName = SHELF_SERIAL_NUM_ATT_NAME.c_str();
 	attribute_supplier.attrName = SHELF_SUPPLIER_ATT_NAME.c_str();
 	attribute_manufactDate.attrName = SHELF_MAN_DATE_ATT_NAME.c_str();
 	attribute_name.attrName =SHELF_NAME_ATT_NAME.c_str();

 	std::vector<ACS_APGCC_ImmAttribute *> attributes;

 	/*push back on vector*/
 	attributes.push_back(&attribute_rdn);
 	attributes.push_back(&attribute_row);
 	attributes.push_back(&attribute_num);
 	attributes.push_back(&attribute_xPos);
 	attributes.push_back(&attribute_yPos);
// 	attributes.push_back(&attribute_prodName);
// 	attributes.push_back(&attribute_prodNum);
// 	attributes.push_back(&attribute_prodRev);
 	attributes.push_back(&attribute_serNum);
 	attributes.push_back(&attribute_supplier);
 	attributes.push_back(&attribute_manufactDate);
 	attributes.push_back(&attribute_name);


 	result = objectManager.getAttribute(dnMagazine.c_str(), attributes);
 	if (result != ACS_CC_SUCCESS){
 		//cout << "retrieveXmlInfo::getting data for shelf dnMagazine" << dnMagazine << "FAILURE"<<endl;
 		return -1;
 	}

 	/*set info in hwi data structure*/
 	std::string MagAddr = getNamefromRDN(dnMagazine);
 	//std::cout<<"!!!TD retrieveXmlShelfInfo 2"<<std::endl;
 	int cabRow 	= -1;
 	int cabNum 	= -1;
 	int cabXPos = -1;
 	int cabYPos = -1;

 	if ( attribute_row.attrValuesNum >0){
 		//newModel nowINT cabRow = atoi (reinterpret_cast<const char*>(*(attribute_row.attrValues)) );
 		cabRow =  *(reinterpret_cast<const int*>(*(attribute_row.attrValues))) ;
 	}


 	if ( attribute_num.attrValuesNum >0){
 		//newModel nowINT cabNum = atoi (reinterpret_cast<const char*>(*(attribute_num.attrValues)) );
 		cabNum = *(reinterpret_cast<const int*>(*(attribute_num.attrValues)) );
 	}
 	if ( attribute_xPos.attrValuesNum >0){
 		//newModel nowINT cabXPos = atoi ( reinterpret_cast<const char*>(*(attribute_xPos.attrValues)) );
 		cabXPos =  *( reinterpret_cast<const int*>(*(attribute_xPos.attrValues)) );
 	}
 	if ( attribute_yPos.attrValuesNum >0){
 		//newModel nowINT cabYPos = atoi (reinterpret_cast<const char*>(*(attribute_yPos.attrValues))  );
 		cabYPos = * ( reinterpret_cast<const int*>(*(attribute_yPos.attrValues))  );
 	}
 	/*Back Panel info*/

 	std::string Productname = "";
	std::string productNumber= "";
	std::string rev = "";
	std::string serialNo= "";
	std::string supplier= "";
	std::string name = "NOT AVAILABLE";
	std::string mandate = "";

	/*added for newModel compliance it substitute the following commeted lines*/
	getProductIdentityDataFromIMM(objectManager, dnMagazine, Productname, productNumber, rev );

//	if ( attribute_prodName.attrValuesNum >0)
//		Productname = ( reinterpret_cast<const char*>(*(attribute_prodName.attrValues)) );
//
//	if ( attribute_prodNum.attrValuesNum >0)
//		productNumber = ( reinterpret_cast<const char*>(*(attribute_prodNum.attrValues)) );
//
//	if ( attribute_prodRev.attrValuesNum >0)
//		rev = ( reinterpret_cast<const char*>(*(attribute_prodRev.attrValues)) );

	if ( attribute_serNum.attrValuesNum >0)
		serialNo = ( reinterpret_cast<const char*>(*(attribute_serNum.attrValues)) );

	if ( attribute_supplier.attrValuesNum >0)
		supplier = ( reinterpret_cast<const char*>(*(attribute_supplier.attrValues)) );

 	if (attribute_name.attrValuesNum>0)
 		name =(reinterpret_cast<const char*>(*(attribute_name.attrValues)));

 	if (attribute_manufactDate.attrValuesNum>0)
 		mandate = ( reinterpret_cast<const char*>(*(attribute_manufactDate.attrValues)) );

 	//std::cout<<"!!!TD retrieveXmlShelfInfo 3"<<std::endl;

 	if ( (p_hwi.addBackplaneData(Productname, productNumber, rev, serialNo, supplier, mandate)) != ACS_APBM_HWIData::DATA_SUCCESS ){
 		//std::cout<<"retrieveXmlInfo::hwi.addBackplaneData::FAILURE!!!!!!!! "<< std::endl;
 		return -1;
 	}

 	//std::cout<<"!!!TD retrieveXmlShelfInfo 4"<<std::endl;

 	//if ( p_hwi.addMagazinePositionData(cabRow, cabNum, cabXPos, cabYPos, name) != ACS_APBM_HWIData::DATA_SUCCESS)
 	p_hwi.addMagazinePositionData(cabRow, cabNum, cabXPos, cabYPos, name);
 	//std::cout<<"retrieveXmlInfo::hwi.addMagazinePositionData::FAILURE!!!!!!!! "<< std::endl;

 	return 0;



 }
 /*end: retrieve info to generate xml file*/

 /*Get PFM Info*/
 int retrieveXmlPFMInfo(ACS_APBM_HWIData &p_hwi, const std::string &dnMagazine, const std::string &lowerUpper, OmHandler objectManager){

 	ACS_CC_ReturnType result;
 	//std::cout<<"!!!TD retrieveXmlPFMInfo 1"<<std::endl;
 	//TODO get IMM data
 	ACS_APGCC_ImmAttribute attribute_rdn;				/*hwiShelfId*/
 	ACS_APGCC_ImmAttribute attribute_prodName; 			/*productName*/
 	ACS_APGCC_ImmAttribute attribute_prodNum;			/*productNumber*/
 	ACS_APGCC_ImmAttribute attribute_prodRev;			/*productRevision*/
 	ACS_APGCC_ImmAttribute attribute_serNum;			/*serialNumber*/
 	ACS_APGCC_ImmAttribute attribute_deviceType;		/*device Type*/
 	ACS_APGCC_ImmAttribute attribute_hardVersion;		/*hardware version*/
 	ACS_APGCC_ImmAttribute attribute_manufactDate;		/*manufactDate*/

 	/*set the att name*/
 	attribute_rdn.attrName 			= PFM_ID_ATT_NAME.c_str();
 	attribute_prodName.attrName 	= PFM_PROD_NAME_ATT_NAME.c_str();
 	attribute_prodNum.attrName 		= PFM_PROD_NUM_ATT_NAME.c_str();
 	attribute_prodRev.attrName 		= PFM_PROD_REV_ATT_NAME.c_str();
 	attribute_serNum.attrName 		= PFM_SERIAL_NUM_ATT_NAME.c_str();
 	attribute_deviceType.attrName 	= PFM_DEVICE_TYPE_ATT_NAME.c_str();
 	attribute_hardVersion.attrName 	= PFM_HW_VERSION_ATT_NAME.c_str();
 	attribute_manufactDate.attrName = PFM_MAN_DATE_ATT_NAME.c_str();


 	std::vector<ACS_APGCC_ImmAttribute *> attributes;

 	/*push back on vector*/
 	attributes.push_back(&attribute_rdn);
 	attributes.push_back(&attribute_prodName);
 	attributes.push_back(&attribute_prodNum);
 	attributes.push_back(&attribute_prodRev);
 	attributes.push_back(&attribute_serNum);
 	attributes.push_back(&attribute_deviceType);
 	attributes.push_back(&attribute_hardVersion);
 	attributes.push_back(&attribute_manufactDate);




 	/*Power Fan Module**/
 	std::string pfmInstanceU = "";
 	std::string pfmInstanceL = "";
 	std::string pfmProductName = "";
 	std::string pfmProductNumber = "";
 	std::string pfmRev = "";
 	std::string pfmSerialNo = "";
 	std::string pfmDevtype = "";
 	std::string pfmHWVer = "";
 	std::string pfmMandate = "";

// 	char lineToFill[50];
 	std::string lineToPrint= "";

 	/*get values*/
 	//4std:: string pfmUpperDn = "powerFanModuleId="+lowerUpper+","+dnMagazine;
 	std:: string pfmUpperDn = "powerFanModuleInfoId="+lowerUpper+","+dnMagazine;

 	result = objectManager.getAttribute(pfmUpperDn.c_str(), attributes);
 	if (result == ACS_CC_SUCCESS ){
 			//todo print data
 			//pfmInstanceU 		= "UPPER" ;
			if ( attribute_prodName.attrValuesNum >0)
				pfmProductName 		= reinterpret_cast<const char*>(*(attribute_prodName.attrValues)) ;

			if ( attribute_prodNum.attrValuesNum >0)
				pfmProductNumber 	= reinterpret_cast<const char*>(*(attribute_prodNum.attrValues)) ;

			if ( attribute_prodRev.attrValuesNum >0)
				pfmRev 				= reinterpret_cast<const char*>(*(attribute_prodRev.attrValues)) ;

			if ( attribute_serNum.attrValuesNum >0)
				pfmSerialNo			= reinterpret_cast<const char*>(*(attribute_serNum.attrValues)) ;

 			if (attribute_deviceType.attrValuesNum !=0){
 				//pfmDevtype			= reinterpret_cast<const char*>(*(attribute_deviceType.attrValues)) ;
 				/*now device type is enumration in IMM 1 for hod 0 for lod so it is necessary to translate the
 				 * enumaration to the proper string value*/
 				int pfmDevtypeINT	= *(reinterpret_cast<const int*>(*attribute_deviceType.attrValues)) ;
				if (pfmDevtypeINT == 1)//hod
					pfmDevtype ="hod";
				else if (pfmDevtypeINT == 0)//lod
					pfmDevtype ="lod";
 			}
 			if ( attribute_hardVersion.attrValuesNum >0)
 				pfmHWVer			= reinterpret_cast<const char*>(*(attribute_hardVersion.attrValues)) ;

 			if ( attribute_manufactDate.attrValuesNum >0)
 				pfmMandate			= reinterpret_cast<const char*>(*(attribute_manufactDate.attrValues)) ;
 		}else {
 			//std::cout<<"ERROR in retrieveXmPFMInfo!!!!!"<<std::endl;
 			return -1;
 		}


 	if (lowerUpper.compare("LOWER")==0 )
 		p_hwi.addPFMUpperLowerData(pfmProductName,
 							pfmProductNumber,
 							pfmRev,
 							pfmSerialNo,
 							pfmDevtype,
 							pfmHWVer,
 							pfmMandate,
 							false);
 	else
 		p_hwi.addPFMUpperLowerData(pfmProductName,
 			pfmProductNumber,
 			pfmRev,
 			pfmSerialNo,
 			pfmDevtype,
 			pfmHWVer,
 			pfmMandate,
 			true);

 	//std::cout<<"!!!TD retrieveXmlPFMInfo ending"<<std::endl;
 	return 0;
 }

 /*end Get PFM Info*/



 /*GET BOARD DATA*/

 int retrieveXmlBoardInfo(ACS_APBM_HWIData &p_hwi, const std::string &dnMagazine, OmHandler objectManager){

 	std::vector<std::string> boardDNList;
 	//std::cout<<"!!!TD retrieveXmlPFMInfo starting"<<std::endl;
 	getboardDNList(dnMagazine, objectManager, boardDNList);

 	if (boardDNList.size() > 0){

 		for (std::vector<std::string>::iterator boardIt = boardDNList.begin() ; boardIt != boardDNList.end() ; ++boardIt ) {

 			std::string shelfName = getNamefromRDN((*boardIt));

 			retrieveXmlSingleBoardInfo(p_hwi, (*boardIt) ,shelfName, objectManager);

 		}/*end board ITeartion*/

 	}/*no board*/
 	//std::cout<<"!!!TD retrieveXmlPFMInfo ending"<<std::endl;
 	return 0;

 }

 int retrieveXmlSingleBoardInfo(ACS_APBM_HWIData &p_hwi, const std::string &dnBoard, const std:: string & /*magazineName*/, OmHandler objectManager ){

 	ACS_CC_ReturnType result;

 	//TODO get IMM data
 	ACS_APGCC_ImmAttribute attribute_rdn;				/*hwiShelfId*/
 	ACS_APGCC_ImmAttribute attribute_xPosition;			/*xPosition*/
 	ACS_APGCC_ImmAttribute attribute_yPosition;			/*yPosition*/
// 	ACS_APGCC_ImmAttribute attribute_prodName; 			/*productName*/
// 	ACS_APGCC_ImmAttribute attribute_prodNum;			/*productNumber*/
// 	ACS_APGCC_ImmAttribute attribute_prodRev;			/*productRevision*/
 	ACS_APGCC_ImmAttribute attribute_serNum;			/*serialNumber*/
 	ACS_APGCC_ImmAttribute attribute_supplier;			/*supplier*/
 	ACS_APGCC_ImmAttribute attribute_busType;			/*busType*/
 	ACS_APGCC_ImmAttribute attribute_manufactDate;		/*manufactDate*/

 	/*set the att name*/
 	attribute_rdn.attrName 			= BOARD_ID_ATT_NAME .c_str();
 	attribute_xPosition.attrName 	= BOARD_XPOS_ATT_NAME.c_str();
 	attribute_yPosition.attrName 	= BOARD_YPOS_ATT_NAME.c_str();
// 	attribute_prodName.attrName 	= BOARD_PROD_NAME_ATT_NAME.c_str();
// 	attribute_prodNum.attrName 		= BOARD_PROD_NUM_ATT_NAME.c_str();
// 	attribute_prodRev.attrName 		= BOARD_PROD_REV_ATT_NAME.c_str();
 	attribute_serNum.attrName 		= BOARD_SERIAL_NUM_ATT_NAME.c_str();
 	attribute_supplier.attrName 	= BOARD_SUPPLIER_ATT_NAME.c_str();
 	attribute_busType.attrName 		= BOARD_BUS_TYPE_ATT_NAME.c_str();
 	attribute_manufactDate.attrName = BOARD_MAN_DATE_ATT_NAME.c_str();




 	std::vector<ACS_APGCC_ImmAttribute *> attributes;

 	/*push back on vector*/
 	attributes.push_back(&attribute_rdn);
 	attributes.push_back(&attribute_xPosition);
 	attributes.push_back(&attribute_yPosition);
// 	attributes.push_back(&attribute_prodName);
// 	attributes.push_back(&attribute_prodNum);
// 	attributes.push_back(&attribute_prodRev);
 	attributes.push_back(&attribute_serNum);
 	attributes.push_back(&attribute_supplier);
 	attributes.push_back(&attribute_busType);
 	attributes.push_back(&attribute_manufactDate);

 	/*Board slot data*/
 	std::string boardProdName = "";
 	std::string boardProdNo = "";
 	std::string boardRev = "";
 	std::string boardSerialNum = "";
 	std::string boardSupplier = "";
 	std::string boardBusType = "";
 	std::string boardMandate = "";



 	result = objectManager.getAttribute(dnBoard.c_str(), attributes);

 	if (result == ACS_CC_SUCCESS ){
 	int boardSlot  = 0;
 	int boardXPos  = -1;
 	int  boardYPos = -1;
 		//TODO the following method
 		boardSlot = atoi((getBoardSlot(dnBoard)).c_str());

 		if ( attribute_xPosition.attrValuesNum >0){
 			//newModel now INT boardXPos = atoi (reinterpret_cast<const char*>(*(attribute_xPosition.attrValues))) ;
 			boardXPos =  *(reinterpret_cast<const int*>(*attribute_xPosition.attrValues)) ;
 		}
 		if ( attribute_yPosition.attrValuesNum >0)
 			//newModel now INT  boardYPos = atoi (reinterpret_cast<const char*>(*(attribute_yPosition.attrValues)) ) ;
 			boardYPos =  *(reinterpret_cast<const int*>(*attribute_yPosition.attrValues) ) ;

 		/*added for newModel compliance it substitute the following commeted lines*/
 		getProductIdentityDataFromIMM(objectManager, dnBoard, boardProdName, boardProdNo, boardRev );

// 		if ( attribute_prodName.attrValuesNum >0)
// 			boardProdName 	= reinterpret_cast<const char*>(*(attribute_prodName.attrValues)) ;
//
// 		if ( attribute_prodNum.attrValuesNum >0)
// 			boardProdNo 	= reinterpret_cast<const char*>(*(attribute_prodNum.attrValues)) ;
//
// 		if ( attribute_prodRev.attrValuesNum >0)
// 			boardRev 		= reinterpret_cast<const char*>(*(attribute_prodRev.attrValues)) ;

 		if ( attribute_serNum.attrValuesNum >0)
 			boardSerialNum	= reinterpret_cast<const char*>(*(attribute_serNum.attrValues)) ;

 		if ( attribute_supplier.attrValuesNum >0)
 			boardSupplier	= reinterpret_cast<const char*>(*(attribute_supplier.attrValues)) ;

 		if ( attribute_busType.attrValuesNum >0){
 			//new model boardBusType	= reinterpret_cast<const char*>(*(attribute_busType.attrValues)) ;
 			/*now bus type in IMMis enumerated 1 for IPMI 2 for MBUS so it is necessary to translate the
 			 * enumetation to a string*/
 			int boardBusTypeInt	= *(reinterpret_cast<const int*>(*attribute_busType.attrValues)) ;
			if ( boardBusTypeInt == 1 )//IPMI
				boardBusType ="ipmi";
			else if (boardBusTypeInt == 2 )//MBUS
				boardBusType ="mbus";
 		}

 		if ( attribute_manufactDate.attrValuesNum >0)
 			boardMandate	= reinterpret_cast<const char*>(*(attribute_manufactDate.attrValues)) ;



 		//if ( (p_hwi.addBoardData(boardSlot, boardProdNo, boardProdName, boardRev,  boardSerialNum, boardMandate, boardMandate, boardBusType)) != ACS_APBM_HWIData::DATA_SUCCESS )
 		if ( (p_hwi.addBoardData(boardSlot, boardProdNo, boardProdName, boardRev,  boardSerialNum, boardMandate, boardSupplier, boardBusType)) != ACS_APBM_HWIData::DATA_SUCCESS ){
 			//std::cout<<"retrieveXmlSingleBoardInfo::p_hwi.addBoardData::FAILURE!!!!!!!! "<< std::endl;
 			return -1;
 		}

 		if ( (p_hwi.addSlotPosData(boardSlot, boardXPos, boardYPos)  ) != ACS_APBM_HWIData::DATA_SUCCESS )
 				//std::cout<<"retrieveXmlSingleBoardInfo::p_hwi.addSlotPosData::FAILURE!!!!!!!! "<< std::endl;
 		return 0;


 	}
 	//std::cout<<"retrieveXmlSingleBoardInfo::getAttribute FAILURE!!!!!!!! "<< std::endl;
 	return -1;

 }
 /*end GET BOARD DATA*/
 /************************** END XML******************/

/**
 * print Magazine data: input the magazine name
 */

int printMagazineData(const std::string &dnMagazine, OmHandler objectManager, int cmd){
	ACS_CC_ReturnType result;

	ACS_APGCC_ImmAttribute attribute_rdn;			/*hwiShelfId*/
	ACS_APGCC_ImmAttribute attribute_row; 			/*row*/
	ACS_APGCC_ImmAttribute attribute_num; 			/*number*/
	ACS_APGCC_ImmAttribute attribute_xPos; 			/*xPosition*/
	ACS_APGCC_ImmAttribute attribute_yPos; 			/*yPosition*/
//	ACS_APGCC_ImmAttribute attribute_prodName; 		/*productName*/
//	ACS_APGCC_ImmAttribute attribute_prodNum;		/*productNumber*/
//	ACS_APGCC_ImmAttribute attribute_prodRev;		/*productRevision*/
	ACS_APGCC_ImmAttribute attribute_serNum;		/*serialNumber*/
	ACS_APGCC_ImmAttribute attribute_supplier;		/*supplier*/
	ACS_APGCC_ImmAttribute attribute_manufactDate;	/*manufactDate*/
	ACS_APGCC_ImmAttribute attribute_name		;	/*name*/


	/*set the att name*/
	attribute_rdn.attrName = SHELF_ID_ATT_NAME.c_str();
	attribute_row.attrName = SHELF_ROW_ATT_NAME.c_str();
	attribute_num.attrName = SHELF_NUMBER_ATT_NAME.c_str();
	attribute_xPos.attrName = SHELF_XPOS_ATT_NAME.c_str();
	attribute_yPos.attrName = SHELF_YPOS_ATT_NAME.c_str();
//	attribute_prodName.attrName = SHELF_PROD_NAME_ATT_NAME.c_str();
//	attribute_prodNum.attrName =SHELF_PROD_NUM_ATT_NAME.c_str();
//	attribute_prodRev.attrName =SHELF_PROD_REV_ATT_NAME.c_str();
	attribute_serNum.attrName = SHELF_SERIAL_NUM_ATT_NAME.c_str();
	attribute_supplier.attrName = SHELF_SUPPLIER_ATT_NAME.c_str();
	attribute_manufactDate.attrName = SHELF_MAN_DATE_ATT_NAME.c_str();
	attribute_name.attrName =SHELF_NAME_ATT_NAME.c_str();

	std::vector<ACS_APGCC_ImmAttribute *> attributes;

	/*push back on vector*/
	attributes.push_back(&attribute_rdn);
	attributes.push_back(&attribute_row);
	attributes.push_back(&attribute_num);
	attributes.push_back(&attribute_xPos);
	attributes.push_back(&attribute_yPos);
//	attributes.push_back(&attribute_prodName);
//	attributes.push_back(&attribute_prodNum);
//	attributes.push_back(&attribute_prodRev);
	attributes.push_back(&attribute_serNum);
	attributes.push_back(&attribute_supplier);
	attributes.push_back(&attribute_manufactDate);
	attributes.push_back(&attribute_name);

	/*get values*/
//	cout << "getting data for shelf dnMagazine" << dnMagazine << endl;
	result = objectManager.getAttribute(dnMagazine.c_str(), attributes);
	if (result != ACS_CC_SUCCESS){
//		cout << "getting data for shelf dnMagazine" << dnMagazine << "FAILURE"<<endl;
		//std::cout<<"objectManager.getInternalLastError() = "<<objectManager.getInternalLastError()<< std::endl;
		//std::cout<<"objectManager.getInternalLastErrorText() = "<<objectManager.getInternalLastErrorText()<<std::endl;

		std::cout<<EXIT_CODE_1_TEXT<<std::endl;
		exit (EXIT_CODE_1);
	}



	char lineToFill[50];
	std::string lineToPrint= "";

	/*Data Magazine info*/


	std::string MagAddr = getNamefromRDN(dnMagazine);

	std::string cabRow("-");
	std::string cabNum("-");
	std::string cabXPos("-");
	std::string cabYPos("-");
//newModel added
	char bufferRow[4] ;
	char bufferNum[4] ;
	char bufferXPos[4] ;
	char bufferYPos[4] ;
//newModel added end
	if ( attribute_row.attrValuesNum >0){
		//newModel cabRow = reinterpret_cast<const char*>(*(attribute_row.attrValues)) ;
		int cabRowInt = *(reinterpret_cast<const int*>(*(attribute_row.attrValues)));
		sprintf(bufferRow, "%d", cabRowInt);
		cabRow.assign(bufferRow);

	}
	if ( attribute_num.attrValuesNum >0){
		//newModel cabNum = reinterpret_cast<const char*>(*(attribute_num.attrValues)) ;
		int cabNumInt = *(reinterpret_cast<const int*>(*(attribute_num.attrValues)));
		sprintf(bufferNum, "%d", cabNumInt);
		cabNum.assign(bufferNum);
	}
	if ( attribute_xPos.attrValuesNum >0){
		//newModel cabXPos = reinterpret_cast<const char*>(*(attribute_xPos.attrValues)) ;
		int cabXPosInt = *(reinterpret_cast<const int*>(*(attribute_xPos.attrValues))) ;
		sprintf(bufferXPos, "%d", cabXPosInt);
		cabXPos.assign(bufferXPos);
	}
	if ( attribute_yPos.attrValuesNum >0){
		//newModel cabYPos = reinterpret_cast<const char*>(*(attribute_yPos.attrValues))  ;
		int cabYPosInt = *(reinterpret_cast<const int*>(*(attribute_yPos.attrValues)));
		sprintf(bufferYPos, "%d",  cabYPosInt);
		cabYPos.assign(bufferYPos);
	}
	/*Back Panel info*/
	std::string Productname = "-";
	std::string productNumber = "-";
	std::string rev = "-";
	std::string serialNo = "-";
	std::string supplier = "-";
	std::string mandate = "-";

	/*added for newModel compliance it substitute the following commeted lines*/
	getProductIdentityDataFromIMM(objectManager, dnMagazine, Productname, productNumber, rev );

//	if ( attribute_prodName.attrValuesNum >0)
//		Productname = ( reinterpret_cast<const char*>(*(attribute_prodName.attrValues)) );
//
//	if ( attribute_prodNum.attrValuesNum >0)
//		productNumber = ( reinterpret_cast<const char*>(*(attribute_prodNum.attrValues)) );
//
//	if ( attribute_prodRev.attrValuesNum >0)
//		rev = ( reinterpret_cast<const char*>(*(attribute_prodRev.attrValues)) );

	if ( attribute_serNum.attrValuesNum >0)
		serialNo = ( reinterpret_cast<const char*>(*(attribute_serNum.attrValues)) );

	if ( attribute_supplier.attrValuesNum >0)
		supplier = ( reinterpret_cast<const char*>(*(attribute_supplier.attrValues)) );

	if ( attribute_manufactDate.attrValuesNum >0)
		mandate = ( reinterpret_cast<const char*>(*(attribute_manufactDate.attrValues)) );

	printf("MAGAZINE INFO\n");
	printf("MAGADDR   CABROW   CABNO   XPOS   YPOS \n");

	sprintf(lineToFill,"%-10s", MagAddr.c_str());
	lineToPrint+= lineToFill;

	sprintf(lineToFill,"%-9s", cabRow.c_str());
	lineToPrint+= lineToFill;

	sprintf(lineToFill,"%-8s", cabNum.c_str());
	lineToPrint+= lineToFill;

	sprintf(lineToFill,"%-7s", cabXPos.c_str());
	lineToPrint+= lineToFill;

	sprintf(lineToFill,"%-7s", cabYPos.c_str());

	lineToPrint+= lineToFill;

	printf("%s", lineToPrint.c_str());
	//printf("%s\n", lineToPrint.c_str());

	lineToPrint.clear();

	/*to be printed only if option different from -c is provided ===> cmd = 24*/
	if (cmd == 24){

		printf("\n\nBACKPANE INFO\n");

		printf("MAGADDR   PRODUCTNAME   PRODUCTNO    REV   SERIALNO   SUPPLIER     MANDATE\n");

		sprintf(lineToFill,"%-10s", MagAddr.c_str());
		lineToPrint+= lineToFill;

		sprintf(lineToFill,"%-14s", Productname.c_str());
		lineToPrint+= lineToFill;

		//sprintf(lineToFill,"%-12s", productNumber.c_str());
		sprintf(lineToFill,"%-13s", productNumber.c_str());
		lineToPrint+= lineToFill;

		sprintf(lineToFill,"%-6s", rev.c_str());
		lineToPrint+= lineToFill;

		sprintf(lineToFill,"%-11s", serialNo.c_str());
		lineToPrint+= lineToFill;

		sprintf(lineToFill,"%-13s", supplier.c_str());
		lineToPrint+= lineToFill;

		sprintf(lineToFill,"%-7s", mandate.c_str());
		lineToPrint+= lineToFill;

		printf("%s\n", lineToPrint.c_str());
		lineToPrint.clear();

		printf("%s\n", lineToPrint.c_str());

		lineToPrint.clear();

	}

	return 0;

}


/*
 * print Power Fan Module info
 */

void printPFMDataLowerUpper(const std::string &dnMagazine, OmHandler objectManager,const std::string &lowerUpper){

	ACS_CC_ReturnType result;

	//TODO get IMM data
	ACS_APGCC_ImmAttribute attribute_rdn;				/*hwiShelfId*/
	ACS_APGCC_ImmAttribute attribute_prodName; 			/*productName*/
	ACS_APGCC_ImmAttribute attribute_prodNum;			/*productNumber*/
	ACS_APGCC_ImmAttribute attribute_prodRev;			/*productRevision*/
	ACS_APGCC_ImmAttribute attribute_serNum;			/*serialNumber*/
	ACS_APGCC_ImmAttribute attribute_deviceType;		/*device Type*/
	ACS_APGCC_ImmAttribute attribute_hardVersion;		/*hardware version*/
	ACS_APGCC_ImmAttribute attribute_manufactDate;		/*manufactDate*/

	/*set the att name*/
	attribute_rdn.attrName 			= PFM_ID_ATT_NAME.c_str();
	attribute_prodName.attrName 	= PFM_PROD_NAME_ATT_NAME.c_str();
	attribute_prodNum.attrName 		= PFM_PROD_NUM_ATT_NAME.c_str();
	attribute_prodRev.attrName 		= PFM_PROD_REV_ATT_NAME.c_str();
	attribute_serNum.attrName 		= PFM_SERIAL_NUM_ATT_NAME.c_str();
	attribute_deviceType.attrName 	= PFM_DEVICE_TYPE_ATT_NAME.c_str();
	attribute_hardVersion.attrName 	= PFM_HW_VERSION_ATT_NAME.c_str();
	attribute_manufactDate.attrName = PFM_MAN_DATE_ATT_NAME.c_str();


	std::vector<ACS_APGCC_ImmAttribute *> attributes;

	/*push back on vector*/
	attributes.push_back(&attribute_rdn);
	attributes.push_back(&attribute_prodName);
	attributes.push_back(&attribute_prodNum);
	attributes.push_back(&attribute_prodRev);
	attributes.push_back(&attribute_serNum);
	attributes.push_back(&attribute_deviceType);
	attributes.push_back(&attribute_hardVersion);
	attributes.push_back(&attribute_manufactDate);




	/*Power Fan Module**/
	std::string pfmInstanceU;
	std::string pfmInstanceL;
	std::string pfmProductName = "-";
	std::string pfmProductNumber = "-";
	std::string pfmRev = "-";
	std::string pfmSerialNo = "-";
	std::string pfmDevtype = "-";
	std::string pfmHWVer = "-";
	std::string pfmMandate = "-";

	char lineToFill[50];
	std::string lineToPrint= "";

	/*get values*/
	//cout << "getting data for PFM UPPER"<<endl;
	//4std:: string pfmUpperDn = "powerFanModuleId="+lowerUpper+","+dnMagazine;
	std:: string pfmUpperDn = "powerFanModuleInfoId="+lowerUpper+","+dnMagazine;

	//printf("\nPOWER AND FAN MODULE INFO\n");

	result = objectManager.getAttribute(pfmUpperDn.c_str(), attributes);
	if (result == ACS_CC_SUCCESS ){
		//todo print data
		//pfmInstanceU 		= "UPPER" ;
		if ( attribute_prodName.attrValuesNum >0)
			pfmProductName 		= reinterpret_cast<const char*>(*(attribute_prodName.attrValues)) ;

		if ( attribute_prodNum.attrValuesNum >0)
			pfmProductNumber 	= reinterpret_cast<const char*>(*(attribute_prodNum.attrValues)) ;

		if ( attribute_prodRev.attrValuesNum >0)
			pfmRev 				= reinterpret_cast<const char*>(*(attribute_prodRev.attrValues)) ;

		if ( attribute_serNum.attrValuesNum >0)
			pfmSerialNo			= reinterpret_cast<const char*>(*(attribute_serNum.attrValues)) ;

		if ( attribute_deviceType.attrValuesNum >0){
			//newModel pfmDevtype			= reinterpret_cast<const char*>(*(attribute_deviceType.attrValues)) ;
			int pfmDevtypeINT	= *(reinterpret_cast<const int*>(*attribute_deviceType.attrValues)) ;
			if (pfmDevtypeINT == 1)//hod
				pfmDevtype ="hod";
			else if (pfmDevtypeINT == 0)//lod
				pfmDevtype ="lod";
		}

		if ( attribute_hardVersion.attrValuesNum >0)
			pfmHWVer			= reinterpret_cast<const char*>(*(attribute_hardVersion.attrValues)) ;

		if ( attribute_manufactDate.attrValuesNum >0)
			pfmMandate			= reinterpret_cast<const char*>(*(attribute_manufactDate.attrValues)) ;

		printf("MAGADDR   INSTANCE   PRODUCTNAME   PRODUCTNO   REV\n");

		sprintf(lineToFill,"%-10s", getNamefromRDN(dnMagazine).c_str());
		lineToPrint+= lineToFill;

		//sprintf(lineToFill,"%-11s", pfmInstanceU.c_str());
		sprintf(lineToFill,"%-11s", lowerUpper.c_str());
		lineToPrint+= lineToFill;

		sprintf(lineToFill,"%-14s", pfmProductName.c_str());
		lineToPrint+= lineToFill;

		sprintf(lineToFill,"%-12s", pfmProductNumber.c_str());
		lineToPrint+= lineToFill;

		sprintf(lineToFill,"%s", pfmRev.c_str());
		lineToPrint+= lineToFill;

		printf("%s\n", lineToPrint.c_str());
		lineToPrint.clear();

		//printf("          SERIALNO      DEVTYPE     HWVER   MANDATE");
		printf("                     SERIALNO      DEVTYPE     HWVER   MANDATE");

		sprintf(lineToFill,"\n                     " );
		lineToPrint+= lineToFill;

		//sprintf(lineToFill,"%-14s", pfmSerialNo.c_str());
		sprintf(lineToFill,"%-14s", pfmSerialNo.c_str());
		lineToPrint+= lineToFill;

		sprintf(lineToFill,"%-12s", pfmDevtype.c_str());
		lineToPrint+= lineToFill;

		sprintf(lineToFill,"%-8s", pfmHWVer.c_str());
		lineToPrint+= lineToFill;

		sprintf(lineToFill,"%s", pfmMandate.c_str());
		lineToPrint+= lineToFill;

		printf("%s\n", lineToPrint.c_str());

		lineToPrint.clear();


	}

}/*end print PFM DATA*/



/*****************************************************************/
void printPFMData(const std::string &dnMagazine, OmHandler objectManager){

	printf("POWER AND FAN MODULE INFO\n");

	printPFMDataLowerUpper(dnMagazine, objectManager,  "LOWER");

	printf("\n");

	printPFMDataLowerUpper(dnMagazine, objectManager, "UPPER");


}
/********************************************************************/

/*
 * prints board data
 *
 * */

void printBoardData(const std::string &dnBoard,const std:: string &magazineName, OmHandler objectManager, int cmd, bool& titleAlreadyprintedForCOption){
	//cout << " Start !" << endl; // prints

		ACS_CC_ReturnType result;

		//TODO get IMM data
		ACS_APGCC_ImmAttribute attribute_rdn;				/*hwiShelfId*/
		ACS_APGCC_ImmAttribute attribute_xPosition;			/*xPosition*/
		ACS_APGCC_ImmAttribute attribute_yPosition;			/*yPosition*/
//		ACS_APGCC_ImmAttribute attribute_prodName; 			/*productName*/
//		ACS_APGCC_ImmAttribute attribute_prodNum;			/*productNumber*/
//		ACS_APGCC_ImmAttribute attribute_prodRev;			/*productRevision*/
		ACS_APGCC_ImmAttribute attribute_serNum;			/*serialNumber*/
		ACS_APGCC_ImmAttribute attribute_supplier;			/*supplier*/
		ACS_APGCC_ImmAttribute attribute_busType;			/*busType*/
		ACS_APGCC_ImmAttribute attribute_manufactDate;		/*manufactDate*/

		/*set the att name*/
		attribute_rdn.attrName 			= BOARD_ID_ATT_NAME .c_str();
		attribute_xPosition.attrName 	= BOARD_XPOS_ATT_NAME.c_str();
		attribute_yPosition.attrName 	= BOARD_YPOS_ATT_NAME.c_str();
//		attribute_prodName.attrName 	= BOARD_PROD_NAME_ATT_NAME.c_str();
//		attribute_prodNum.attrName 		= BOARD_PROD_NUM_ATT_NAME.c_str();
//		attribute_prodRev.attrName 		= BOARD_PROD_REV_ATT_NAME.c_str();
		attribute_serNum.attrName 		= BOARD_SERIAL_NUM_ATT_NAME.c_str();
		attribute_supplier.attrName 	= BOARD_SUPPLIER_ATT_NAME.c_str();
		attribute_busType.attrName 		= BOARD_BUS_TYPE_ATT_NAME.c_str();
		attribute_manufactDate.attrName = BOARD_MAN_DATE_ATT_NAME.c_str();




		std::vector<ACS_APGCC_ImmAttribute *> attributes;

		/*push back on vector*/
		attributes.push_back(&attribute_rdn);
		attributes.push_back(&attribute_xPosition);
		attributes.push_back(&attribute_yPosition);
//		attributes.push_back(&attribute_prodName);
//		attributes.push_back(&attribute_prodNum);
//		attributes.push_back(&attribute_prodRev);
		attributes.push_back(&attribute_serNum);
		attributes.push_back(&attribute_supplier);
		attributes.push_back(&attribute_busType);
		attributes.push_back(&attribute_manufactDate);

		/*Board slot data*/
		std::string boardSlot;
		std::string boardXPos("-");
		std::string boardYPos("-");
		std::string boardProdName("-");
		std::string boardProdNo("-");
		std::string boardRev("-");
		std::string boardSerialNum("-");
		std::string boardSupplier("-");
		std::string boardBusType("-");
		std::string boardMandate("-");

		char lineToFill[50];
		std::string lineToPrint= "";
		bool xyPosDataPresent = false;

		/*added newModel*/
		char bufferXPos[4];
		char bufferYPos[4];
		/*end added newModel*/

		result = objectManager.getAttribute(dnBoard.c_str(), attributes);

		if (result == ACS_CC_SUCCESS ){
			//TODO the following method
			boardSlot = getBoardSlot(dnBoard);

			if ( attribute_xPosition.attrValuesNum >0){
				//newModel boardXPos = reinterpret_cast<const char*>(*(attribute_xPosition.attrValues)) ;
				int boardXPosInt = *(reinterpret_cast<const int*>(*(attribute_xPosition.attrValues)));
				sprintf(bufferXPos, "%d", boardXPosInt);
				boardXPos.assign(bufferXPos);

				xyPosDataPresent = true;
			}

			if ( attribute_yPosition.attrValuesNum >0){
				//newModel boardYPos = reinterpret_cast<const char*>(*(attribute_yPosition.attrValues)) ;
				int boardYPosInt = *(reinterpret_cast<const int*>(*(attribute_yPosition.attrValues)));
				sprintf(bufferYPos, "%d", boardYPosInt);
				boardYPos.assign(bufferYPos);

				xyPosDataPresent = true;
			}

/*added for newModel compliance it substitute the following commeted lines*/
			getProductIdentityDataFromIMM(objectManager, dnBoard, boardProdName, boardProdNo, boardRev );


//			if ( attribute_prodName.attrValuesNum >0)
//				boardProdName 	= reinterpret_cast<const char*>(*(attribute_prodName.attrValues)) ;
//
//			if ( attribute_prodNum.attrValuesNum >0)
//				boardProdNo 	= reinterpret_cast<const char*>(*(attribute_prodNum.attrValues)) ;
//
//			if ( attribute_prodRev.attrValuesNum >0)
//				boardRev 		= reinterpret_cast<const char*>(*(attribute_prodRev.attrValues)) ;

			if ( attribute_serNum.attrValuesNum >0)
				boardSerialNum	= reinterpret_cast<const char*>(*(attribute_serNum.attrValues)) ;

			if ( attribute_supplier.attrValuesNum >0)
				boardSupplier	= reinterpret_cast<const char*>(*(attribute_supplier.attrValues)) ;

			if ( attribute_busType.attrValuesNum >0){
				//newModel boardBusType	= reinterpret_cast<const char*>(*(attribute_busType.attrValues)) ;
				int boardBusTypeInt	= *(reinterpret_cast<const int*>(*attribute_busType.attrValues)) ;
				//cout<<"TODELETE: attribute_busType.attrValuesNum ="<<attribute_busType.attrValuesNum<<" boardBusTypeInt ="<<boardBusTypeInt<<endl;
				//cout<<"TODELETE: *attribute_busType.attrValues ="<<*attribute_busType.attrValues<<endl;
				if ( boardBusTypeInt == 1 )//IPMI
					boardBusType ="ipmi";
				else if (boardBusTypeInt == 2 )//MBUS
					boardBusType ="mbus";

			}

			if ( attribute_manufactDate.attrValuesNum >0)
				boardMandate	= reinterpret_cast<const char*>(*(attribute_manufactDate.attrValues)) ;

			if (cmd == 24){/* all info to be printed*/

				printf("MAGADDR   SLOT   XPOS   YPOS   PRODUCTNAME   PRODUCTNO     REV\n");

//xyPosData			}/*else
//xyPosData				printf("MAGADDR   SLOT   XPOS   YPOS\n");*/

			sprintf(lineToFill,"%-10s", magazineName.c_str());
			lineToPrint+= lineToFill;

			sprintf(lineToFill,"%-7s", boardSlot.c_str());
			lineToPrint+= lineToFill;

			sprintf(lineToFill,"%-7s", boardXPos.c_str());
			lineToPrint+= lineToFill;

			sprintf(lineToFill,"%-7s", boardYPos.c_str());
			lineToPrint+= lineToFill;

//xyPosData			if (cmd == 24){

				sprintf(lineToFill,"%-14s", boardProdName.c_str());
				lineToPrint+= lineToFill;

				sprintf(lineToFill,"%-14s", boardProdNo.c_str());
				lineToPrint+= lineToFill;

				sprintf(lineToFill,"%-14s", boardRev.c_str());
				lineToPrint+= lineToFill;

				printf("%s\n", lineToPrint.c_str());
				lineToPrint.clear();

				printf("                               SERIALNO      SUPPLIER      BUSTYPE   MANDATE\n");
				sprintf(lineToFill,"                               " );

				lineToPrint+= lineToFill;

				sprintf(lineToFill,"%-14s", boardSerialNum.c_str());
				lineToPrint+= lineToFill;

				sprintf(lineToFill,"%-14s",boardSupplier.c_str());
				lineToPrint+= lineToFill;

				sprintf(lineToFill,"%-10s",boardBusType.c_str());
				lineToPrint+= lineToFill;

				sprintf(lineToFill,"%-10s",boardMandate.c_str());
				lineToPrint+= lineToFill;

				printf("%s\n", lineToPrint.c_str());


			}else if (cmd == 26){/*only data inserted by user should be printed, if present**/


				if 	(xyPosDataPresent){

					if ( !titleAlreadyprintedForCOption  ){
						sprintf(lineToFill,"\n\nSLOT POSITION\n");
						lineToPrint+= lineToFill;
						sprintf(lineToFill,"MAGADDR   SLOT   XPOS   YPOS\n");
						lineToPrint+= lineToFill;
						titleAlreadyprintedForCOption = true;
					}


					//sprintf(lineToFill,"MAGADDR   SLOT   XPOS   YPOS\n");
					//lineToPrint+= lineToFill;

					sprintf(lineToFill,"%-10s", magazineName.c_str());
					lineToPrint+= lineToFill;

					sprintf(lineToFill,"%-7s", boardSlot.c_str());
					lineToPrint+= lineToFill;

					sprintf(lineToFill,"%-7s", boardXPos.c_str());
					lineToPrint+= lineToFill;

					sprintf(lineToFill,"%-7s", boardYPos.c_str());
					lineToPrint+= lineToFill;

					printf("%s\n", lineToPrint.c_str());



				}/*in this case -c option is provided and only user insered data are printed data */


			}

			//printf("%s\n", lineToPrint.c_str());

			lineToPrint.clear();


		}
}
/*
 * gets the list of Boards DN once the DN of the Shelf is provided
 */
void getboardDNList(const std::string &dnMagazine,OmHandler objectManager, std::vector<string>& boardDNList){

	ACS_CC_ReturnType result;
	std::vector<std::string> childList;

	acs_apgcc_paramhandling PHA;
	char char_value[256]= {0};


	/*retrieve the list of children*/

	result = objectManager.getChildren(dnMagazine.c_str(), ACS_APGCC_SUBLEVEL, &childList);

	if (result != ACS_CC_SUCCESS) {
		//cout << "ERROR: getChildren FAILURE!!!\n";
		std::cout<<EXIT_CODE_1_TEXT<<std::endl;
		exit (EXIT_CODE_1);

		return;
	}

//	cout << "childList size " << childList.size() << std::endl;


	/*for each element checks if is instance of board class*/
	for (std::vector<std::string>::iterator it = childList.begin() ; it != childList.end() ; ++it ){

		result = PHA.getParameter<256>((*it).c_str(),"SaImmAttrClassName",char_value);
		//22if (strcmp(char_value, "BoardInventory")==0)
		//7 if (strcmp(char_value, "BladeInfo")==0)
		if (strcmp(char_value, BLADE_INFO_CLASS_NAME)==0)
	//if (strcmp(char_value, "Board")==0)
			boardDNList.push_back((*it));

	}

	/*******To delete start ************/
//	std::cout<<"IN getboardDNList"<<std::endl;
//	for (unsigned int i= 0 ; i < boardDNList.size(); i++ ){
//
//		std::cout<<"boardDNList["<< i << "]" <<" = "<< boardDNList[i]<<std::endl;
//	}
//
//	int j = strcmp("1", "11");
//	std::cout<<"IN getboardDNList strcmp(1, 11) = "<<j<< std::endl;
//	std::cout<<"IN getboardDNList Exiting"<<std::endl;

	/*******to delete end     *********/

}

/*
 * gets the list of Boards DN once the DN of the Shelf is provided and order it by Board ID
 */
void getboardDNListOrdered(const std::string &dnMagazine,OmHandler objectManager, std::vector<string>& boardDNList){

	ACS_CC_ReturnType result;
	std::vector<std::string> childList;

	acs_apgcc_paramhandling PHA;
	char char_value[256] = {0};

	std::map<int, string> mapName;


	/*retrieve the list of children*/

	result = objectManager.getChildren(dnMagazine.c_str(), ACS_APGCC_SUBLEVEL, &childList);

	if (result != ACS_CC_SUCCESS) {
		//cout << "ERROR: getChildren FAILURE!!!\n";
		std::cout<<EXIT_CODE_1_TEXT<<std::endl;
		exit (EXIT_CODE_1);

		return;
	}


	/*for each element checks if is instance of board class*/
	for (std::vector<std::string>::iterator it = childList.begin() ; it != childList.end() ; ++it ){

		result = PHA.getParameter<256>((*it).c_str(),"SaImmAttrClassName",  char_value);
		//2if (strcmp(char_value, "BoardInventory")==0)
		//7 if (strcmp(char_value, "BladeInfo")==0)
		if (strcmp(char_value, BLADE_INFO_CLASS_NAME)==0)
		//if (strcmp(char_value, "Board")==0)

		mapName.insert(make_pair( atoi((getBoardSlot(*it)).c_str()), (*it) ));


	}

	map<int,string>::iterator iter;
	for( iter = mapName.begin(); iter != mapName.end(); ++iter ) {
	 // cout << "Key: '" << iter->first << "', Value: " << iter->second << endl;
	  boardDNList.push_back(iter->second);
	}


}



/*********************************************************/
bool checkFigures(std::istringstream& istr ){
	//std::cout<<"IN .........checkFigures" <<std::endl;

	istr.clear();
	(void)istr.seekg(0);
	char c = istr.get();

	while (c != EOF){
		if ( (c < '0' ) || ( c > '9'))
			return false;

		c = istr.get();

	}
	istr.clear();
	(void)istr.seekg(0);
	return true;


}

bool isCorrectMagAddr(std::string inVal){

	char* string1 =const_cast<char*>(inVal.c_str());
	bool isCorrect = true;
	int howManyTokens = 0;
	long tempLong =0;

	char * pch;
	//4pch = strtok(string1, "._");
	pch = strtok(string1, ".");
	while(pch!=NULL && isCorrect){

		std::string token(pch);
		std::istringstream converter (token);
		if (!checkFigures(converter))
		{
			//std::cout<<"ATTENZIONE: pch" << pch<<std::endl;
			//std::cout<<"checkFigures FALSE" <<std::endl;

//			std::cout<<EXIT_CODE_18_TEXT<<std::endl;
//			return (EXIT_CODE_18);
			return false;

		}

		tempLong = strtol(pch, (char**)NULL, 10);
//			std::cout<<"ATTENZIONE: tempLong"<< tempLong<<"ERRNO="<< errno<<std::endl;
//		if (errno == 22)
//			std::cout<<"ATTENZIONE:LETTERA" <<std::endl;

		if (howManyTokens==0 || howManyTokens==1 ||howManyTokens == 3){

			if (    ! (  (tempLong >-1) && (tempLong <16)   )   )
				isCorrect = false;

		}else if (howManyTokens==2 ){
			if ( tempLong != 0 )
				isCorrect = false;

		}else {//more than 4 token
			isCorrect = false;
		}
		howManyTokens++;
		//4pch =strtok(NULL, "._");
		pch =strtok(NULL, ".");
	}

		//TR if ( !isCorrect ){
	if ( !isCorrect || howManyTokens < 4 ){
		/*it must check if less 4 tokens were found: the filter should be xx.yy.zz.tt less token are an error
		 * even if the related check were ok*/
//			std::cout<<"howManyTokens="<<howManyTokens<<std::endl;
//			return (EXIT_CODE_18);
			return false;

		}

	return true;

}
/*********************************************************/

//********************************************************
static void printUsage();

//********************************************************


int main(int argc, char* argv[]) {

	//cout << " Start 1 !" << endl; // prints

	/*******/
//		char* string1 = strdup(argv[1]);
//		bool isCorrect = true;
//		int howManyTokens = 0;
//		long tempLong =0;
//
//		char * pch;
//		pch = strtok(string1, "._");
//		while(pch!=NULL && isCorrect){
//
//			std::string token(pch);
//			std::istringstream converter (token);
//			if (!checkFigures(converter))
//			{
//				std::cout<<"ATTENZIONE: pch" << pch<<std::endl;
//				std::cout<<"checkFigures FALSE" <<std::endl;
//
//				std::cout<<EXIT_CODE_18_TEXT<<std::endl;
//				return (EXIT_CODE_18);
//
//			}
//
//			tempLong = strtol(pch, (char**)NULL, 10);
////			std::cout<<"ATTENZIONE: tempLong"<< tempLong<<"ERRNO="<< errno<<std::endl;
//			if (errno == 22)
//				std::cout<<"ATTENZIONE:LETTERA" <<std::endl;
//
//			if (howManyTokens==0 || howManyTokens==1 ||howManyTokens == 3){
//
//				if (    ! (  (tempLong >-1) && (tempLong <16)   )   )
//					isCorrect = false;
//
//			}else if (howManyTokens==2 ){
//				if ( tempLong != 0 )
//					isCorrect = false;
//
//			}else {//more than 4 token
//				isCorrect = false;
//			}
//			howManyTokens++;
//			pch =strtok(NULL, "._");
//		}
//
//			if ( !isCorrect ){
//				std::cout<<EXIT_CODE_18_TEXT<<std::endl;
//				return (EXIT_CODE_18);
//
//			}
		/**********************/


	// check if EVO Architecture  : if not, exits with error code 115: Illegal command in this system configuration
	// check if EVO Architecture : if not able to get it, returns with error 32: Unable to get Node Architecture



	acs_apgcc_paramhandling PHA;

	ACS_CC_ReturnType result;
	int nodeArchitecture;

	/*added to manage axeFunction apgShelfarchtecture**/
	/* start connecting to IMM to get info*/
	OmHandler omManager;
	//ACS_CC_ReturnType result;

	/*try initialize imm connection to IMM*/
	result = omManager.Init();

	if (result != ACS_CC_SUCCESS){
		std::cout<<EXIT_CODE_1_TEXT<<std::endl;
		return(EXIT_CODE_1);

	}/*exiting on IMM init failure: generic error*/

	std::vector<std::string> AxeFunctions_dnList;
   /*get instances of AXEFunctions*/
	result = omManager.getClassInstances("AxeFunctions", AxeFunctions_dnList);
	if (result != ACS_CC_SUCCESS || (AxeFunctions_dnList.size() != 1) ) {
		   //unable to get node architecture exiting with error 32
		   std::cout<<EXIT_CODE_32_TEXT<<std::endl;
		   return (EXIT_CODE_32);
	}

	string axe_class_instance_name = AxeFunctions_dnList[0];

	result = PHA.getParameter(axe_class_instance_name, "apgShelfArchitecture", &nodeArchitecture);
	if (result != ACS_CC_SUCCESS){
		//unable to get node architecture exiting with error 32
		std::cout<<EXIT_CODE_32_TEXT<<std::endl;
		return (EXIT_CODE_32);
	}

	/*end added to manage axeFunction apgShelfarchtecture**/

	/*commented: now the parameter to be retrieved is apgShelfArchitecture of AxeFunctions and is retrieved in the section up*/
//	result = PHA.getParameter(ApzFunctionsId1.c_str(),nodeArchitectureAttr, &nodeArchitecture);
//	if (result != ACS_CC_SUCCESS){
//		//unable to get node architecture exiting with error 32
//		std::cout<<EXIT_CODE_32_TEXT<<std::endl;
//		return (EXIT_CODE_32);
//	}



	if ((nodeArchitecture != EGEM2_SCX) && (nodeArchitecture != EGEM2_SMX)){
		//get node architecture. nodeArchitecture=0 -> NOT EVO , nodeArchitecture=1 -> EVO. now exiting since not evo
		std::cout<<EXIT_CODE_115_TEXT<<std::endl;
		return (EXIT_CODE_115);
	}

	/*check if running on Active Node: if not, exits with error code 31*/
	ACS_PRC_API  prcObj;

	if (prcObj.askForNodeState() != ACS_PRC_NODE_ACTIVE ){
		std::cout<<EXIT_CODE_31_TEXT<<std::endl;
		return (EXIT_CODE_31);

	}
	/*is on the active node of EVO architecture .... parse command line**/

	int cmdTypeInput = 24;
	std::string magFilter = "";

	if (argc > 4)
	{
		printUsage();
		return 2;
	}
	if (argc == 4)
	{
		std::string option1 = argv[1];
		std::string option2 = argv[2];
		std::string option3 = argv[3];

		if (option1.compare("-m") == 0)
		{
			if (option3.compare("-c") == 0)
			{
				//std::cout<<"DBG:"<<__LINE__<<std::endl;
				magFilter = option2;
				for (size_t i=0 ; i<magFilter.length() ; i++)
				{
					if (magFilter[i]==' ') {
						magFilter.erase(i,1);
						i--;
					}
				}
				std::string str2 ("-");
				size_t found;
				found = magFilter.find(str2);

				//std::cout <<"found: "<<found<<std::endl;

				if (found == 0)
				{
					printUsage();
					return 2;
				}
				else
				{
					cmdTypeInput = 26;
				}

			} //if (option3.compare("-c") == 0)
			else
			{
				printUsage();
				return 2;
			}
		}
		else if (option1.compare("-c") == 0)
		{
			if (option2.compare("-m") == 0)
			{
				magFilter = option3;
				for (size_t i=0 ; i<magFilter.length() ; i++)
				{
					if (magFilter[i]==' ')
					{
						magFilter.erase(i,1);
						i--;
					}
				}
				std::string str2 ("-");
				size_t found;
				found = magFilter.find(str2);

				//std::cout <<"found: "<<found<<std::endl;

				if (found == 0)
				{
					printUsage();
					return 2;
				}
				else
				{
					cmdTypeInput = 26;
				}
			}
			else
			{
				printUsage();
				return 2;
			}
//			printf("\nMAGAZINE INFO\n");
//						printf("MAGADDR   CABROW   CABNO   XPOS   YPOS \n");
		}
		else
		{
			printUsage();
			return 2;
		}
	} //if (argc == 4)
	else if (argc == 3)
	{
		std::string option = argv[1];
		if (option.compare("-m") == 0)
		{
			magFilter = argv[2];
			for (size_t i=0 ; i<magFilter.length() ; i++) {
				if (magFilter[i]==' ')
				{
					magFilter.erase(i,1);
					i--;
				}
			}
			std::string str2 ("-");
			size_t found;
			found = magFilter.find(str2);
			if (found == 0)
			{
				printUsage();
				return 2;
			}
			else
			{
				cmdTypeInput = 24;
			}
		}
		else
		{
			printUsage();
			return 2;
		}
	} //else if (argc == 3)
	else if (argc == 2)
	{
		std::string argumnent = argv[1];
		size_t length = argumnent.length();
		std::string option = "";

		if ((length == 2) && (argumnent.compare("-x") == 0))
		{
			cmdTypeInput = 25;
		}
		else if ((length == 2) && (argumnent.compare("-c") == 0))
		{
			cmdTypeInput = 26;
		}
		else
		{
			printUsage();
			return 2;
		}
	} //else if (argc == 2)


	//cout<<" magFilter = "<<magFilter<<endl;

	/*check correct mag address*/
	if (magFilter.compare("")!=0){
		std::string magFiltertoParse= magFilter;

		if (!isCorrectMagAddr(magFilter)){
			//cout<<"isCorrectMagAddr ::: FALSE"<<endl;
			std::cout<<EXIT_CODE_18_TEXT<<std::endl;
			return (EXIT_CODE_18);

		}
	}
	//cout<<"isCorrectMagAddr ::: True"<<endl;

//TODO : the following section has to be deleted when tr on SAF IMM related to . management in IMM object DN is solved
	/*
	 * Now changing each . in magFilter with _
	 * 1.1.0.4 is changed in 1_1_0_4 to be used as DN of IMM object
	 * when IMM will be able to manage . as element of DN thi section has to be deleted
	 */

//	if (magFilter.compare("")!=0){
//		size_t foundDot;
//		//cout<<"magFilter before ="<<magFilter<<endl;
//		foundDot = magFilter.find_first_of(".");
//
//		while (foundDot != string::npos){
//
//			magFilter[foundDot] = '_';
//			foundDot = magFilter.find_first_of(".",foundDot+1 );
//
//		}
//		//cout<<"magFilter after ="<<magFilter<<endl;
//
//	}/*now the magazine address is modified*/


//TODO end section to be deleted


	//collegarsi ad IMM per prendere i dati commented and moved up to AxeFunction apgShelfArchitecture management instead
	//  of ApzFunction nodeArchitecture

//	/* start connecting to IMM to get info*/
//	OmHandler omManager;
//	//ACS_CC_ReturnType result;
//
//	/*try initialize imm connection to IMM*/
//	result = omManager.Init();
//
//	if (result != ACS_CC_SUCCESS){
//		std::cout<<EXIT_CODE_1_TEXT<<std::endl;
//		return(EXIT_CODE_1);
//
//	}/*exiting on IMM init failure: generic error*/


	/* get the root object instance of HardwareInventory class
	 * if no obj found return on Failure
	 * if more than one obj found returns error: just one instance hardwareInventoryId=1 must be present
	 */
	std::vector<std::string> p_dnList;
	result = omManager.getClassInstances(baseClassName.c_str(), p_dnList);
	if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1) ) {
		/*exiting on error either an erroe occurred or more than one instance found */
		std::cout<<EXIT_CODE_1_TEXT<<std::endl;
		return(EXIT_CODE_1);
	}


	std::string rootBaseObj = p_dnList[0];

	/*on success start searching for all magazines*/
	std::vector<std::string> shelfList;
	result = omManager.getChildren(rootBaseObj.c_str(), ACS_APGCC_SUBLEVEL, &shelfList);

		if (result != ACS_CC_SUCCESS || (shelfList.size() == 0) ) {
			//cout << "ERROR: getChildren FAILURE!!!\n";
			//no children found under the root node ... exiting with error 6: HWI data not available
			std::cout<<EXIT_CODE_6_TEXT<<std::endl;
			return (EXIT_CODE_6);

			//return -1;
		}

		switch (cmdTypeInput){

			case 24:
			case 26:{

				if(shelfList.size() != 0) {
					/*****/
					if (magFilter.compare("")!=0){/*only one magazine data to be printed*/
						int resPos =findInputMagazine(magFilter, shelfList );
						//cout << "resPo " << resPos << std::endl;
						if (resPos >=0){
							printf("\nAXE HARDWARE INVENTORY\n\n");
							printMagazineData( shelfList[resPos], omManager, cmdTypeInput  );

							if (cmdTypeInput == 24)
								printPFMData( shelfList[resPos], omManager );

							std::vector<std::string> boardDNList;

							//Paola getboardDNList(shelfList[resPos], omManager, boardDNList);
							getboardDNListOrdered(shelfList[resPos], omManager, boardDNList);

							if (boardDNList.size() > 0){
								//cmdTypeInput == 24 ?  printf("\nBOARDS INFO\n"): printf("\n\nSLOT POSITION\nMAGADDR   SLOT   XPOS   YPOS\n");
								bool titleAlreadyPrintedCOption = false;
								if (cmdTypeInput == 24)
									printf("\nBOARDS INFO\n");

								for (std::vector<std::string>::iterator boardIt = boardDNList.begin() ; boardIt != boardDNList.end() ; ++boardIt ) {
									/*the parameter titleAlreadyPrintedCOption is intended to detail if the SLOT POSITION STring has to be printed*/
									printBoardData((*boardIt),magFilter, omManager, cmdTypeInput, titleAlreadyPrintedCOption);
									//cmdTypeInput == 24 ?  printf("\n"): printf("");
									if (cmdTypeInput == 24)
										printf("\n");

								}/*end board ITeartion*/
								//cmdTypeInput == 24 ?  printf(""): printf("\n");
								if (cmdTypeInput != 24)
									printf("\n");
							}
							return 0;

						}else{/*Not Found invalid magazine value*/
							//std::cout<<EXIT_CODE_18_TEXT<<std::endl;
							//return(EXIT_CODE_18);
							/*The magazine address value was correct but not present in the cabinet:
							 * this is not an error condition just the title is printed */
							printf("\nAXE HARDWARE INVENTORY\n\n");
							return(0);
						}

					}/*end only one magazine*/

					/**ALL MAgazines***/
					printf("\nAXE HARDWARE INVENTORY\n\n");
					for (std::vector<std::string>::iterator it = shelfList.begin() ; it != shelfList.end() ; ++it ) {

//cout<<"TODELETE : magazine = "<<(*it)<<endl;
						printMagazineData( (*it), omManager , cmdTypeInput );

						std::string shelfName = getNamefromRDN((*it));

						if (cmdTypeInput == 24)
								printPFMData( (*it), omManager );

						std::vector<std::string> boardDNList;

						//Paola getboardDNList((*it), omManager, boardDNList);

						getboardDNListOrdered((*it), omManager, boardDNList);
//cout<<"TODELETE : magazine = "<<(*it)<<endl;
						if (boardDNList.size() > 0){
							//cmdTypeInput == 24 ?  printf("\nBOARDS INFO\n"): printf("\n\nSLOT POSITION\nMAGADDR   SLOT   XPOS   YPOS\n");
							bool titleAlreadyPrintedCOption = false;
							if (cmdTypeInput == 24)
								printf("\nBOARDS INFO\n");

							for (std::vector<std::string>::iterator boardIt = boardDNList.begin() ; boardIt != boardDNList.end() ; ++boardIt ) {
								printBoardData((*boardIt),shelfName, omManager, cmdTypeInput, titleAlreadyPrintedCOption);
								//cmdTypeInput == 24 ?  printf("\n"): printf("");
								if (cmdTypeInput == 24)
									printf("\n");

							}/*end board ITeartion*/
							//cmdTypeInput == 24 ?  printf(""): printf("\n");
							if (cmdTypeInput != 24)
								printf("\n");
						}/*no board*/

					}
					return 0;
				}/*end loop on found shelf elements**/
				else {
					//TODO No element found
					//std::cout << " NO ELEMENTS !!!!!!!! " << std::endl;
					std::cout<<EXIT_CODE_6_TEXT<<std::endl;
					return(EXIT_CODE_6);
				}

			}/*end case 24 26*/
			break;

			case 25: /*xml to be printed*/{
				ACS_APBM_Hwi_Xml_Maker xmlMaker;
				ACS_APBM_HWIData* hwi;
				//std::cout<<"!!!TD Case 25"<<std::endl;
				for (std::vector<std::string>::iterator it = shelfList.begin() ; it != shelfList.end() ; ++it ) {

					hwi = new ACS_APBM_HWIData;
					std::string MagAddr = getNamefromRDN( (*it) );
					if ( retrieveXmlShelfInfo(*hwi, (*it), omManager )==0 ) {
						retrieveXmlPFMInfo	(*hwi, (*it), "LOWER", omManager);
						retrieveXmlPFMInfo	(*hwi, (*it), "UPPER", omManager);
						retrieveXmlBoardInfo(*hwi, (*it), omManager );
						xmlMaker.cab.insert(make_pair(MagAddr , hwi) );

					}

					//std::cout<<"!!!before insert 1"<<std::endl;
					//xmlMaker.insertMagazine(*hwi, MagAddr);
					//std::cout<<"!!!after insert 1"<<std::endl;

				}/*end for on each magazine*/

				xmlMaker.getStringXmlDocument();


			}
			break;

			default:{}

		}/*end switch*/
	/* end   connecting to IMM to get info*/

	return 0;
}



void printUsage(){

	cout <<"Incorrect usage" << endl;
	cout << "\n";
	cout << "Usage: hwiprint [-c][-m magazine]" << endl;
	cout << "       hwiprint [-x]" << endl;
} // End of PrintUsage


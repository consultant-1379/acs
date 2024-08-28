/*
 * ACS_APBM_HWIData.cpp
 *
 *  Created on: Oct 18, 2011
 *      Author: xlucdor
 */

#include "acs_apbm_hwidata.h"

#include <iostream> //TODO delete it !

ACS_APBM_HWIData::ACS_APBM_HWIData() {

	m_magazinePosition =  new HWI_DataStructure::magazinePositionData();
	m_magazinePosition->magNo = -1;
	m_magazinePosition->magRow = -1;
	m_magazinePosition->magXpos = -1;
	m_magazinePosition->magYpos = -1;
	m_magazinePosition->subRackName = "";
        m_slotMaster= 0;

	m_backplaneData = new HWI_DataStructure::backplaneData();
	m_backplaneData->prodName = "";
	m_backplaneData->prodNo = "";
	m_backplaneData->prodRev = "";
	m_backplaneData->prodSerialNo = "";
	m_backplaneData->prodSupplier= "";
	m_backplaneData->prodManufactureDate = "";

	m_pfmUpper = new HWI_DataStructure::pfmData;
	m_pfmUpper->pfmProductName = "";
	m_pfmUpper->pfmProductNumber = "";
	m_pfmUpper->pfmProductRevision = "";
	m_pfmUpper->pfmHwVersion = "";
	m_pfmUpper->pfmSerialNo = "";
	m_pfmUpper->pfmDeviceType = "";
	m_pfmUpper->pfmManufactureDate = "";
	m_pfmUpper->pfmPresence = 0;
	m_pfmUpper->pfmInstance = HWI_DataStructure::UPPER;

	m_pfmLower = new HWI_DataStructure::pfmData;
	m_pfmLower->pfmProductName = "";
	m_pfmLower->pfmProductNumber = "";
	m_pfmLower->pfmProductRevision = "";
	m_pfmLower->pfmHwVersion = "";
	m_pfmLower->pfmSerialNo = "";
	m_pfmLower->pfmDeviceType = "";
	m_pfmLower->pfmManufactureDate = "";
	m_pfmLower->pfmPresence = 0;
	m_pfmLower->pfmInstance = HWI_DataStructure::LOWER;

	/**3 Added to be compliant to new model PFM configuration -Start *********/
	m_pfmUpperPosition = new HWI_DataStructure::slotPosizionData,
	m_pfmUpperPosition->xPos = -1;
	m_pfmUpperPosition->yPos = -1;

	m_pfmLowerPosition = new HWI_DataStructure::slotPosizionData,
	m_pfmLowerPosition->xPos = -1;
	m_pfmLowerPosition->yPos = -1;

	/**3 Added to be compliant to new model PFM configuration -End *********/

	m_mapSlotPos.empty();
	m_mapBoardData.empty();

}

ACS_APBM_HWIData::~ACS_APBM_HWIData() {

	//std::cout << __FUNCTION__ << "@" << __LINE__ << " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ " << std::endl;

	if (m_magazinePosition != NULL) {
		delete m_magazinePosition;
		m_magazinePosition = NULL;
	}
	if (m_backplaneData != NULL) {
		delete m_backplaneData;
		m_backplaneData = NULL;
	}
	if (m_pfmUpper != NULL) {
		delete m_pfmUpper;
		m_pfmUpper = NULL;
	}
	if (m_pfmLower != NULL) {
		delete m_pfmLower;
		m_pfmLower = NULL;
	}

	/**3 Added to be compliant to new model PFM configuration -Start *********/
	if (m_pfmUpperPosition != NULL) {
		delete m_pfmUpperPosition;
		m_pfmUpperPosition = NULL;
	}

	if (m_pfmLowerPosition != NULL) {
		delete m_pfmLowerPosition;
		m_pfmLowerPosition = NULL;
	}
	/**3 Added to be compliant to new model PFM configuration -END   *********/

	if (m_mapSlotPos.size() > 0 ) {
		for (std::map <int, HWI_DataStructure::slotPosizionData *>::iterator it = m_mapSlotPos.begin()
				; it != m_mapSlotPos.end() ; it++) {

			//Delete boardData pointer within the map
			delete (*it).second;
			//m_mapSlotPos.erase(it); TR HR55108 commented
			//std::cout << __FUNCTION__ << "@" << __LINE__ << " .... deleted !" << std::endl;
		}// end for
		/*TR HR55108 here clear the map*/
		m_mapSlotPos.clear();
	}

	if (m_mapBoardData.size() > 0 ) {
		for (std::map <int, HWI_DataStructure::boardData *>::iterator it = m_mapBoardData.begin()
				; it != m_mapBoardData.end() ; it++) {

			//Delete boardData pointer within the map
			delete (*it).second;
			//m_mapBoardData.erase(it); TR HR55108 commented
			//std::cout << __FUNCTION__ << "@" << __LINE__ << " .... deleted !" << std::endl;
		}// end for
		/*TR HR55108 here clear the map*/
		m_mapBoardData.clear();
	}

	//std::cout << __FUNCTION__ << "@" << __LINE__ << " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ " << std::endl;

}


int ACS_APBM_HWIData::addMagazinePositionData(const int &magRow,
		const int &magNo,
		const int &magXpos,
		const int &magYpos) {

	m_magazinePosition->magNo = magRow;
	m_magazinePosition->magRow = magNo;
	m_magazinePosition->magXpos = magXpos;
	m_magazinePosition->magYpos = magYpos;
	return DATA_SUCCESS;
}

int ACS_APBM_HWIData::addMagazinePositionData(const int &magRow,
		const int &magNo,
		const int &magXpos,
		const int &magYpos,
		const std::string &subRackName) {

	m_magazinePosition->magNo = magRow;
	m_magazinePosition->magRow = magNo;
	m_magazinePosition->magXpos = magXpos;
	m_magazinePosition->magYpos = magYpos;
	m_magazinePosition->subRackName = subRackName;
	return DATA_SUCCESS;
}

int ACS_APBM_HWIData::addSubRackName(const std::string &subRackName) {
	m_magazinePosition->subRackName = subRackName;
	return DATA_SUCCESS;
}

int ACS_APBM_HWIData::addBackplaneData(const std::string &prodName,
			const std::string &prodNo,
			const std::string &prodRev,
			const std::string &prodSerialNo,
			const std::string &prodSupplier,
			const std::string &prodManufactureDate) {

	m_backplaneData->prodName = prodName;
	m_backplaneData->prodNo = prodNo;
	m_backplaneData->prodRev = prodRev;
	m_backplaneData->prodSerialNo = prodSerialNo;
	m_backplaneData->prodSupplier= prodSupplier;
	m_backplaneData->prodManufactureDate = prodManufactureDate;
	return DATA_SUCCESS;
}


void ACS_APBM_HWIData::getMagazinePositionData(int &magRow,
											int &magNo,
											int &magXpos,
											int &magYpos,
											std::string &subRackName)const {

	magRow = m_magazinePosition->magNo;
	magNo = m_magazinePosition->magRow;
	magXpos = m_magazinePosition->magXpos;
	magYpos = m_magazinePosition->magYpos;
	subRackName = m_magazinePosition->subRackName;

}


void ACS_APBM_HWIData::getBackplaneData(std::string &prodName,
			std::string &prodNo,
			std::string &prodRev,
			std::string &prodSerialNo,
			std::string &prodSupplier,
			std::string &prodManufactureDate)const {

	prodName = m_backplaneData->prodName;
	prodNo = m_backplaneData->prodNo;
	prodRev = m_backplaneData->prodRev;
	prodSerialNo = m_backplaneData->prodSerialNo;
	prodSupplier = m_backplaneData->prodSupplier;
	prodManufactureDate = m_backplaneData->prodManufactureDate;

}

int ACS_APBM_HWIData::addPFMUpperLowerData(
				const std::string &pfmProductName,
				const std::string &pfmProductNumber,
				const std::string &pfmProductRevision,
				const std::string &pfmSerialNo,
				const std::string &pfmDeviceType,
				const std::string &pfmHwVersion,
				const std::string &pfmManufactureDate,
				const bool &isUpper) {

	if (isUpper) {
		m_pfmUpper->pfmProductName = pfmProductName;
		m_pfmUpper->pfmProductNumber = pfmProductNumber;
		m_pfmUpper->pfmProductRevision = pfmProductRevision;
		m_pfmUpper->pfmSerialNo = pfmSerialNo;
		m_pfmUpper->pfmDeviceType = pfmDeviceType;
		m_pfmUpper->pfmHwVersion = pfmHwVersion;
		m_pfmUpper->pfmManufactureDate = pfmManufactureDate;
		m_pfmUpper->pfmPresence = 1;
		m_pfmUpper->pfmInstance = HWI_DataStructure::UPPER;
	}
	else {
		m_pfmLower->pfmProductName = pfmProductName;
		m_pfmLower->pfmProductNumber = pfmProductNumber;
		m_pfmLower->pfmProductRevision = pfmProductRevision;
		m_pfmLower->pfmSerialNo = pfmSerialNo;
		m_pfmLower->pfmDeviceType = pfmDeviceType;
		m_pfmLower->pfmHwVersion = pfmHwVersion;
		m_pfmLower->pfmManufactureDate = pfmManufactureDate;
		m_pfmLower->pfmPresence = 1;
		m_pfmLower->pfmInstance = HWI_DataStructure::LOWER;
	}
	return DATA_SUCCESS;
}

/**3 Added to be compliant to new model PFM configuration -Start *********/
int ACS_APBM_HWIData::addPFMPositionData(
			const bool &isUpper,
			const int &pfmXpos,
			const int &pfmYpos
			){

	if (isUpper){

		m_pfmUpperPosition->xPos = pfmXpos;
		m_pfmUpperPosition->yPos = pfmYpos;
	}
	else{
		m_pfmLowerPosition->xPos = pfmXpos;
		m_pfmLowerPosition->yPos = pfmYpos;
	}
	return DATA_SUCCESS;
}

void ACS_APBM_HWIData::getPFMPositionData(const bool &isUpper,
				int &pfmXpos,
				int &pfmYpos
				)const{
	if (isUpper){
		pfmXpos = m_pfmUpperPosition->xPos;
		pfmYpos = m_pfmUpperPosition->yPos;
	}
	else{
		pfmXpos = m_pfmLowerPosition->xPos;
		pfmYpos = m_pfmLowerPosition->yPos;
	}

}
/**3 Added to be compliant to new model PFM configuration -End *********/



int ACS_APBM_HWIData::emptyPFMUpperLowerData(
				const bool &isUpper) {

	if (isUpper) {
		m_pfmUpper->pfmProductName = "";
		m_pfmUpper->pfmProductNumber = "";
		m_pfmUpper->pfmProductRevision = "";
		m_pfmUpper->pfmHwVersion = "";
		m_pfmUpper->pfmSerialNo = "";
		m_pfmUpper->pfmDeviceType = "";
		m_pfmUpper->pfmManufactureDate = "";
		m_pfmUpper->pfmPresence = 0;
		m_pfmUpper->pfmInstance = HWI_DataStructure::UPPER;
	}
	else {
		m_pfmLower->pfmProductName = "";
		m_pfmLower->pfmProductNumber = "";
		m_pfmLower->pfmProductRevision = "";
		m_pfmLower->pfmHwVersion = "";
		m_pfmLower->pfmSerialNo = "";
		m_pfmLower->pfmDeviceType = "";
		m_pfmLower->pfmManufactureDate = "";
		m_pfmLower->pfmPresence = 0;
		m_pfmLower->pfmInstance = HWI_DataStructure::LOWER;
	}
	return DATA_SUCCESS;
}


int ACS_APBM_HWIData::addSlotPosData(const int &slot,
		const int &xPos,
		const int &yPos,
		const bool &update)
{
	//std::cout << __FUNCTION__ << "@" << __LINE__ << " " << std::endl;

	exitCode res;
	std::map<int, HWI_DataStructure::slotPosizionData*>::iterator it;

	it = m_mapSlotPos.find(slot);

	if (it == m_mapSlotPos.end()) {

		HWI_DataStructure::slotPosizionData *slopPos = new HWI_DataStructure::slotPosizionData();
		slopPos->xPos = xPos;
		slopPos->yPos = yPos;

		m_mapSlotPos.insert(std::make_pair<int, HWI_DataStructure::slotPosizionData*>(slot, slopPos));

		res = DATA_SUCCESS;
	} //if (it == m_mapSlotPos.end())
	else {

		if (update) {
			(*it).second->xPos = xPos;
			(*it).second->yPos = yPos;
			res = DATA_UPDATED;
		}
		else {
			res = DATA_ALREADY_PRESENT;
		}

	} //else if (it == m_mapSlotPos.end())

	return res;
}

int ACS_APBM_HWIData::getSlotPosData(const int &slot,
									int &xPos,
									int &yPos) {

	//std::cout << __FUNCTION__ << "@" << __LINE__ << " " << std::endl;

	exitCode res;
	std::map<int, HWI_DataStructure::slotPosizionData*>::iterator it;

	it = m_mapSlotPos.find(slot);

	if (it != m_mapSlotPos.end()) {

		xPos = (*it).second->xPos;
		yPos = (*it).second->yPos;

		res = DATA_SUCCESS;

	} //if (it != m_mapSlotPos.end())
	else {
		xPos = -1;
		yPos = -1;
		res = DATA_NOT_PRESENT;
	}
	return res;
}


int ACS_APBM_HWIData::addBoardData(const int &slot,
			const std::string &boardName,
			const std::string &boardProdNo,
			const std::string &boardProdRev,
			const std::string &boardSerialNo,
			const std::string &boardSupplier,
			const std::string &boardBusType,
			const std::string &boardManufactureDate,
			const bool &update) {


	//std::cout << __FUNCTION__ << "@" << __LINE__ << " " << std::endl;

	exitCode res;
	std::map<int, HWI_DataStructure::boardData*>::iterator it;

	it = m_mapBoardData.find(slot);

	if (it == m_mapBoardData.end()) {

		//std::cout << __FUNCTION__ << "@" << __LINE__ << " " << std::endl;
		HWI_DataStructure::boardData *board = new HWI_DataStructure::boardData();

		board->boardName = boardName;
		board->boardProdNo = boardProdNo;
		board->boardProdRev = boardProdRev;
		board->boardSerialNo = boardSerialNo;
		board->boardSupplier = boardSupplier;
		board->boardBusType = boardBusType;
		board->boardManufactureDate = boardManufactureDate;

		m_mapBoardData.insert(std::make_pair<int, HWI_DataStructure::boardData*>(slot, board));

		res = DATA_SUCCESS;
	} //if (it == m_mapBoardData.end())
	else {
		if (update) {
			(*it).second->boardName = boardName;
			(*it).second->boardProdNo = boardProdNo;
			(*it).second->boardProdRev = boardProdRev;
			(*it).second->boardSerialNo = boardSerialNo;
			(*it).second->boardSupplier = boardSupplier;
			(*it).second->boardBusType = boardBusType;
			(*it).second->boardManufactureDate = boardManufactureDate;
			res = DATA_UPDATED;
		}
		else {
			res = DATA_ALREADY_PRESENT;
		}
	} //else if (it == m_mapBoardData.end())

	return res;
}



int ACS_APBM_HWIData::getBoardData(const int &slot,
									std::string &boardName,
									std::string &boardProdNo,
									std::string &boardProdRev,
									std::string &boardSerialNo,
									std::string &boardSupplier,
									std::string &boardBusType,
									std::string &boardManufactureDate) {


	//std::cout << __FUNCTION__ << "@" << __LINE__ << " " << std::endl;

	exitCode res;
	std::map<int, HWI_DataStructure::boardData*>::iterator it;

	it = m_mapBoardData.find(slot);

	if (it != m_mapBoardData.end()) {

		//std::cout << __FUNCTION__ << "@" << __LINE__ << " " << std::endl;

		boardName = (*it).second->boardName;
		boardProdNo = (*it).second->boardProdNo;
		boardProdRev = (*it).second->boardProdRev;
		boardSerialNo = (*it).second->boardSerialNo;
		boardSupplier = (*it).second->boardSupplier;
		boardBusType = (*it).second->boardBusType;
		boardManufactureDate = (*it).second->boardManufactureDate;

		res = DATA_SUCCESS;
	} //if (it != m_mapBoardData.end())
	else {
		boardName = "";
		boardProdNo = "";
		boardProdRev = "";
		boardSerialNo = "";
		boardBusType = "";
		boardSupplier = "";
		boardManufactureDate = "";

		res = DATA_NOT_PRESENT;
	}

	return res;
}

int ACS_APBM_HWIData::getBoardNumber(){
	return m_mapBoardData.size();
}

void ACS_APBM_HWIData::getBoardList(std::vector<int> &boardList){

	if (m_mapBoardData.size() > 0) {
		std::map<int, HWI_DataStructure::boardData*>::iterator it;

		for (it = m_mapBoardData.begin() ; it != m_mapBoardData.end() ; ++it) {
			boardList.push_back(it->first);
		}
	}
	else {
		boardList.clear();
	}
}


int ACS_APBM_HWIData::removeBoardData(const int &slot){

	exitCode res = DATA_NOT_PRESENT;

	std::map<int, HWI_DataStructure::boardData*>::iterator it;

	it = m_mapBoardData.find(slot);

	//delete board
	if (it != m_mapBoardData.end()) {
		delete it->second;
		m_mapBoardData.erase(it);

		res = DATA_SUCCESS;
	}

	std::map<int, HWI_DataStructure::slotPosizionData*>::iterator itslot;

	itslot = m_mapSlotPos.find(slot);

	if (itslot != m_mapSlotPos.end()) {
		delete itslot->second;
		m_mapSlotPos.erase(itslot);
	}

	return res;
}




void ACS_APBM_HWIData::printData() {

	std::cout << "---------" << this->m_magName << "----------" << std::endl;


	std::cout << "MAGAZINE INFO " << std::endl;
	std::cout << m_magazinePosition->magNo << " " <<
			m_magazinePosition->magRow << " " <<
			m_magazinePosition->magXpos << " " <<
			m_magazinePosition->magYpos << " " <<
			m_magazinePosition->subRackName << " " <<
			std::endl << std::endl;

	std::cout << "BACKPLANE INFO " << std::endl;
	std::cout << m_backplaneData->prodName << " " <<
			m_backplaneData->prodNo << " " <<
			m_backplaneData->prodRev << " " <<
			m_backplaneData->prodSerialNo << " " <<
			m_backplaneData->prodSupplier << " " <<
			m_backplaneData->prodManufactureDate << " " <<
			std::endl << std::endl;

	std::cout << "POWER AND FAN MODULE INFO " << std::endl;
	std::cout << m_pfmLower->pfmInstance << " " <<
			m_pfmLower->pfmProductName << " " <<
			m_pfmLower->pfmProductNumber << " " <<
			m_pfmLower->pfmProductRevision << " " <<
			m_pfmLower->pfmSerialNo << " " <<
			m_pfmLower->pfmDeviceType << " " <<
			m_pfmLower->pfmHwVersion << " " <<
			m_pfmLower->pfmManufactureDate << " " <<
			std::endl << std::endl;

	std::cout << m_pfmUpper->pfmInstance << " " <<
			m_pfmUpper->pfmProductName << " " <<
			m_pfmUpper->pfmProductNumber << " " <<
			m_pfmUpper->pfmProductRevision << " " <<
			m_pfmUpper->pfmSerialNo << " " <<
			m_pfmUpper->pfmDeviceType << " " <<
			m_pfmUpper->pfmHwVersion << " " <<
			m_pfmUpper->pfmManufactureDate << " " <<
			std::endl << std::endl;

	std::map <int, HWI_DataStructure::slotPosizionData *>::iterator it;

	for (it = m_mapSlotPos.begin() ; it != m_mapSlotPos.end() ; ++it) {

		std::pair<int, HWI_DataStructure::slotPosizionData *> p = (*it);

		std::cout << "Key: " << p.first << " - Val: " << p.second->xPos << " " << p.second->yPos  << std::endl;
	}

	std::map <int, HWI_DataStructure::boardData *>::iterator itboard;

	for (itboard = m_mapBoardData.begin() ; itboard != m_mapBoardData.end() ; ++itboard) {

		std::pair<int, HWI_DataStructure::boardData *> p = (*itboard);

		std::cout << "Key: " << p.first << " - Val: " << p.second->boardName
				<< " " << p.second->boardProdNo
				<< " " << p.second->boardProdRev
				<< " " << p.second->boardSerialNo
				<< " " << p.second->boardBusType
				<< " " << p.second->boardSupplier
				<< " " << p.second->boardManufactureDate
				<< std::endl;
	}

}


bool ACS_APBM_HWIData::isPFMUpperPresent()const {
	return m_pfmUpper->pfmPresence;
}

bool ACS_APBM_HWIData::isPFMLowerPresent()const {
	return m_pfmLower->pfmPresence;
}

void ACS_APBM_HWIData::getPFMUpperLowerData(
		std::string &pfmProductName,
		std::string &pfmProductNumber,
		std::string &pfmProductRevision,
		std::string &pfmHwVersion,
		std::string &pfmSerialNo,
		std::string &pfmDeviceType,
		std::string &pfmManufactureDate,
		const bool &upperData)const {

	if (upperData) {
		pfmProductName = m_pfmUpper->pfmProductName;
		pfmProductNumber = m_pfmUpper->pfmProductNumber;
		pfmProductRevision = m_pfmUpper->pfmProductRevision;
		pfmHwVersion = m_pfmUpper->pfmHwVersion;
		pfmSerialNo = m_pfmUpper->pfmSerialNo;
		pfmDeviceType = m_pfmUpper->pfmDeviceType;
		pfmManufactureDate = m_pfmUpper->pfmManufactureDate;
	}
	else {
		pfmProductName = m_pfmLower->pfmProductName;
		pfmProductNumber = m_pfmLower->pfmProductNumber;
		pfmProductRevision = m_pfmLower->pfmProductRevision;
		pfmHwVersion = m_pfmLower->pfmHwVersion;
		pfmSerialNo = m_pfmLower->pfmSerialNo;
		pfmDeviceType = m_pfmLower->pfmDeviceType;
		pfmManufactureDate = m_pfmLower->pfmManufactureDate;
	}

}

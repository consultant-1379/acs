/*
 * ACS_APBM_HWIData.h
 *
 *  Created on: Oct 18, 2011
 *      Author: xlucdor
 */

#ifndef ACS_APBM_HWIDATA_H_
#define ACS_APBM_HWIDATA_H_

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <vector>

namespace HWI_DataStructure {

	const std::string LOWER = "lower";
	const std::string UPPER = "upper";

/*
 * Magazine position information
 * (data provided by user)
 *
 * 			1			1
 * HWI Data ------------- magazinePosition
 */
typedef struct {

	//magRow	[1 to 999]
	//magNo 	[1 to 99]
	//magXpos 	[0 to 255]
	//magYpos 	[0 to 255]

	int magRow;
	int magNo;
	int magXpos;
	int magYpos;
	std::string subRackName;

} magazinePositionData;

/*
 * Backplane information data
 * (data provided by SNMP)
 *
 * 			1			1
 * HWI Data ------------- backplaneData
 */
typedef struct {

	std::string prodName;
	std::string prodNo;
	std::string prodRev;
	std::string prodSerialNo;
	std::string prodSupplier;
	std::string prodManufactureDate;

} backplaneData;


/*
 * Slot position information
 * (data provided by user)
 *
 * 			1          26
 * HWI Data ------------- slotPosizion
 */
typedef struct {

	//Xpos 	[0 to 255]
	//Ypos 	[0 to 255]

	int xPos;
	int yPos;

} slotPosizionData;


/*
 * Boards information
 * (data provided by SNMP)
 *
 * 			1          26
 * HWI Data ------------- boardData
 */
typedef struct {

	std::string boardName;
	std::string boardProdNo;
	std::string boardProdRev;
	std::string boardSerialNo;
	std::string boardBusType;
	std::string boardSupplier;
	std::string boardManufactureDate;

} boardData;


/*
 * Power, Fan module information
 * (data provided by SNMP)
 *
 * 			1			2
 * HWI Data ------------- pfmData
 */
typedef struct {

	std::string pfmProductName;
	std::string pfmProductNumber;
	std::string pfmProductRevision;
	std::string pfmHwVersion;
	std::string pfmSerialNo;
	std::string pfmDeviceType;
	std::string pfmManufactureDate;

	std::string pfmInstance;
	int	pfmPresence;

} pfmData;

} //namespace HWI_DataStructure


class ACS_APBM_HWIData {

public:


	enum exitCode {
		DATA_ERROR = -1,
		DATA_SUCCESS = 0,
		DATA_UPDATED,
		DATA_ALREADY_PRESENT,
		DATA_DELETED,
		DATA_NOT_PRESENT
	};


	ACS_APBM_HWIData();
	~ACS_APBM_HWIData();

	void printData(void); //TODO to be delete



	int addMagazinePositionData(const int &magRow,
			const int &magNo,
			const int &magXpos,
			const int &magYpos);

	int addMagazinePositionData(const int &magRow,
			const int &magNo,
			const int &magXpos,
			const int &magYpos,
			const std::string &subRackName);

	int addSubRackName(const std::string &subRackName);

	int addBackplaneData(const std::string &prodName,
			const std::string &prodNo,
			const std::string &prodRev,
			const std::string &prodSerialNo,
			const std::string &prodSupplier,
			const std::string &prodManufactureDate);

	void getMagazinePositionData(int &magRow,
				int &magNo,
				int &magXpos,
				int &magYpos,
				std::string &subRackName) const;

	void getBackplaneData(std::string &prodName,
				std::string &prodNo,
				std::string &prodRev,
				std::string &prodSerialNo,
				std::string &prodSupplier,
				std::string &prodManufactureDate) const;


	int addPFMUpperLowerData(
			const std::string &pfmProductName,
			const std::string &pfmProductNumber,
			const std::string &pfmProductRevision,
			const std::string &pfmHwVersion,
			const std::string &pfmSerialNo,
			const std::string &pfmDeviceType,
			const std::string &pfmManufactureDate,
			const bool &isUpper = true);
	 /**3 Added to be compliant to new model PFM configuration -Start *********/
	int addPFMPositionData(
				const bool &isUpper,
				const int &pfmXpos,
				const int &pfmYpos
				);

	void getPFMPositionData(const bool &isUpper,
				int &pfmXpos,
				int &pfmYpos
				) const;

	 /**3 Added to be compliant to new model PFM configuration -End *********/
	bool isPFMUpperPresent() const;
	bool isPFMLowerPresent() const;

	void getPFMUpperLowerData(
			std::string &pfmProductName,
			std::string &pfmProductNumber,
			std::string &pfmProductRevision,
			std::string &pfmHwVersion,
			std::string &pfmSerialNo,
			std::string &pfmDeviceType,
			std::string &pfmManufactureDate,
			const bool &upperData) const;

	int emptyPFMUpperLowerData(const bool &isUpper);

	int addSlotPosData(const int &slot,
					const int &xPos,
					const int &yPos,
					const bool &update = false);

	int getSlotPosData(const int &slot,
					int &xPos,
					int &yPos);

	int addBoardData(const int &slot,
					const std::string &productNumber,
					const std::string &productName,
					const std::string &productRevision,
					const std::string &serialNo,
					const std::string &manufactureDate,
					const std::string &supplier,
					const std::string &busType,
					const bool &update = false);

	int getBoardData(const int &slot,
					std::string &productNumber,
					std::string &productName,
					std::string &productRevision,
					std::string &serialNo,
					std::string &manufactureDate,
					std::string &supplier,
					std::string &busType);

	int getBoardNumber();
	void getBoardList(std::vector<int> &boardList);

	int removeBoardData(const int &slot);


    std::string getEgem2L2Switch0EthA() const
    {
        return m_egem2L2Switch0EthA;
    }

    std::string getEgem2L2Switch0EthB() const
    {
        return m_egem2L2Switch0EthB;
    }

    std::string getEgem2L2Switch25EthA() const
    {
        return m_egem2L2Switch25EthA;
    }

    std::string getEgem2L2Switch25EthB() const
    {
        return m_egem2L2Switch25EthB;
    }

    std::string getMagName() const
    {
	return m_magName;
    }

    int getSlotMaster() const
    {
        return m_slotMaster;
    }


    void setEgem2L2Switch0EthA(std::string m_egem2L2Switch0EthA)
    {
        this->m_egem2L2Switch0EthA = m_egem2L2Switch0EthA;
    }

    void setEgem2L2Switch0EthB(std::string m_egem2L2Switch0EthB)
    {
        this->m_egem2L2Switch0EthB = m_egem2L2Switch0EthB;
    }

    void setEgem2L2Switch25EthA(std::string m_egem2L2Switch25EthA)
    {
        this->m_egem2L2Switch25EthA = m_egem2L2Switch25EthA;
    }

    void setEgem2L2Switch25EthB(std::string m_egem2L2Switch25EthB)
    {
        this->m_egem2L2Switch25EthB = m_egem2L2Switch25EthB;
    }

    void setMagName(std::string m_magName)
    {
        this->m_magName = m_magName;
    }

    void setSlotMaster(int slotMaster)
    {
        this->m_slotMaster = slotMaster;
    }


private:

    HWI_DataStructure::magazinePositionData *m_magazinePosition;
    HWI_DataStructure::backplaneData *m_backplaneData;

    std::map<int, HWI_DataStructure::slotPosizionData*> m_mapSlotPos;
    std::map<int, HWI_DataStructure::boardData*> m_mapBoardData;

    HWI_DataStructure::pfmData *m_pfmUpper;
    HWI_DataStructure::pfmData *m_pfmLower;

    /**3 Added to be compliant to new model PFM configuration -Start *********/
    HWI_DataStructure::slotPosizionData *m_pfmUpperPosition;
    HWI_DataStructure::slotPosizionData *m_pfmLowerPosition;
    /**3 Added to be compliant to new model PFM configuration -END *********/


    std::string m_egem2L2Switch0EthA;
    std::string m_egem2L2Switch0EthB;
    std::string m_egem2L2Switch25EthA;
    std::string m_egem2L2Switch25EthB;
    std::string m_magName;
    int m_slotMaster;
};

#endif /* ACS_APBM_HWIDATA_H_ */

/*
 * ACS_PRC_suInfo.h
 *
 *  Created on: Nov 17, 2010
 *      Author: xpaomaz
 */

#ifndef ACS_PRC_SUINFO_H_
#define ACS_PRC_SUINFO_H_

#include <string>
#include <map>
#include <vector>

#include <stdio.h>
#include <iostream>

#include "acs_prc_api.h"
#include "ACS_TRA_Logging.h"

/** @class  ACS_PRC_suInfo  ACS_PRC_suInfo.h
 *	@brief  ACS_PRC_suInfo class This class is intended to retrieve SU list related to APG and
 *	@brief  to compute the run level for each node
 *	@author xpaomaz (Paola Mazzone)
 *	@date 2010-11-25
 *	@version R1A
 */


class ACS_PRC_suInfo {
public:
	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_PRC_suInfo Default constructor
	 *
	 *	ACS_PRC_suInfo Constructor
	 *
	 *	@remarks Remarks
	 */
	ACS_PRC_suInfo();


	/** @brief ACS_PRC_suInfo Destructor
	 *
	 *	ACS_APGCC_ACS_PRC_suInfo Destructor
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_PRC_suInfo();

	//===========//
	// Functions //
	//===========//

	/**	@brief getInfo method.
	 *  @brief getInfo method. This method get the list of Service Unit for SC-1 and SC-2.
	 *
	 *	@return int. returns 0 on success
	 *
	 */

	int getInfo();

	/**	@brief getRunLevel method.
	 *  getRunLevel method. This method computes and returns the run level
	 *  for the node provided as input parameter.
	 *  @param node the node for which the run level has to be calculated. Possible values are:
	 *  ACS_PRC_LOCAL_NODE (default value)
	 *  ACS_PRC_REMOTE_NODE
	 *
	 *	@return uint32_t. returns a value [0,5]
	 *
	 */

	uint32_t getRunLevel(string node );

	void getMap( map<string,int>*, map<string,int>* );

private:
	map<string,int> suStateSC1Map;
	map<string,int> suStateSC2Map;
	//vector<string>::iterator itSu;

	uint32_t runLevelSC1;
	uint32_t runLevelSC2;

	/**
	 * @brief computeRunLevelSC1 method
	 * This method computes the run level of node 1 and set the related value in runLevelSC1 class variable
	 *
	 * @return uint32_t return 0
	 */
	int computeRunLevelSC1();
	/**
	 * @brief computeRunLevelSC2 method
	 * This method computes the run level of node 2 and set the related value in runLevelSC2 class variable
	 *
	 * @return uint32_t return 0
	 */
	int computeRunLevelSC2();
};

#endif /* ACS_PRC_SUINFO_H_ */

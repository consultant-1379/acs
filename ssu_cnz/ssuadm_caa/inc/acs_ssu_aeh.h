
// -*- C++ -*-

//=============================================================================
/**
 *  @file    ACS_SSU_AEH.h
 *
 *  @copyright  Ericsson AB, Sweden 2010. All rights reserved.
 *
 *  @author 2010-07-7 by XSIRKUM
 *
 *  @documentno
 *
 */
//=============================================================================


#ifndef ACS_SSU_AEH_H
#define ACS_SSU_AEH_H

#define CAUSE_SYSTEM_ANALYSIS    ACE_TEXT("AP SYSTEM ANALYSIS")
#define CAUSE_AP_INTERNAL_FAULT  ACE_TEXT("AP INTERNAL FAULT")
#define CAUSE_AP_FAULT           ACE_TEXT("AP FAULT")
#define PROCESS_NAME             ACE_TEXT("acs_ssumonitord")

/*!
 * @class ACS_SSU_AEH
 *
 * @brief Implementation class for SSU AEH event handler
 *
 * This class uses APGCC common classes for event reporting and
 * to raise alarms
 *
 */

class ACS_SSU_AEH
{
protected:
   /*!
    * @brief  Not allowed to instanciate an object of this class.
    *
    */

   ACS_SSU_AEH();

public:
   /*!
    * @brief This function should be used for event reporting. This internally used
    * @param EventNumber
    * @param Severity
    * @param Cause
    * @param ObjectOfReference
    * @param Data
    * @param Text
    * @param bReportToAEH
    * @return
    */
   static bool ReportEvent(const ACE_UINT32 EventNumber,
                           const ACE_TCHAR* Severity,
                           const ACE_TCHAR* Cause,
                           const ACE_TCHAR* ObjectOfReference,
                           const ACE_TCHAR* Data,
                           const ACE_TCHAR* Text,
                           bool bReportToAEH = true);
};

#endif

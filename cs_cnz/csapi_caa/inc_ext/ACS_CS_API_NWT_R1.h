#ifndef ACS_CS_API_NWT_R1_h
#define ACS_CS_API_NWT_R1_h 1

#include <list>
#include "ACS_CS_API_NetworkTable_R1.h"

class ACS_CS_API_NWT_R1
{

public:
    virtual ~ACS_CS_API_NWT_R1();


    static ACS_CS_API_NWT_BoardSearch_R1 * createNetworkSearchInstance ();

    static void deleteNetworkSearchInstance (ACS_CS_API_NWT_BoardSearch_R1 *instance);

    virtual ACS_CS_API_NS::CS_API_Result getNetworkTable (std::list<ACS_CS_API_NetworkTable_R1> &tableRows, BoardID boardId) = 0;

    virtual ACS_CS_API_NS::CS_API_Result getIntNetwID (ACS_CS_API_NameList_R1 &listObj, BoardID boardId) = 0;

    virtual ACS_CS_API_NS::CS_API_Result getExtNetwID (ACS_CS_API_NameList_R1 &listObj, BoardID boardId) = 0;

    virtual ACS_CS_API_NS::CS_API_Result getAdminState (ACS_CS_API_NameList_R1 &listAdminStates, BoardID boardId) = 0;

    virtual ACS_CS_API_NS::CS_API_Result getInterfaceID (ACS_CS_API_NameList_R1 &listInterfaces, BoardID boardId) = 0;

    virtual ACS_CS_API_NS::CS_API_Result getNtwName (ACS_CS_API_NameList_R1 &listNtwNames, BoardID boardId) = 0;

    virtual ACS_CS_API_NS::CS_API_Result getMAC (ACS_CS_API_MacList_R1 &macList, ACS_CS_API_NWT_BoardSearch_R1 *searchObj) = 0;

    virtual ACS_CS_API_NS::CS_API_Result getBoardIds (ACS_CS_API_IdList_R1 &listObj, ACS_CS_API_NWT_BoardSearch_R1 *searchObj) = 0;


};

#endif

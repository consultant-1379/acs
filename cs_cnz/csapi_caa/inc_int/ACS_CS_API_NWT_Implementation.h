#ifndef ACS_CS_API_NWT_Implementation_h
#define ACS_CS_API_NWT_Implementation_h 1

#include "ACS_CS_API.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_API_NWT_R1.h"


class ACS_CS_API_NWT_Implementation : public ACS_CS_API_NWT_R1
{

  public:
      ACS_CS_API_NWT_Implementation();

      virtual ~ACS_CS_API_NWT_Implementation();

      virtual ACS_CS_API_NS::CS_API_Result getNetworkTable (std::list<ACS_CS_API_NetworkTable_R1> &tableRows, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getIntNetwID (ACS_CS_API_NameList_R1 &listObj, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getExtNetwID (ACS_CS_API_NameList_R1 &listObj, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getAdminState (ACS_CS_API_NameList_R1 &listAdminStates, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getInterfaceID (ACS_CS_API_NameList_R1 &listInterfaces, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getNtwName (ACS_CS_API_NameList_R1 &listNtwNames, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getMAC (ACS_CS_API_MacList_R1 &macList, ACS_CS_API_NWT_BoardSearch_R1 *searchObj);

      virtual ACS_CS_API_NS::CS_API_Result getBoardIds (ACS_CS_API_IdList_R1 &listObj, ACS_CS_API_NWT_BoardSearch_R1 *searchObj);

      virtual ACS_CS_API_NS::CS_API_Result getNtwTableRows (std::list<ACS_CS_API_NetworkTable_R1> &tableRows, ACS_CS_API_NWT_BoardSearch_R1 *searchObj);



  private:

      ACS_CS_ImModel *model;
      ACS_CS_ImIMMReader * immReader;
      string getAdminStateName(AdminState adminState);
      BoardID getBoardIdFromComputeResource(const ACS_CS_ImComputeResource* computResource);
};


#endif

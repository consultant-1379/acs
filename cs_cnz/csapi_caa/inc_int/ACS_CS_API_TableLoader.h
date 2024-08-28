//	 Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_API_TableLoader_h
#define ACS_CS_API_TableLoader_h 1

#include <set>
#include <map>
#include <vector>

#include "ACS_CS_Protocol.h"
#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_API_Util_Internal.h"

class ACS_CS_Table;
class ACS_CS_TableOperationEntry;

class ACS_CS_API_TableLoader 
{

  public:

      ACS_CS_API_TableLoader();

      virtual ~ACS_CS_API_TableLoader();

      ACS_CS_API_NS::CS_API_Result loadCPTable (ACS_CS_Table &cpTable);

      ACS_CS_API_NS::CS_API_Result loadCPGroupTable (stringCPIDSetMap &CPGroupMap);

      ACS_CS_API_NS::CS_API_Result loadHWCTable (ACS_CS_Table &HWCTable);

      ACS_CS_API_NS::CS_API_Result loadFunctionDistributionTable (ACS_CS_Table &FDTable);

      ACS_CS_API_NS::CS_API_Result loadNETable (ACS_CS_Table &NETable);

  private:

      ACS_CS_API_TableLoader(const ACS_CS_API_TableLoader &right);

      ACS_CS_API_TableLoader & operator=(const ACS_CS_API_TableLoader &right);

};

#endif

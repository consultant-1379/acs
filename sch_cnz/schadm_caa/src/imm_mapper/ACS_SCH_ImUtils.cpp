#include <cstdarg>
#include <sstream>
#include <set>
#include <string.h>
#include <stdlib.h>

#include "acs_apgcc_paramhandling.h"
#include "ACS_CC_Types.h"
#include "ACS_SCH_ImUtils.h"
map<string, string> ACS_SCH_ImUtils::ClassMap;
string ACS_SCH_ImUtils::faultyValue;
ACS_SCH_ImUtils::ACS_SCH_ImUtils() {
}

ACS_SCH_ImUtils::~ACS_SCH_ImUtils() {
}
		
string ACS_SCH_ImUtils::getIdAttributeFromClassName (const string & className) {

  map <string,string>* myMap = getClassMap();

  map<string, string>::iterator it;
  it = myMap->find(className);

  if (it != myMap->end()) {
    return it->second;
  } else
    return "";

}
//End of strings defined for IMM Operations
//----------------------------------------------------------------------------
/*std::string
ACS_SCH_ImUtils::getObjRdnValue(std::string &objName,std::string CRvalue)
{
  std::string myIdStr = objName.substr(objName.find_first_of('=')+CRvalue+"_"+"Role",
      objName.find_first_of(',')-objName.find_first_of('=')-1);
  return myIdStr;
}*/

/*std::string ACS_SCH_ImUtils::getClassName(ClassType type)
{
	std:: string typeName;

		   switch(type)
				    {
							 case SHELF_T:
								      typeName = ACS_CS_ImmMapper::CLASS_SHELF;
											break;
							 default:
											 break;
																	   }

			    return typeName;
}*/

 map <string,string>* ACS_SCH_ImUtils::getClassMap() {

  if (ClassMap.size() == 0){
  //  ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CRMGMT, ACS_CS_ImmMapper::ATTR_CRM_ID));
 }

  return &ClassMap;
}



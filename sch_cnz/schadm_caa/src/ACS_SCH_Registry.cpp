#include "ACS_SCH_Registry.h"
bool ACS_SCH_Registry::isDebugEnabled = false;

ACS_SCH_Registry::ACS_SCH_Registry()
{
}

ACS_SCH_Registry::~ACS_SCH_Registry()
{
}

bool ACS_SCH_Registry::isDebug()
{
	     return isDebugEnabled;
			   //return true;
}

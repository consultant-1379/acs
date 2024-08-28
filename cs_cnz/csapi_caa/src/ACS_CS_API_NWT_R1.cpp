#include "ACS_CS_API_NWT_BoardSearch_Implementation.h"
#include "ACS_CS_API_NWT_R1.h"


ACS_CS_API_NWT_R1::~ACS_CS_API_NWT_R1()
{
}

 ACS_CS_API_NWT_BoardSearch_R1 * ACS_CS_API_NWT_R1::createNetworkSearchInstance ()
{

	return new ACS_CS_API_NWT_BoardSearch_Implementation();

}

 void ACS_CS_API_NWT_R1::deleteNetworkSearchInstance (ACS_CS_API_NWT_BoardSearch_R1 *instance)
{
	if (instance)
		delete instance;
}

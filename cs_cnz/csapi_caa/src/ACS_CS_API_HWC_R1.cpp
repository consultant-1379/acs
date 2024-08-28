
//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_API_BoardSearch_Implementation.h"


// ACS_CS_API_HWC_R1
#include "ACS_CS_API_HWC_R1.h"
#include "ACS_CS_API_HWC_R2.h"



// Class ACS_CS_API_HWC_R1 

ACS_CS_API_HWC_R1::~ACS_CS_API_HWC_R1()
{
}

 ACS_CS_API_BoardSearch_R1 * ACS_CS_API_HWC_R1::createBoardSearchInstance ()
{

	return new ACS_CS_API_BoardSearch_Implementation();

}

 void ACS_CS_API_HWC_R1::deleteBoardSearchInstance (ACS_CS_API_BoardSearch_R1 *instance)
{

	if (instance)
		delete instance;

}


 ACS_CS_API_BoardSearch_R2 * ACS_CS_API_HWC_R2::createBoardSearchInstance ()
 {

	 return new ACS_CS_API_BoardSearch_Implementation();

 }

// Additional Declarations


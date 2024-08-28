#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
#include "ACS_CS_Util.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_API_NeHandling.h"

using namespace std;

TEST(ACS_CS_API, DISABLED_startupProfileSupervisionComplete)
{

   EXPECT_TRUE(true);
}


TEST(ACS_CS_API, cancelClusterOpModeSupervisionTimer)
{
	bool result = ACS_CS_API_NeHandling::cancelClusterOpModeSupervisionTimer();

	EXPECT_TRUE(result);
}


TEST(ACS_CS_API, DISABLED_setOmProfileSupervisionTimer)
{
	ACS_CS_API_NeHandling::setOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::Validate, 60);
	EXPECT_TRUE(true);
}


TEST(ACS_CS_API, cancelOmProfileSupervisionTimer)
{

	bool result = ACS_CS_API_NeHandling::cancelOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::Validate);;

	EXPECT_TRUE(result);
}


TEST(ACS_CS_API, getsetNeSubscriberCount)
{
	int neSubscriberCount = ACS_CS_API_NeHandling::getNeSubscriberCount();

	//cout << "1 NE SUBSCRIBERS = " << neSubscriberCount << endl;

	ACS_CS_API_NeHandling::setNeSubscriberCount(1234);

	neSubscriberCount = ACS_CS_API_NeHandling::getNeSubscriberCount();

	//cout << "2 NE SUBSCRIBERS = " << neSubscriberCount << endl;

	EXPECT_EQ(1234, neSubscriberCount);

	ACS_CS_API_NeHandling::setNeSubscriberCount(0);

	neSubscriberCount = ACS_CS_API_NeHandling::getNeSubscriberCount();

	//cout << "3 NE SUBSCRIBERS = " << neSubscriberCount << endl;

	EXPECT_EQ(0, neSubscriberCount);
}


TEST(ACS_CS_API, setPhaseSubscriberCount)
{
	// Arrange
	ACS_CS_API_NE_NS::SetPhaseSubscriberData phaseData;
	memset(&phaseData, 0, sizeof(phaseData));

	phaseData.validate = 111;
	phaseData.apNotify = 222;
	phaseData.cpNotify = 333;
	phaseData.commit   = 444;

	// Act
	bool result = ACS_CS_API_NeHandling::setPhaseSubscriberCount(phaseData);

	// Assert
	EXPECT_TRUE(result == true);
}


TEST(ACS_CS_API, getPhaseSubscriberCount)
{
	int validate = ACS_CS_API_NeHandling::getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::Validate);
	int apNotify = ACS_CS_API_NeHandling::getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::ApNotify);
	int cpNotify = ACS_CS_API_NeHandling::getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::CpNotify);
	int commit = ACS_CS_API_NeHandling::getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::Commit);

	EXPECT_EQ(111, validate);
	EXPECT_EQ(222, apNotify);
	EXPECT_EQ(333, cpNotify);
	EXPECT_EQ(444, commit);

	ACS_CS_API_NE_NS::SetPhaseSubscriberData phaseData;
	memset(&phaseData, 0, sizeof(phaseData));
	phaseData.validate = 0;
	phaseData.apNotify = 0;
	phaseData.cpNotify = 0;
	phaseData.commit   = 0;
	ACS_CS_API_NeHandling::setPhaseSubscriberCount(phaseData);
}


TEST(ACS_CS_API, updatePhaseChange)
{
	// Arrange
	ACS_CS_API_OmProfileChange omProfileChange;

	omProfileChange.aptCurrent = 1;
	omProfileChange.aptQueued = 2;
	omProfileChange.aptRequested = 3;

	omProfileChange.apzCurrent = 4;
	omProfileChange.apzQueued = 5;
	omProfileChange.apzRequested = 6;

	omProfileChange.omProfileCurrent = 7;
	omProfileChange.omProfileRequested = 8;

	omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NechCommand;
	omProfileChange.phase = ACS_CS_API_OmProfilePhase::Validate;

	// Act
	bool result = ACS_CS_API_NeHandling::updatePhaseChange(omProfileChange);

	// Assert
	EXPECT_TRUE(result);
}

TEST(ACS_CS_API, loadNeTable)
{
	ACS_CS_Table * table;

	table = ACS_CS_API_NeHandling::loadNeTable();

	if(table == NULL)
	{
		FAIL() << "loadNeTable failed";
	}

	ACS_CS_TableEntry omEntry = table->getEntry(ACS_CS_NS::ENTRY_ID_OM_PROFILE);
	ACS_CS_TableEntry neEntry = table->getEntry(ACS_CS_NS::ENTRY_ID_NETWORK_IDENTIFIER);

	int val = 0;
	string neId = "";

	ACS_CS_Attribute omProfileCurrent = omEntry.getValue(ACS_CS_Protocol::Attribute_NE_OmProfileCurrent);
	ACS_CS_Attribute neid = neEntry.getValue(ACS_CS_Protocol::Attribute_NE_NetworkIdentifier);

	if(omProfileCurrent.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
	{
		FAIL() << "Attribute_NotSpecified: Attribute_NE_OmProfileCurrent";
	}
	else
	{
		char buffer[sizeof(int) + 1];
		omProfileCurrent.getValue(buffer, sizeof(int));
		val = *(reinterpret_cast<int  *>(buffer));
		cout  << "Attribute_NE_ApzProfileCurrent = " << val << endl;
	}

	if(neid.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
	{
		FAIL() << "Attribute_NotSpecified: Attribute_NE_NetworkIdentifier";
	}
	else
	{
		char buffer[256];
		neid.getValue(buffer, sizeof(buffer));
		cout  << "Attribute_NE_NetworkIdentifier = " << buffer << endl;
	}

	EXPECT_TRUE(table != NULL);
}


//****************************************************************
//
//  NAME 
//    ACAMSD_MsgCollection.C
//
//  COPYRIGHT
//    COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//    All rights reserved.
//
//    The Copyright to the computer program(s) herein 
//    is the property of Ericsson Utvecklings AB, Sweden.
//    The program(s) may be used and/or copied only with 
//    the written permission from Ericsson Utvecklings AB or in 
//    accordance with the terms and conditions stipulated in the 
//    agreement/contract under which the program(s) have been 
//    supplied.

//  DOCUMENT NO
//    CAA 109 0312

//  AUTHOR 
//    EAB/UZ/DG Carl Johannesson


//  DESCRIPTION
//    This class maintains an ordered sequence of MTAP_Messages.
//    This ordered sequence guarantees that MTAP_Messages are stored
//    correctly on disk, i.e with increasing Message number.

//    This class is also involved in the synchronization phase
//    by indicating missing messages.

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO          DATE          NAME       DESCRIPTION
//    A               2000-05-10    uabcajn    product release
//    B               2004-03-10    uabcajn    rogue wave removed.
//****************************************************************

#include <new>

#include "acs_aca_msd_msg_store.h"
#include "acs_aca_msd_mtap_message.h"
#include "acs_aca_ms_const_values.h"
#include "acs_aca_logger.h"
#include "acs_aca_msd_msg_collection.h"

//****************************************************************
// Constructor
//****************************************************************
ACAMSD_MsgCollection::ACAMSD_MsgCollection (const ACAMSD_MsgStore * owner, unsigned long firstMtapNum, int size) // from parameter list
	: myOwner(owner), myFirstMtapNum(firstMtapNum), //0-5999
		myNumOfMsg(0), myMaxSize(size), // window size, typically 32.
		myMsgArray(new (std::nothrow) ACAMSD_MTAP_Message * [size]) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering - myFirstMtapNum=%lu, myNumOfMsg=%d", myFirstMtapNum, myNumOfMsg);

	if (myMsgArray) for (int i = 0; i < size; myMsgArray[i++] = 0) ;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving - myFirstMtapNum=%lu, myNumOfMsg=%d", myFirstMtapNum, myNumOfMsg);
}

//****************************************************************
// Destructor
//****************************************************************
ACAMSD_MsgCollection::~ACAMSD_MsgCollection () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering - myFirstMtapNum=%lu, myNumOfMsg=%d", myFirstMtapNum, myNumOfMsg);
	delete[] myMsgArray;
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving - myFirstMtapNum=%lu, myNumOfMsg=%d", myFirstMtapNum, myNumOfMsg);
}

//****************************************************************
// Add a Message to the collection if it's not already there, or 
// stored on disk.
//****************************************************************
ACAMSD_MsgCollection::MsgStatus ACAMSD_MsgCollection::addMsg (ACAMSD_MTAP_Message * msg) {
	unsigned int theMtapNumber = msg->getMtapNumber();
	int index = theMtapNumber - myFirstMtapNum;

	if ((index >= myMaxSize) || (index < -myMaxSize)) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Out of range, leaving  - "
				"theMtapNumber=%u, index=%d, myFirstMtapNum=%lu, myNumOfMsg=%d", theMtapNumber, index, myFirstMtapNum, myNumOfMsg);
		return OutOfRange;
	}

	if (theMtapNumber < myFirstMtapNum) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Message already stored, leaving - "
				"theMtapNumber=%u, index=%d, myFirstMtapNum=%lu, myNumOfMsg=%d", theMtapNumber, index, myFirstMtapNum, myNumOfMsg);
		return AlreadyStored; // This Msg has already been written to disk.
	}

	if (myMsgArray[index] != 0) {  // Renew the channel index with latest
		myMsgArray[index]->putChannel(msg->getChannel());
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Message already in the collection, leaving - "
				"theMtapNumber=%u, index=%d, myFirstMtapNum=%lu, myNumOfMsg=%d", theMtapNumber, index, myFirstMtapNum, myNumOfMsg);
		return AlreadyInCollection;
	}

	// OK to add Msg. First complete it with a Message Number.
	unsigned long long msgNum = myOwner->getNextMsgNumber();
	msgNum += index;
	msg->putMsgNumber(msgNum);

	// Now add the Message
	myMsgArray[index] = msg;
	++myNumOfMsg;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Message correctly added to the collection! - "
			"theMtapNumber=%u, index=%d, msgNum=%llu, myFirstMtapNum=%lu, myNumOfMsg=%d",
			theMtapNumber, index, msgNum, myFirstMtapNum, myNumOfMsg);
	return Added;
}

//****************************************************************
// Allocate a buffer containing the message data of all Messages 
// (if any) in the collection in sequence, and remove them from 
// the collection. If parameter 'stopAtFirstHole' is true, 
// messages after the first 'hole' in the sequence will be left
// in the collection.
// Returns number of Messages flushed. The messages flushed are 
// passed to the caller, which is given the responsibility to 
// delete them.
// Also returns number of 'holes' ( = 0 if 'stopAtFirstHole' is 
// true), and number of zero-sized (skipped) messages.
//
// Note that the buffer allocated must be deleted by the user.
//****************************************************************
int ACAMSD_MsgCollection::flush (
		unsigned char * & buffer,
		unsigned int & bufSize,
		bool stopAtFirstHole,
		vector<ACAMSD_MTAP_Message *> & flushedMsgList,
		int & numberOfMissingMsg,
		int & numberOfSkippedMsg,
		int & numberOfFlushedMsg) {
	buffer = 0;
	bufSize = 0;
	numberOfMissingMsg = 0;
	numberOfSkippedMsg = 0;
	numberOfFlushedMsg = 0;

	if (myNumOfMsg == 0) return 0; // No message at all in the collection

	bufSize = getTotalMsgSize(stopAtFirstHole);

	if (bufSize == 0) return 0; // Nothing to flush because the collection starts with a hole

	// There are messages to flush
	buffer = new unsigned char[bufSize]; // Here it's better to avoid nothrow version of new.
	unsigned char * tmpBuf = buffer;
	int i = 0;
	unsigned int bufLengthStore = 0;

	while ((numberOfFlushedMsg < myNumOfMsg) && (i < myMaxSize)) {
		if (myMsgArray[i]) {
			// Message found. Copy its data into the buffer and move it to the list of flushed Messages.
			unsigned int tmpLength = myMsgArray[i]->getDataLength();
			bufLengthStore = bufLengthStore + tmpLength;
			if (tmpLength == (unsigned int)ACAMS_FileOverhead) {
				++numberOfSkippedMsg; // Empty message
			}

			unsigned int theMtapNumber = myMsgArray[i]->getMtapNumber();
			ACS_ACA_LOG(LOG_LEVEL_TRACE, "Message flushed - bufLengthStore=%d, bufSize=%d, tmpLength=%d, "
					"theMtapNumber=%u, index=%d, myFirstMtapNum=%lu, myNumOfMsg=%d",
					bufLengthStore, bufSize, tmpLength, theMtapNumber, i, myFirstMtapNum, myNumOfMsg);

			memcpy(tmpBuf, myMsgArray[i]->getData(), tmpLength);
			tmpBuf += tmpLength;
			flushedMsgList.push_back(myMsgArray[i]);
			++numberOfFlushedMsg;
		}
		else { // There is a hole in the sequence.
			if (stopAtFirstHole) break;
			++numberOfMissingMsg;
		}
		++i;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Message flushed leaving - "
			"numberOfMissingMsg=%d, numberOfFlushedMsg=%d, stopAtFirstHole=%d, numberOfSkippedMsg=%d, "
			"bufLengthStore=%d, bufSize=%d, index=%d, myFirstMtapNum=%lu, myNumOfMsg=%d", numberOfMissingMsg,
			numberOfFlushedMsg, stopAtFirstHole, numberOfSkippedMsg, bufLengthStore, bufSize, i, myFirstMtapNum, myNumOfMsg);
	return numberOfFlushedMsg + numberOfMissingMsg;
}

//****************************************************************
// The message collection has been flushed to a common buffer and 
// written to hard disk. Now the message collection should be up-
// dated as well.
//****************************************************************
void ACAMSD_MsgCollection::flushCommit (int & numberOfFlushedMsg) {
	if (numberOfFlushedMsg > 0) {
		int i;
		if (numberOfFlushedMsg < myNumOfMsg) {
			// We still have messages left. Move them 'numberOfFlushedMsg' steps up
			int fromIndex = numberOfFlushedMsg;
			for (i = 0; i < (myMaxSize - numberOfFlushedMsg); myMsgArray[i++] = myMsgArray[fromIndex++]) {}

// WARNING BEGIN: Here memory leaks are possible
			// Clear all Message Pointers no longer relevant
			for (i = myMaxSize - numberOfFlushedMsg; i < myMaxSize; myMsgArray[i++] = 0) {}
		}
		else for (i = 0; i < myNumOfMsg; myMsgArray[i++] = 0) {}
// WARNING END: Here memory leaks are possible
	}     
      
	// Update state
	myNumOfMsg -= numberOfFlushedMsg;
	myFirstMtapNum += numberOfFlushedMsg;
}

//****************************************************************
// Add all message numbers not yet stored in the collection up to 
// the given message number to the list passed as argument. 
// Return number of msg in list.
//****************************************************************
int ACAMSD_MsgCollection::getMissingMsg (unsigned long uptoMTAP_Number, std::list<unsigned long> & missingList) {
	int diff = uptoMTAP_Number - myFirstMtapNum;
	unsigned long msgNumber = myFirstMtapNum;
	bool hole = false;

	if (diff < -myMaxSize) return -1;

	if (diff > myMaxSize) diff = myMaxSize;

	for (int i = 0; i <= diff; ++i) {
		if ((myMsgArray[i] == 0) || hole) {
			hole = true;
			missingList.push_back(msgNumber);
		}
		++msgNumber;
	}

	return missingList.size();
}

//****************************************************************
//                           Private help-functions
//****************************************************************

//****************************************************************
// Get total size of Messages in collection
//****************************************************************
unsigned int ACAMSD_MsgCollection::getTotalMsgSize (bool stopAtFirstHole) const {
	unsigned int totalSize = 0;
	int i = 0;

	while ( i < myMaxSize ) //to handle buffer overrun case while cp restarted  HR98575
	{
		if (myMsgArray[i] != 0) totalSize += myMsgArray[i]->getDataLength();
		else if (stopAtFirstHole) break; // There is a hole in the sequence, break out.
		++i;
	}
	return totalSize;
}

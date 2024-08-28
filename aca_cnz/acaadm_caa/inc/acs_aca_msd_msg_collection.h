/*=================================================================== */
/**
        @file           acs_aca_msd_msg_collection.h

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACAMSD_MsgCollection class.

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       08/11/2012     XHARBAV   APG43 on Linux.

==================================================================== */

/*=====================================================================
                                DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACAMSD_MsgCollection_H
#define ACAMSD_MsgCollection_H

/*====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <vector>
#include <list>
/*=====================================================================
                                FORWARD DECLARATION SECTION
==================================================================== */
class ACAMSD_MsgStore;
class ACAMSD_MTAP_Message;
/*=====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACAMSD_MsgCollection {
/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief         Constructor for ACAMSD_MsgCollection class.

        @param         owner

        @param         firstMtapNum

        @param         size           


=================================================================== */
	ACAMSD_MsgCollection (const ACAMSD_MsgStore * owner, unsigned long firstMtapNum, int size);
/*=====================================================================
                                CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACAMSD_MsgCollection ();
/*=====================================================================
                                ENUMERATION DECLARATION SECTION
==================================================================== */
/*===================================================================

        @brief         MsgStatus

=================================================================== */
	enum MsgStatus {
		Added,                 // Message added
		AlreadyInCollection,   // This msg was already in the collection
		AlreadyStored,         // This msg has already been written to disk
		OutOfRange             // Message number too large
	};
/*===================================================================

        @brief         addMsg

        @param         msg

=================================================================== */
	MsgStatus addMsg (ACAMSD_MTAP_Message * msg);
/*===================================================================

        @brief         flush
                        Allocate a buffer containing the message data of all Messages (if any) in
                        the collection in sequence, and remove them from the collection.
                        If parameter 'stopAtFirstHole' is true, messages after the first
                        'hole' in the sequence will be left in the collection.
                        Returns number of Messages flushed. The messages flushed are passed
                        to the caller, which is given the responsibility to delete them.
                        Also returns number of missing Messages (to make a complete sequence)
                        if 'stopAtFirstHole' is false, otherwise it is set to zero.
                        Number of skipped messages (= messages of size zero) is also returned.

        @param         buffer

        @param         bufSize

        @param         stopAtFirstHole

        @param         flushedMsgList

        @param         numberOfMissingMsg

        @param         numberOfSkippedMsg

        @param         numberOfFlushedMsg 

=================================================================== */

	int flush (
			unsigned char * & buffer,
			unsigned int & bufSize,
			bool stopAtFirstHole,
			std::vector<ACAMSD_MTAP_Message *> & flushedMsgList,
			int & numberOfMissingMsg,
			int & numberOfSkippedMsg,
			int & numberOfFlushedMsg);

/*===================================================================

        @brief         flushCommit
                        Confirm flushing

        @param         numberOfFlushed

=================================================================== */
	void flushCommit (int & numberOfFlushedMsg);
/*===================================================================

        @brief         getMissingMsg
                        Add all message numbers not yet stored in the collection up to the given
                        message number to the list passed as argument. Return number of msg in list. 

        @param         uptoMTAP_Number

        @param         missingList

        @return        int    

=================================================================== */

	int getMissingMsg (unsigned long uptoMTAP_Number, std::list<unsigned long> & missingList);
/*===================================================================

        @brief         size
                        Return current collection size.
                        
        @return        myNumOfMsg

=================================================================== */
	inline int size () { return myNumOfMsg; }
/*===================================================================

        @brief         maxSize
                        Return collection max size.

        @return        myMaxSize

=================================================================== */

	inline int maxSize () { return myMaxSize; }
/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

        @brief         getTotalMsgSize

        @param         stopAtFirstHole

        @return        unsigned int 

=================================================================== */
	unsigned int getTotalMsgSize (bool stopAtFirstHole = true) const;
/*=====================================================================
                                DATA MEMBERS
==================================================================== */
	const ACAMSD_MsgStore * myOwner;
	unsigned long myFirstMtapNum;      // MTAP message number (0-5999) of the first message in the collection
	int myNumOfMsg;                    // Current size
	int myMaxSize;                     // Maximum size
	ACAMSD_MTAP_Message ** myMsgArray; // Array of pointers to Messages
};

#endif

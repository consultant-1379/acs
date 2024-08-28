/*
 * ACS_PRC_IspLogEvent.h
 *
 *  Created on: Nov 18, 2010
 *      Author: xgiufer
 */

#ifndef ACS_PRC_ISPLOGEVENT_H_
#define ACS_PRC_ISPLOGEVENT_H_

#include <cstdlib>
#include <iostream>
#include <string.h>
#include <fstream>
#include <stdio.h>



class ACS_PRC_IspLogEvent {

private:

//Fields
	std::string hostname;
	std::string runLevel;
	std::string event;
	std::string reason;
	std::string reasonInfo;
	std::string date;
	std::string miscInfo;
	std::string resourceName;

public:
	//constructor
	ACS_PRC_IspLogEvent();
	ACS_PRC_IspLogEvent(const ACS_PRC_IspLogEvent&);

	//destructor
	virtual ~ACS_PRC_IspLogEvent();

	//Set Methods
	void setHostname(std::string h){
		if ( h == "SC-2-1")
			hostname = "A";
		else if  ( h == "SC-2-2")
			hostname = "B";
		else
			hostname = " ";
	}
	void setRunLevel(std::string r){runLevel = r;}
	void setEvent(std::string e){event = e;}
	void setReason(std::string rs){reason = rs;}
	void setReasonInfo(std::string rsi){reasonInfo = rsi;}
	void setDate(std::string d){date = d;}
	void setMiscInfo(std::string m){miscInfo = m;}
	void setResourceName(std::string o){resourceName = o;}

	//Get Methods
	std::string getHostname()const{return this->hostname;}
	std::string getRunLevel()const{return this->runLevel;}
	std::string getEvent()const{return this->event;}
	std::string getReason()const{return this->reason;}
	std::string getReasonInfo()const{return this->reasonInfo;}
	std::string getDate()const{return this->date;}
	std::string getMiscInfo()const{return this->miscInfo;}
	std::string getResourceName()const{return this->resourceName;}

	int getEventSize(){ return hostname.size() + runLevel.size() + event.size() + reason.size() + reasonInfo.size() + date.size() + miscInfo.size() + resourceName.size(); }


//	ACS_PRC_IspLogEvent& operator< (const ACS_PRC_IspLogEvent& x)
//		    {
//				//std::cout<<"minore"<<std::endl;
//		        if( strcmp( date.c_str(), x.getDate().c_str())<0)
//		        return x;
//
//		    }

	ACS_PRC_IspLogEvent& operator= (const ACS_PRC_IspLogEvent& x)
	        {

			hostname.clear();
			runLevel.clear();
			event.clear();
			reason.clear();
			reasonInfo.clear();
			date.clear();
			miscInfo.clear();
			resourceName.clear();
			hostname     = x.hostname;
			runLevel     = x.runLevel;
			event        = x.event;
			reason       = x.reason;
			reasonInfo   = x.reasonInfo;
			date         = x.date;
			miscInfo 	 = x.miscInfo;
			resourceName = x.resourceName;
			//std::cout<<"assign"<<std::endl;
	        return *this;
	        }



};


#endif /* ACS_PRC_ISPLOGEVENT_H_ */

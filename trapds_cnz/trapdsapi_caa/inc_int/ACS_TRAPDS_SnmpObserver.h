/*
 * ACS_TRAPDS_SnmpObserver.h
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_SNMPOBSERVER_H_
#define ACS_TRAPDS_SNMPOBSERVER_H_


#include <boost/serialization/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/assume_abstract.hpp>


class ACS_TRAPDS_SnmpObserver
{

public:
	virtual int sendPackage()=0;

	   friend class boost::serialization::access;
	    //...
	    // only required when using method 1 below
	    // no real serialization required - specify a vestigial one
	    template<class Archive>
	    void serialize(Archive & ar, const unsigned int file_version){}
};



#endif /* ACS_TRAPDS_SNMPOBSERVER_H_ */

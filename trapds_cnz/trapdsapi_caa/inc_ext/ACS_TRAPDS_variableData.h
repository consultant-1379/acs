/*
 * ACS_TRAPDS_variableData.h
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_VARIABLEDATA_H_
#define ACS_TRAPDS_VARIABLEDATA_H_


#include "ACS_TRAPDS_counter64a.h"
#include "ACS_TRAPDS_cl_oid.h"

//#include <boost/serialization/serialization.hpp>
//#include <boost/serialization/list.hpp>
//#include <boost/serialization/string.hpp>
//
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/serialization/export.hpp>

//BOOST_CLASS_TRACKING(ACS_TRAPDS_counter64a, track_never);


class ACS_TRAPDS_variableData{

public:

	ACS_TRAPDS_variableData();

	virtual ~ACS_TRAPDS_variableData();

	long           integer;
	std::string         string;
	ACS_TRAPDS_cl_oid         objid;
	u_char         bitstring[2000];
	ACS_TRAPDS_counter64a 	   counter64;
	float          floatVal;
	double         doubleVal;

	inline ACS_TRAPDS_variableData operator=( ACS_TRAPDS_variableData &other )
	{
		this->integer=other.integer;
		this->string=other.string;
		this->objid=other.objid;
		memset(this->bitstring,0,sizeof(u_char)*2000);
		memcpy(this->bitstring,other.bitstring,sizeof(u_char)*2000);
		this->counter64=other.counter64;
		this->floatVal=other.floatVal;
		this->doubleVal=other.doubleVal;

		return *this;
	}



private:


	friend class boost::serialization::access;
	// When the class Archive corresponds to an output archive, the
	// & operator is defined similar to <<.  Likewise, when the class Archive
	// is a type of input archive the & operator is defined similar to >>.

	template<class Archive>

	void serialize(Archive & ar, const unsigned int /*version*/)
	{
		ar&(integer);
		ar&(string);
		ar&(objid);
		ar&(bitstring);
		ar&(floatVal);
		ar&(doubleVal);
		ar&(counter64);
	}
};


#endif /* ACS_TRAPDS_VARIABLEDATA_H_ */

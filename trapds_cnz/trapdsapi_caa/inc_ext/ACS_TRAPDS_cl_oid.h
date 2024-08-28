/*
 * ACS_TRAPDS_cl_oid.h
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_CL_OID_H_
#define ACS_TRAPDS_CL_OID_H_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/snmp_transport.h>


#include <boost/serialization/serialization.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


class ACS_TRAPDS_cl_oid {
public:

	ACS_TRAPDS_cl_oid();
	ACS_TRAPDS_cl_oid(int l,oid *oid);
	virtual ~ACS_TRAPDS_cl_oid();
	void setData(int lun,oid *);

	u_long oids[150];
	int length;

	inline ACS_TRAPDS_cl_oid operator=( ACS_TRAPDS_cl_oid &other )
	{
		this->length=other.length;
		int i=0;

		for(i=0;i<150;i++)
		{
			this->oids[i]=other.oids[i];
		}

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
		int t;

		ar&length;

		for(t=0;t<length;t++)
		{
			ar&oids[t];
		}

	}

};

#endif /* ACS_TRAPDS_CL_OID_H_ */

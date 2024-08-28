/*
 * ACS_TRAPDS_counter64a.h
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_COUNTER64A_H_
#define ACS_TRAPDS_COUNTER64A_H_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/snmp_transport.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class ACS_TRAPDS_counter64a {

public:

	ACS_TRAPDS_counter64a(u_long high,u_long low);

	inline ACS_TRAPDS_counter64a operator=( ACS_TRAPDS_counter64a &other )
	{
		this->high=other.high;
		this->low=other.low;

		return *this;
	}


	ACS_TRAPDS_counter64a();
	virtual ~ACS_TRAPDS_counter64a();

	u_long          high;
	u_long          low;

private:

		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.

		template<class Archive>

		void serialize(Archive & ar, const unsigned int /*version*/)
		{
			ar&(high);
			ar&(low);
		}

};


#endif /* ACS_TRAPDS_COUNTER64A_H_ */

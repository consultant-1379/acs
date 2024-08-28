/*
 * ACS_TRAPDS_StructVariable.h
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_STRUCTVAR_H_
#define ACS_TRAPDS_STRUCTVAR_H_

#include <vector>

#include "ace/OS.h"
#include "ace/SOCK_Dgram.h"
#include "ace/INET_Addr.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/snmp_transport.h>

//#include <boost/serialization/serialization.hpp>
//#include <boost/serialization/list.hpp>
//#include <boost/serialization/string.hpp>
//#include <boost/serialization/set.hpp>
//#include <boost/serialization/export.hpp>
//#include <boost/serialization/access.hpp>
//#include <boost/serialization/base_object.hpp>
//#include <boost/serialization/export.hpp>
//#include <boost/archive/basic_archive.hpp>
//
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/serialization/tracking.hpp>

#include "ACS_TRAPDS_varlist.h"
//#include "ACS_TRAPDS_counter64a.h"
//#include "ACS_TRAPDS_variableData.h"

//BOOST_CLASS_EXPORT(std::list<ACS_TRAPDS_varlist>);
//BOOST_CLASS_TRACKING(ACS_TRAPDS_varlist, track_never);

class ACS_TRAPDS_StructVariable
{

public:
	ACS_TRAPDS_StructVariable(struct snmp_pdu *pdu, std::string ipTrap);

	virtual ~ACS_TRAPDS_StructVariable();

	int inline insertData()
	{
		struct variable_list *vars = 0;

		if (pdu)
		{
			for(vars = pdu->variables; vars; vars = vars->next_variable)
			{
				ACS_TRAPDS_varlist *va = new ACS_TRAPDS_varlist(vars,ipTrap);

				ACS_TRAPDS_varlist val = (*va);

				nvls.push_back(val);

				delete(va);
			}
		}

		return 0;
	}


	inline ACS_TRAPDS_StructVariable operator=( ACS_TRAPDS_StructVariable &other )
	{
		ACS_TRAPDS_varlist vl;

		 for (std::list<ACS_TRAPDS_varlist>::iterator it = other.nvls.begin(); it != other.nvls.end(); it++)
		 {
			 this->nvls.push_back(*it);
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
		ar&nvls;

//		int i;
//		for(i = 0; i < 10; ++i)
//			ar & *nvls[i];

	}

	struct snmp_pdu *pdu;

	std::string ipTrap;

public:

	std::list<ACS_TRAPDS_varlist> nvls;

	ACS_TRAPDS_varlist nvl;


};


#endif /* ACS_TRAPDS_STRUCTVAR_H_ */

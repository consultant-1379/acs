/*
 * ACS_TRAPDS_varlist.h
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_VARLIST_H_
#define ACS_TRAPDS_VARLIST_H_

#include "ACS_TRAPDS_variableData.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/snmp_transport.h>

//#include <boost/serialization/serialization.hpp>
//#include <boost/serialization/list.hpp>
//#include <boost/serialization/string.hpp>
//
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/serialization/export.hpp>

#include "ACS_TRAPDS_cl_oid.h"

//BOOST_CLASS_TRACKING(ACS_TRAPDS_variableData, track_never);
//BOOST_CLASS_TRACKING(ACS_TRAPDS_cl_oid, track_never);

class ACS_TRAPDS_varlist
{


public:

	ACS_TRAPDS_varlist();

	ACS_TRAPDS_varlist(struct variable_list *vars, std::string ipTrap);

	virtual ~ACS_TRAPDS_varlist();

	ACS_TRAPDS_cl_oid          name;
	size_t          name_length;
	u_char          type;
	ACS_TRAPDS_variableData	val;
	size_t          val_len;
	oid             name_loc[MAX_OID_LEN];
	u_char          buf[40];
//	void           	*data;
//	void            (*dataFreeHook)(void *);
	std::string		ipTrap;
	int             index;


	inline ACS_TRAPDS_varlist operator=( ACS_TRAPDS_varlist &other )
	{
		this->name=other.name;
		this->name_length=other.name_length;
		this->type=other.type;
		this->val=other.val;
		this->val_len=other.val_len;
		memcpy(this->name_loc,other.name_loc,sizeof(oid)*MAX_OID_LEN);
		memcpy(this->buf,other.buf,sizeof(char)*40);
		this->ipTrap=other.ipTrap;
		this->index=other.index;

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
   	//	ar & boost::serialization::base_object<snmp_observer>(*this);

  		ar&(name);
   		ar&(name_length);
   		ar&(type);
   		ar&(val);
   		ar&(val_len);
   		ar&(name_loc);
   		ar&(buf);
   		ar&(ipTrap);
   		ar&(index);
   	}

};

//typedef struct variable_list netsnmp_variable_list;


#endif /* ACS_TRAPDS_VARLIST_H_ */

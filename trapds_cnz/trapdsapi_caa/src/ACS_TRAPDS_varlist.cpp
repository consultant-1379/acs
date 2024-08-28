/*
 * ACS_TRAPDS_varlist.cpp
 *
 *  Created on: 17/gen/2012
 *      Author: renato
 */

#include "ACS_TRAPDS_varlist.h"
#include <iostream>
#include <sstream>

ACS_TRAPDS_varlist::ACS_TRAPDS_varlist() {
	// TODO Auto-generated constructor stub

}

ACS_TRAPDS_varlist::ACS_TRAPDS_varlist(struct variable_list *vars, std::string ipTrap)
{


	this->name.setData(vars->name_length,vars->name);

	this->type=vars->type;

	val.integer=*vars->val.integer;

	unsigned char bchar[1000]={0};
	int string_len = vars->val_len/sizeof(u_char)+1;

	///////
	unsigned char *end=vars->val.string;

	std::cout <<"String received: " << end << std::endl;
	///////

	memcpy(bchar,vars->val.string,string_len);

	val.string = std::string(reinterpret_cast< char const* >(bchar),vars->val_len);

	val_len=vars->val_len;

	val.objid.setData((val_len/sizeof(oid)),vars->val.objid);

	printf("Lunghezza %d %d %u \n",(int)val_len,(int)vars->name_length,(int)(val_len/sizeof(oid)));


	memset(val.bitstring,0,2000);
	memcpy(val.bitstring,vars->val.bitstring,vars->val_len);

	printf ("Bit: ");
	for (int i=0; i<vars->val_len/sizeof(u_char) ; i++ ) {
		printf("%c", val.bitstring[i]);
	}
	printf("\n");

	val.counter64.high=vars->val.counter64->high;
	val.counter64.low=vars->val.counter64->low;

//	if (vars->val.floatVal)
//		val.floatVal=*vars->val.floatVal;
//	else
//		val.floatVal=0;
//
//	if (vars->val.doubleVal)
//		val.doubleVal=*vars->val.doubleVal;
//	else
//		val.doubleVal=0;

	val.floatVal=0;
	val.doubleVal=0;

	memset(this->name_loc,0,sizeof(oid)*MAX_OID_LEN);
	memcpy(this->name_loc, vars->name_loc, MAX_OID_LEN);

	memset(this->buf,0,sizeof(u_char)*40);
	memcpy(this->buf, vars->buf, 40);

	this->ipTrap = ipTrap;

	this->index=index;

};


ACS_TRAPDS_varlist::~ACS_TRAPDS_varlist() {
	// TODO Auto-generated destructor stub
}

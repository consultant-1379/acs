/*
 * ACS_CC_Types.h
 *
 *  Created on: May 26, 2010
 *      Author: designer
 */

#ifndef ACS_CC_TYPES_H_
#define ACS_CC_TYPES_H_
#include "saImm.h"
#include "saImmOm.h"
#include "saImmOi.h"
//#include "saImmOm.h"
#include <iostream>
#include <typeinfo>
#include <vector>
/*the following defines are intended to wrap the related IMM defines
 * providing info about the Class Attribute properties. In particular
 * they are used to specify the general characteristic of the attributes
 *
 */
#define ATTR_MULTI_VALUE SA_IMM_ATTR_MULTI_VALUE
#define ATTR_RDN         SA_IMM_ATTR_RDN
#define ATTR_CONFIG      SA_IMM_ATTR_CONFIG
#define ATTR_WRITABLE    SA_IMM_ATTR_WRITABLE
#define ATTR_INITIALIZED SA_IMM_ATTR_INITIALIZED
#define ATTR_RUNTIME     SA_IMM_ATTR_RUNTIME
#define ATTR_PERSISTENT  SA_IMM_ATTR_PERSISTENT
#define ATTR_CACHED      SA_IMM_ATTR_CACHED
/** the following type is intended to define the attributes flag
 *
 */
typedef SaUint64T ACS_CC_AttrFlagsT;

typedef enum {
	ACS_CC_SUCCESS = 0,
	ACS_CC_FAILURE = 1

}ACS_CC_ReturnType;

typedef enum {
	CONFIGURATION = 1,
	RUNTIME       = 2

}ACS_CC_ClassCategoryType;


typedef enum {
	ATTR_INT32T  = 1, /* Int32T  int */
	ATTR_UINT32T = 2, /* Uint32T unsigned int */
	ATTR_INT64T  = 3, /* Int64T  long long */
	ATTR_UINT64T = 4, /* Uint64T unsigned long long*/
	ATTR_TIMET   = 5, /* TimeT   long long*/
	ATTR_NAMET   = 6, /* NameT   array of char*/
	ATTR_FLOATT  = 7, /* FloatT  float*/
	ATTR_DOUBLET = 8, /* DoubleT double*/
	ATTR_STRINGT = 9, /* StringT array of char*/
	ATTR_ANYT    = 10 /* AnyT */

}ACS_CC_AttrValueType;

typedef struct {
	char * attrName;
	ACS_CC_AttrValueType attrType;
	ACS_CC_AttrFlagsT attrFlag;
	void* attrDefaultVal;
}ACS_CC_AttrDefinitionType;

/*Added For Object creation*/
typedef struct {
	char * attrName;
	ACS_CC_AttrValueType attrType;
	unsigned int attrValuesNum;
	void** attrValues;
}ACS_CC_ValuesDefinitionType;
/*End Added For Object creation*/




typedef enum {
	ACS_APGCC_ONE 		= 1,
	ACS_APGCC_SUBLEVEL 	= 2,
	ACS_APGCC_SUBTREE 	= 3
}ACS_APGCC_ScopeT;

typedef enum {
	ACS_APGCC_DISPATCH_ONE 		= 1,
	ACS_APGCC_DISPATCH_ALL 		= 2,
	ACS_APGCC_DISPATCH_BLOCKING = 3
}ACS_APGCC_DispatchFlags;

typedef SaImmOiHandleT ACS_APGCC_OiHandle;

typedef SaImmOiCcbIdT ACS_APGCC_CcbId;

typedef struct {
	char * attrName;
	ACS_CC_AttrValueType attrType;
	unsigned int attrValuesNum;
	void** attrValues;
}ACS_APGCC_AttrValues;

typedef enum {
	ACS_APGCC_ATTR_VALUES_ADD 		= 1,
	ACS_APGCC_ATTR_VALUES_DELETE 	= 2,
	ACS_APGCC_ATTR_VALUES_REPLACE 	= 3
}ACS_APGCC_AttrModificationTypeT;

typedef struct {
	ACS_APGCC_AttrModificationTypeT  modType;
	ACS_APGCC_AttrValues modAttr;
}ACS_APGCC_AttrModification;


class Types{
public:
	Types(){;}
	virtual ~Types(){;}
};

class StringType :  public Types{
public:
	StringType(){;}
	~StringType(){;}
	std::string value;
};

class IntType :  public Types{
public:
	IntType(){;}
	~IntType(){;}
	int value;
};

class UnsIntType :  public Types{
public:
	UnsIntType(){;}
	~UnsIntType(){;}
	unsigned int value;
};


class LongLongIntType :  public Types{
public:
	LongLongIntType(){;}
	~LongLongIntType(){;}
	long long int value;
};

class UnsLongLongIntType :  public Types{
public:
	UnsLongLongIntType(){;}
	~UnsLongLongIntType(){;}
	unsigned long long int value;
};

class DoubleType :  public Types{
public:
	DoubleType(){;}
	~DoubleType(){;}
	double value;
};


class FloatType :  public Types{
public:
	FloatType(){;}
	~FloatType(){;}
	float value;
};




/*Added For Attribute value Search*/
 struct ACS_CC_ImmParameter {
	char * attrName;
	ACS_CC_AttrValueType attrType;
	unsigned int attrValuesNum;
	void** attrValues;
	int flag;
	ACS_CC_ImmParameter(){ flag = 0; }
	void ACS_APGCC_IMMFreeMemory(int f){ flag = f; }
	~ACS_CC_ImmParameter(){ if (flag != 0 ) delete [] attrValues; }
};
/*End Added For Attribute value Search*/



class ACS_APGCC_ImmAttribute {
public:
	std::string attrName;
	int attrType;
	unsigned int attrValuesNum;
	void* attrValues[2048];
	int flag;
	std::vector<Types *> pointers;
	ACS_APGCC_ImmAttribute(){;}
	~ACS_APGCC_ImmAttribute(){

		Types *c = 0;

		for(unsigned  int i = 0; i<pointers.size(); i++){
			c = pointers[i];
			freeMemory(c);
		}
	}
private:

	void freeMemory(Types *p_pointer){
		IntType *i = dynamic_cast<IntType *>(p_pointer);
		if(i){
			delete i;
			return;
		}

		StringType *p = dynamic_cast<StringType *>(p_pointer);
		if(p){
			delete p;
			return ;
		}


		UnsIntType *u = dynamic_cast<UnsIntType *>(p_pointer);
		if(u){
			delete u;
			return;
		}

		LongLongIntType *l = dynamic_cast<LongLongIntType *>(p_pointer);
		if(l){
			delete l;
			return;
		}

		UnsLongLongIntType *ul = dynamic_cast<UnsLongLongIntType *>(p_pointer);
		if(ul){
			delete ul;
			return;
		}

		DoubleType *d = dynamic_cast<DoubleType *>(p_pointer);
		if(d){
			delete d;
			return;
		}

		FloatType *f = dynamic_cast<FloatType *>(p_pointer);
		if(f){
			delete f;
			return;
		}
	}
};

class ACS_APGCC_ImmObject{

public:
	std::string objName;
	std::vector<ACS_APGCC_ImmAttribute> attributes;
	std::vector<Types *> pointers;
	ACS_APGCC_ImmObject(){;}
	~ACS_APGCC_ImmObject(){

		Types *c = 0;

		for(unsigned  int i = 0; i<pointers.size(); i++){
			c = pointers[i];
			freeMemory(c);
		}
	}

private:

	/**The copy costructor is private to prevent the coping of the object**/
	ACS_APGCC_ImmObject(ACS_APGCC_ImmObject &p_object){

		/*Only to avoid the warning message*/
		objName = p_object.objName;
	}

	/**is private to prevent the coping of the object**/
	ACS_APGCC_ImmObject& operator=(const ACS_APGCC_ImmObject &p_obj){

		/*Only to avoid the warning message*/
		objName = p_obj.objName;
		return *this;
	}

	void freeMemory(Types *p_pointer){
		IntType *i = dynamic_cast<IntType *>(p_pointer);
		if(i){
			delete i;
			return;
		}

		StringType *p = dynamic_cast<StringType *>(p_pointer);
		if(p){
			delete p;
			return ;
		}


		UnsIntType *u = dynamic_cast<UnsIntType *>(p_pointer);
		if(u){
			delete u;
			return;
		}

		LongLongIntType *l = dynamic_cast<LongLongIntType *>(p_pointer);
		if(l){
			delete l;
			return;
		}

		UnsLongLongIntType *ul = dynamic_cast<UnsLongLongIntType *>(p_pointer);
		if(ul){
			delete ul;
			return;
		}

		DoubleType *d = dynamic_cast<DoubleType *>(p_pointer);
		if(d){
			delete d;
			return;
		}

		FloatType *f = dynamic_cast<FloatType *>(p_pointer);
		if(f){
			delete f;
			return;
		}
	}

};

/**the following define for ccb flags
 *
 */
#define NO_REGISTERED_OI 0
#define REGISTERED_OI    1

/*ticket 1963*/
typedef SaImmCcbHandleT ACS_APGCC_CcbHandle;
typedef SaImmAdminOwnerHandleT ACS_APGCC_AdminOwnerHandle;
/*end ticket 1963*/


#endif /* ACS_CC_TYPES_H_ */


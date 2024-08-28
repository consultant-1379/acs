#include "acs_apgcc_oihandler_V4.h"
#include "acs_apgcc_oihandler_V4_impl.h"

acs_apgcc_oihandler_V4::acs_apgcc_oihandler_V4(){
	oiHandlerimpl = new acs_apgcc_oihandler_V4_impl();
}

acs_apgcc_oihandler_V4::acs_apgcc_oihandler_V4(acs_apgcc_oihandler_V4 &oi){
	this->oiHandlerimpl =  new acs_apgcc_oihandler_V4_impl(oi.oiHandlerimpl);
}

acs_apgcc_oihandler_V4::~acs_apgcc_oihandler_V4(){
	delete oiHandlerimpl;
}

char* acs_apgcc_oihandler_V4::getInternalLastErrorText(){
	return oiHandlerimpl->getInternalLastErrorText();
}

int acs_apgcc_oihandler_V4::getInternalLastError(){
	return oiHandlerimpl->getInternalLastError();
}

acs_apgcc_oihandler_V4& acs_apgcc_oihandler_V4::operator=(const acs_apgcc_oihandler_V4 &oiHandler){
	return *this;
}


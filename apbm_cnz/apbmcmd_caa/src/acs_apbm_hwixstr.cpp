
//#include "ACS_APBM_HwiXStr.h"
#include <acs_apbm_hwixstr.h>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>


//XERCES_CPP_NAMESPACE_USE;

ACS_APBM_HwiXStr::ACS_APBM_HwiXStr(const char* const toTranscode)
{
	// Call the private transcoding method
	fUnicodeForm = XERCES_CPP_NAMESPACE::XMLString::transcode(toTranscode);
}

ACS_APBM_HwiXStr::~ACS_APBM_HwiXStr()
{
	XERCES_CPP_NAMESPACE::XMLString::release(&fUnicodeForm);
}


const XMLCh* ACS_APBM_HwiXStr::unicodeForm() const
{
	return fUnicodeForm;
}


void ACS_APBM_HwiXStr::setCharString(const char* toTranscode)
{
	XERCES_CPP_NAMESPACE::XMLString::release(&fUnicodeForm);
	fUnicodeForm = XERCES_CPP_NAMESPACE::XMLString::transcode(toTranscode);
}

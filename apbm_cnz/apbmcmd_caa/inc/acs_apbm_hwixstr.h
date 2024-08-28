
#ifndef ACS_APBM_HwiXStr_H
#define ACS_APBM_HwiXStr_H

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

//XERCES_CPP_NAMESPACE_USE;

class ACS_APBM_HwiXStr {

public:

	ACS_APBM_HwiXStr(const char* const toTranscode);

	virtual ~ACS_APBM_HwiXStr();

	const XMLCh* unicodeForm() const;

	void setCharString(const char* toTranscode);

private:

	XMLCh* fUnicodeForm;
};

#endif /* ACS_APBM_HwiACS_APBM_HwiXStr */

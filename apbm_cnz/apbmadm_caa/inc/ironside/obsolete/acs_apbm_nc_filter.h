#ifndef ACS_CS_NETCONFFILTERINT_H
#define	ACS_CS_NETCONFFILTERINT_H

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <xercesc/util/OutOfMemoryException.hpp>

XERCES_CPP_NAMESPACE_USE


class acs_apbm_nc_filter {
public:

    acs_apbm_nc_filter() {

    }

    virtual ~acs_apbm_nc_filter() {

    }

protected:

    class XStr {
    public:
        // -----------------------------------------------------------------------
        //  Constructors and Destructor
        // -----------------------------------------------------------------------

        XStr(const char* const toTranscode) {
            // Call the private transcoding method
            fUnicodeForm = XMLString::transcode(toTranscode);
        }

        ~XStr() {
            XMLString::release(&fUnicodeForm);
        }

        const XMLCh* unicodeForm() const {
            return fUnicodeForm;
        }

    private:
        XMLCh* fUnicodeForm;
    };

    #define X(str) XStr(str).unicodeForm()

};

#endif	/* ACS_CS_NETCONFFILTERINT_H */


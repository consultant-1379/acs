#include <xercesc/sax/DocumentHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

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

#ifndef ACS_APBM_NETCONFBUILDER_H_
#define ACS_APBM_NETCONFBUILDER_H_

class acs_apbm_netconfbuilder {
public:
        acs_apbm_netconfbuilder();
        virtual ~acs_apbm_netconfbuilder();

        int createShelfQuery(std::string shelfId, std::string physicaladdress, std::string &xml);
	int createBladeQuery(std::string tenantId, std::string blade_id,std::string &xml);
        int createTenantQuery(std::string &xml);
	int createsetbiosimageQuery(std::string tenantId, std::string blade_id,std::string bios_image,std::string &xml);
	int createsetbiospointerQuery(std::string tenantId, std::string blade_id,std::string bios_pointer,std::string &xml);
        int getTenantsNames(std::string &xml);
        int getBladeUserLabel(std::string tenantId, std::string blade_Id,std::string &xml);

private:
       int getXml(DOMDocument* Doc, std::string &xml);
       int getBladeStructure(DOMDocument **docOut,DOMElement **element, std::string tenantId, std::string blade_Id); 

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

#endif /* ACS_APBM_NETCONFBUILDER_H_ */
                

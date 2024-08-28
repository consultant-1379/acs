#ifndef ACS_APBM_NETCONFPARSER_H
#define ACS_APBM_NETCONFPARSER_H

#include <xercesc/sax/DocumentHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <map>
#include <string>
#include <vector>
#include "acs_apbm_cmd_netconfmanager.h"

using namespace xercesc;
class acs_apbm_cmd_netconfmanager;

class acs_apbm_netconfparser : virtual  HandlerBase {
public:
        typedef std::map <std::string,std::string> tenantNamelist;
        acs_apbm_netconfparser();
        acs_apbm_netconfparser(acs_apbm_cmd_netconfmanager* man);
        virtual ~acs_apbm_netconfparser();


        //      Metod to handle errors that occured during parsing
        virtual void warning (const SAXParseException &exception);

        //      Metod to handle errors that occured during parsing
        virtual void error (const SAXParseException &exception);


        //      Metod to handle errors that occured during parsing
        virtual void fatalError (const SAXParseException &exception);

        //      Method to handle the start events that are generated for each element when
        //      an XML file is parsed

        virtual void characters (const XMLCh* const  chars, const unsigned int length);

        //      Method to handle the start events that are generated for each element when
        //      an XML file is parsed

        virtual void startElement(const XMLCh* const name,AttributeList& attributes);

        virtual void endElement(const XMLCh* const name);
       
  	void getphysicaladdress(std::string &ret);
	int  getBiosDefaultImage();
	int  getBiosPointer();
	void setBiosDefaultImage(int m_biosimage);
	void setBiosPointer(int m_biospointer);
        int  getTenants(tenantNamelist &tenantlist);
private:
 	 std::string physicaladdress;
         std::string shelfId;
         int biosImage;
         int biosPointer;

protected:

         void resetFlags();

         bool fl_shelfId;

         bool fl_physicalAddress;
         bool fl_biosPointer;
         bool fl_biosImage;
         bool fl_tenantId;
         tenantNamelist::iterator it;
         tenantNamelist  _tenantNamelist;
         int tenantCounter;  

 
};

#endif
                                                         


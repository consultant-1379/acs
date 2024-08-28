#ifndef ACS_CS_NETCONFSHELFFILTER_H
#define	ACS_CS_NETCONFSHELFFILTER_H

#include <iostream>
#include <sstream>
#include "acs_apbm_nc_filter.h"


#include <xercesc/sax/DocumentHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

using namespace xercesc;

using namespace std;



class acs_apbm_nc_shelffilter : public acs_apbm_nc_filter {
    
    
public:
    acs_apbm_nc_shelffilter();
    acs_apbm_nc_shelffilter(std::string mag,int slot);
    virtual ~acs_apbm_nc_shelffilter();

    virtual int createShelfQuery(std::string shelfId,std::string userLabel,std::string rack,std::string shelfType,std::string position,std::string physicalAddress,std::string &xml);
    int getListOfShelfs(std::string &xml);
    int getShelfidFromPhysicalAddress(std::string physicalAddress,std::string &xml);
    
private:

    int getXml(DOMDocument*d,std::string&);

};

#endif


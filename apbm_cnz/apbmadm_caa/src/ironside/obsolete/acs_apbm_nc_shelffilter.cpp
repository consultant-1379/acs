

#include "acs_apbm_nc_shelffilter.h"
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>
#include <stdio.h>
#include <algorithm>
#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"


acs_apbm_nc_shelffilter::~acs_apbm_nc_shelffilter() {
}

acs_apbm_nc_shelffilter::acs_apbm_nc_shelffilter()
{
    
}



int acs_apbm_nc_shelffilter::createShelfQuery(std::string shelfId,std::string userLabel,std::string rack,std::string shelfType,std::string position,std::string physicalAddress,std::string &xml)
{
    try 
    {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) 
    {
        char *pMsg = XMLString::transcode(toCatch.getMessage());
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
                << "  Exception message:"
                << pMsg;
        XMLString::release(&pMsg);
        return 1;
    }

    /*
   <ManagedElement xmlns="http://www.ericsson.com/dmx">
            <Equipment>
                <Shelf>
                    <physicalAddress>1057</physicalAddress>
                <Slot>
                        <slotId>3</slotId>
                        <Blade>
                    <bladeId/>
                            <firstMacAddress/>
                        </Blade>
                    </Slot>
                </Shelf>
            </Equipment>
        </ManagedElement>
   */

    // Watch for special case help request
    int errorCode = 0;

    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

    if (impl != NULL) {
        
        try 
        {

            DOMDocument* doc =impl->createDocument(0,X("ManagedElement"),0);

            DOMElement* rootElem = doc->getDocumentElement();

            //rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));
            
            DOMElement* managedElementIdElem = doc->createElement(X("managedElementId"));

            DOMText* managedElementIdVal = doc->createTextNode(X(std::string("1").c_str()));

            managedElementIdElem->appendChild(managedElementIdVal);

            rootElem->appendChild(managedElementIdElem);


            DOMElement* equipfElem = doc->createElement(X("Equipment"));

            rootElem->appendChild(equipfElem);

            DOMElement* equipmentIdElem = doc->createElement(X("equipmentId"));

            DOMText* equipmentIdVal = doc->createTextNode(X(std::string("1").c_str()));

            equipmentIdElem->appendChild(equipmentIdVal);

            equipfElem->appendChild(equipmentIdElem);

            DOMElement* shelf = doc->createElement(X("Shelf"));

            equipfElem->appendChild(shelf);

            DOMElement* shelfId_tag = doc->createElement(X("shelfId"));;

            if(!shelfId.empty())
            {
                    DOMText* shelfIdText = doc->createTextNode(X(shelfId.c_str()));
                    shelfId_tag->appendChild(shelfIdText);
                    
            }

            shelf->appendChild(shelfId_tag);

            DOMElement* physicalAddress_tag = doc->createElement(X("physicalAddress"));
            
            if(!physicalAddress.empty())
            {
                   // physicalAddress_tag=doc->createElement(X("physicalAddress"));
                    DOMText* physicalAddressText = doc->createTextNode(X(physicalAddress.c_str()));
                    physicalAddress_tag->appendChild(physicalAddressText);
                   // shelf->appendChild(physicalAddress_tag);
            }
            shelf->appendChild(physicalAddress_tag);

            DOMElement* userLabel_tag = 0;
            
            if(!userLabel.empty())
            {
                    userLabel_tag=doc->createElement(X("userLabel"));
                    DOMText* userLabelText = doc->createTextNode(X(userLabel.c_str()));
                    userLabel_tag->appendChild(userLabelText);
                    shelf->appendChild(userLabel_tag);
            }


            DOMElement* rack_tag = 0;

            if(!rack.empty())
            {
                    rack_tag=doc->createElement(X("rack"));
                    DOMText* rackText = doc->createTextNode(X(rack.c_str()));
                    rack_tag->appendChild(rackText);
                    shelf->appendChild(rack_tag);
            }

            
            DOMElement* shelfType_tag = 0;

            if(!shelfType.empty())
            {
                    shelfType_tag=doc->createElement(X("shelfType"));
                    DOMText* shelfTypeText = doc->createTextNode(X(shelfType.c_str()));
                    shelfType_tag->appendChild(shelfTypeText);
                    shelf->appendChild(shelfType_tag);
            }
            

            DOMElement* position_tag = 0;

            if(!position.empty())
            {
                    position_tag=doc->createElement(X("position"));
                    DOMText* positionText = doc->createTextNode(X(position.c_str()));
                    position_tag->appendChild(positionText);
                    shelf->appendChild(position_tag);
            }            

            
            getXml(doc,xml);
            

            doc->release();
        } catch (const OutOfMemoryException&) {
            XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
            errorCode = 5;
        } catch (const DOMException& e) {
            XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
            errorCode = 2;
        } catch (...) {
            XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
            errorCode = 3;
        }
    }// (inpl != NULL)
    else {
        XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
        errorCode = 4;
    }

    XMLPlatformUtils::Terminate();
    return errorCode;
}

int acs_apbm_nc_shelffilter::getListOfShelfs(std::string &xml)
{
    try 
    {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) 
    {
        char *pMsg = XMLString::transcode(toCatch.getMessage());
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
                << "  Exception message:"
                << pMsg;
        XMLString::release(&pMsg);
        return 1;
    }
    // Watch for special case help request
    int errorCode = 0;

    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

    if (impl != NULL) {
        
        try 
        {
            DOMDocument* doc =impl->createDocument(0,X("ManagedElement"),0);

            DOMElement* rootElem = doc->getDocumentElement();

            rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));

            DOMElement* equipfElem = doc->createElement(X("Equipment"));

            rootElem->appendChild(equipfElem);

            DOMElement* shelf = doc->createElement(X("Shelf"));

            equipfElem->appendChild(shelf);

            DOMElement* shelfId_tag = doc->createElement(X("shelfId"));;

            shelf->appendChild(shelfId_tag);
            
            getXml(doc,xml);
            
            doc->release();
        } catch (const OutOfMemoryException&) {
            XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
            errorCode = 5;
        } catch (const DOMException& e) {
            XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
            errorCode = 2;
        } catch (...) {
            XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
            errorCode = 3;
        }
    }// (inpl != NULL)
    else {
        XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
        errorCode = 4;
    }

    XMLPlatformUtils::Terminate();
    return errorCode;

}




int acs_apbm_nc_shelffilter::getXml(DOMDocument* Doc,std::string &xml) {

    XMLCh* gOutputEncoding = 0;

    gOutputEncoding = XMLString::transcode("UTF-8");
    
    std::string xmlStr;

    try {
        // get a serializer, an instance of DOMWriter
        XMLCh tempStr[10000];
        XMLString::transcode("LS", tempStr, 10000);
        DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
        DOMWriter *theSerializer = ((DOMImplementationLS*) impl)->createDOMWriter();

        // set user specified output encoding
        theSerializer->setEncoding(gOutputEncoding);

        XMLCh* out = theSerializer->writeToString(*Doc);

        char *msg = XERCES_CPP_NAMESPACE::XMLString::transcode(out);

        xmlStr=msg;

        int found = xmlStr.find_first_of("?");
        int last = xmlStr.find_first_of("?", found + 1);

        last += 1;
        std::string t = xmlStr.replace(found - 1, last + 1, "");
//        printf("%s\n %d %d\n", t.c_str(), found - 1, last + 1);

        free(out);
        free(msg);
        
        xmlStr=t;
        
        
        delete theSerializer;
    //    delete impl;

    } catch (const OutOfMemoryException&) {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
    } catch (XMLException& e) {
    }
    
    free(gOutputEncoding);

    ACS_APBM_LOG(LOG_LEVEL_INFO, "TO REMOVE:Entering Method %s",  xmlStr.c_str());
    xml=xmlStr;

    return 0;
}


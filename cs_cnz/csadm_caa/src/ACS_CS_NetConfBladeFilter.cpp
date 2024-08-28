/* 
 * File:   ACS_CS_NetConfBladeFilter.cpp
 * Author: renato
 * 
 * Created on 10 ottobre 2012, 17.08
 */

#include "ACS_CS_NetConfBladeFilter.h"
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
//#include <xercesc/dom/DOMWriter.hpp>

//XERCES 3.1
#include <xercesc/dom/DOMLSSerializer.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>
#include <stdio.h>
#include <algorithm>


ACS_CS_NetConfBladeFilter::ACS_CS_NetConfBladeFilter() {

}

ACS_CS_NetConfBladeFilter::~ACS_CS_NetConfBladeFilter() {
}

    
int ACS_CS_NetConfBladeFilter::createBladeQuery(std::string physicalAddress,std::string blade_id,std::string slot_id,std::string productname,std::string productnumber,std::string productrevisionstate,std::string firstmacaddress,std::string &xml)
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
        try {

          DOMDocument* doc =impl->createDocument(0,X("ManagedElement"),0);

		  DOMElement* rootElem = doc->getDocumentElement();

		  rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));

		  DOMElement* equipfElem = doc->createElement(X("Equipment"));

		  rootElem->appendChild(equipfElem);

		  DOMElement* shelf = doc->createElement(X("Shelf"));

		  equipfElem->appendChild(shelf);

		  DOMElement* physicalAddress_tag = doc->createElement(X("physicalAddress"));

		  if(!physicalAddress.empty())
		  {
			  DOMText* physicalAddressText = doc->createTextNode(X(physicalAddress.c_str()));
			  physicalAddress_tag->appendChild(physicalAddressText);
		  }

		  shelf->appendChild(physicalAddress_tag);

		  DOMElement* slot = doc->createElement(X("Slot"));

		  shelf->appendChild(slot);

		  DOMElement* slotId = doc->createElement(X("slotId"));

		  if(!slot_id.empty())
		  {
			  DOMText* slotIdVal = doc->createTextNode(X(slot_id.c_str()));
			  slotId->appendChild(slotIdVal);
		  }

		  slot->appendChild(slotId);

		  DOMElement* bladeElem = doc->createElement(X("Blade"));

		  slot->appendChild(bladeElem);

		  DOMElement* bladeId = doc->createElement(X("bladeId"));

		  if(!blade_id.empty())
		  {
			  DOMText* bladeIdVal = doc->createTextNode(X(blade_id.c_str()));
			  bladeId->appendChild(bladeIdVal);
		  }

		  bladeElem->appendChild(bladeId);

		  DOMElement* productName = doc->createElement(X("productName"));

		  if(!productname.empty())
		  {
			  DOMText* productNameVal = doc->createTextNode(X(productname.c_str()));
			  productName->appendChild(productNameVal);
		  }

		  DOMElement* productNumber = doc->createElement(X("productNumber"));

		  if(!productnumber.empty())
		  {
			  DOMText* productNumberVal = doc->createTextNode(X(productnumber.c_str()));
			  productNumber->appendChild(productNumberVal);
		  }

		  DOMElement* productRevisionState = doc->createElement(X("productRevisionState"));

		  if(!productrevisionstate.empty())
		  {
			  DOMText* productRevisionStateVal = doc->createTextNode(X(productrevisionstate.c_str()));
			  productRevisionState->appendChild(productRevisionStateVal);
		  }


/*		  DOMElement* firstMacAddress = doc->createElement(X("firstMacAddress"));

		  if(!firstmacaddress.empty())
		  {
			  DOMText* firstMacAddressVal = doc->createTextNode(X(firstmacaddress.c_str()));
			  firstMacAddress->appendChild(firstMacAddressVal);
		  }*/


		  bladeElem->appendChild(productName);
		  bladeElem->appendChild(productNumber);
		  bladeElem->appendChild(productRevisionState);
	//	  bladeElem->appendChild(firstMacAddress);

//            unsigned int elementCount = doc->getElementsByTagName(X("*"))->getLength();
/*            XERCES_STD_QUALIFIER cout << "The tree just created contains: " << elementCount
                    << " elements." << XERCES_STD_QUALIFIER endl;
*/
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

int ACS_CS_NetConfBladeFilter::getXml(DOMDocument* Doc,std::string &xml) {

/*    XMLCh* gOutputEncoding = 0;

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
    
    xml=xmlStr;
*/

	//XERCES 3.1 - BEGIN
	DOMImplementation* impl = DOMImplementation::getImplementation();

	DOMLSSerializer* theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();

	XMLCh* toTranscode = theSerializer->writeToString(Doc);

	//xml = std::string(XMLString::transcode(toTranscode));
	char* xmlChar = XERCES_CPP_NAMESPACE::XMLString::transcode(toTranscode);

	xml = std::string(xmlChar);

	XERCES_CPP_NAMESPACE::XMLString::release(&xmlChar);
	XERCES_CPP_NAMESPACE::XMLString::release(&toTranscode);
	theSerializer->release();
	//XERCES 3.1 - END

    return 0;
}



int ACS_CS_NetConfBladeFilter::createMultipleQueryBlade(std::string blade_id1,std::string blade_id2,std::string productname,std::string productnumber,std::string productrevisionstate,std::string firstmacaddress,std::string &xml)
{
   try 
    {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& toCatch) {
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
        try {
            DOMDocument* doc = impl->createDocument(0,X("Shelf"),0);

            DOMElement* rootElem = doc->getDocumentElement();

            DOMElement* prodElem = doc->createElement(X("shelfId"));
            rootElem->appendChild(prodElem);

            DOMText* prodDataVal = doc->createTextNode(X("0"));
            prodElem->appendChild(prodDataVal);

            DOMElement* slot = doc->createElement(X("Slot"));
            rootElem->appendChild(slot);

            DOMElement* bladeElem = doc->createElement(X("Blade"));

            slot->appendChild(bladeElem);

            DOMElement* bladeId = doc->createElement(X("bladeId"));
            
            if(!blade_id1.empty())
            {
                DOMText* bladeIdVal = doc->createTextNode(X(blade_id1.c_str()));
                bladeId->appendChild(bladeIdVal);
            }
            

            bladeElem->appendChild(bladeId);

            DOMElement* productName = doc->createElement(X("productName"));
            
            if(!productname.empty())
            {
                DOMText* productNameVal = doc->createTextNode(X("1"));
                productName->appendChild(productNameVal);
            }
            
            DOMElement* productNumber = doc->createElement(X("productNumber"));

            if(!productnumber.empty())
            {
                DOMText* productNumberVal = doc->createTextNode(X(productnumber.c_str()));
                productNumber->appendChild(productNumberVal);
            }

            DOMElement* productRevisionState = doc->createElement(X("productRevisionState"));

            if(!productrevisionstate.empty())
            {
                DOMText* productRevisionStateVal = doc->createTextNode(X(productrevisionstate.c_str()));
                productRevisionState->appendChild(productRevisionStateVal);
            }
            

            DOMElement* firstMacAddress = doc->createElement(X("firstMacAddress"));
            
            if(!firstmacaddress.empty())
            {
                DOMText* firstMacAddressVal = doc->createTextNode(X(firstmacaddress.c_str()));
                firstMacAddress->appendChild(firstMacAddressVal);
            }
            
   
            bladeElem->appendChild(productName);
            bladeElem->appendChild(productNumber);
            bladeElem->appendChild(productRevisionState);
            bladeElem->appendChild(firstMacAddress);

            
            if(!blade_id2.empty())
            {
                
                DOMElement* bladeElem2 = doc->createElement(X("Blade"));
                slot->appendChild(bladeElem2);

                DOMElement* bladeId2 = doc->createElement(X("bladeId"));
            
                DOMText* bladeIdVal2 = doc->createTextNode(X(blade_id2.c_str()));
                bladeId2->appendChild(bladeIdVal2);
                
                bladeElem2->appendChild(bladeId2);
                
                bladeElem2->appendChild(productName);
                bladeElem2->appendChild(productNumber);
                bladeElem2->appendChild(productRevisionState);
                bladeElem2->appendChild(firstMacAddress);
                
            }

            
            unsigned int elementCount = doc->getElementsByTagName(X("*"))->getLength();
            XERCES_STD_QUALIFIER cout << "The tree just created contains: " << elementCount
                    << " elements." << XERCES_STD_QUALIFIER endl;

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

int ACS_CS_NetConfBladeFilter::createShelfQuery(std::string shelfId,std::string userLabel,std::string rack,std::string shelfType,std::string position,std::string physicalAddress,std::string &xml)
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

            DOMElement* equipIdElem = doc->createElement(X("equipmentId"));
            DOMText* equipIdVal = doc->createTextNode(X(std::string("1").c_str()));
            equipIdElem->appendChild(equipIdVal);
            equipfElem->appendChild(equipIdElem);


            DOMElement* shelf = doc->createElement(X("Shelf"));

            equipfElem->appendChild(shelf);

            DOMElement* shelfId_tag = doc->createElement(X("shelfId"));;

            if(!shelfId.empty())
            {
                    DOMText* shelfIdText = doc->createTextNode(X(shelfId.c_str()));
                    shelfId_tag->appendChild(shelfIdText);

            }

            shelf->appendChild(shelfId_tag);

            DOMElement* physicalAddress_tag=doc->createElement(X("physicalAddress"));

            if(!physicalAddress.empty())
            {
                    DOMText* physicalAddressText = doc->createTextNode(X(physicalAddress.c_str()));
                    physicalAddress_tag->appendChild(physicalAddressText);
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

int ACS_CS_NetConfBladeFilter::createMacBladeQuery(std::string vEqm_id, std::string blade_id,std::string productname,std::string productnumber,std::string productrevisionstate,std::string firstmacaddress,std::string &xml)
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

    int errorCode = 0;

    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

    if (impl != NULL) {
        try {

                  DOMDocument* doc =impl->createDocument(0,X("ManagedElement"),0);

                  DOMElement* rootElem = doc->getDocumentElement();

                 // rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));

                 
                  DOMElement* managedElementIdElem = doc->createElement(X("managedElementId"));
                  DOMText* managedElementIdVal = doc->createTextNode(X(std::string("1").c_str()));
                  managedElementIdElem->appendChild(managedElementIdVal);

                  rootElem->appendChild(managedElementIdElem);


                  DOMElement* DmxcfElem = doc->createElement(X("DmxcFunction"));

                  rootElem->appendChild(DmxcfElem);

                  DOMElement* DmxcfElemIdElem = doc->createElement(X("dmxcFunctionId"));
                  DOMText* DmxcfIdVal = doc->createTextNode(X(std::string("1").c_str()));
                  DmxcfElemIdElem->appendChild(DmxcfIdVal);
                  DmxcfElem->appendChild(DmxcfElemIdElem);

                  DOMElement* eqm = doc->createElement(X("Eqm"));

                  DmxcfElem->appendChild(eqm);
                  DOMElement* eqmIdElem = doc->createElement(X("eqmId"));
                  DOMText* eqmIdVal = doc->createTextNode(X(std::string("1").c_str()));
                  eqmIdElem->appendChild(eqmIdVal);
                  eqm->appendChild(eqmIdElem);


                  DOMElement* virtualEq = doc->createElement(X("VirtualEquipment"));

                  eqm->appendChild(virtualEq);
                  DOMElement* virtualEqIdElem = doc->createElement(X("virtualEquipmentId"));
                  DOMText* virtualEqIdVal = doc->createTextNode(X(vEqm_id.c_str()));
                  virtualEqIdElem->appendChild(virtualEqIdVal);
                  virtualEq->appendChild(virtualEqIdElem);


                  DOMElement* bladeElem = doc->createElement(X("Blade"));

                  virtualEq->appendChild(bladeElem);

                  DOMElement* bladeId = doc->createElement(X("bladeId"));

                  if(!blade_id.empty())
                  {
                          DOMText* bladeIdVal = doc->createTextNode(X(blade_id.c_str()));
                          bladeId->appendChild(bladeIdVal);
                  }

                  bladeElem->appendChild(bladeId);

                  DOMElement* productName = doc->createElement(X("productName"));

                  if(!productname.empty())
                  {
                          DOMText* productNameVal = doc->createTextNode(X(productname.c_str()));
                          productName->appendChild(productNameVal);
                  }

                  DOMElement* productNumber = doc->createElement(X("productNumber"));

                  if(!productnumber.empty())
                  {
                          DOMText* productNumberVal = doc->createTextNode(X(productnumber.c_str()));
                          productNumber->appendChild(productNumberVal);
                  }

                  DOMElement* productRevisionState = doc->createElement(X("productRevisionState"));

                  if(!productrevisionstate.empty())
                  {
                          DOMText* productRevisionStateVal = doc->createTextNode(X(productrevisionstate.c_str()));
                          productRevisionState->appendChild(productRevisionStateVal);
                  }


            /*      DOMElement* firstMacAddress = doc->createElement(X("firstMacAddr"));

                  if(!firstmacaddress.empty())
                  {
                          DOMText* firstMacAddressVal = doc->createTextNode(X(firstmacaddress.c_str()));
                          firstMacAddress->appendChild(firstMacAddressVal);
                  }

*/
                  DOMElement* serialNumber = doc->createElement(X("serialNumber"));
                  DOMElement* vendorName = doc->createElement(X("vendorName"));
                  DOMElement* manufacturingDate = doc->createElement(X("manufacturingDate"));




                  bladeElem->appendChild(productName);
                  bladeElem->appendChild(productNumber);
                  bladeElem->appendChild(productRevisionState);
  //                bladeElem->appendChild(firstMacAddress);
                  bladeElem->appendChild(serialNumber);
                  bladeElem->appendChild(vendorName);
                  bladeElem->appendChild(manufacturingDate);

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

int ACS_CS_NetConfBladeFilter::createBladeLabelQuery(std::string vEqm_id, std::string blade_id,std::string &xml)
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

	int errorCode = 0;

	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	if (impl != NULL) {
		try {

			DOMDocument* doc =impl->createDocument(0,X("ManagedElement"),0);

			DOMElement* rootElem = doc->getDocumentElement();

			// rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));


			DOMElement* managedElementIdElem = doc->createElement(X("managedElementId"));
			DOMText* managedElementIdVal = doc->createTextNode(X(std::string("1").c_str()));
			managedElementIdElem->appendChild(managedElementIdVal);

			rootElem->appendChild(managedElementIdElem);


			DOMElement* DmxcfElem = doc->createElement(X("DmxcFunction"));

			rootElem->appendChild(DmxcfElem);

			DOMElement* DmxcfElemIdElem = doc->createElement(X("dmxcFunctionId"));
			DOMText* DmxcfIdVal = doc->createTextNode(X(std::string("1").c_str()));
			DmxcfElemIdElem->appendChild(DmxcfIdVal);
			DmxcfElem->appendChild(DmxcfElemIdElem);

			DOMElement* eqm = doc->createElement(X("Eqm"));

			DmxcfElem->appendChild(eqm);
			DOMElement* eqmIdElem = doc->createElement(X("eqmId"));
			DOMText* eqmIdVal = doc->createTextNode(X(std::string("1").c_str()));
			eqmIdElem->appendChild(eqmIdVal);
			eqm->appendChild(eqmIdElem);


			DOMElement* virtualEq = doc->createElement(X("VirtualEquipment"));

			eqm->appendChild(virtualEq);
			DOMElement* virtualEqIdElem = doc->createElement(X("virtualEquipmentId"));
			DOMText* virtualEqIdVal = doc->createTextNode(X(vEqm_id.c_str()));
			virtualEqIdElem->appendChild(virtualEqIdVal);
			virtualEq->appendChild(virtualEqIdElem);


			DOMElement* bladeElem = doc->createElement(X("Blade"));

			virtualEq->appendChild(bladeElem);

			DOMElement* bladeId = doc->createElement(X("bladeId"));

			if(!blade_id.empty())
			{
				DOMText* bladeIdVal = doc->createTextNode(X(blade_id.c_str()));
				bladeId->appendChild(bladeIdVal);
			}

			bladeElem->appendChild(bladeId);

			DOMElement* userLabel = doc->createElement(X("userLabel"));

			bladeElem->appendChild(userLabel);

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


int ACS_CS_NetConfBladeFilter::createTenantQuery(std::string &xml)
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

	int errorCode = 0;

	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	if (impl != NULL) {
		try {

			DOMDocument* doc =impl->createDocument(0,X("ManagedElement"),0);

			DOMElement* rootElem = doc->getDocumentElement();

			// rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));


			DOMElement* managedElementIdElem = doc->createElement(X("managedElementId"));
			DOMText* managedElementIdVal = doc->createTextNode(X(std::string("1").c_str()));
			managedElementIdElem->appendChild(managedElementIdVal);

			rootElem->appendChild(managedElementIdElem);


			DOMElement* DmxcfElem = doc->createElement(X("DmxcFunction"));

			rootElem->appendChild(DmxcfElem);

			DOMElement* DmxcfElemIdElem = doc->createElement(X("dmxcFunctionId"));
			DOMText* DmxcfIdVal = doc->createTextNode(X(std::string("1").c_str()));
			DmxcfElemIdElem->appendChild(DmxcfIdVal);
			DmxcfElem->appendChild(DmxcfElemIdElem);

			DOMElement* eqm = doc->createElement(X("Eqm"));

			DmxcfElem->appendChild(eqm);
			DOMElement* eqmIdElem = doc->createElement(X("eqmId"));
			DOMText* eqmIdVal = doc->createTextNode(X(std::string("1").c_str()));
			eqmIdElem->appendChild(eqmIdVal);
			eqm->appendChild(eqmIdElem);


			DOMElement* virtualEq = doc->createElement(X("VirtualEquipment"));

			eqm->appendChild(virtualEq);
			DOMElement* virtualEqIdElem = doc->createElement(X("virtualEquipmentId"));
			virtualEq->appendChild(virtualEqIdElem);

			getXml(doc,xml);

			doc->release();
		}
		catch (const OutOfMemoryException&) {
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



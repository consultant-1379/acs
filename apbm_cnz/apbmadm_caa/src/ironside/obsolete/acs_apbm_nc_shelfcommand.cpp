
#include "acs_apbm_nc_shelfcommand.h"

#include <stdio.h>

typedef std::map <std::string,std::string> nresponse; 


acs_apbm_nc_shelfcommand::acs_apbm_nc_shelfcommand():
	_parser(0),
	_sender(0),
	_shelf_filter(0)
{

}

acs_apbm_nc_shelfcommand::~acs_apbm_nc_shelfcommand() {
    
	if (_parser)
		delete(_parser);

	if (_sender)
		delete(_sender);

	if (_shelf_filter)
		delete(_shelf_filter);
}


int acs_apbm_nc_shelfcommand::execute()
{
   int status=1;
   
   std::string res("");
   int result=0;
   
   if (!_sender || !_parser)
	   return status;

   result = _sender->sendNetconfMessage(xml_filter);
   
   if(result==0)
   {
     res = _sender->getData();
    
     try
     {
         XMLPlatformUtils::Initialize();
     }
     catch (const XMLException& e)
     {

     }

     SAXParser* parser = new SAXParser;

     parser->setValidationScheme(SAXParser::Val_Auto);
     parser->setDoNamespaces(false);
     parser->setDoSchema(false);
     parser->setValidationSchemaFullChecking(false);

     DocumentHandler* docHandler =  (HandlerBase*)_parser;

     ErrorHandler* errHandler = (ErrorHandler*) docHandler;

     parser->setDocumentHandler(docHandler);
     parser->setErrorHandler(errHandler);

     MemBufInputSource myxml_buf((const XMLByte*)res.c_str(), res.size(), "example",false);

     try
     {
    	 _parser->clearResponse();
    	 parser->parse(myxml_buf);

    	 //_parser->getResponse(_response);

    	 status=0;

     }
     catch (const XMLException& toCatch) 
     {
             message_error="Parse Error";
             message_error_type=0;
         
             char* message = XMLString::transcode(toCatch.getMessage());
             cout << "Exception message is: \n"
                  << message << "\n";
             XMLString::release(&message);
     }
     catch (const SAXParseException& toCatch) 
     {
         
            message_error="Parse Error";
            message_error_type=0;

             char* message = XMLString::transcode(toCatch.getMessage());
             cout << "Exception message is: \n"
                  << message << "\n";
             XMLString::release(&message);
     }
     catch (...) 
     {
            message_error="Parse Error";
            message_error_type=0;

             cout << "Unexpected Exception \n" ;
     }

         delete parser;
         
   }
   else
   {
        message_error=_sender->getErrorMessage();
        message_error_type=_sender->getErrorType();
   }

   return status;

}


void acs_apbm_nc_shelfcommand::setParser(acs_apbm_nc_parser* new_par)
{
	if (_parser)
	{
		delete _parser;
		_parser = 0;
	}
    _parser = new_par;
    
}


void acs_apbm_nc_shelfcommand::getResult(nresponse &ret)
{
	if (_parser)
		_parser->getResponse(ret);
}


void acs_apbm_nc_shelfcommand::setSender(acs_apbm_nc_sender* send)
{
	if (_sender)
	{
		delete _sender;
		_sender = 0;
	}
    _sender = send;

}

 void acs_apbm_nc_shelfcommand::setXmlFilter(std::string filter)
 {
    xml_filter = filter;
 }
 
 void acs_apbm_nc_shelfcommand::setFilter(acs_apbm_nc_filter* filter)
 {
	 if (_shelf_filter)
	 {
		 delete _shelf_filter;
		 _shelf_filter = 0;
	 }

     _shelf_filter = (acs_apbm_nc_shelffilter*)filter;
 }
 
const char *acs_apbm_nc_shelfcommand::getErrorMessage()
{
    return message_error;
}

int acs_apbm_nc_shelfcommand::getErrorType()
{
    return message_error_type;
}


int acs_apbm_nc_shelfcommand::getErrorSeverity()
{
    return message_error_severity;
}

const char* acs_apbm_nc_shelfcommand::getErrorMessageInfo()
{
    return message_error_info;
}


 int acs_apbm_nc_shelfcommand::createShelfQuery(std::string shelfId,std::string userLabel="",std::string rack="",std::string shelfType="",std::string position="",std::string physicalAddress="")
 {
     int res=0;
     
     if (_shelf_filter)
    	 res = _shelf_filter->createShelfQuery(shelfId,userLabel,rack,shelfType,position,physicalAddress,xml_filter);
     else
    	 res = 1;
     
     return res;
 }
 
 
 int acs_apbm_nc_shelfcommand::getListOfShelfs()
 {
     
    int res=0;

    if (_shelf_filter)
    	res = _shelf_filter->getListOfShelfs(xml_filter);
    else
    	res = 1;

    return res;
 }

    

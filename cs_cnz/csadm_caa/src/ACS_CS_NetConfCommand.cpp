
#include "ACS_CS_NetConfCommand.h"

#include <stdio.h>
#include "ACS_CS_Trace.h"
typedef std::map <std::string,std::string> nresponse; 
ACS_CS_Trace_TDEF(ACS_CS_NetConfCommand_TRACE);

ACS_CS_NetConfCommand::ACS_CS_NetConfCommand():
    	    		xml_filter(""),message_error(),message_error_type(0),
    	    		message_error_severity(0),message_error_info(),
    	    		par(0),
    	    		netSen(0)
{

}

ACS_CS_NetConfCommand::~ACS_CS_NetConfCommand() {
    
	if (par)
		delete(par);

	if (netSen)
		delete(netSen);
}

int ACS_CS_NetConfCommand::executeUdp(std::string &udp_data)
{
   int result=0;
   udp_data="";
   cout << "ACS_CS_NetConfCommand::executeUDP : Entering Method"<<endl;

   ACS_CS_FTRACE((ACS_CS_NetConfCommand_TRACE, LOG_LEVEL_INFO,
   "ACS_CS_NetConfCommand::executeUDP()\n"
   "Entering Method"));


   result=netSen->sendUdpMessage(xml_filter);
   if(result==0)
   {
     udp_data=netSen->getUdpData();
     cout<<"udp_data"<<udp_data<<endl;
   
   }
   ACS_CS_FTRACE((ACS_CS_NetConfCommand_TRACE, LOG_LEVEL_INFO,
     "ACS_CS_NetConfCommand::executeUdp()\n"
     "udp_data = %s",udp_data.c_str()));
   return result;

}
int ACS_CS_NetConfCommand::execute()
{
   int status=1;
 
   cout << "ACS_CS_NetConfCommand::execute : Entering Method"<<endl;

   ACS_CS_FTRACE((ACS_CS_NetConfCommand_TRACE, LOG_LEVEL_INFO,
   "ACS_CS_NetConfCommand::execute()\n"
   "Entering Method"));
 
   std::string res("");
   int result=0;
   
   if (!netSen || !par)
	   return status;

   result=netSen->sendNetconfMessage(xml_filter);
   
   if(result==0)
   {
     res=netSen->getData();
    
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

     DocumentHandler* docHandler =  (HandlerBase*)par;

     ErrorHandler* errHandler = (ErrorHandler*) docHandler;

     parser->setDocumentHandler(docHandler);
     parser->setErrorHandler(errHandler);

     MemBufInputSource myxml_buf((const XMLByte*)res.c_str(), res.size(), "example",false);

     cout << "ACS_CS_NetConfCommand::execute : Response is "<<res.c_str()<<endl;

     ACS_CS_FTRACE((ACS_CS_NetConfCommand_TRACE, LOG_LEVEL_INFO,
     "ACS_CS_NetConfCommand::execute()\n"
     "Response = %s",res.c_str()));


     try
     {
          parser->parse(myxml_buf);

          ACS_CS_FTRACE((ACS_CS_NetConfCommand_TRACE, LOG_LEVEL_INFO,
          "ACS_CS_NetConfCommand::execute()\n"
          "parser->parse(myxml_buf)"));

          par->getResponse(respon);
          
          status=0;
          
     }
     catch (const XMLException& toCatch) 
     {
             message_error="Parse Error";
             message_error_type=0;
         
             char* message = XMLString::transcode(toCatch.getMessage());
             cout << "Exception message is: \n"
                  << message << "\n";


          ACS_CS_FTRACE((ACS_CS_NetConfCommand_TRACE, LOG_LEVEL_INFO,
          "ACS_CS_NetConfCommand::execute()\n"
          "XMLException message is %s",message));

             XMLString::release(&message);
     }
     catch (const SAXParseException& toCatch) 
     {
         
            message_error="Parse Error";
            message_error_type=0;

             char* message = XMLString::transcode(toCatch.getMessage());
             cout << "Exception message is: \n"
                  << message << "\n";

          ACS_CS_FTRACE((ACS_CS_NetConfCommand_TRACE, LOG_LEVEL_INFO,
          "ACS_CS_NetConfCommand::execute()\n"
          "SAXParserException message is %s",message));
             
             XMLString::release(&message);
     }
     catch (...) 
     {
            message_error="Parse Error";
            message_error_type=0;

             cout << "Unexpected Exception \n" ;

           ACS_CS_FTRACE((ACS_CS_NetConfCommand_TRACE, LOG_LEVEL_INFO,
           "ACS_CS_NetConfCommand::execute()\n"
           "Unexpected Exception"));
     }

         delete parser;
         
   }
   else
   {
        message_error=netSen->getErrorMessage();
        message_error_type=netSen->getErrorType();
   }

   ACS_CS_FTRACE((ACS_CS_NetConfCommand_TRACE, LOG_LEVEL_INFO,
   "ACS_CS_NetConfCommand::execute()\n"
   "return status is %d",status));

   cout << "ACS_CS_NetConfCommand::execute : return status is "<<status<<endl;
   return status;

}


void ACS_CS_NetConfCommand::setParser(ACS_CS_NetConfParserInt*par)
{
    this->par=par;
    
}


void ACS_CS_NetConfCommand::getResult(nresponse &ret)
{
    ret=respon;
}


void ACS_CS_NetConfCommand::setSender(ACS_CS_NetConfSenderInt* send)
{
    netSen=send;
}

 void ACS_CS_NetConfCommand::setFilter(std::string filter)
 {
    xml_filter=filter;
 }
 
const char *ACS_CS_NetConfCommand::getErrorMessage()
{
    return message_error;
}

int ACS_CS_NetConfCommand::getErrorType()
{
    return message_error_type;
}


int ACS_CS_NetConfCommand::getErrorSeverity()
{
    return message_error_severity;
}

const char* ACS_CS_NetConfCommand::getErrorMessageInfo()
{
    return message_error_info;
}
    


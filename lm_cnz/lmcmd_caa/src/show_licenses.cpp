//******************************************************************************
//
//  NAME
//     show_licenses.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2009. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR 
//     2009-04-27 by XCSRPAD PA1
//
//  SEE ALSO 
//     -
//
//******************************************************************************
#include "acs_lm_common.h"
#include "acs_lm_common_util.h"  /* HW-Decoupling: Defined fetching IP address method on ACS_LM_Common_Util class. */
#include "acs_lm_cmd.h"
#include "acs_lm_cmdclient.h"
#include<iostream>
#include<string.h>
#define quote  "\""
struct ShowLk
{
	std::string lkid;
	std::string paramName;
	std::string setName;
	std::string beginDate;
	std::string endDate;
	std::string value;
	std::string vendorInfo;
};

std::string fingerPrint = "";
std::string arbitrary = "";

static void printResult(std::list<ShowLk*>& );
//static ACS_LM_AppExitCode checkOpt(const argc, char** argv);


static ACS_LM_AppExitCode checkOpt(int argc, char** argv)
{
	ACS_LM_AppExitCode exitCode = ACS_LM_RC_INCUSAGE;
	if(!ACS_LM_Common::isCommandAllowed())
	{
		return ACS_LM_RC_CMD_NOTALLOWED;
	}
    if(argc == 3)
    {       
        if(strcmp(argv[1], "-e") == 0)
        {
			if(::strlen(argv[2])!=0)
            {
                if(::strlen(argv[2])>70)
                {
                    exitCode = ACS_LM_RC_UNREAS;
                }
                else
                {
                    if(!ACS_LM_Common::isAlphanumeric(argv[2]))
                    {
                         exitCode = ACS_LM_RC_UNREAS;
                    }
                    else
                    {
                        exitCode = ACS_LM_RC_OK;
                    }
                    
                }
                 arbitrary = argv[2];
            }
            else
            {
                 arbitrary = "";
                exitCode = ACS_LM_RC_OK;
            }
            
        }           
    }      
	else if(argc == 1)
	{
		exitCode = ACS_LM_RC_OK;
    }

	return exitCode;
}

//int status,value;
int main(int argc, char** argv)
{
    std::list<ShowLk*> outList;
   	ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
	std::string lservPath;
	exitCode = checkOpt(argc,argv);
	DEBUG("show_licenses::main()- %s","Entering");
	//cout <<endl<<"show_licenses::main()-Entering"<<endl;
	if(exitCode != ACS_LM_RC_OK)
	{
		std::string ftpVolPath = "/license_file";
		ACS_LM_Common::getLicenseKeyFilePath(ftpVolPath);
		ACS_LM_Common::getlatestFile(ftpVolPath, lservPath);
		//XLUDIR TR HU82186
		if(exitCode != ACS_LM_RC_INCUSAGE)
			std::cout<< "ERROR: "<< lservPath << " " <<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
		else
			std::cout<< "ERROR: "<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
		//END
	}
	else
	{   
        // get the output from fpprint       
        exitCode = ACS_LM_RC_SERVERNOTRESPONDING;
		ACS_LM_CmdClient cmdClient;
		//if(cmdClient.connect(".", LM_CMD_SERVICE))
#if 0
                DEBUG("show_licenses::main() - cmdClient.connect() %s","Entering"); 
                //cout << "calling cmdClient.connect()" << endl;

		if(cmdClient.connect())
		{
			ACS_LM_Cmd cmdSend(ACS_LM_CMD_FPPRINT, argc, argv);
  
                        DEBUG("show_licenses::main() - cmdClient.send() %s","Entering"); 
                        //cout << "calling cmdClient.send()" << endl;

			if(cmdClient.send(cmdSend))
			{
				ACS_LM_Cmd cmdRecv;
  
                                DEBUG("show_licenses::main() - cmdClient.cmdRecv() %s","Entering"); 
                                //cout << "calling cmdClient.cmdRecv()" << endl;

				if(cmdClient.receive(cmdRecv))
				{
					if((exitCode=(ACS_LM_AppExitCode)cmdRecv.commandCode()) == ACS_LM_RC_OK)
					{
						std::list<std::string> args = cmdRecv.commandArguments();
						{
							fingerPrint = (*args.begin());
						}			
					}				
				}
			}
		}
#endif

        ACS_LM_CmdClient cmdClient1;
		// get the list of LK's
	    //if(cmdClient1.connect(".", LM_CMD_SERVICE))
  
            DEBUG("show_licenses::main() - cmdClient1.connect() %s","Entering"); 
           // cout << "calling cmdClient1.connect()" << endl;

	    if(cmdClient1.connect())
		{
            ACS_LM_Cmd cmdSend(ACS_LM_CMD_SHOWLICENSE, argc, argv);
  
                        DEBUG("show_licenses::main() - cmdClient1.send() %s","Entering"); 
                       // cout << "calling cmdClient1.send()" << endl;

			if(cmdClient1.send(cmdSend))
			{
				do
				{
					ACS_LM_Cmd cmdRecv;
  
                                        DEBUG("show_licenses::main() - cmdClient1.cmdRecv() %s","Entering"); 
                                       // cout << "calling cmdClient1.cmdRecv()" << endl;

					if(cmdClient1.receive(cmdRecv))
					{
						exitCode=(ACS_LM_AppExitCode)cmdRecv.commandCode();
						std::list<std::string> argList = cmdRecv.commandArguments();
						
						if(argList.size() == 7)
						{
							ShowLk* lk = new ShowLk();
							std::list<std::string>::iterator itr = argList.begin();
							lk->lkid = (*(itr++));
							lk->paramName = (*(itr++));
							lk->setName = (*(itr++));
							lk->beginDate = (*(itr++));
							lk->endDate = (*(itr++));
							lk->value = (*(itr++));
							lk->vendorInfo = (*(itr++));
							outList.push_back(lk);
							argList.clear();
						}
					}
					else
					{	
						break;
					}
				}while(exitCode == ACS_LM_RC_MOREDATA);
			}
		}
		if(exitCode == ACS_LM_RC_OK)
		{
			printResult(outList);
		}
		else
		{
			std::cout<<"ERROR: "<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
		}
		cmdClient1.disconnect();
		//cout << "leaving cmdClient1.disconnect()" << endl;
    }

    outList.clear();
	return exitCode;
}
static void printXmlversion()
{
    std::cout<< "<?xml version=" << quote << "1.0" << quote << " encoding=" << quote << "utf-8" << quote << "?>" << std::endl;

    std::cout << "<LicenseInventory xmlns:xsi=" << quote << "http://www.w3.org/2001/XMLSchema-instance" << quote<<  std::endl;
    std::cout << "xmlns=" << quote << "http://www.ericsson.se/oss/shm/licpi/" << quote << std::endl;
    std::cout << "xsi:schemaLocation=" << quote << "http://www.ericsson.se/oss/shm/licpi/LicenseInventory.xsd"<< quote << ">" <<std::endl;  
}
static void printHeader()
{
	//Get the current date and time
 	time_t curTimeUtc = ::time(NULL); 
	struct tm curTimeLocal = *localtime(&curTimeUtc); 

	OmHandler* theOmHandlerPtr = 0;
	theOmHandlerPtr = new OmHandler();

       	if(theOmHandlerPtr != 0)
	{
		if( theOmHandlerPtr->Init() == ACS_CC_FAILURE )
		{
			DEBUG("show_license - printHeader : License_install %s","theOmHandlerPtr->Init() FAILED");
			std::cout<<"ERROR: OmHandlerInit Failed" << std::endl;
			return;
		}
	}
	else
	{
		return;
	}

	std::string myFingerPrint("");
	/* HW-Decoupling: Ignoring "generateFingerPrint" method defined on "ACS_LM_Common" class.*/ 
	bool myFingerPrintFlag = ACS_LM_Common_Util::generateFingerPrint(myFingerPrint, theOmHandlerPtr);
	if(!myFingerPrintFlag)
        {
                std::cout << "Failure in fetching the Finger Print " << std::endl;
                return;
        }

	theOmHandlerPtr->Finalize();

	std::cout<< "\t" << "<Fingerprint>" <<  myFingerPrint << "</Fingerprint>" << std::endl;    
    
	std::cout<< "\t"<<"<TimeStamp>"  << std::setw(4)<< std::setfill('0') << curTimeLocal.tm_year + 1900 
			<< "-" << std::setw(2) << std::setfill('0') << curTimeLocal.tm_mon + 1 
			<< "-" << std::setw(2) << std::setfill('0') << curTimeLocal.tm_mday 
			<< "T" << std::setw(2) << std::setfill('0') << curTimeLocal.tm_hour 
			<< ":" << std::setw(2) << std::setfill('0') << curTimeLocal.tm_min 
			<< ":" << std::setw(2) << std::setfill('0') << curTimeLocal.tm_sec << "</TimeStamp>" << std::endl;      
}
static void printLicensekeys(std::list<ShowLk*>& outList)
{
    char setParName[35];
    
    for(std::list<ShowLk*>::iterator it=outList.begin();
		it != outList.end(); ++it)
	{        
        bool  startDateAvailable = true;
        bool  endDateAvailable = true;
        ::memset(setParName,0,35);

		ShowLk* lk = (*it);                
//        std::string searchString( "/" );
//        std::string replaceString( "-" );
        std::string startDate = lk->beginDate.c_str();
        replace(startDate.begin(),startDate.end(),'/','-');
        std::string endDate = lk->endDate.c_str();
        replace(endDate.begin(),endDate.end(),'/','-');
        if((::strlen(lk->paramName.c_str())==0) || (::strlen(lk->setName.c_str())==0))
        {
             std::snprintf(setParName, 35, "%s",lk->paramName.c_str());
        }
        else
        {
            std::snprintf(setParName, 35, "%s_%s",
                lk->setName.c_str(),lk->paramName.c_str());
        }
       
//        std::string::size_type pos = 0;
        //Start HQ38227
        if(startDate.compare("NEVER")== 0)

        {
            startDateAvailable = false;
        }

        if(endDate.compare("NEVER")== 0)
        {
             endDateAvailable = false;
        }
        //End HQ38227
		std::cout <<"\t" << "<LicenseKey id=" << quote<< lk->lkid.c_str() << quote << ">" << std::endl;        
        std::cout <<"\t\t" << "<Description>" << lk->vendorInfo.c_str()<< "</Description>" << std::endl;        
        std::cout <<"\t\t" << "<value>" << lk->value.c_str()<< "</value>" << std::endl;
        std::cout <<"\t\t" << "<NodeSpecificAttributes>"<<std::endl;
        std::cout <<"\t\t\t" << "<Attribute name=" << quote << "SETNAME_PARNAME" <<quote << ">"
                                          << setParName << "</Attribute>" <<  std::endl;
        std::cout <<"\t\t" << "</NodeSpecificAttributes>"<<std::endl;
        if(startDateAvailable)
        {
            std::cout <<"\t\t" << "<start>" << startDate.c_str() << "</start>" << std::endl;  
        }
        else
        {
            std::cout <<"\t\t" << "<start />" << std::endl; 
        }
        if(endDateAvailable)
        {
            std::cout <<"\t\t" <<"<stop>"<<endDate.c_str()<<"</stop>"<<std::endl;  
        }
        else
        {
            std::cout <<"\t\t" << "<stop />" <<std::endl;  
        }
        std::cout <<"\t" <<"</LicenseKey>"<<std::endl;
              	delete lk;
	}
    
}
 static void printFooter()
 {
    std::cout << "</LicenseInventory>" << std::endl;
    std::cout << arbitrary <<std::endl;

 }

 static void printResult(std::list<ShowLk*>& outList)
{   	
    printXmlversion();
    printHeader();
    printLicensekeys(outList);
    printFooter();	   	
}


//======================================================================
//
// NAME
//      TransferQueue.cpp
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-08-19 by EEDKBR
// CHANGES
//     
//======================================================================

#include "acs_hcs_transferqueue.h"

#include <sstream>
//#include <strstream>

#ifndef ACS_HCS_SIMULATION_USE

#include "aes_afp_api.h"
#include "aes_gcc_errorcodes.h"
#include "aes_ohi_filehandler.h"
#include "aes_ohi_errorcodes.h"
#include "acs_hcs_configuration.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_tracer.h"
#include "acs_hcs_global_ObjectImpl.h"

#include "acs_hcs_progressReport_objectImpl.h"
#include "acs_hcs_progressReportHandler.h"
#include "acs_hcs_healthcheckservice.h"


using namespace std;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	void TransferQueue::sendFilesToTq(const string& jobId, const string& transferQueue, const string& jobName, const string time_stamp, int compressionState )
	{
		DEBUG("%s","Entering TransferQueue::sendFilesToTq ");  
		cout<<"jobId"<<jobId<<" jobName  "<<jobName<<" time_stamp"<<time_stamp<<endl;
		try

		{
			//To transfer the contents of the Job's report directory
			char * pDest = (char *) transferQueue.c_str();
			cout<<"inside Entering TransferQueue::sendFilesToTq"<<endl;
			string filePath = "";
			filePath = Configuration::Singleton::get()->getReportJobName(jobId) + "/" + time_stamp;
			cout<<"the file path is: "<<filePath<<endl;	
			if( compressionState == 0 ) //Compression is enabled, so, take the zip file path
			{	
				sendFileToTq(/*file,*/ pDest, filePath, jobId, jobName,time_stamp, 0);
			}
			else
			{
				cout<<"no compression"<<endl;
				sendFileToTq(/*file,*/ pDest, filePath,jobId,jobName,time_stamp );
			}	
		}
		catch(...)
		{
			//ACS_HCS_TRACE("sendFilesToTq(): Unable to send files.");
		}
		DEBUG("%s","Leaving TransferQueue::sendFilesToTq ");
	}

	void TransferQueue::sendFileToTq(/*const string& fileName,*/ char * pDest, const string& filePath, const string& jobId, const string& jobName, const string time_stamp, int compressionState )
	{
		DEBUG("%s","Entering TransferQueue::sendFileToTq( ");
		unsigned int result = 0;
		size_t pos = -1;
		bool tqExists_flag = false;
		unsigned int index;
		acs_hcs_global_ObjectImpl obj;
		try
		{
			cout<<"Inside TransferQueue::sendFileToTq, file path is: "<<filePath<<endl;
			//create directory
			string dirDestPath = Configuration::Singleton::get()->getTQDirName(pDest); 
			cout<<"dirDestPath is "<<dirDestPath<<endl;		 
			Directory dirDestTQname(dirDestPath);	
			dirDestTQname.createDir(dirDestPath);
			if ( !dirDestTQname.exists())	
		        {
				cout<<"Destination Directory for the tRansfer Queue not created"<<endl;
				DEBUG("%s","Destination Directory for the Transfer Queue not created");
				return;
			}
			AES_OHI_FileHandler fileHandler("ACS", "acs_hcd", pDest, "", dirDestPath.c_str()); //filePath.c_str());
			result = fileHandler.attach();
                        //DEBUG("result in second debug statement for TransferQueue::sendFileT Tq is :: %d",result);	
			cout<<"result is:"<<result<<endl;
			switch (result)
			{
			case AES_OHI_NOERRORCODE:
				cout << "sendFileToTq(): No Error Code" << endl;
				tqExists_flag = true;
				DEBUG("%s","sendFileToTq(): No Error Code");
				break;
			case AES_OHI_NOPROCORDER:
				cout << "sendFileToTq(): AES_OHI_NOPROCORDER" << endl;
				break;
			case AES_OHI_NODESTINATION:
				cout << "sendFileToTq(): Transfer Queue \"" << pDest << "\" not defined in the output handler" << endl << endl;
				break;
			case AES_OHI_NOSERVERACCESS:
				cout << "sendFileToTq(): \"AES_AFP_server\" server not available" << endl << endl;
				break;
			case AES_OHI_APPLDIRNOTFOUND:
				cout<<" source/destination folder does not exist"<<endl;
				break;
			default:
				cout << "sendFileToTq(): Invalid Transfer Queue" << endl;
				DEBUG("%s","sendFileToTq(): Invalid Transfer Queue");
				break;
			}
			index = obj.getProgressReportIndex(jobName);
			 // TR HS37486.. checking whether progress report exists or not before updating it
			if(AcsHcs::HealthCheckService::progressDeleted == false)
			{
				if(result == AES_OHI_APPLDIRNOTFOUND)
					 acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo +="Reports folder doe not exist;\n";
				
				else if(result != AES_OHI_NOERRORCODE  )
					acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo +="Destination URI does not exist;\n";
			}
			if(tqExists_flag == true)
			{
				Directory dirReportJob(filePath);
				cout<<"file path:"<<filePath<<endl;
				cout<<" Move all the files from the current directory to the destination directory"<<endl;
				DEBUG("%s", "Move all the files from the current directory to the destination directory");
				dirReportJob.moveTo(dirDestPath);
				dirReportJob.remove();
				cout<<"Files Moved to the Destination Path"<<endl;
				Directory::FileNames fileNames;
				string fileNametoSearch = "";
				string file = "";	
				if(compressionState == 0 )
				{		
					cout<<"Inside compressionState"<<endl;
					fileNametoSearch = jobId +"_" + time_stamp + ".zip";
					cout<<"in transfer queue the zip file path is: "<<fileNametoSearch<<endl;
					dirDestTQname.ls(fileNames,fileNametoSearch.c_str());
				}
			        else
				{
					
					dirDestTQname.ls(fileNames,false,true);
		
				}
				cout<<"after else"<<endl;

				for(Directory::FileNames::const_iterator fileName = fileNames.begin(); fileName != fileNames.end(); ++fileName)
				{
					cout<<"in for loop"<<endl;
					file = (string)fileName->c_str();
					cout<<"file is: "<<file<<endl;
					pos = file.find_last_of("/");
					cout<<"pos is:"<<pos<<endl;
					if(pos != size_t(-1))
					{ 
						file = (string) file.substr(pos + 1, file.length() - pos);
						cout<<" file name is "<<endl;

					}
					if( compressionState == 0 )
					{
						//As we are sending zipped file, changed it AES_OHI_BINARY
						result = fileHandler.send(file.c_str(),AES_OHI_BINARY);
					}
					else
					{
						cout<<"in else of for"<<endl;
						result = fileHandler.send(file.c_str(),AES_OHI_ASCII);
						cout<<"result is: "<<result<<endl;
					}		
					if (result != AES_NOERRORCODE)
					{
						cout << "sendFileToTq(): Filename  "  << file.c_str()  << " could not be sent via transfer queue." << endl;
						DEBUG("sendFileToTq(): Filename %s has not been sent via transfer queue",file.c_str());
					}
					else
					{
						DEBUG("sendFileToTq(): Filename %s has been sent via transfer queue",file.c_str());
						cout << "sendFileToTq(): Filename  "  << file.c_str()  << " has been sent via transfer queue." << endl;
					}	
				}
				cout<<"after for loop"<<endl;
				result = fileHandler.detach();

				if (result != AES_NOERRORCODE)
				{
					cout << "sendFileToTq(): Failed to disconnect file handler." << endl;
				}
				else
				{
					cout << "sendFileToTq(): Disconnected file handler successfully" << endl;
					DEBUG("%s","sendFileToTq(): Disconnected file handler successfully");
				}
			}
		}
		catch (const TransferQueue::ExceptionTQNotFound& ex)
		{
			cout<<" Tq Not Found"<<endl;
		}
		catch(...)
		{

		}
		DEBUG("%s","Leaving TransferQueue::sendFileToTq( ");
	}


	bool TransferQueue::isTQValid(const string& transferQueue)
	{ 
		DEBUG("%s","Entering TransferQueue::isTQValid ");
		unsigned int result = 0;
		bool isTQValid = false;
		char * pDest = (char *) transferQueue.c_str();
		string filePath = Configuration::Singleton::get()->getReportsDirName();

		// Replace all "/" by "\" to make the transfer-queue work.
		size_t pos = string::npos;

		while ((pos = filePath.rfind("/", pos)) != string::npos)
			filePath.replace(pos, 1, "\\");

		AES_OHI_FileHandler fileHandler("ACS", "HCS", pDest, "", filePath.c_str());

		result = fileHandler.attach();

		stringstream info;

		if(result == AES_OHI_NOERRORCODE)
		{
			info << "isTQValid():" << "transferQueue '" << transferQueue << "' is valid.";
			//ACS_HCS_TRACE(info.str().c_str());
			isTQValid = true;
		}
		else
		{
			info << "isTQValid():" << "transferQueue '" << transferQueue << "' is invalid.";
			//ACS_HCS_TRACE(info.str().c_str());
			isTQValid = false;
		}

		result = fileHandler.detach();

		if(result == AES_OHI_NOERRORCODE)
		{
			//ACS_HCS_TRACE("isTQValid(): File handler detached successfully.");
		}
		else
		{
			//ACS_HCS_TRACE("isTQValid(): File handler could not be detached.");
		}
		DEBUG("%s","Leaving TransferQueue::isTQValid ");
		return isTQValid;
	}
}

#else // ACS_HCS_SIMULATION_USE

using namespace std;

namespace AcsHcs
{
	void TransferQueue::sendFilesToTq(const string& jobId, const string& transferQueue)
	{
		DEBUG("%s","Entering  TransferQueue::sendFilesToTq ");
		 DEBUG("%s","Leaving  TransferQueue::sendFilesToTq ");	
	}

	void TransferQueue::sendFileToTq(const string& fileName, char * pDest, const string& filePath)
	{
		DEBUG("%s","Entering TransferQueue::sendFileToTq ");
		DEBUG("%s","Leaving TransferQueue::sendFileToTq ");
	}

	bool TransferQueue::isTQValid(const string& transferQueue)
	{
		DEBUG("%s","returning true TransferQueue::isTQValid ");
		return true;
	}
}

#endif // ACS_HCS_SIMULATION_USE

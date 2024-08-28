#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <ACS_APGCC_CommonLib.h>
#include "acs_hcs_inotify.h"
#include "acs_hcs_healthcheckservice.h"

#define EVENT_SIZE		(sizeof(struct inotify_event))
#define EVENT_BUF_LEN     	(1024 * (EVENT_SIZE + 16))
#define RULESETFILE 		"HealthCheckRuleSetFile"
#define ATTRFILE "fileName"

using namespace std;

class DocumentXml;

bool Inotify::stopRequested = false;
bool Inotify::threadExited = false;

Inotify::Inotify()
{
DEBUG("%s","In Inotify constructor ");
}

void Inotify::watch_dir()
{
	DEBUG("%s"," Entering  Inotify::watch_dir(");
	int length, i = 0;
	int fd;
	int wd;
	char buffer[EVENT_BUF_LEN];
	struct inotify_event *event;
        string check;
	string fileUploaded;
	string completePath;
	bool eventReceived = false;

	/* print files in current directory in reverse order */

	struct dirent **namelist;
	int n,size;
	//n = scandir("/data/opt/ap/internal_root/health_check/rules/", &namelist, 0, alphasort); 
	n = scandir(NBI_RULES, &namelist, 0, alphasort);
	removeExistingRID();	
	if(n>=0)
	{ 
		while(n--) 
		{ 
			check=string(namelist[n]->d_name);
			if(check!="." && check!="..")
			{
				size=check.length();
				// start of TR HU53889
				std::size_t found = check.find_last_of(".\\");
				if(found!=std::string::npos)
				{
				string temp = check.substr(found+1,3);
				if(temp == "xml")
				//if(check[size-4]=='.' && check[size-3]=='x' && check[size-2]=='m' && check[size-1]=='l')
				{
					//completePath = "/data/opt/ap/internal_root/health_check/rules/" + check;
					completePath = NBI_RULES + check;
				        ACE_OS::sleep(1);	
					ruleSetFileModify(check, completePath);
				}
				}
				// end of TR HU53889
			}
			free(namelist[n]); 
		} 
		free(namelist); 
	} 

	fd = inotify_init();

	if ( fd < 0 ) 
	{
	        perror("inotify_init");
		cout << "inotify_init failed" << endl;
	}

	//wd = inotify_add_watch( fd, "/data/opt/ap/internal_root/health_check/rules", IN_CREATE | IN_DELETE | IN_MODIFY);
	wd = inotify_add_watch( fd, NBI_RULES, IN_CREATE | IN_DELETE | IN_MODIFY);

	int ret;
	while(!Inotify::stopRequested)
	{	
		i = 0;
		struct pollfd myPollFds;
		myPollFds.fd = fd;
		myPollFds.events = POLLIN;
		ret = poll(&myPollFds, 1, 500);
		if (ret == 0)
		{
			continue;
		}
		else if (ret < 0)
		{
			continue;
		}
		else if(ret >= 1)
		{
			if(myPollFds.revents == POLLIN && !Inotify::stopRequested)
			{
				length = read(fd, buffer, EVENT_BUF_LEN);
			}
			else
			{
				length = -1;
			}
		}
		//length = read( fd, buffer, EVENT_BUF_LEN ); 

		if ( length < 0 ) 
		{
			perror("read");
			cout<< "Read for inotify failed" << endl;
		}  
		while ( i < length )
		{ 
			event = ( struct inotify_event*) &buffer[i];    
			if ( event->len ) 
			{
				if (event->mask & IN_CREATE) 
				{
					DEBUG("%s","CREATE inotify call received");
					check = event->name;
					if(check[0] != '.')
					{
						eventReceived = true;
						cout<< event->name << " has been created in the NBI path"<<endl;
						// start of TR HU53889
						size_t found = check.find_last_of(".\\");
						if (found != std::string::npos)
						{
						//fileUploaded = check.substr(check.size()-4, 4);
						fileUploaded = check.substr(found+1,3);
						if(fileUploaded != "xml")
						{	DEBUG("%s"," Uploaded file is not XML file in inotify watch_dir()");
							cout << "Uploaded file is not XML file" << endl;
							break;
						}
						}
						//end of TR HU53889
						//completePath = "/data/opt/ap/internal_root/health_check/rules/" + check;
						completePath = NBI_RULES + check;
							
						cout<<completePath<<endl;
						ACE_OS::sleep(2);		
						ruleSetFileInit(check, completePath);
						cout << "RulesetFile node added..." << endl;
					}
				}
				else if (event->mask & IN_DELETE) 
				{
					cout << "DELETE" << endl;
					DEBUG("%s","DELETE inotify call received");
					check = event->name;
					if(check[0] != '.')
					{
						cout << event->name << " has been deleted from NBI path" << endl;
						//completePath = "/data/opt/ap/internal_root/health_check/rules/" + check;
						completePath = NBI_RULES + check;
						ruleSetFileRemove(check);
						cout << "RulesetFile node removed..." << endl;
					}
				}
				else if (event->mask & IN_MODIFY)
				{
					cout << "MODIFY" << endl;
					DEBUG("%s","MODIFY inotify call received");
					if(eventReceived == false)
					{
						check = event->name;
						if(check[0] != '.')
						{
							eventReceived = true;
							cout << event->name << " has been modified in NBI path"<<endl;
							//completePath = "/data/opt/ap/internal_root/health_check/rules/" + check;
							completePath = NBI_RULES + check;
						  	ACE_OS::sleep(2);	
							ruleSetFileModify(check, completePath);
							cout << "Rulesetfile node modified..." << endl;
						}
					}
					else 
					{
						eventReceived = false;
					}
				}
			}
			i += EVENT_SIZE + event->len;
		}
		ACE_OS::sleep(1);; // Sleep for a while to reduce the load
	}
	DEBUG("%s","Leaving  Inotify::watch_dir( ");
	close( fd );
}

int Inotify::svc()
{
	DEBUG("%s","Entering Inotify::svc() ");
	cout<<"In inotify thread"<<endl;
	//int loop = 0;
	Inotify obj;	

	obj.watch_dir();
	cout << "Inotify exiting" << endl;
	Inotify::threadExited = true;
	DEBUG("%s","Leaving Inotify::svc() ");
	return 0;
}


void Inotify::ruleSetFileInit(string check, string completePath)
{
	DEBUG("%s","Entering Inotify::ruleSetFileInit(string check, string completePath) ");
	std::string ruleSetId = "";
        
	try
	{
		AcsHcs::DocumentXml doc(completePath);
		ruleSetId = doc.getRulesetIdValue();	
		if ( ruleSetId == "")
                {
                         cout << "ERROR while uploading : XML read error" << endl;
                         return;
                }

	}
	catch(...)
	{
		cout << "ERROR while uploading : XML read error" << endl;
		return;
	}
	
	cout<<"rulesetId value is : "<<ruleSetId<<endl;
	OmHandler immHandler;
	ACS_CC_ReturnType returnCode;
	returnCode=immHandler.Init();
	if(returnCode!=ACS_CC_SUCCESS)
	{
	        cout<<"ERROR: init FAILURE!!!\n";
        }

	vector<ACS_CC_ValuesDefinitionType> ruleSetFileAttrList;
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeFileName;
	char *className = const_cast<char*>(RULESETFILE);

        attributeFileName.attrName = (char*)fileNameAttr;
        attributeFileName.attrType = ATTR_STRINGT;
        char* temp = const_cast<char*>(check.c_str());
        void* value1[1]={ reinterpret_cast<void*>(temp)};
        attributeFileName.attrValuesNum = 1;
        attributeFileName.attrValues = value1;

	string r1(rulesetFileIdAttr_with_EQU);
	string r2(ruleSetId);
	string r3 = r1 + r2;

        attributeRDN.attrName = (char*)rulesetFileIdAttr;
        attributeRDN.attrType = ATTR_STRINGT;
        attributeRDN.attrValuesNum = 1;
	
        char* rdnValue = const_cast<char*>(r3.c_str());
        void* value2[1]={ reinterpret_cast<void*>(rdnValue)};
        attributeRDN.attrValues = value2;
		
	ruleSetFileAttrList.push_back(attributeFileName);  
	ruleSetFileAttrList.push_back(attributeRDN);

        returnCode = immHandler.createObject(className, READYTOUSEDN, ruleSetFileAttrList);	
	if(returnCode == ACS_CC_FAILURE)
	{
		if(immHandler.getInternalLastError() == -14)
		{
			ACS_CC_ImmParameter changed_fileName;
			char fileAttr[]=fileNameAttr;
			changed_fileName.attrName = fileAttr;
			changed_fileName.attrType = ATTR_STRINGT;
			changed_fileName.attrValuesNum = 1;
			char* temp = const_cast<char*>(check.c_str());
			void* value2[1]={ reinterpret_cast<void*>(temp)};
			changed_fileName.attrValues = value2;
			string fullRDN = r3 + "," + READYTOUSEDN;
			returnCode = immHandler.modifyAttribute(fullRDN.c_str(), &changed_fileName);
			if( returnCode == ACS_CC_FAILURE )
			{
				DEBUG("%s","Modify attribute fail in rulesetFileInit ");
				cout << "Modify attribute failed" << endl;
				cout<<"getInternalLastError :" << immHandler.getInternalLastError() << endl;
				cout<<"getInternalLastErrorText" << immHandler.getInternalLastErrorText() << endl;
			}
		}
		DEBUG("%s","IMM Create RuntimeObject failure in rulesetFileInit ");
		cout<<"IMM Create RuntimeObject failure"<<std::endl;
		cout<<"getInternalLastError :"<< immHandler.getInternalLastError()<<endl;
                cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastErrorText()<<endl;
	}
	DEBUG("%s"," Calling immHandler.Finalize()");
	immHandler.Finalize();
	DEBUG("%s","Leaving Inotify::ruleSetFileInit(string check, string completePath) ");
}


void Inotify::ruleSetFileRemove(string check)
{
	DEBUG("%s","Entering Inotify::ruleSetFileRemove(string check ");
	OmHandler immHandler;
        ACS_CC_ReturnType returnCode;
        returnCode = immHandler.Init();
        if(returnCode != ACS_CC_SUCCESS)
        {
        	cout<<"ERROR: init FAILURE!!!\n";
        }

	cout << "File deleted: " << check.c_str() << endl;	

	std::vector<std::string> rdnList, ruleSetList;
	char *rootName = const_cast<char*>(READYTOUSEDN);
	returnCode = immHandler.getChildren(rootName, ACS_APGCC_SUBTREE, &rdnList);

	cout << "rdnList: " << endl;	
	for(unsigned int i = 0; i < rdnList.size(); i++)
	{
		if(rdnList[i].find(rulesetFileIdAttr) != std::string::npos)
		{
			ruleSetList.push_back(rdnList[i]);
		}
	}
	
	cout << "Size: " << ruleSetList.size() << endl;

	std::vector<std::string> p_dnList;

	/*char *classNamep =const_cast<char*>(RULESETFILE);
        returnCode = immHandler.getClassInstances(classNamep, p_dnList);
	if(returnCode != ACS_CC_SUCCESS)
	{
              	cout << "Method OmHandler::getCLassInstances Failure!!!" << endl;
	        cout << "ERROR CODE: "<< immHandler.getInternalLastError() << endl;
                cout << "ERROR MESSAGE: " << immHandler.getInternalLastErrorText() << endl;
	}*/

	ACS_APGCC_ImmAttribute fileName;
	char attrname[]=fileNameAttr;
        fileName.attrName = attrname;
        std::vector<ACS_APGCC_ImmAttribute*> attributes;
        attributes.push_back(&fileName);
	char file[64] = "";

	for(unsigned int i = 0; i < ruleSetList.size(); i++)
	{
		returnCode = immHandler.getAttribute(ruleSetList[i].c_str(), attributes);
		if(returnCode == ACS_CC_FAILURE)
		{
			DEBUG("%s"," Get Attribute failure in ruleSetFileRemove ");
			cout << "GetAttribute failed" << endl;
		}
		else
		{
			strcpy(file, reinterpret_cast<const char*>(*(fileName.attrValues)));
			cout << "file: " << file << endl;
			if(!strcmp(file, check.c_str()))
			{
				returnCode = immHandler.deleteObject(ruleSetList[i].c_str());
				if( returnCode == ACS_CC_FAILURE )
				{
					DEBUG("%s","Deletion of IMM runtime object for readyToUse is failed in ruleSetFileRemove ");
					cout << "Deletion of IMM runtime object for readyToUse is failed" << endl;
					cout<<"getInternalLastError :" << immHandler.getInternalLastError() << endl;
					cout<<"getInternalLastErrorText :" << immHandler.getInternalLastErrorText() << endl;
				}
				else
				{
					cout << "Node deleted" << endl;
				}
			}
			else
			{
				cout << "No match found" << endl;
			}
		}
	}

	cout << "Finalizing" << endl;
	DEBUG("%s","Calling immHandler.Finalize()");
	immHandler.Finalize();
	cout << "Returning.." << endl;
	DEBUG("%s","Leaving Inotify::ruleSetFileRemove(string check ");
}

void Inotify::ruleSetFileModify(string check, string completePath)
{
	DEBUG("%s","Entering Inotify::ruleSetFileModify(string check, string completePath) ");
        OmHandler immHandler;
        ACS_CC_ReturnType returnCode;
        returnCode = immHandler.Init();
	
	std::string ruleSetId = "";
        try
        {
                AcsHcs::DocumentXml doc(completePath);
                ruleSetId = doc.getRulesetIdValue();
		if ( ruleSetId == "")
		{
			 cout << "ERROR while uploading : XML read error" << endl;
			 DEBUG("%s","Entering Inotify::ruleSetFileModify");	
			 return;
		}	
        }
        catch(...)
        {
                cout << "ERROR while uploading : XML read error" << endl;
		DEBUG("%s","Entering Inotify::ruleSetFileModify catch ... unknown exception");
                return;
        }
        
	if(returnCode != ACS_CC_SUCCESS)
        {
                cout<<"ERROR: init FAILURE!!!\n";
        }

        cout << "File modified: " << check.c_str() << endl;

        /*std::vector<std::string> p_dnList;

        char *classNamep =const_cast<char*>(RULESETFILE);
        returnCode = immHandler.getClassInstances(classNamep, p_dnList);
        if(returnCode != ACS_CC_SUCCESS)
        {
                cout << "Method OmHandler::getCLassInstances Failure!!!" << endl;
                cout << "ERROR CODE: "<< immHandler.getInternalLastError() << endl;
                cout << "ERROR MESSAGE: " << immHandler.getInternalLastErrorText() << endl;
        }
	
	cout << "immHandler.getClassInstances success" << endl;*/

        std::vector<std::string> rdnList, p_dnList;
        char *rootName = const_cast<char*>(READYTOUSEDN);
        returnCode = immHandler.getChildren(rootName, ACS_APGCC_SUBTREE, &p_dnList);

        cout << "rdnList: " << endl;
        for(unsigned int i = 0; i < rdnList.size(); i++)
        {
                if(rdnList[i].find(rulesetFileIdAttr) != std::string::npos)
                {
                        p_dnList.push_back(rdnList[i]);
                }
        }

        cout << "Size: " << p_dnList.size() << endl;

	std::vector<ACS_APGCC_ImmAttribute *> attributes;
		
        ACS_APGCC_ImmAttribute fileName;
        fileName.attrName = ATTRFILE;
	attributes.push_back(&fileName);
        //char file[64] = EMPTY_STR;
	//char rdn[64];
	char fileValue[64] = EMPTY_STR;
	char rdnValue[64] = EMPTY_STR;

	bool fileFound=false;
        for(unsigned int i = 0; i < p_dnList.size(); i++)
        {
		std::size_t found_equal = p_dnList[i].find_first_of(EQUALTO_STR);
		std::size_t found_comma;
		std::string RDN;
		if(found_equal != std::string::npos)
		{
			found_comma = p_dnList[i].find_first_of(COMMA_STR, found_equal+1);	
			if(found_comma != std::string::npos)
			{
				RDN = p_dnList[i].substr(found_equal+1, found_comma - (found_equal+1));
			}
		}
	
                returnCode = immHandler.getAttribute(p_dnList[i].c_str(), attributes);
                if(returnCode == ACS_CC_FAILURE)
                {
                        cout << "GetAttribute failed" << endl;
                }
                else
                {
			cout << "GetAttribute success" << endl; 	
			strcpy(fileValue, EMPTY_STR);
			strcpy(rdnValue, EMPTY_STR);
			string temp_fileValue = (const char*)attributes[0]->attrValues[0];	
			strcpy(fileValue,temp_fileValue.c_str());	
                        if(!strcmp(fileValue, check.c_str()))
			{
				fileFound=true;
				break;
			}
		}
	}

	if(!fileFound)
	{
		/* creating an object if empty xml file is loaded and then modified */
		ruleSetFileInit(check, completePath);	
		DEBUG("%s","Return after !filefound in rulesetfilemodify ");
		return;
	}

        for(unsigned int i = 0; i < p_dnList.size(); i++)
        {
		std::size_t found_equal = p_dnList[i].find_first_of(EQUALTO_STR);
		std::size_t found_comma;
		std::string RDN;
		if(found_equal != std::string::npos)
		{
			found_comma = p_dnList[i].find_first_of(COMMA_STR, found_equal+1);	
			if(found_comma != std::string::npos)
			{
				RDN = p_dnList[i].substr(found_equal+1, found_comma - (found_equal+1));
			}
		}
	
                returnCode = immHandler.getAttribute(p_dnList[i].c_str(), attributes);
                if(returnCode == ACS_CC_FAILURE)
                {
                        cout << "GetAttribute failed" << endl;
                }
                else
                {
			cout << "GetAttribute success" << endl; 	
			strcpy(fileValue, EMPTY_STR);
			strcpy(rdnValue, EMPTY_STR);
			string temp_fileValue = (const char*)attributes[0]->attrValues[0];
                        strcpy(fileValue,temp_fileValue.c_str());	
                        if(!strcmp(fileValue, check.c_str()))
                        {
				/* Same rule set ID  */
				//if(!strcmp(RDN, ruleSetId.c_str()))
				if(RDN == ruleSetId)
				{
					cout << "Same file and same RDN" << endl;		
					break;
					/* Do nothing */
				}
				else
				{
					cout << "same file and diff rdn" << endl;	
					returnCode = immHandler.deleteObject(p_dnList[i].c_str());
					if( returnCode == ACS_CC_FAILURE )
					{
						DEBUG("%s","Deletion of IMM runtime object for readyToUse is failed in same file same rdn ");
						cout << "Deletion of IMM runtime object for readyToUse is failed" << endl;
						cout<<"getInternalLastError :" << immHandler.getInternalLastError() << endl;
						cout<<"getInternalLastErrorText :" << immHandler.getInternalLastErrorText() << endl;
					}
					ruleSetFileInit(check, completePath);										
					break;
                                }
                        }
                }
        }
	DEBUG("%s"," Leaving Inotify::ruleSetFileModify(string check, string completePath) ");	
}



void Inotify::removeExistingRID()
{
	DEBUG("%s","IN Inotify::removeExistingRID ");
	struct dirent **namelist;
        std::vector<std::string> Filenames;
        int i,size;
	OmHandler immHandler;
        int returnCode;
        returnCode=immHandler.Init();
        std::vector<std::string> rdnList, p_dnList, rid;

	i = scandir(NBI_RULES, &namelist, 0, alphasort);

        if(i>=0)
        {
		DEBUG("%s","in i >=0");  
              while(i--)
                {

                        if(string(namelist[i]->d_name)!="." && string(namelist[i]->d_name)!="..")
                        {
                                string temp = string(namelist[i]->d_name);
				// start of TR HU53889
				std::size_t found = temp.find_last_of(".\\");
				if (found != std::string::npos)
				{
				//string temp1 = temp.substr(temp.size()-4, 4);
				string temp1 = temp.substr(found+1,3);
				if(temp1 == "xml")
                               	Filenames.push_back(temp);
				}
				//end of TR HU53889
			}
			free(namelist[i]);	
		}
		free(namelist);
	}

	returnCode = immHandler.getChildren(READYTOUSEDN, ACS_APGCC_SUBTREE, &p_dnList);
        if( returnCode == ACS_CC_FAILURE )
        {
	        DEBUG("%s","Modify attribute fail in rulesetFileInit ");
        }

	
	if(Filenames.size() == 0)
	{
		DEBUG("%s","In Filenames.size() == 0");	
		for(unsigned int i = 0; i < p_dnList.size(); i++)
		{	
			if(!(p_dnList[i] == "id=1,readyToUseId=1,ruleFileManagerId=1,HealthCheckhealthCheckMId=1"))
			{
				size_t comma_pos1 = p_dnList[i].find_first_of(',');
	                        size_t equ_pos1 = p_dnList[i].find_first_of('=');
        	                string rid1 = p_dnList[i].substr(equ_pos1+1,((comma_pos1 - equ_pos1)-1));
				ACS_CC_ReturnType errorCode;				
				int count = 0;
			 	//returnCode = immHandler.deleteObject(p_dnList[i].c_str());
				do
				{
					errorCode = immHandler.deleteObject(p_dnList[i].c_str());
					count++;
					if(errorCode != ACS_CC_SUCCESS )
					{
						DEBUG("%d",immHandler.getInternalLastError());
						DEBUG("%s",immHandler.getInternalLastErrorText());
						if(!Inotify::stopRequested)
							sleep(0.5);
						else
							break;
					}
					else
						break;
				}while(count<=25);
			}

		}

	}
	
	bool match = false;
	for(unsigned int i = 0; i < p_dnList.size(); i++)
	{
		if(!(p_dnList[i] == "id=1,readyToUseId=1,ruleFileManagerId=1,HealthCheckhealthCheckMId=1"))
                {
			size_t comma_pos = p_dnList[i].find_first_of(',');
                        size_t equ_pos = p_dnList[i].find_first_of('=');
                        string rid = p_dnList[i].substr(equ_pos+1,((comma_pos - equ_pos)-1));
			acs_hcs_global_ObjectImpl obj;
			string fname = obj.getImmAttribute(p_dnList[i],"fileName");
		
			for(unsigned int j = 0; j < Filenames.size(); j++)
			{
				string complepath = NBI_RULES + Filenames[j];
				string ruleSetId = "";
				try
				{	
					AcsHcs::DocumentXml doc(complepath);
					ruleSetId = doc.getRulesetIdValue();
				}
        			catch(...)
        			{
                			cout << "ERROR in XML file" << endl;
        			}
				

				match = false;
				if( (strcmp(rid.c_str(), ruleSetId.c_str())== 0) && (strcmp(Filenames[j].c_str(),fname.c_str())==0)) 
				{
					match = true;		
					break;	
				}
			}
			if(match == false)
			{
				returnCode = immHandler.deleteObject(p_dnList[i].c_str());				
			}
		
		}
	}	
	DEBUG("%s","Leaving Inotify::removeExistingRID ");
}

// acs_modd_api_test.cpp : Defines the entry point for the console application.
//
/*RoGa
#include <winsock2.h>
#include <windows.h>
RoGa */
#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <string.h>
#include <cstdlib>
#include <limits.h>
//RoGa #include <ClusApi.h> Windows cluster management service API

typedef unsigned long DWORD;

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::vector;
using std::istringstream;
using std::fstream;
using std::ios;
using std::ios_base;
using std::ostringstream;
using std::stringstream;
using std::setw;
using std::map;

vector<string *> tableLines;
int failed = 0, success = 0;		// Variables for the no of successful and failed tests
map<SysID, string> cpIdMap;
map<APID, string> apIdMap;
map<SysID, string>::iterator it;
int cpCount = 0, bcCount = 0, apCount = 0;
DWORD oldTicks = 0;
int connections = 0;

void CPTablePrint();
void createCPMapWithNamesFromHWC();
void resetTypeAndAlias();
void testCPTableList();
void testCPTableType();
void testCPTableName();
void testCPTableId();
void testCPTableIdWithAliasSet();
void testCPTableWithTypeAndAliasSet();
void removeOperatorDefinedCPGroups();
void testCPGroupMembers();
void testCPGroupNamesAndMembers();
void testNetworkElement();
void testNetworkElementPrint();
void readTable();
void clearTable();
void testSysId();
void testGetValues();
void testGetBoardList();
void testCPCH();
void testCPGDEF();
void testNECH();
void testFDDEF();
void testFuncDistFunctions();
void testParallel();
void testNegative();
void testNotFinished();
void testAPISetFunctions();
bool compAddress(string tableValue, unsigned long apiValue);
bool compValue(string tableValue, unsigned short apiValue);
void testConnections();

//connections overload test
void testConnectionsOverload();
void testCPTableConnection();
DWORD /*RoGa WINAPI*/ ThreadProc(void * lpParam);
////
using namespace std;
int main(int argc, char* argv[]) 
{
	if (argc > 1) {
		if (strcmp(argv[1], "-o") == 0) testConnectionsOverload();
		else {
			std::cout << "Usage:" << std::endl << argv[0] << " [-o]" << std::endl << "-o : connection overload test" << std::endl;
			exit(0);
		}
	}

   readTable();		               //Read HWC table from file (used to compare the API values to the actual table values)
   createCPMapWithNamesFromHWC();   //Creates two MAPs, one with CPIDs and corresponding default CP names and one with APIDs 
                                    //and corresponding default AP names from data in HWC table
   resetTypeAndAlias();             //Reset type and alias for each defined CP
   removeOperatorDefinedCPGroups(); //Remove all operator defined groups

   // Functions printing their result
   testCPGroupNamesAndMembers();    //Test function getGroupNames and getGroupMembers in CP group table (prints group ALL and ALLBC)
   testNetworkElementPrint();       //Prints data from NE functions
   testCPTableWithTypeAndAliasSet();//Test function getAPZType and getCPName with type and alias set
   testNegative();                  //Perform negative tests on different functions, stops and restarts the service
   testCPCH();                      //Sets some values using command CPCH, then tries to retrieve these values using the API functions 
   testCPGDEF();                    //Sets some values using command CPGDEF, then tries to retrieve these values using the API functions 
   testNECH();                      //Set a value using command NECH, then tries to retrieve this value using the API function
   testFDDEF();                     //Sets some values using command FDDEF, then tries to retrieve these values using the API functions 

   for (int i = 1; i < 21; i++)
   {
      // HWC table tests
      testSysId();                     //Test setSysid in BoardSearch and getSysid in HWC
      connections = connections + 1;
      testGetValues();                 //Test get functions (getXXX)
      connections = connections + 1;
      testGetBoardList();              //Test search function (getBoardIds)
      connections = connections + 1;
      
      // CP table tests
      testCPTableList();               //Test function getCPList in CP table
      connections = connections + 1;
      testCPTableName();               //Test function getCPName in CP table 
      connections = connections + 1;
      testCPTableType();               //Test function getAPZType in CP table
      connections = connections + 1;
      testCPTableId();                 //Test function getCPId in CP table
      connections = connections + 1;
      testCPTableIdWithAliasSet();     //Test function getCPId in CP table, should work even with CP alias set
      connections = connections + 1;

      // CP group table tests
      testCPGroupMembers();            //Test function getGroupMembers for group ALL and ALLBC
      connections = connections + 1;

      // Functional distribution table tests
      testFuncDistFunctions();         //Test functional distribution functions     
      connections = connections + 1;
      
      // Network Element tests
      testNetworkElement();            //Test and print the result of the NE functions
      connections = connections + 7;

      // API set functions test
      testAPISetFunctions();
      connections = connections + 4;

      // Miscellaneous tests
      testParallel();                  //Create several simultaneous instances
      connections = connections + 4;
      testConnections();               //Loop creating 10 HWC and CP table instances
      connections = connections + 20;

      if (i % 10 == 0 || i == 1)  //print result every 10th iteration
      {
         if (i == 1)
         {
            cout<<setiosflags(ios::left)<<setw(12)<<"Iteration"<<setw(11)<<"Time"<<setw(11)<<"Timediff"<<"Number of connections"<<endl;
            //RoGa oldTicks = GetTickCount();
         }

         /*RoGa SYSTEMTIME time;
         GetLocalTime(&time);
         WORD second = time.wSecond;
         WORD minute = time.wMinute;
         WORD hour = time.wHour;
         
         DWORD ticks = GetTickCount();
         DWORD diffTicks = ticks - oldTicks;
         
         cout<<setiosflags(ios::left)<<setw(12)<<i<<resetiosflags(ios::left)<<std::setfill('0')<<
            setw(2)<<hour<<setw(1)<<":"<<setw(2)<<minute<<setw(1)<<":"<<setw(2)<<second<<
            setiosflags(ios::left)<<std::setfill(' ')<<setw(3)<<""<<setw(11)<<diffTicks/1000<<connections<<endl;
         
         oldTicks = ticks;
	RoGa */
      }

      //RoGa Sleep(1); 
	sleep(1);
   }	

   clearTable();		               //Deallocate table content

   // Print test results
   cout << endl << "Successful tests: " << success << endl;
	cout << "Failed tests: " << failed << endl;
	cout << "Finished" << endl;

   (void)cin.get();
}

void testConnections()
{
   for (int i = 0; i < 10; i++)
   {
      ACS_CS_API_HWC* hwcTable = ACS_CS_API::createHWCInstance();

      if (hwcTable)
      {
         ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NS::Result_Failure;

         unsigned short fbn;
		 ACS_CS_API_IdList list;
		 ACS_CS_API_BoardSearch * search = 0;
         result = hwcTable->getFBN(fbn, 1);
		 hwcTable->getBoardIds(list, search);

         if (result == ACS_CS_API_NS::Result_Success)
            success++;
         else
            failed++;

         ACS_CS_API::deleteHWCInstance(hwcTable);
      }

      ACS_CS_API_CP* cpTable = ACS_CS_API::createCPInstance();

      if (cpTable)
      {
         ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NS::Result_Failure;

         ACS_CS_API_IdList list;
         result = cpTable->getCPList(list);

         if (result == ACS_CS_API_NS::Result_Success)
            success++;
         else
            failed++;

         ACS_CS_API::deleteCPInstance(cpTable);
      }
   }
}

void testCPTableId()
{
   ACS_CS_API_CP* cpTable = ACS_CS_API::createCPInstance();

   if (cpTable)
   {
      ACS_CS_API_IdList cpIds;

      ACS_CS_API_NS::CS_API_Result result = cpTable->getCPList(cpIds);  //get list of all CPs in CP table

      if (result == ACS_CS_API_NS::Result_Success)                      
      {
         for (unsigned int i = 0; i < cpIds.size(); i++)                //loop through each CP identity
         {
            if (cpIds.size() != cpIdMap.size())
            {
               failed++;
               ACS_CS_API::deleteCPInstance(cpTable);
               return;
            }

            ACS_CS_API_Name cpName;
            ACS_CS_API_NS::CS_API_Result result = cpTable->getCPName(cpIds[i], cpName);   //get CP name for current CP identity

            if (result == ACS_CS_API_NS::Result_Success)
            {
               CPID newcpid = 0;
               result = cpTable->getCPId(cpName, newcpid);              //get CP identity for current CP name

               if (result == ACS_CS_API_NS::Result_Success)
               {
                  size_t nameLength = (size_t)cpName.length();
                  char* tempNameChar = new char[nameLength];
                  if (tempNameChar)
                  {
                     result = cpName.getName(tempNameChar, nameLength);

                     if (result == ACS_CS_API_NS::Result_Success)
                     {
                        string cpTableNameString = tempNameChar;

                        bool nameFound = false;

                        for (it = cpIdMap.begin(); it != cpIdMap.end(); it++)  //loop through MAP
	                     {
                           CPID tempCPId = it->first;  //store current CP identity from MAP
                           string tempNameString = it->second;       //store current CP name from MAP

                           if (tempNameString == cpTableNameString)  //compare CP name from MAP with CP name retrived from CP table
                           {
                              nameFound = true;

                              if (tempCPId == newcpid)   //compare CP id from MAP with CP id retrived from CP table
                                 success++;
                              else                       //the identities don't match, something is wrong
                                 failed++;
                           }
                        }

                        if (! nameFound)
                           failed++;
                     }
                     else
                        failed++;
                  
                     delete [] tempNameChar;
                  }
                  else
                     failed++;
               }
               else
                  failed++;
            }
            else
               failed++;
         }
      }
      else
         failed++;

      ACS_CS_API::deleteCPInstance(cpTable);
   }
   else
      failed++;
}

void testCPTableIdWithAliasSet()
{
   ACS_CS_API_CP* cpTable = ACS_CS_API::createCPInstance();
   const char _tmpname[4] = {'C','P','0',0x0};
   if (cpTable)
   {

	  ACS_CS_API_Name cpName(_tmpname);
      CPID newcpid = 0;

      ACS_CS_API_NS::CS_API_Result result = cpTable->getCPId(cpName, newcpid);   //get CP identity for "CP0"

      if (result == ACS_CS_API_NS::Result_Success)
         success++;
      else
         failed++;

      ACS_CS_API::deleteCPInstance(cpTable);
   }
   else
      failed++;
}

void testCPTableWithTypeAndAliasSet()
{
   int i = 0;
   int j = 0;

   for (it = cpIdMap.begin(); it != cpIdMap.end(); it++)
   {
      stringstream command;
      
      CPID tmpCPID = it->first;

      if (tmpCPID < 1000)
         command<<"cpch -a TSC-B0"<<i++<<" -s APZ21401 -t 21255 "<<tmpCPID;   //e.g. "cpch -a TSC-B01 -s APZ21401 -t 21255 0"
      else if (tmpCPID < 2000)
         command<<"cpch -a SPX-0"<<j++<<" -s APZ21255 -t 21255 "<<tmpCPID;    //e.g. "cpch -a SPX-01 -s APZ21255 -t 21255 1000"

      if (system(command.str().c_str()) != 0)
         failed++;
      else
         success++;
   }


   ACS_CS_API_CP * cpTable = ACS_CS_API::createCPInstance();

   if (cpTable)
   {
      ACS_CS_API_IdList cpIds;

      ACS_CS_API_NS::CS_API_Result result = cpTable->getCPList(cpIds);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         for (unsigned int i = 0; i < cpIds.size(); i++)
         {
            //Check if the APZ type set is retrievable
            ACS_CS_API_NS::CS_API_APZ_Type apzType;

            ACS_CS_API_NS::CS_API_Result result = cpTable->getAPZType(cpIds[i], apzType);
            
            if (result == ACS_CS_API_NS::Result_Success)
            {
               if (cpIds[i] < 1000)
               {
                  if (apzType == ACS_CS_API_NS::APZ21401)
                     success++;
                  else
                     failed++;
               }
               else if (cpIds[i] < 2000)
               {
                  if (apzType == ACS_CS_API_NS::APZ21255)
                     success++;
                  else
                     failed++;
               }
            }
            else
               failed++;

            //Check if the APZ system set is retrievable
            unsigned short apzSystem;

            result = cpTable->getAPZSystem(cpIds[i], apzSystem);
            
            if (result == ACS_CS_API_NS::Result_Success)
            {
               if (cpIds[i] < 1000)
               {
                  if (apzSystem == 21401)
                     success++;
                  else
                     failed++;
               }
               else if (cpIds[i] < 2000)
               {
                  if (apzSystem == 21255)
                     success++;
                  else
                     failed++;
               }
            }
            else
               failed++;

            //Check if the CP type set is retrievable
            unsigned short cpType;

            result = cpTable->getCPType(cpIds[i], cpType);
            
            if (result == ACS_CS_API_NS::Result_Success)
            {
               if (cpType == 21255)
                  success++;
               else
                  failed++;
            }
            else
               failed++;

            //Check if the alias set is retrievable
            ACS_CS_API_Name cpName;
            result = cpTable->getCPName(cpIds[i], cpName);

            if (result == ACS_CS_API_NS::Result_Success)
            {
               size_t nameLength =(size_t) cpName.length();
               char* tempName = new char[nameLength];
               if (tempName)
               {
                  result = cpName.getName(tempName, nameLength);

                  if (result == ACS_CS_API_NS::Result_Success)
                  {
                     stringstream cpAlias;

                     if (cpIds[i] < 1000)
                        cpAlias<<"TSC-B0"<<cpIds[i];
                     else if (cpIds[i] < 2000)
                        cpAlias<<"SPX-0"<<cpIds[i] - 1000;

                     string cpAliasString = cpAlias.str();

                     if (cpAliasString == tempName)
                        success++;
                     else
                        failed++;
                  }
                  else
                     failed++;

                  delete [] tempName;
               }
               else
                  failed++;
            }
            else
               failed++;
         }
      }
      else
         failed++;

      ACS_CS_API::deleteCPInstance(cpTable);
   }
   else
      failed++;

   resetTypeAndAlias();             //Reset type and alias for each defined CP
}

void createCPMapWithNamesFromHWC()
{
   ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance();
   if (hwc)
   {
      ACS_CS_API_BoardSearch * boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();
      if (boardSearch)
      {
         ACS_CS_API_NS::CS_API_Result result;
         ACS_CS_API_IdList boardList;

         result = hwc->getBoardIds(boardList,boardSearch);  //wildcard search
         if (result == ACS_CS_API_NS::Result_Success)
         {
            for (unsigned int i = 0; i < boardList.size(); i++)
            {
               SysID sysId;
               result = hwc->getSysId(sysId, boardList[i]);
               if (result == ACS_CS_API_NS::Result_Success)
               {
                  if (sysId < 2000)       //BC or CP
                  {
                     if (cpIdMap.find(sysId) == cpIdMap.end())  //check if sysId isn't added to the MAP
                     {
                        stringstream number;

                        if (sysId < 1000)
                        {
                           number<<"BC"<<sysId % 1000;
                           bcCount++;
                        }
                        else
                        {
                           number<<"CP"<<sysId % 1000;
                           cpCount++;
                        }

                        cpIdMap[sysId] = number.str();
                     }
                  }
                  else if (sysId < 3000)
                  {
                     if (apIdMap.find(sysId) == apIdMap.end())  //check if sysId isn't added to the MAP
                     {
                        apCount++;
                        stringstream number;
                        number<<"AP"<<sysId % 1000;
                        apIdMap[sysId] = number.str();
                     }
                  }
               }
               else if (result == ACS_CS_API_NS::Result_NoValue)
               {
                  //do nothing
               }
               else
                  failed++;
            }
         }
         else
            failed++;

         ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
      }
      else
         failed++;

      ACS_CS_API::deleteHWCInstance(hwc);
   }
   else
      failed++;
}

void testCPTableName()
{
   ACS_CS_API_CP * cpTable = ACS_CS_API::createCPInstance();

   if (cpTable)
   {
      ACS_CS_API_IdList cpIds;

      ACS_CS_API_NS::CS_API_Result result = cpTable->getCPList(cpIds);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         for (unsigned int i = 0; i < cpIds.size(); i++)
         {
            if (cpIds.size() != cpIdMap.size())
            {
               failed++;
               ACS_CS_API::deleteCPInstance(cpTable);
               return;
            }

            ACS_CS_API_Name cpName;
            ACS_CS_API_NS::CS_API_Result result = cpTable->getCPName(cpIds[i], cpName);

            if (result == ACS_CS_API_NS::Result_Success)
            {
               size_t nameLength =(size_t) cpName.length();
               char* tempName = new char[nameLength];
               if (tempName)
               {
                  result = cpName.getName(tempName, nameLength);

                  if (result == ACS_CS_API_NS::Result_Success)
                  {
                     if (cpIds.size() != cpIdMap.size())
                     {
                        failed++;
                        ACS_CS_API::deleteCPInstance(cpTable);
                        return;
                     }

                     CPID cpId = cpIds[i];

                     if (cpIdMap[cpId] == tempName)
                        success++;
                     else
                        failed++;
                  }
                  else
                     failed++;

                  delete [] tempName;
               }
               else
                  failed++;
            }
            else
               failed++;
         }
      }
      else
         failed++;

      ACS_CS_API::deleteCPInstance(cpTable);
   }
   else
      failed++;
}

void testCPTableType()
{
   ACS_CS_API_CP * cpTable = ACS_CS_API::createCPInstance();

   if (cpTable)
   {
      ACS_CS_API_IdList cpIds;

      ACS_CS_API_NS::CS_API_Result result = cpTable->getCPList(cpIds);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         for (unsigned int i = 0; i < cpIds.size(); i++)
         {
            ACS_CS_API_NS::CS_API_APZ_Type apzType;

            ACS_CS_API_NS::CS_API_Result result = cpTable->getAPZType(cpIds[i], apzType);

            if (result == ACS_CS_API_NS::Result_NoValue)
               success++;
            else
               failed++;
         }
      }
      else
         failed++;

      ACS_CS_API::deleteCPInstance(cpTable);
   }
   else
      failed++;
}

void resetTypeAndAlias()
{
   for (it = cpIdMap.begin(); it != cpIdMap.end(); it++)
   {
      stringstream command;

      CPID tmpCPID = it->first;
      command<<"cpch -r "<<tmpCPID;  //e.g. cpch -r 1001

      if (system(command.str().c_str()) != 0)
         failed++;
      else
         success++;
   }
}

void testCPTableList()
{  
   ACS_CS_API_CP * cpTable = ACS_CS_API::createCPInstance();

   if (cpTable)
   {
      ACS_CS_API_IdList cpIds;

      ACS_CS_API_NS::CS_API_Result result = cpTable->getCPList(cpIds);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         for (unsigned int i = 0; i < cpIds.size(); i++)
         {
            if (cpIds.size() != cpIdMap.size())
            {
               failed++;
               ACS_CS_API::deleteCPInstance(cpTable);
               return;
            }

            CPID cpId = cpIds[i];

	         if (cpIdMap.find(cpId) != cpIdMap.end())   //search for cpId in MAP
		         success++;
            else
               failed++;
         }
      }
      else
         failed++;

      ACS_CS_API::deleteCPInstance(cpTable);
   }
   else
      failed++;
}

void testSysId()
{
   ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();

   if (hwc)
   {
      ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();
	   
      if (boardSearch)
      {
         ACS_CS_API_IdList boardList;

		   (void) hwc->getBoardIds(boardList, boardSearch);
		   int hits = boardList.size();

		   for (int i = 0; i < hits; i++)
		   {
			   unsigned short boardId = boardList[i];

            bool sysIdFound = false;
			   unsigned short sysid = 0, systype = 0, sysno = 0, seqno = 0, tmpValue = 0;
            ACS_CS_API_NS::CS_API_Result result;
            ACS_CS_API_BoardSearch* testSearch = ACS_CS_API_HWC::createBoardSearchInstance();
			   ACS_CS_API_IdList testList;

            result = hwc->getSysType(tmpValue, boardId);

            if (result == ACS_CS_API_NS::Result_Success)
            {
               systype = tmpValue;

               if (systype == 0)    //BC
               {
                  result = hwc->getSeqNo(tmpValue, boardId);

                  if (result == ACS_CS_API_NS::Result_Success)
                  {
                     seqno = tmpValue;
                     sysid = systype + seqno;
                     sysIdFound = true;
                  }
               }
               else                 //CP or AP
               {
                  result = hwc->getSysNo(tmpValue, boardId);

                  if (result == ACS_CS_API_NS::Result_Success)
                  {
                     sysno = tmpValue;
                     sysid = systype + sysno;
                     sysIdFound = true;
                  }
               }
            }

            // Test getBoardIds
            if (sysIdFound)
            {
               testSearch->setSysId(sysid);

               (void) hwc->getBoardIds(testList, testSearch);

               bool boardFound = false;

               for (unsigned int j = 0; j < testList.size(); j++)
               {
                  unsigned short tmpBoardId = testList[j];
                  unsigned short tmpSysno = 0, tmpSystype = 0, tmpSeqno = 0, tmpSysid = 0;

                  result = hwc->getSysType(tmpValue, tmpBoardId);

                  if (result == ACS_CS_API_NS::Result_Success)
                     tmpSystype = tmpValue;

                  result = hwc->getSysNo(tmpValue, tmpBoardId);

                  if (result == ACS_CS_API_NS::Result_Success)
                     tmpSysno = tmpValue;

                  result = hwc->getSeqNo(tmpValue, tmpBoardId);

                  if (result == ACS_CS_API_NS::Result_Success)
                     tmpSeqno = tmpValue;

                  if (tmpSystype == 0)
                     tmpSysid = tmpSystype + tmpSeqno;
                  else
                     tmpSysid = tmpSystype + sysno;

                  if (tmpSysid == sysid)
                     success++;
                  else
                     failed++;
               }
            }

            ACS_CS_API_HWC::deleteBoardSearchInstance(testSearch);

            // Test getSysid
            if (sysIdFound)
            {
               result = hwc->getSysId(tmpValue, boardId);

               if (result == ACS_CS_API_NS::Result_Success)
               {
                  if (tmpValue == sysid)  // Compare values
                     success++;
                  else
                     failed++;
               }
               else
                  failed++;

            }
            else  // Shouldn't have sysid
            {
                result = hwc->getSysId(tmpValue, boardId);

                if (result == ACS_CS_API_NS::Result_NoValue)
                   success++;
                else
                  failed++;
            }
         }
         ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
      }
      ACS_CS_API::deleteHWCInstance(hwc);
   }
}

void testNegative()
{
	uint32_t longValue = 0;
	unsigned short shortValue = 0;
	BoardID boardId = 15;
   CPID cpId = 1000;
   ACS_CS_API_NS::CS_API_APZ_Type type = ACS_CS_API_NS::APZ21255;
   const char _tmpname[4] = {'C','P','0',0x0};
   ACS_CS_API_Name name(_tmpname);
   ACS_CS_API_IdList list;

   if (system("net stop ACS_CS_Service") != 0)
      failed++;

   ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();
   ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();

	// Test functions when disconnected
   ACS_CS_API_NS::CS_API_Result result = hwc->getMagazine(longValue, boardId);

   if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getSlot(shortValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getSysType(shortValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

   result = hwc->getSysId(shortValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getSysNo(shortValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getFBN(shortValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getSide(shortValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getSeqNo(shortValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getIPEthA(longValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getIPEthB(longValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getAliasEthA(longValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getAliasEthB(longValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getAliasNetmaskEthA(longValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getAliasNetmaskEthB(longValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getDhcpMethod(shortValue, boardId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	result = hwc->getBoardIds(list, boardSearch);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

	// Test passing null values to the API (disconnected)

	result = hwc->getBoardIds(list, 0);
	success++;

   ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();

	// Test CP table functions when disconnected
   result = CP->getAPZType(shortValue, type);

   if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

   result = CP->getCPId(name, cpId);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

   result = CP->getCPList(list);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

   result = CP->getCPName(cpId, name);

	if (result != ACS_CS_API_NS::Result_NoAccess)
		failed++;
	else
		success++;

   if (system("net start ACS_CS_Service") != 0)
      failed++;

   ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
   ACS_CS_API::deleteHWCInstance(hwc);
   ACS_CS_API::deleteCPInstance(CP);

   hwc = ACS_CS_API::createHWCInstance();
   boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();
   CP = ACS_CS_API::createCPInstance();
	
	// Test passing null values to the API (connected)

	if (hwc)
	{
      result = hwc->getBoardIds(list, 0);
		success++;	
	}
	else
		failed++;

   if (CP)
   {
      type = (ACS_CS_API_NS::CS_API_APZ_Type)0;
      cpId = 0;

      result = CP->getAPZType(0, type);
      result = CP->getCPId(0, cpId);
      result = CP->getCPList(list);
      result = CP->getCPName(0, name);
      success++;
   }
   else
      failed++;

	// Test repetitive calls

	ACS_CS_API::deleteHWCInstance(hwc);
   ACS_CS_API::deleteCPInstance(CP);

	hwc = ACS_CS_API::createHWCInstance();
   hwc = ACS_CS_API::createHWCInstance();
   CP = ACS_CS_API::createCPInstance();
   CP = ACS_CS_API::createCPInstance();
	
	if (hwc && CP)
		success++;
	else
		failed++;

	if (hwc)
	{
		result = hwc->getBoardIds(list, boardSearch);
      if (result != ACS_CS_API_NS::Result_Success)
		   failed++;
	   else
		   success++;
	}

   if (CP)
	{
      result = CP->getCPList(list);
      if (result != ACS_CS_API_NS::Result_Success)
		   failed++;
	   else
		   success++;
	}

   list.setSize(0);
   list.setValue(0, 0);

   name.setName(0);
   name.setName(0x0);

	// Test range on boardlist (empty)
	shortValue = list[-10000000];
	success++;
	shortValue = list[-1];
	success++;
	shortValue = list[0];
	success++;
	shortValue = list[1];
	success++;
	shortValue = list[10000000];
	success++;

	if (hwc)
      (void)hwc->getBoardIds(list, boardSearch);

	// Test range on boardlist (not empty)
	shortValue = list[-10000000];
	success++;
	shortValue = list[-1];
	success++;
	shortValue = list[0];
	success++;
	shortValue = list[1];
	success++;
	shortValue = list[10000000];
	success++;

	if (hwc)
      ACS_CS_API::deleteHWCInstance(hwc);
   if (boardSearch)
      ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
   if (CP)
      ACS_CS_API::deleteCPInstance(CP);
}

void testParallel()
{
	ACS_CS_API_HWC* hwc1 = ACS_CS_API::createHWCInstance();
   ACS_CS_API_BoardSearch* boardSearch1 = ACS_CS_API_HWC::createBoardSearchInstance();
   ACS_CS_API_IdList boardList1;

   ACS_CS_API_CP * cpTable1 = ACS_CS_API::createCPInstance();
   ACS_CS_API_IdList cpIds1;

   ACS_CS_API_HWC* hwc2 = ACS_CS_API::createHWCInstance();
   ACS_CS_API_BoardSearch* boardSearch2 = ACS_CS_API_HWC::createBoardSearchInstance();
   ACS_CS_API_IdList boardList2;

   ACS_CS_API_CP * cpTable2 = ACS_CS_API::createCPInstance();
   ACS_CS_API_IdList cpIds2;

	(void)hwc1->getBoardIds(boardList1, boardSearch1);
   (void)hwc2->getBoardIds(boardList2, boardSearch2);
   (void)cpTable1->getCPList(cpIds1);
   (void)cpTable2->getCPList(cpIds2);

   ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch1);
   ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch2);
   ACS_CS_API::deleteHWCInstance(hwc1);
   ACS_CS_API::deleteHWCInstance(hwc2);
   ACS_CS_API::deleteCPInstance(cpTable1);
   ACS_CS_API::deleteCPInstance(cpTable2);
}

void testGetBoardList()
{
   ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();

   if (hwc)
   {
      ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();
	   
      if (boardSearch)
      {
         ACS_CS_API_IdList boardList;

		   (void) hwc->getBoardIds(boardList, boardSearch);
		   int hits = boardList.size();

		   for (int i = 0; i < hits; i++)
		   {
			   unsigned short boardId = boardList[i];

			   uint32_t mag, ipa, ipb, ala, alb, ma, mb;
			   unsigned short slot, systype, sysno, fbn, side, seqno, dhcp;
            ACS_CS_API_NS::CS_API_Result result;
            ACS_CS_API_BoardSearch* testSearch = ACS_CS_API_HWC::createBoardSearchInstance();
			   ACS_CS_API_IdList testList;

			   result = hwc->getMagazine(mag, boardId);

            if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setMagazine(mag);

			   result = hwc->getSlot(slot, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setSlot(slot);

			   result = hwc->getSysType(systype, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setSysType(systype);

			   result = hwc->getSysNo(sysno, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setSysNo(sysno);

			   result = hwc->getFBN(fbn, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setFBN(fbn);

			   result = hwc->getSide(side, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setSide(side);

			   result = hwc->getSeqNo(seqno, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setSeqNo(seqno);

			   result = hwc->getIPEthA(ipa, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setIPEthA(ipa);

			   result = hwc->getIPEthB(ipb, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setIPEthB(ipb);

			   result = hwc->getAliasEthA(ala, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setAliasEthA(ala);

			   result = hwc->getAliasEthB(alb, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setAliasEthB(alb);

			   result = hwc->getAliasNetmaskEthA(ma, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setAliasNetmaskEthA(ma);

			   result = hwc->getAliasNetmaskEthB(mb, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setAliasNetmaskEthB(mb);

			   result = hwc->getDhcpMethod(dhcp, boardId);

			   if (result == ACS_CS_API_NS::Result_Success)
				   testSearch->setDhcpMethod(dhcp);

			   (void) hwc->getBoardIds(testList, testSearch);

			   if (testList[0] == boardId)
				   success++;
			   else
				   failed++;

			   testSearch->reset();

            ACS_CS_API_HWC::deleteBoardSearchInstance(testSearch);
         }
         ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
      }
      ACS_CS_API::deleteHWCInstance(hwc);
   }
}

void testGetValues()
{
   ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();

	if (hwc)
   {
      ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();

      if (boardSearch)
	   {
		   ACS_CS_API_IdList boardList;

         ACS_CS_API_NS::CS_API_Result result = hwc->getBoardIds(boardList, boardSearch);

		   if (result == ACS_CS_API_NS::Result_Success)
		   {
			   unsigned int hits = boardList.size();

			   for (unsigned int i = 0; i < hits; i++)
			   {
				   string line;

				   if (tableLines.size() > 0)
               {
					   if (tableLines.size() > i)
                     line = *tableLines[i];
                  else
                     break;
               }
   				
				   string cell;
				   istringstream tokenizer(line);
				   unsigned short boardId = boardList[i];
				   uint32_t longValue = 0;
				   unsigned short shortValue = 0;
               ACS_CS_API_NS::CS_API_Result getResult;

				   tokenizer >> cell;
				   getResult = hwc->getMagazine(longValue, boardId);

               if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compAddress(cell, longValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }

				   tokenizer >> cell;
				   getResult = hwc->getSlot(shortValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compValue(cell, shortValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }


				   tokenizer >> cell;
				   getResult = hwc->getSysType(shortValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compValue(cell, shortValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }
   				
				   tokenizer >> cell;
				   getResult = hwc->getSysNo(shortValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compValue(cell, shortValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }

				   tokenizer >> cell;
				   getResult = hwc->getFBN(shortValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compValue(cell, shortValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }

				   tokenizer >> cell;
				   getResult = hwc->getSide(shortValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compValue(cell, shortValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }

				   tokenizer >> cell;
				   getResult = hwc->getSeqNo(shortValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compValue(cell, shortValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }

				   tokenizer >> cell;
				   getResult = hwc->getIPEthA(longValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compAddress(cell, longValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }

				   tokenizer >> cell;
				   getResult = hwc->getIPEthB(longValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compAddress(cell, longValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }

				   tokenizer >> cell;
				   getResult = hwc->getAliasEthA(longValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compAddress(cell, longValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }

				   tokenizer >> cell;
				   getResult = hwc->getAliasEthB(longValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compAddress(cell, longValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }

				   tokenizer >> cell;
				   getResult = hwc->getAliasNetmaskEthA(longValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compAddress(cell, longValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }

				   tokenizer >> cell;
				   getResult = hwc->getAliasNetmaskEthB(longValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compAddress(cell, longValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }

				   tokenizer >> cell;
				   getResult = hwc->getDhcpMethod(shortValue, boardId);

				   if (getResult == ACS_CS_API_NS::Result_Success)
				   {
					   if ( compValue(cell, shortValue) )
						   success++;
					   else
						   failed++;
				   }
				   else if (getResult == ACS_CS_API_NS::Result_NoValue)
				   {
					   if (cell == "NA")
						   success++;
					   else
						   failed++;
				   }
				   else
				   {
					   failed++;
				   }
			   }
		   }
		   else
		   {
			   failed++;
		   }

         ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
	   }
      ACS_CS_API::deleteHWCInstance(hwc);
   }
}

void testNetworkElement()
{
   ACS_CS_API_SET_NS::CS_API_Set_Result resultSet = ACS_CS_API_SET_NS::Result_Failure;
   ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NS::Result_Failure;

   CPID alarmMasterSet = 1000;
   CPID alarmMasterGet = 0;

   resultSet = ACS_CS_API_Set::setAlarmMaster(alarmMasterSet);

   if (resultSet == ACS_CS_API_SET_NS::Result_Success)
      success++;
   else if (resultSet == ACS_CS_API_SET_NS::Result_Incorrect_CP_Id)
   {
      cout<<"An invalid CP identifier ("<<alarmMasterSet<<") was given in setAlarmMaster"<<endl;
      failed++;
   }
   else
      failed++;

   result = ACS_CS_API_NetworkElement::getAlarmMaster(alarmMasterGet);

   if (result == ACS_CS_API_NS::Result_Success)
   {
      if (alarmMasterSet == alarmMasterGet)
         success++;
      else
         failed++;
   }
   else
      failed++;

   
   CPID clockMasterSet = 1000;
   CPID clockMasterGet = 0;

   resultSet = ACS_CS_API_Set::setClockMaster(clockMasterSet);

   if (resultSet == ACS_CS_API_SET_NS::Result_Success)
      success++;
   else if (resultSet == ACS_CS_API_SET_NS::Result_Incorrect_CP_Id)
   {
      cout<<"An invalid CP identifier ("<<clockMasterSet<<") was given in setClockMaster"<<endl;
      failed++;
   }
   else
      failed++;

   result = ACS_CS_API_NetworkElement::getClockMaster(clockMasterGet);

   if (result == ACS_CS_API_NS::Result_Success)
      if (clockMasterSet == clockMasterGet)
         success++;
      else
         failed++;
   else
      failed++;

   
   unsigned int singleCPCount;
   result = ACS_CS_API_NetworkElement::getSingleSidedCPCount(singleCPCount);

   if (result == ACS_CS_API_NS::Result_Success)
   {
      if (singleCPCount == bcCount)
         success++;
      else
         failed++;
   }
   else
      failed++;

   
   unsigned int doubleCPCount;
   result = ACS_CS_API_NetworkElement::getDoubleSidedCPCount(doubleCPCount);

   if (result == ACS_CS_API_NS::Result_Success)
   {
      if (doubleCPCount == cpCount)
         success++;
      else
         failed++;
   }
   else
      failed++;

   
   unsigned int apgCount;
   result = ACS_CS_API_NetworkElement::getAPGCount(apgCount);

   if (result == ACS_CS_API_NS::Result_Success)
   {
      if (apgCount == apCount)
         success++;
      else
         failed++;
   }
   else
      failed++;
}

void testNetworkElementPrint()
{  
   ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NS::Result_Failure;

   APID frontAPG;
   result = ACS_CS_API_NetworkElement::getFrontAPG(frontAPG);

   if (result == ACS_CS_API_NS::Result_Success)
      cout << "Front APG: " << frontAPG << endl;
   else
      cout << "Failed to get front APG" << endl;

   
   bool multipleCPSystem;
   result = ACS_CS_API_NetworkElement::isMultipleCPSystem(multipleCPSystem);

   if (result == ACS_CS_API_NS::Result_Success)
   {
      if (multipleCPSystem)
         cout << "Multiple CP system" << endl;
      else
         cout << "Single CP system" << endl;
   }
   else
      cout << "Failed to get multiple CP system" << endl;

   
   bool testEnvironment;
   result = ACS_CS_API_NetworkElement::isTestEnvironment(testEnvironment);

   if (result == ACS_CS_API_NS::Result_Success)
   {
      if (testEnvironment)
         cout << "Test environment" << endl;
      else
         cout << "Not a test environment" << endl;
   }
   else
      cout << "Failed to get test environment" << endl;

   
   ACS_CS_API_Name neid;
   result = ACS_CS_API_NetworkElement::getNEID(neid);

   if (result == ACS_CS_API_NS::Result_Success)
   {
      size_t length = (size_t)neid.length();
      char * buffer = new char[length];
      neid.getName(buffer, length);

      cout << "NEID: " << buffer << endl;

      delete [] buffer;
   }
   else
      cout << "Failed to get NEID" << endl;

   
   uint32_t BSOMIPethA, BSOMIPethB;
   result = ACS_CS_API_NetworkElement::getBSOMIPAddress(BSOMIPethA, BSOMIPethB);

   if (result == ACS_CS_API_NS::Result_Success)
   {
      stringstream IPethA, IPethB;
      IPethA<<(BSOMIPethA >> 24 & 0xFF)<<"."<<(BSOMIPethA >> 16 & 0xFF)<< "."<<(BSOMIPethA >> 8 & 0xFF)<<"."<<(BSOMIPethA & 0xFF);
      IPethB<<(BSOMIPethB >> 24 & 0xFF)<<"."<<(BSOMIPethB >> 16 & 0xFF)<< "."<<(BSOMIPethB >> 8 & 0xFF)<<"."<<(BSOMIPethB & 0xFF);
      cout<<"BSOM IP on eth A "<<IPethA.str()<<endl;
      cout<<"BSOM IP on eth B "<<IPethB.str()<<endl;
   }
   else
      cout<<"Failed to get BSOM IP addresses"<<endl;

   cout<<endl;
}

void readTable(void)
{
	//lint --e{429}

	fstream file;
	string line;
   file.open("hwctable.txt", ios_base::in);

	if (! file.is_open())
		return;

	(void) getline(file, line);	// Skip first line

	while(getline(file, line))
	{
		//lint --e{429}

		string * newLine = new string(line);
		tableLines.push_back(newLine);
	};

	file.close();
}

void clearTable()
{
	vector<string *>::iterator it;

	for (it = tableLines.begin(); it != tableLines.end(); it++)
	{
      string * ptr = *it;

		if (ptr)
			delete ptr;
	}
   tableLines.clear();
}

bool compAddress(string tableValue, unsigned long apiValue)
{
	ostringstream stream;
	string apiAddress;

	stream << ( (apiValue >> 24 & 0xFF) ) << "."
		   << ( (apiValue >> 16 & 0xFF) ) << "."
		   << ( (apiValue >> 8 & 0xFF) ) << "."
		   << ( apiValue & 0xFF);

	apiAddress = stream.str();

	if (tableValue == apiAddress)
		return true;
	else
		return false;
}

bool compValue(string tableValue, unsigned short apiValue)
{
	istringstream stream(tableValue);
	unsigned short value = USHRT_MAX;
	
	stream >> value;

	if (value == apiValue)
		return true;
	else
		return false;
}

void testAPISetFunctions()
{
   ACS_CS_API_SET_NS::CS_API_Set_Result result = ACS_CS_API_SET_NS::Result_Failure;

   result = ACS_CS_API_Set::setAlarmMaster(1000);

   if (result == ACS_CS_API_SET_NS::Result_Success)
      success++;
   else
      failed++;

   result = ACS_CS_API_Set::setClockMaster(1000);

   if (result == ACS_CS_API_SET_NS::Result_Success)
      success++;
   else
      failed++;

   //try to set not valid CP identity
   result = ACS_CS_API_Set::setAlarmMaster(1778);

   if (result != ACS_CS_API_SET_NS::Result_Incorrect_CP_Id)
      failed++;
   else
      success++;

   result = ACS_CS_API_Set::setClockMaster(1778);

   if (result != ACS_CS_API_SET_NS::Result_Incorrect_CP_Id)
      failed++;
   else
      success++;
}

void removeOperatorDefinedCPGroups()
{
   ACS_CS_API_CPGroup* cpGroupTable = ACS_CS_API::createCPGroupInstance();

   if (cpGroupTable)
   {
      ACS_CS_API_NameList nameList;

      ACS_CS_API_NS::CS_API_Result result = cpGroupTable->getGroupNames(nameList);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         for (unsigned int i = 0; i < nameList.size(); i++)
         {
            ACS_CS_API_Name cpGroupName = nameList[i];

            size_t cpGroupNameLength = (size_t)cpGroupName.length();
            char* cpGroupNameChar = new char[cpGroupNameLength];
            
            if (cpGroupNameChar)
            {
               result = cpGroupName.getName(cpGroupNameChar, cpGroupNameLength);

               if (result == ACS_CS_API_NS::Result_Success)
               {
                  stringstream command;
                  string tempCPGroupName = cpGroupNameChar;

                  if (tempCPGroupName != "ALL" && tempCPGroupName != "ALLBC")
                  {
                     command<<"cpgrm "<<cpGroupNameChar;  //e.g. cpgrm mygroup

                     if (system(command.str().c_str()) != 0)
                        failed++;
                     else
                        success++;
                  }
               }
               else
                  failed++;
            }
            else
               failed++;
         }  //end for loop
      }
      else
         failed++;

      ACS_CS_API::deleteCPGroupInstance(cpGroupTable);
   }
   else
      failed++;
}

void testCPGroupNamesAndMembers()
{
   ACS_CS_API_CPGroup* cpGroupTable = ACS_CS_API::createCPGroupInstance();

   if (cpGroupTable)
   {
      ACS_CS_API_NameList nameList;

      ACS_CS_API_NS::CS_API_Result result = cpGroupTable->getGroupNames(nameList);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         for (unsigned int i = 0; i < nameList.size(); i++)
         {
            ACS_CS_API_Name cpGroupName = nameList[i];
            
            ACS_CS_API_IdList cpList;

            result = cpGroupTable->getGroupMembers(cpGroupName, cpList);

            if (result == ACS_CS_API_NS::Result_Success)
            {
               bool first = true;
               stringstream cpStringStream;

               for (unsigned int j = 0; j < cpList.size(); j++)
               {
                  CPID cpId = cpList[j];
                  
                  if (first)
                  {
                     cpStringStream<<cpId;
                     first = false;
                  }
                  else
                     cpStringStream<<", "<<cpId;
               }  //end for loop

               size_t groupNameLength = (size_t)cpGroupName.length();
               char* groupNameChar = new char [groupNameLength];
               (void)cpGroupName.getName(groupNameChar, groupNameLength);

               cout<<"CP group named "<<groupNameChar<<" contains CPs "<<cpStringStream.str()<<endl;
            }
            else
               cout<<"Error"<<endl;
         }  //end for loop

         cout<<endl;
      }
      else
         cout<<"Error"<<endl;

      ACS_CS_API::deleteCPGroupInstance(cpGroupTable);
   }
   else
      cout<<"Error"<<endl;
}

void testCPGroupMembers()
{
   stringstream allCPStringStream, allBCStringStream;

   for (it = cpIdMap.begin(); it != cpIdMap.end(); it++)
   {
      CPID tmpCPID = it->first;
      allCPStringStream<<tmpCPID;

      if (tmpCPID >= 0 && tmpCPID < 1000)
         allBCStringStream<<tmpCPID;
   }

   string allCPString = allCPStringStream.str();
   string allBCString = allBCStringStream.str();
   
   string cpString = "";
   string bcString = "";


   ACS_CS_API_CPGroup* cpGroupTable = ACS_CS_API::createCPGroupInstance();
   
   if (cpGroupTable)
   {
	   const char _tmpname[4] = {'C','P','0',0x0};
	  ACS_CS_API_Name cpGroupName(_tmpname);
      ACS_CS_API_IdList cpList;

      ACS_CS_API_NS::CS_API_Result result = cpGroupTable->getGroupMembers(cpGroupName, cpList);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         stringstream cpStringStream;

         for (unsigned int i = 0; i < cpList.size(); i++)
         {
            CPID cpId = cpList[i];
               
            cpStringStream<<cpId;
         }

         cpString = cpStringStream.str();
      }
      else if (result == ACS_CS_API_NS::Result_NoEntry)
         cpString = "";
      else
         failed++;
      const char _tmpsname[6] = {'A','L','L','B','C',0x0};
      cpGroupName.setName(_tmpsname);

      result = cpGroupTable->getGroupMembers(cpGroupName, cpList);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         stringstream bcStringStream;

         for (unsigned int i = 0; i < cpList.size(); i++)
         {
            CPID cpId = cpList[i];
               
            bcStringStream<<cpId;
         }

         bcString = bcStringStream.str();
      }
      else if (result == ACS_CS_API_NS::Result_NoEntry)
         bcString = "";
      else
         failed++;

      ACS_CS_API::deleteCPGroupInstance(cpGroupTable);
   }
   else
      failed++;

   if (allCPString == cpString)
      success++;
   else
      failed++;

   if (allBCString == bcString)
      success++;
   else
      failed++;
}

void testFuncDistFunctions()
{
   ACS_CS_API_FunctionDistribution* fdTable = ACS_CS_API::createFunctionDistributionInstance();

   if (fdTable)
   {
      ACS_CS_API_NameList funcNameList;

      if (fdTable->getFunctionNames(funcNameList) == ACS_CS_API_NS::Result_Success)
      {
         if (funcNameList.size() == 0)
            success++;
         else
            failed++;
      }  
      else
         failed++;

      
      ACS_CS_API_IdList apList;
      const char _tmpfname[4] = {'S','T','S',0x0};
      ACS_CS_API_Name funcName(_tmpfname);

      if (fdTable->getFunctionProviders(funcName, apList) == ACS_CS_API_NS::Result_NoEntry)
         success++;
      else
         failed++;

      APID apId = 2001;
      ACS_CS_API_IdList cpList;

      if (fdTable->getFunctionUsers(apId, funcName, cpList) == ACS_CS_API_NS::Result_NoEntry)
         success++;
      else
         failed++;        

      ACS_CS_API::deleteFunctionDistributionInstance(fdTable);
   }
   else
      failed++;
}

void testNECH()
{
   string neidString = "MSC-S Helsinki";
   stringstream neidStringStream;

   neidStringStream<<"nech -i \""<<neidString<<"\"";

   if (system(neidStringStream.str().c_str()) != 0)
      failed++;
   else
      success++;

   ACS_CS_API_Name neidName;

   ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::getNEID(neidName);

   size_t nameLength = (size_t)neidName.length();
   char* nameChar = new char[nameLength];

   if (nameChar)
   {
      neidName.getName(nameChar, nameLength);
      string tmpNEID = nameChar;
      delete [] nameChar;

      if (neidString == tmpNEID)
         success++;
      else
         failed++;
   }
   else
      failed++;
}

void testFDDEF()
{
   stringstream bcStringStream;
   map<APID, string> apFuncNameMap;
   map<APID, string>::iterator mapIt;

   for (it = cpIdMap.begin(); it != cpIdMap.end(); it++)
   {
      CPID cpId = it->first;
      string cpName = it->second;

      if (cpId < 1000)
      {
         if (bcStringStream.str().length() == 0)
            bcStringStream<<cpName;
         else
            bcStringStream<<","<<cpName;
      }
   }

   for (it = apIdMap.begin(); it != apIdMap.end(); it++)
   {
      stringstream funcName;
      APID apId = it->first;
      string apName = it->second;
      funcName<<"STS"<<apId % 2000;

      string command = "fddef -d MTAP -s " + funcName.str() + " -cp " + bcStringStream.str() + " " + apName;  //e.g. fddef -d MTAP -s STS1 -cp BC0,BC1,BC2,BC3,BC4,BC5 AP1

      if (system(command.c_str()) != 0)
         failed++;
      else
      {
         apFuncNameMap[apId] = "MTAP:" + funcName.str();
         success++;
      }
   }

   ACS_CS_API_FunctionDistribution* fdTable = ACS_CS_API::createFunctionDistributionInstance();

   if (fdTable)
   {
      ACS_CS_API_NameList funcNameList;

      if (fdTable->getFunctionNames(funcNameList) == ACS_CS_API_NS::Result_Success)
      {
         if (funcNameList.size() == apIdMap.size())
            success++;
         else
            failed++;
      }  
      else
         failed++;

      for (unsigned int i = 0; i < funcNameList.size(); i++)
      {
         ACS_CS_API_IdList apList;
         ACS_CS_API_Name funcName = funcNameList[i];

         if (fdTable->getFunctionProviders(funcName, apList) == ACS_CS_API_NS::Result_Success)
         {
            if (apList.size() == 1)
            {
               mapIt = apFuncNameMap.find(apList[0]);
               
               if (mapIt != apFuncNameMap.end())
               {
                  string tmpFuncName = mapIt->second;
                  size_t funcNameLength = (size_t)funcName.length();
                  char* funcNameChar = new char[funcNameLength];

                  if (funcNameChar)
                  {
                     if (funcName.getName(funcNameChar, funcNameLength) == ACS_CS_API_NS::Result_Success)
                     {
                        if (funcNameChar == tmpFuncName)
                        {
                           ACS_CS_API_IdList cpList;

                           if (fdTable->getFunctionUsers(apList[0], funcName, cpList) == ACS_CS_API_NS::Result_Success)
                           {
                              stringstream tmpBCStringStream;

                              for (unsigned int i = 0; i < cpList.size(); i++)
                              {
                                 CPID cpId = cpList[i];

                                 if (tmpBCStringStream.str().length() == 0)
                                    tmpBCStringStream<<"BC"<<cpId;
                                 else
                                    tmpBCStringStream<<","<<"BC"<<cpId;;
                              }

                              if (bcStringStream.str() == tmpBCStringStream.str())
                                 success++;
                              else
                                 failed++;
                           }
                           else
                              failed++;
                        }
                        else
                           failed++;
                     }
                     else
                        failed++;
                  }
                  else
                     failed++;
               }
               else
                  failed++;
            }
            else
               failed++;
         }
         else
            failed++;
      }  

      ACS_CS_API::deleteFunctionDistributionInstance(fdTable);
   }
   else
      failed++;

   for (it = apIdMap.begin(); it != apIdMap.end(); it++)
   {
      stringstream funcName;
      APID apId = it->first;
      string apName = it->second;
      funcName<<"STS"<<apId % 2000;

      string command = "fdrm -d MTAP -s " + funcName.str() + " " + apName;  //e.g. fdrm -d MTAP -s STS1 AP1

      if (system(command.c_str()) != 0)
         failed++;
      else
         success++;
   }
}

void testCPCH()
{
   for (it = cpIdMap.begin(); it != cpIdMap.end(); it++)
   {
      stringstream command;
      CPID tmpCPID = it->first;

      if (tmpCPID < 2000)
      {
         if (tmpCPID < 1000)
            command<<"cpch -s APZ21401 -t 21255 -a TSC-"<<tmpCPID + 1<<" "<<tmpCPID;  //e.g. cpch -s APZ21401 -t 21255 -a TSC-1 0
         else
            command<<"cpch -s APZ21255 -t 21255 -a MSC-"<<tmpCPID % 1000 + 1<<" "<<tmpCPID;  //e.g. cpch -s APZ21255 -t 21255 -a MSC-1 1000

         if (system(command.str().c_str()) != 0)
            failed++;
         else
            success++;
      }
      else
         failed++;
   }

   ACS_CS_API_CP* cpTable = ACS_CS_API::createCPInstance();

   if (cpTable)
   {
      ACS_CS_API_IdList cpList;

      ACS_CS_API_NS::CS_API_Result result = cpTable->getCPList(cpList);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         for (unsigned int i = 0; i < cpList.size(); i++)
         {
            CPID cpId = cpList[i];

            if (cpIdMap.find(cpId) != cpIdMap.end())
            {
               ACS_CS_API_NS::CS_API_APZ_Type apzType;

               result = cpTable->getAPZType(cpId, apzType);

               if (result == ACS_CS_API_NS::Result_Success)
               {
                  if (cpId < 1000)
                  {
                     if (apzType == ACS_CS_API_NS::APZ21401)
                        success++;
                     else
                        failed++;
                  }
                  else if (cpId < 2000)
                  {
                     if (apzType == ACS_CS_API_NS::APZ21255)
                        success++;
                     else
                        failed++;
                  }
                  else
                     failed++;


                  ACS_CS_API_Name cpName;

                  result = cpTable->getCPName(cpId, cpName);

                  if (result == ACS_CS_API_NS::Result_Success)
                  {
                     size_t nameLength = (size_t)cpName.length();
                     char* cpNameChar = new char[nameLength];

                     if (cpNameChar)
                     {
                        result = cpName.getName(cpNameChar, nameLength);

                        if (result == ACS_CS_API_NS::Result_Success)
                        {
                           if (cpId < 1000)
                           {
                              stringstream cpNameStringStream;

                              cpNameStringStream<<"TSC-"<<cpId + 1;

                              if (cpNameChar == cpNameStringStream.str())
                                 success++;
                              else
                                 failed++;
                           }
                           else if (cpId < 2000)
                           {
                              stringstream cpNameStringStream;

                              cpNameStringStream<<"MSC-"<<cpId % 1000 + 1;

                              if (cpNameChar == cpNameStringStream.str())
                                 success++;
                              else
                                 failed++;
                           }
                           else
                              failed++;
                        }
                        else
                           failed++;

                        delete [] cpNameChar;
                     }
                     else
                        failed++;
                  }
                  else
                     failed++;

                  CPID tmpCPId;

                  if (cpTable->getCPId(cpName, tmpCPId) == ACS_CS_API_NS::Result_Success)
                  {
                     if (tmpCPId == cpId)
                        success++;
                     else
                        failed++;
                  }
                  else
                     failed++;
               }
               else
                  failed++;
            }
            else
               failed++;
         }  //end for-loop
      }
      else
         failed++;

      ACS_CS_API::deleteCPInstance(cpTable);
   }
   else
      failed++;

   for (it = cpIdMap.begin(); it != cpIdMap.end(); it++)
   {
      stringstream command;

      CPID tmpCPID = it->first;
      command<<"cpch -r "<<tmpCPID;  //e.g. cpch -r 1000

      if (system(command.str().c_str()) != 0)
         failed++;
      else
         success++;
   }
}

void testCPGDEF()
{
   map<string, string> groupNameCPNamesMap;
   map<string, string>::iterator mapIt;
   stringstream bcStringStream;
   stringstream cpStringStream;

   for (it = cpIdMap.begin(); it != cpIdMap.end(); it++)
   {
      CPID cpId = it->first;
      string cpName = it->second;

      if (cpId < 1000)
      {
         if (bcStringStream.str().length() == 0)
            bcStringStream<<cpName;
         else
            bcStringStream<<","<<cpName;
      }
      else if (cpId < 2000)
      {
         if (cpStringStream.str().length() == 0)
            cpStringStream<<cpName;
         else
            cpStringStream<<","<<cpName;
      }
   }

   string allCPString = "";

   if (bcStringStream.str().length() == 0)
      allCPString = cpStringStream.str();
   else if (cpStringStream.str().length() == 0)
      allCPString = bcStringStream.str();
   else
      allCPString = bcStringStream.str() + "," + cpStringStream.str();

   groupNameCPNamesMap["ALL"] = allCPString;
   groupNameCPNamesMap["ALLBC"] = bcStringStream.str();

   stringstream commandBCGrp;
   stringstream commandCPGrp;

   commandBCGrp<<"cpgdef -cp "<<bcStringStream.str()<<" ALLBCs";  //e.g. cpgdef -cp BC0,BC1,BC2,BC3,BC4,BC5 ALLBCs

   if (system(commandBCGrp.str().c_str()) == 0)
   {
      groupNameCPNamesMap["ALLBCs"] = bcStringStream.str();
      success++;
   }
   else
      failed++;

   commandCPGrp<<"cpgdef -cp "<<cpStringStream.str()<<" ALLCPs";  //e.g. cpgdef -cp CP0,CP1 ALLCPs

   if (system(commandCPGrp.str().c_str()) == 0)
   {
      groupNameCPNamesMap["ALLCPs"] = cpStringStream.str();
      success++;
   }
   else
      failed++;


   ACS_CS_API_CPGroup* cpGroupTable = ACS_CS_API::createCPGroupInstance();
   
   if (cpGroupTable)
   {
      ACS_CS_API_NameList groupNameList;

      ACS_CS_API_NS::CS_API_Result result = cpGroupTable->getGroupNames(groupNameList);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         if (groupNameList.size() != groupNameCPNamesMap.size())  //number of predefined and operator defined groups
            failed++;

         for (unsigned int i = 0; i < groupNameList.size(); i++)
         {
            ACS_CS_API_Name groupName = groupNameList[i];
            
            ACS_CS_API_IdList cpList;

            result = cpGroupTable->getGroupMembers(groupName, cpList);

            if (result == ACS_CS_API_NS::Result_Success)
            {
               size_t groupNameLength = (size_t)groupName.length();
               char* groupNameChar = new char[groupNameLength];

               if (groupNameChar)
               {
                  if (groupName.getName(groupNameChar, groupNameLength) == ACS_CS_API_NS::Result_Success)
                  {
                     mapIt = groupNameCPNamesMap.find(groupNameChar);

                     if (mapIt != groupNameCPNamesMap.end())
                     {
                        stringstream tmpCPStringStream;
                        string cpString = mapIt->second;

                        for (unsigned int i = 0; i < cpList.size(); i++)
                        {
                           CPID cpId = cpList[i];

                           if (tmpCPStringStream.str().length() == 0)
                           {
                              if (cpId < 1000)
                                 tmpCPStringStream<<"BC"<<cpId;
                              else if (cpId < 2000)
                                 tmpCPStringStream<<"CP"<<cpId % 1000;
                              else
                                 failed++;
                           }
                           else
                           {
                              if (cpId < 1000)
                                 tmpCPStringStream<<","<<"BC"<<cpId;
                              else if (cpId < 2000)
                                 tmpCPStringStream<<","<<"CP"<<cpId % 1000;
                              else
                                 failed++;
                           }
                        }  //end for-loop

                        if (cpString == tmpCPStringStream.str())
                           success++;
                        else
                           failed++;
                     }
                     else
                        failed++;
                  }
                  else
                     failed++;
               }
               else
                  failed++;
            }
            else
               failed++;
         }  //end for-loop
      }
      else
         failed++;

      ACS_CS_API::deleteCPGroupInstance(cpGroupTable);
   }
   else
      failed++;

   if (system("cpgrm ALLBCs") == 0 && system("cpgrm ALLCPs") == 0)
      success++;
   else
      failed++;
}

DWORD /*RoGa WINAPI*/ ThreadProc(void *  lpParam ) {
	for (int i = 0; i < 100; i++) testCPTableConnection();
	return 0; 
} 

void testConnectionsOverload() {
	bool qRes = false;

	for (int i = 0; i < 100; i++) {
		//qRes = QueueUserWorkItem(ThreadProc, 0, WT_EXECUTELONGFUNCTION);
		if (qRes != true) std::cerr << "Failed to start thread" << std::endl;
	}
	
	for (int i = 0; i < 1000; i++) {
		cout << i << ") " << "failed: " << failed << ", success: " << success << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
		//RoGa Sleep(1000);
		sleep(1000);
	}
	cout << "\nTEST TERMINATED!\n";
	exit(0);
}

void testCPTableConnection()
{
   for (int i = 0; i < 10; i++)
   {
      ACS_CS_API_CP* cpTable = ACS_CS_API::createCPInstance();

      if (cpTable)
      {
         ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NS::Result_Failure;

         ACS_CS_API_IdList list;
         result = cpTable->getCPList(list);

         if (result == ACS_CS_API_NS::Result_Success)
            success++;
         else
            failed++;

         ACS_CS_API::deleteCPInstance(cpTable);
      }
   }
}

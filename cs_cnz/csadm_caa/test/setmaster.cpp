// setmaster.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <sstream>

#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"

using namespace std;

int main(int argc, char* argv[])
{
   int returnCode = 0;

   if (argc == 2)
   {
      istringstream is(argv[1]);
      unsigned short cpMaster = 65535;

      if (is >> cpMaster)
      {
         ACS_CS_API_SET_NS::CS_API_Set_Result result = ACS_CS_API_Set::setAlarmMaster(cpMaster);

         if (result == ACS_CS_API_SET_NS::Result_Success)
         {
            result = ACS_CS_API_Set::setClockMaster(cpMaster);

            if (result != ACS_CS_API_SET_NS::Result_Success)
            {
               returnCode = 1;
            }
         }
         else
         {
            returnCode = 1;
         }
      }
      else
         returnCode = 2;
   }
   else
      returnCode = 2;

   if (returnCode == 1)
   {
      cerr << "\n" << "Error when executing (general fault)" << endl;
   }
   else if (returnCode == 2)
   {
      cerr << "\n" << "Incorrect usage" << endl;
      cerr << "Usage: setmaster cpid" << endl;
   }

	return returnCode;
}


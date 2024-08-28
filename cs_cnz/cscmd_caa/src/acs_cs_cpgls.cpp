/*
 * acs_cs_cpgls.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: egiulfe
 */

#include "ACS_CS_Command_CPGLS_Handler.h"
#include "ACS_CS_Exception.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
   bool success;
   ACS_CS_Command_CPGLS_Handler cpgls(argc, argv);

   try
   {
      success = cpgls.handleCommand();
   }
   catch (ACS_CS_Exception ex)
	{
      success = false;
   }

   if (! success)
      cout<<endl<<endl<<cpgls.getExitCodeDescription()<<endl<<endl;

   return cpgls.getExitCode();
}

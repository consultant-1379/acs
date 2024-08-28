/*
 * acs_cs_cpls.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: egiulfe
 */

#include "ACS_CS_Command_CPLS_Handler.h"
#include "ACS_CS_Exception.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
   bool success;
   ACS_CS_Command_CPLS_Handler cpls(argc, argv);

   try
   {
      success = cpls.handleCommand();
   }
   catch (ACS_CS_Exception ex)
	{
      success = false;
   }

   if (! success)
      cout<<endl<<endl<<cpls.getExitCodeDescription()<<endl<<endl;

   return cpls.getExitCode();
}

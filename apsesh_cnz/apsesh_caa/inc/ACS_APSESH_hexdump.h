//	.NAME
//	    ACS_APSESH_hexdump
//	.LIBRARY 3C++
//	.PAGENAME ACS_APSESH
//	.HEADER  ACS_APSESH
//
//	.COPYRIGHT
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	.DESCRIPTION
//	   See below
//
//	AUTHOR
//	   enatmeu
//
//	*****************************************************************************
#ifndef ACS_APSESH_HEXDUMP_h
#define ACS_APSESH_HEXDUMP_h 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Returns a string containing a hex dump of the memory block specified by the given start address and length.
// To avoid access faults, make sure that the calling program has legitimate access to the entire memory block. 
int hexdump(char *&strOut, const char *startAddr, int length) 
{
   if (startAddr == NULL) 
   {
      return 0;
   }

   int iHex = 0;  // Incremented each time a hex representation is printed.
   int iAscii = 0;  // Incremented each time a ASCII representation is printed,
   unsigned char c;  // Used for ASCII representations.
   char strTemp[80];

   int sizeOutAllocated = length * 5 * (sizeof(char)) + 64;
   strOut = (char*) realloc(strOut, sizeOutAllocated);

   if (!strOut) 
   {
      return 0;
   }

   memset(strOut, 0, sizeOutAllocated);

   while (iHex < length) 
   {

      if (!(iHex%16)) 
      {
         sprintf(strTemp, "%8p: ", startAddr+iHex);  // Print the first address of this line.
         strOut = strcat(strOut, strTemp);
      }

      sprintf(strTemp, "%02X ", static_cast<unsigned char>(*(startAddr+iHex)));  // Print a value.
      strOut = strcat(strOut, strTemp);

      if (iHex == length-1) 
      {  // If we've printed out all the values...
         while (++iHex % 16) 
         {  // then finish the line with blanks.
            strOut = strcat(strOut, "-- ");   
         }  
      }
      else 
      {
         iHex++;
      }

      if (!(iHex%16)) 
      {  // If we're at the end of a 16-character line...
         strOut = strcat(strOut, "| ");  // Print the separator between the hex and ASCII ouput.
         while (iAscii < iHex && iAscii < length) 
         {  // Print the ASCII representations.
            c = *(startAddr+iAscii);
            sprintf(strTemp, "%c", (c >= 33 && c < 255) ? c : 0x2E);
            strOut = strcat(strOut, strTemp);
            iAscii++;
         }
         strOut = strcat(strOut, "\n"); 
      }  
   }
   strOut = strcat(strOut, "\n");

   return 1;
}

#endif

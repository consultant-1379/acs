
#include <ace/ACE.h>
#include "ACS_APGCC_TempFile.H"
#include "ACS_APGCC_Util.H"
extern "C" {
#include <time.h>
#include <stdio.h>
}
using namespace std;

ostream& ACS_APGCC_TempFile::os()
{

	return file;
}

istream& ACS_APGCC_TempFile::is()
{

	return file;
}

ACS_APGCC_TempFile::ACS_APGCC_TempFile(bool removeOnExit)
:removeOnExit_(removeOnExit), opened_(false)
{
}

ACS_APGCC_TempFile::~ACS_APGCC_TempFile()
{
	if ( removeOnExit_ )
		this->remove();
}

bool ACS_APGCC_TempFile::open(const string& prefix, const string& format, const string& ext)
{
	char* cp;
   string tmpstr;

   fname_ = prefix;

	// Construct date and time string
	char timestr[128];
	time_t ltime;
	struct tm* today;

   // Get current date and time
	::time( &ltime );
	today = ::localtime( &ltime );

   // Check format string, if present
   int i = 0;

   while ( i < format.length() ) {

      switch (format[i])
      {
      case '%':

         // Check what it is
         i++;

         switch (format[i])
         {
         case 'i':   // unique identifier
            cp = ::tmpnam(NULL);
            if ( cp ) {

              if ( !ACS_APGCC::item(tmpstr,cp,"\\/.",0) )

                  tmpstr = cp;
               fname_ += tmpstr;
            }
            break;

         case 'u':   // user name
          /*PR_01 BEGIN commentati per successiva implementazione*/
        	 // tmpstr = ACS_APGCC::getUserLoginName();
            ACS_APGCC::toLower(tmpstr);
        	 /*PR_01 END*/
            fname_ += tmpstr;
            break;

         case 'D':   // date as YYYYMMDD
            ::strftime(timestr, 127, "%Y%m%d", today);
            fname_ += timestr;
            break;

         case 'T':   // time as HHMMSS
            ::strftime(timestr, 127, "%H%M%S", today);
            fname_ += timestr;
            break;

         case 'y':   // year as YYYY
            ::strftime(timestr, 127, "%Y", today);
            fname_ += timestr;
            break;

         case 'm':   // month as MM
            ::strftime(timestr, 127, "%m", today);
            fname_ += timestr;
            break;

         case 'd':   // day as DD
            ::strftime(timestr, 127, "%d", today);
            fname_ += timestr;
            break;

         case 'H':   // hour as HH
            ::strftime(timestr, 127, "%H", today);
            fname_ += timestr;
            break;

         case 'M':   // minute as MM
            ::strftime(timestr, 127, "%M", today);
            fname_ += timestr;
            break;

         case 'S':   // second as SS
            ::strftime(timestr, 127, "%S", today);
            fname_ += timestr;
            break;

         default:
            fname_ += format[i];
         }
         break;

      default:
         fname_ += format[i];
      }
      i++;
   }


   // File name cannot be empty
   if ( fname_.length() == 0 )
      return false;


   // Add extension if given
   if ( ext.length() > 0 ) {
      // Add ending period if not already there
      if ( fname_[fname_.length()-1] != '.' )
         fname_ += '.';
      fname_ += ext;
   }

	//cout << "Temp_File::open(); tmpfile='" << fname_.c_str() << "'" << endl;

	// open the file
	file.open(fname_.c_str(), ios::in|ios::out|ios::app);
	if ( !file )
		return false;

	opened_ = true;

	return true;
}


void ACS_APGCC_TempFile::close()
{
	if ( opened_ )
		file.close();
	opened_ = false;
}


void ACS_APGCC_TempFile::deleteOnExit(bool TrueFalse)
{
   removeOnExit_ = TrueFalse;
}


bool ACS_APGCC_TempFile::remove()
{
	bool res = true;
	this->close();
	if ( fname_.length() > 0 )
		res = ( ACE_OS::unlink(fname_.c_str()) == 0 ? true : false );
	return res;
}

const char* ACS_APGCC_TempFile::filename() const
{

	return fname_.c_str();
}


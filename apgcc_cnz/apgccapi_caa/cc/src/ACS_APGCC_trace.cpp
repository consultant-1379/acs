#include "ACS_APGCC_Trace.H"
#include <ace/OS.h>
#include <ace/ACE.h>


//char __g_mcc_trace_msg[500];
//OSF_Thread_Mutex __g_mcc_trace_lock;


//----------------------------------------------------------------------------------------
// Format given arguments into the tracemsg string.
//----------------------------------------------------------------------------------------

void mcc_format_tracemsg(char* tracestr, va_list argp, const char* format_str)
{

   char* format = ::strdup(format_str);
   char* saved_p = format;
   char* sp;
   char* bp = tracestr;
   int type = 0;

   //TEST--------------------------------------------------
   //printf("%s \n", "start...");
   //------------------------------------------------------

   try {

      *bp = 0;

      // Preformat string with thread id if not given in format string
      sp = strstr(format,"%t");
      if ( !sp ) {
         ::sprintf(bp, "(%u) ", ACE_OS::thr_self());
         // Locate end of bp.
         while (*bp != '\0')
            bp++;
      }

      while (*format != '\0') {
         // Copy input to output until we encounter a %, however a
         // % followed by another % is not a format specification.

         //cout << "format='" << format << "'" << endl;

         if (*format != '%')
            *bp++ = *format++;
         else if (format[1] == '%') {  // An "escaped" '%' (just print one '%').
            *bp++ = *format++;         // Store first %
            format++;      *bp++ = *format++;             // but skip second %
         }
         else {
            char* fp = format++;
            type = 0;

            switch (*format++)
            {
            case 'p': // Format the string associated with the value of GatLastError().
            	/* PR_01 BEGIN
            	::sprintf(bp, "%s", MCS::getSystemErrorText());
            	*/
            	::sprintf(bp, "%s", "MCS::getSystemErrorText()");
            	//TEST------------------------------------

            	 //---------------------------------------
               break;

            case 'q': // Format the string associated with the given value
            	/* PR_01 BEGIN
            	::sprintf(bp, "%s", MCS::getSystemErrorText(va_arg(argp, int)));
            	*/
            	::sprintf(bp, "%s", "ACS_APGCC::getSystemErrorText(va_arg(argp, int))");
            	//TEST---------------------------------------

            	 //------------------------------------------
               break;

            case 'P':      // Format the current process id.
               ::sprintf(bp, "%d", ::getpid());
               //TEST-----------------------------------------

               //---------------------------------------------
               break;

            case 't': // Format thread id.
               ::sprintf(bp, "%u", ACE_OS::thr_self());
               //TEST----------------------------------------------

               //--------------------------------------------------
               break;

            case 'T': // Format the timestamp in hour:minute:sec:usec format.
               {
                  char day_and_time[35];
                  ::sprintf(bp, "%s", ACE::timestamp(day_and_time, size_t(day_and_time)));
                  //TEST----------------------------------------------

                  //---------------------------------------------------
               }
               break;
//-----------------------------------------------------------------------------------
// TO TEST
            case 's':
               type = 1;
               break;

            case 'S':
               try {

                 std::string s = va_arg(argp, std::string);
            	 ::sprintf(bp, "%s", s.c_str());

               }
               catch(...) {
                  // Nada
               }
               break;
//-----------------------------------------------------------------------------------
            case 'W':
               // @@ UNICODE
            case 'd': case 'c': case 'i': case 'o':
            case 'u': case 'x': case 'X':
               type = 2;
               break;

            case 'F': case 'f': case 'e': case 'E':
            case 'g': case 'G':
               type = 3;
               break;

            default:
               // ?
               break;

            }// switch

            if ( type != 0 ) {
               char c = *format;
               *format = 0;

               //cout << "fp='" << fp << "'" << endl;
               switch(type)
               {
               case 1:
                  // A single %s string formatter causes an
                  // exception in the va_arg() call.
                  // Since argp points to the first string, we can use
                  // argp directly if the exception occurs.
                  try {
                     ::sprintf(bp, fp, va_arg(argp, char*));
                     //TEST-------------------------------------------------------

                     //-----------------------------------------------------------
                  }
                  catch(...) {
                     try {
                        ::sprintf(bp, fp, argp);

                     }
                     catch(...) {
                     }
                  }
                  break;
               case 2:
                  ::sprintf(bp, fp, va_arg(argp, int));
                  //TEST--------------------------------------------------------------

                  //------------------------------------------------------------------
                  break;
               case 3:

            	   ::sprintf(bp, fp, va_arg(argp, double));

                  break;
               }
               *format = c; //------------------------------------------------------------------
            }

            while (*bp != '\0') // Locate end of bp.
               bp++;

         }// if

      }// while

      *bp = 0;

      sp = strchr(tracestr,'\n');

      if ( sp )
         *sp = 0;

   }// try
   catch(...) {
   }
   ::free(saved_p);
}

//---------------------------------------------------------------------------------
//TEST
//---------------------------------------------------------------------------------
/**/
/*
void prova_trace(char* tracemsg, const char* format_str, ...){

	va_list argp;
	va_start(argp, format_str);
	mcc_format_tracemsg(tracemsg, argp, format_str);
	va_end(argp);

}
*/
#if 0
int main(){

	char tracemsg[]="prova%t";


	char s0[]="test";
	int numInt=10;
	std::string s1;

	s1="prova";

	//char format_str[]="%p";	Test if(!sp)
	//char format_str[]="%p";		//OK
	//char format_str[]="%q";		//OK
	//char format_str[]="%P";		//OK
	//char format_str[]="%t";		//OK
	//char format_str[]="%T";		//OK	print timestamp
	//char format_str[]="%s";			//OK	parameter  char*
	//char format_str[]="%S";				//parameter string
	//char format_str[]="%i";				parameter int
	char format_str[]="%f";		//OK 	parametre double


	prova_trace(tracemsg, format_str, s0, numInt, 0.898);
	//mcc_format_tracemsg(tracemsg, argp, format_str);


}

//----------------------------------------------------------------------------------

#endif

//=========================================================================================
// NOT USED BELOW
//=========================================================================================

#if 0

MCS_Trace::MCS_Trace()
:last_error_(0), linenum_(0)
{
}

MCS_Trace& MCS_Trace::instance()
{
   static MCS_Trace instance;
   return instance;
}

void MCS_Trace::set(const char* file, int linenum, DWORD last_error)
{
   this->linenum_ = linenum;
   this->last_error_ = last_error;
}

void MCS_Trace::log(ACS_TRA_trace& tracePoint, const char* format_str, ...)
{
   // Start of variable args section.
   va_list argp;
   va_start(argp, format_str);
   this->log(tracePoint, format_str, argp);
   va_end(argp);
}

/*
  void log (const char *format, ...);
  // Format a message to the thread-safe OSF logging mechanism.  Valid
  // options (prefixed by '%', as in printf format strings) include:
  // 'c': print a character
  // 'i', 'd': print a decimal number
  // 'I', indent according to nesting depth
  // 'e', 'E', 'f', 'F', 'g', 'G': print a double
  // 'l', print line number where an error occurred.
  // 'o': print as an octal number
  // 'P': print out the current process id
  // 'p': print out the appropriate errno value from sys_errlist
  // 'R': print return status
  // 's': print out a character string
  // 'T': print timestamp in hour:minute:sec:usec format.
  // 'D': print timestamp in month/day/year hour:minute:sec:usec format.
  // 't': print thread id (1 if single-threaded)
  // 'u': print as unsigned int
  // 'X', 'x': print as a hex number
  // '%': print out a single percent sign, '%'
*/
void MCS_Trace::log(ACS_TRA_trace& tracePoint, const char* format_str, va_list argp)
{
   OSF_Guard<OSF_Thread_Mutex> guard(this->lock_);

   char* bp = this->msg_;
   char* format = ::strdup(format_str);
   char* saved_p = format;
   int type = 0;

   if ( !format )
      return;

   *bp = 0;

   while (*format != '\0') {
      // Copy input to output until we encounter a %, however a
      // % followed by another % is not a format specification.

      //cout << "format='" << format << "'" << endl;

      if (*format != '%')
         *bp++ = *format++;
      else if (format[1] == '%') {  // An "escaped" '%' (just print one '%').
         *bp++ = *format++;         // Store first %
         format++;                  // but skip second %
      }
      else {
         char* fp = format++;
         type = 0;

         switch (*format++)
         {
         case 'l':      // Format current line number
            ::sprintf(bp, "%d", this->linenum_);
            break;

         case 'p': // Format the string assocated with the value of errno.
            {
               errno = OSF::map_errno ( this->last_error_ );
               if (errno >= 0 && errno < sys_nerr) {
                  //@@::sprintf(bp, "%s: %s", va_arg(argp, char *), strerror (errno));
                  ::sprintf(bp, "%s", strerror (errno));
               }
               else {
                  LPTSTR lpMsgBuf = 0;

                  ::FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                     NULL,
                     errno,
                     MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                     (LPTSTR) &lpMsgBuf,
                     0,
                     NULL);

                  // If we don't get a valid response from
                  // <FormatMessage>, we'll assume this is a
                  // WinSock error and so we'll try to convert
                  // it into a string.  If this doesn't work it
                  // returns "unknown error" which is fine for
                  // our purposes.
                  if (lpMsgBuf == 0) {
                     const char *message = OSF::sock_error(errno);
                     //@@::sprintf(bp, "%s: %s", va_arg(argp, const char *), message);
                     ::sprintf(bp, "%s", message);
                  }
                  else {
                     //::sprintf(bp, "%s: %s", va_arg (argp, char *), lpMsgBuf);
                     ::sprintf(bp, "%s", lpMsgBuf);
                     // Free the buffer.
                     ::LocalFree (lpMsgBuf);
                  }
               }
               break;
            }

         case 'P':      // Format the current process id.
            ::sprintf(bp, "%d", ::getpid());
            break;

         case 't': // Format thread id.
            ::sprintf(bp, "%u", OSF_OS::thr_self());
            break;

         case 'T': // Format the timestamp in hour:minute:sec:usec format.
            {
               char day_and_time[35];
               ::sprintf(bp, "%s", OSF::timestamp(day_and_time,sizeof day_and_time));
            }
            break;

         case 's':
            type = 1;
            //cout << "argp=" << (const char*) argp << endl;
            //::sprintf(bp, "%s", argp);
            //va_arg(argp,const char*);
            break;

         case 'W':
            // @@ UNICODE
         case 'd': case 'c': case 'i': case 'o':
         case 'u': case 'x': case 'X':
            type = 2;
            break;

         case 'F': case 'f': case 'e': case 'E':
         case 'g': case 'G':
            type = 3;
            break;

         default:
            // ?
            break;

         }// switch

         if ( type != 0 ) {
            char c = *format;
            *format = 0;

            //cout << "fp='" << fp << "'" << endl;
            switch(type)
            {
            case 1:
               ::sprintf(bp, fp, va_arg(argp, char*));
               break;
            case 2:
               ::sprintf(bp, fp, va_arg(argp, int));
               break;
            case 3:
               ::sprintf(bp, fp, va_arg(argp, double));
               break;
            }
            *format = c;
         }

         while (*bp != '\0') // Locate end of bp.
            bp++;

      }// if

   }// while

   *bp = 0;

   ::free(saved_p);

   char* sp = strchr(this->msg_,'\n');
   if ( sp )
      *sp = 0;
   //OSF_DEBUG((LM_DEBUG,"%s",this->msg_));
   cerr << this->msg_ << endl;
   if (ACS_TRA_ON(tracePoint)) {
      ACS_TRA_event(&tracePoint, this->msg_);
   }
}


#endif // if 0

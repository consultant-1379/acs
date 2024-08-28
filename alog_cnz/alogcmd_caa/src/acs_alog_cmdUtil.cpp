//****************************************************************************
//
//  NAME
//     acs_alog_cmdUtil.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2004. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson  AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  PR           DATE      INITIALS    DESCRIPTION
//  -----------------------------------------------------------
//  N/A       26/07/2012     xbhakat       Initial Release
//  N/A       07/08/2012     xbhakat        Final Release
//  N/A       08/09/2015     xsansud       HT92230
//  ===================================================================
//
//  SEE ALSO
//     -
//
//****************************************************************************


#include <stdio.h>

#include <termios.h>
#include <unistd.h>
#include <acs_alog_cmdUtil.h>
#include <acs_alog_execute.h>
#include <sys/types.h>
#include <pwd.h>
#include <fstream>

using namespace std;


acs_alog_cmdUtil::acs_alog_cmdUtil()
{

}
void acs_alog_cmdUtil::echo(bool on)
{
	struct termios settings;
	tcgetattr(STDIN_FILENO, &settings);
	settings.c_lflag =
			on ? (settings.c_lflag | ECHO) : (settings.c_lflag & ~(ECHO));
	settings.c_lflag |= ICANON;
	tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}
//--------------------------------------------------------------------------------------------------

bool acs_alog_cmdUtil::getPassword(char *pPass, int max) // PL
{
	int i = 0;
	char ch;
	char* pszPsw = pPass;
	char* pszEnd = pszPsw + (max - sizeof(char));
	bool bContinue = true;
	bool bUnacceptableCharFlag = false;
    struct termios newMode;

    tcgetattr(STDIN_FILENO, &newMode);
    newMode.c_lflag = (newMode.c_lflag & ~(ECHO));
    newMode.c_lflag &= ~ICANON;
    newMode.c_lflag &= ~BSDLY;
    tcsetattr(STDIN_FILENO, TCSANOW, &newMode);
	while (bContinue)
	{
		ch = getchar();

		switch (ch) {
		case '\n':

			bContinue = false;
			break;
		case 127:
			if (pszPsw > pPass) {
				pszPsw--;
				*pszPsw = 0;
				cout << ("\b \b") << flush;
				i--;
			}
			break;
		case 34:								// HT92230: Special characters with ASCII values 34, 92, 96 are not allowed in the password
		case 92:
		case 96:
			bUnacceptableCharFlag = true;
		default:

			if (ch >= (' ') && pszPsw < pszEnd) {
				*pszPsw = ch;
				pszPsw++;
				cout << ("*") << flush;
				i++;

				if(ch > ('~'))				// HT92230: all characters should lie within printable ASCII range 32-126
					bUnacceptableCharFlag = true;
			}
			break;
		}
		if (i == (max - 1))
			break;
	}

	*pszPsw = 0;

	echo(true);
	pPass[i] = 0;
	if(bUnacceptableCharFlag)				// HT92230: if a restricted character is present, don't accept the password
		return false;

	return(i < (max-1));


}
//--------------------------------------------------------------------------------------------------
int acs_alog_cmdUtil::getNewConfirmPassword(char *pOld, char *pNew,char *pConfirm) // PL
{
	cout << "type the new password\03: " << flush;

	// get new password
	if (!acs_alog_cmdUtil::getPassword(pNew,101)){
		cerr << endl << "Incorrect Password" << endl;
		return (39);
	}

	if (!acs_alog_cmdUtil::checkComplexity(pNew)){
		cerr << endl << "Incorrect Password " << endl;
		return 39;
	}

	// test confirmation
	if ( strcmp(pNew, pOld ) == 0 ){
		cerr << endl << "Incorrect Password " << endl;
		return 39;
	}

	cout << "\nconfirm the new password\03: " << flush;

	// get Confirm password
	if (!getPassword(pConfirm,101)){
		cerr << endl << "Incorrect Password " << endl;
		return 39;
	}

	if (!checkComplexity(pConfirm)){
		cerr << endl << "Incorrect Password " << endl;
		return 39;
	}

	// test confirmation
	if ( strcmp(pNew, pConfirm ) != 0 ){
		cerr << endl << "Passwords not matching" << endl;
		return 40;
	}

	return (0);
}
//--------------------------------------------------------------------------------------------------


bool acs_alog_cmdUtil::checkComplexity(char *pPass) // PL
{
	int isNumber = 0;
	int isUpper = 0;
	int isLower = 0;
	int isExtra = 0;

	bool checkFirstChar = isalnum((int) pPass[0]);		// HT92230: First character of password should be alphanumeric
	if(!checkFirstChar)
		return checkFirstChar;

	int i = 0;

	while (i < ( (int)strlen(pPass)) )
	{
		if (isdigit((int) pPass[i]))
			isNumber++;
		else
			if (isupper((int) pPass[i]))
				isUpper++;
			else
				if (islower((int) pPass[i]))
					isLower++;
				else
					isExtra++;
		i++;
	}
	bool checkLen = ( strlen(pPass) > MIN_PASSWORD_LENGTH-1 )&&( strlen(pPass)< MAX_PASSWORD_LENGTH+1  );
	i = (isNumber > 0) + (isUpper > 0) + (isLower > 0) + (isExtra > 0 );
	return ( (i > 2) && checkLen  );
}
//--------------------------------------------------------------------------------------------------

int acs_alog_cmdUtil::HandleReturnCode(enumCmdExecuteResult res)
{
	return(HandleReturnCode(res, true));
}
//--------------------------------------------------------------------------------------------------
int acs_alog_cmdUtil::HandleReturnCode(enumCmdExecuteResult res, bool eol)
{
	int nReturn = 0;
	switch (res)
	{
	case cerSuccess:
		// No printout on successfull command execution
		if(eol)
			cout << endl;
		break;

	case cerPwAlreadyDefined: // PL
		// Password already defined in alogset -p
		cerr << endl <<  "Password already defined"  << endl;
		nReturn = 41;
		break;

	case cerPwNotDefined: // PL
		// Password not defined on alogset -p -c
		cerr << endl << "Password not defined"  << endl;
		nReturn = 42;

		break;

	case cerPwNotMatch: // PL
		// Password not defined on alogset -p -c
		cerr << endl << "Invalid Password "  << endl;
		nReturn = 43;

		break;

	case cerServerNotResponding:
		cerr << endl << "Unable to connect to server"  << endl;
		nReturn = 117;
		break;
	case cerFileOpenError:
	case cerFileReadError:
	case cerFileWriteError:
		cerr << endl << "File access error"  << endl;
		nReturn = 22;
		break;
	case cerServerBusy:
		cerr << endl << "PL Function Busy" << endl;
		nReturn = 46;
		break;
	case cerBackupInProgress:
			cerr<<endl<<"Command not executed, AP backup in progress"<<endl <<endl;
			nReturn = 114;
			break;
	case cerAuthFailure:
		cerr<<endl<<"Reset Password Not Authorized"<<endl <<endl;
		nReturn = 45;
		break;
	default:
		cerr << endl << "Failed to change Audit Log attributes "  << endl;
		nReturn = 29;
		break;

	}
	return(nReturn);
}
//--------------------------------------------------------------------------------------------------


int acs_alog_cmdUtil::CheckALOGSysFile( bool check_size_zero , bool if_writable, bool if_print_error) // PL
{
	string szFileName ;
	getAlogDataDiskPath(szFileName);

	szFileName += "/ACS_ALOG/SYS";

	if(if_writable)
	{
		//Do nothing
	}
	ACE_stat statBuff;
	//ACE_INT16 result = -1;
	if( ACE_OS::stat(szFileName.c_str(), &statBuff)  != 0 )
	{
		cerr << endl << "File access error" << endl ;
		return 22;

	}
	//Check if we write permission
	int retcode = 0;

	if(check_size_zero)
	{
		if(statBuff.st_size != 0)
		{
			if(if_print_error)
			cerr << "Password already defined"  << endl;
			retcode = 41;
		}
	}
	else
	{
		if(statBuff.st_size == 0)
		{
			if(if_print_error)
			cerr << endl << "Password not defined"  << endl;
			retcode = 42;
		}
	}
	return retcode;
}

//--------------------------------------------------------------------------------------------------
int acs_alog_cmdUtil::Authenticate(char * cPw)
{
	int nReturn = 0;
	acs_alog_execute CmdExecute;
	enumCmdExecuteResult ExecResult = cerSuccess;
	ExecResult = CmdExecute.Execute(ctCheckPw, not_defined,  cPw);
	nReturn = HandleReturnCode(ExecResult);
	return nReturn;
}
//--------------------------------------------------------------------------------------------------

bool  acs_alog_cmdUtil::getAlogDataDiskPath(string& szFolderName)
{
	std::string szFileName = "/usr/share/pso/storage-paths/config";
		string line;
		string path;
		ifstream file(szFileName.c_str());
		if (file.is_open()) {
			while (file.good()) {
				getline(file, line);
				szFolderName += line;
			}
			file.close();

		}

		else {
			return false;
		}

		return true;
}
//--------------------------------------------------------------------------------------------------


int acs_alog_cmdUtil::CheckAlogMain()
{

	char cPass[101];
	strcpy(cPass,"DEFAULT");
	int rCode = acs_alog_cmdUtil::AuthenticateNoHandle(cPass);
	if(rCode == cerServerNotResponding){
		cerr << endl << "Unable to connect to server"  << endl;
		return 117;
	}
	return 0; // OK

}
//--------------------------------------------------------------------------------------------------

int acs_alog_cmdUtil::AuthenticateNoHandle(char * cPw)
{

//	int nReturn = 0;
	acs_alog_execute CmdExecute;
	enumCmdExecuteResult ExecResult = cerSuccess;
	ExecResult = CmdExecute.Execute(ctCheckPw, not_defined,  cPw);
	return ExecResult;
}
//--------------------------------------------------------------------------------------------------

int acs_alog_cmdUtil::PasswordHandling(char *psw)
{
	int rCode = acs_alog_cmdUtil::CheckALOGSysFile( false , false,true);

	if(rCode != 0){
		return rCode;
	}

	// Command not allowed if alog_main() server is not active
	// -------------------------------------------------------
	rCode = acs_alog_cmdUtil::CheckAlogMain();

	if(rCode != 0 ) return rCode;

	cout << "\nPassword\03: " << flush;

	char cPass[101];

	acs_alog_cmdUtil::getPassword(cPass,101);

	rCode = acs_alog_cmdUtil::Authenticate(cPass);

	strcpy(psw,cPass);

	return rCode;
}

//--------------------------------------------------------------------------------------------------

int acs_alog_cmdUtil::PasswordHandling(void)
{
	int rCode = acs_alog_cmdUtil::CheckALOGSysFile( false , false,true);

	if(rCode != 0){
		return rCode;
	}

	// Command not allowed if alog_main() server is not active
	// -------------------------------------------------------
	rCode = acs_alog_cmdUtil::CheckAlogMain();

	if(rCode != 0 ) return rCode;

	cout << "\nPassword\03: " << flush;

	char cPass[101];

	acs_alog_cmdUtil::getPassword(cPass,101);

	rCode = acs_alog_cmdUtil::Authenticate(cPass);

	return rCode;
}
//--------------------------------------------------------------------------------------------------

std::string acs_alog_cmdUtil::getPasswordInFile()
{
	std::string szFileName;
		getAlogDataDiskPath(szFileName);
		szFileName += "/ACS_ALOG/SYS";


		ifstream is;
				int length;
				char *buffer;
				try {

					is.open (szFileName.c_str(), ios::binary );

					// get length of file:
					is.seekg (0, ios::end);
					length = is.tellg();

					is.seekg (0, ios::beg);

							// allocate memory:
							buffer = new char [length];

							// read data as a block:
							is.read (buffer,length);
							is.close();
				}

				catch(...)
						{
					delete[] buffer;
						}


				char key[6];
					strcpy(key, "alogp");
					char* passwordInFile;

					passwordInFile = new char [length+1];
					ACS_APGCC_CommonLib temp;

					temp.EncryptDecryptData(buffer , " " ,passwordInFile, key, ACS_APGCC_DIR_DECRYPT, length, 5 );

					// Compare old password
					passwordInFile[length] = 0;
					std::string password(passwordInFile);
					return password;

}

int acs_alog_cmdUtil::getUserName(std::string& user)
{
	uid_t userId;
	userId= getuid();
	struct passwd  *ptr;
	ptr = getpwuid(userId);
	std::string mystr(ptr->pw_name);
	user=mystr;
	return 0;

}


bool acs_alog_cmdUtil::runCommand(const string command, string& output) {
   FILE *fp;
   char readLine[10000];
   output = "";

   /* Open the command for reading. */
   fp = popen(command.c_str(), "r");
   if (fp == 0) {
      return false;
   }

   /* Read the output a line at a time and store it. */
   while (fgets(readLine, sizeof(readLine) - 1, fp) != 0) {

	   size_t newbuflen = strlen(readLine);

	   if ( (readLine[newbuflen - 1] == '\r') || (readLine[newbuflen - 1] == '\n') ) {
		   readLine[newbuflen - 1] = '\0';
	   }

	   if ( (readLine[newbuflen - 2] == '\r') || (readLine[newbuflen - 2] == '\n') ) {
			   readLine[newbuflen - 2] = '\0';
	   }

      output += readLine;
   }

   /* close */
   pclose(fp);

   return true;
}


int acs_alog_cmdUtil::checkRoles()
{
//Check for the role of User
	std::string userName;
	int nReturn;
	nReturn = acs_alog_cmdUtil::getUserName(userName);
	string command = "/opt/ap/mcs/bin/acs_get_roles " + userName;
	std::list<string>  listOfRoles;
	std::string output;
	if (acs_alog_cmdUtil::runCommand(command, output)) {
		// parse the result and add each role to the list
		size_t start = 0;
		size_t end = 0;
		string tempStr = "";
		while ((end = output.find(",", start)) != string::npos) {
			tempStr = output.substr(start, end - start);
			listOfRoles.push_back(tempStr);
			start = end + 1;
		}
		tempStr = output.substr(start);
		listOfRoles.push_back(tempStr);
		listOfRoles.sort();
	}
	std::list<string>::iterator it;

	if (!listOfRoles.empty()) {

		for (it = listOfRoles.begin(); it != listOfRoles.end(); ++it) {
			string mmlRole = *it;
			if(strcmp(mmlRole.c_str(),ALOG_USER_ROLE)==0)
			{
				return 0;
			}

		}

	}
	else
	{
		return 45;
	}

	return 45;
}

bool acs_alog_cmdUtil::isNumeric(const char *s){

	int  l = strlen(s);

	for (int i = 0; i<l; i++)
	{
		if (s[i] < '0'  ||  s[i] > '9')  return false;
	}
	return true;
}


// TR HX49170 - Hide 7z password from 'ps' by providing the password interactively to 7z child process

bool acs_alog_cmdUtil::execute7zCmd(string command, string plogPassword, int &subprocStatus)
{
	pid_t pid = 0;
	int outpipefd[2];
	int pipeRes = pipe(outpipefd);
	if(pipeRes == -1)
		return false;

	pid = fork();

	if (pid == 0)
	{
		// Child
		pid_t sid = ACE_OS::setsid();
		if(sid == -1)
		{
			close(outpipefd[0]);
			close(outpipefd[1]);
			exit(-1);
		}

		close(outpipefd[1]);
		if(dup2(outpipefd[0], STDIN_FILENO) == -1)
		{
			close(outpipefd[0]);
			exit(-1);
		}
		close(outpipefd[0]);
		execl("/bin/sh", "sh", "-c", command.c_str(), (char*)NULL);
		exit(1);			//execl() will never return in successful case
	}
	else if (pid == -1)
	{
		close(outpipefd[0]);
		close(outpipefd[1]);
		return false;
	}
	close(outpipefd[0]);
	write(outpipefd[1], plogPassword.c_str(), plogPassword.length());
	close(outpipefd[1]);
	ACE_OS::waitpid(pid, &subprocStatus, 0);

	return true;
}

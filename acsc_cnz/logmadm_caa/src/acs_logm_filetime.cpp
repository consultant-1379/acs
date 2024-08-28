#include<acs_logm_filetime.h>

acs_logm_filetime::acs_logm_filetime(std::string aFilePath)
:acs_logm_time(),
 theFilePath(aFilePath),
 theIsDirectory(false)
{
	/* frequently called function - Do not put log or traces*/
    struct stat sb;
    if (stat(theFilePath.c_str(), &sb) != -1)
    {
     if((sb.st_mode & S_IFMT) == S_IFDIR)
     {
       theIsDirectory = true;
     }
     theTime = sb.st_mtime;
     theTimeTM = localtime(&theTime);
    }
}

acs_logm_filetime::~acs_logm_filetime()
{
	/* frequently called function - Do not put log or traces*/
}

bool acs_logm_filetime::isFileObjectExists(std::string aFilePath)
{
	/* frequently called function - Do not put log or traces*/
	struct stat sb;
	if (stat(aFilePath.c_str(), &sb) != -1)
	{
		//INFO("Entering  %s","File exists acs_logm_filetime::isFileObjectExists() ");
		return true;
	}
	return false;
}

bool acs_logm_filetime::isFileOlderThanGivenDays(int aNumberDays)
{

  bool myResult = false;
  double olderSeconds = SECONDS_IN_DAY * aNumberDays;
  double myElapsedTimeSeconds = elapsedTimeWithCurrentTime();
  if(olderSeconds < myElapsedTimeSeconds )
  {
    myResult = true;
  }

  return myResult;
}
bool acs_logm_filetime::isFileOlderThanGivenHours(int aNumberHours)
{

  bool myResult = false;
  double olderSeconds = SECONDS_IN_HOUR * aNumberHours;
  double myElapsedTimeSeconds = elapsedTimeWithCurrentTime();
  if(olderSeconds < myElapsedTimeSeconds )
  {
    myResult = true;
  }

  return myResult;

}
bool acs_logm_filetime::isFileOlderThanGivenMinutes(int aNumberMinutes)
{
  bool myResult = false;
  double olderSeconds = SECONDS_IN_MINUTE * aNumberMinutes;
  double myElapsedTimeSeconds = elapsedTimeWithCurrentTime();
  if(olderSeconds < myElapsedTimeSeconds )
  {
    myResult = true;
  }
  return myResult;

}
bool acs_logm_filetime::isDirectory()
{
  return theIsDirectory;
}



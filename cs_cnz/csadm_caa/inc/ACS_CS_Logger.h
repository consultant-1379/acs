

//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************


#ifndef ACS_CS_Logger_h
#define ACS_CS_Logger_h 1



#include <fstream>
#include <string>




class ACS_CS_ReaderWriterLock;




//	This class is used to log events to a application
//	specific log file. There are also functions to retrieve
//	the path for the AP data disk and system disk. The class
//	is a singleton with static functions.



class ACS_CS_Logger 
{

  public:




    typedef enum {Info = 0, Warning = 1, Error = 2} LogType;



      void log (std::string message, ACS_CS_Logger::LogType logType = ACS_CS_Logger::Info);

      static std::string getDataDir ();

      static std::string getSystemDir ();

      static  ACS_CS_Logger* createInstance ();

  public:
    // Additional Public Declarations

  protected:
      ACS_CS_Logger();

      virtual ~ACS_CS_Logger();

    // Additional Protected Declarations

  private:
      ACS_CS_Logger(const ACS_CS_Logger &right);

      ACS_CS_Logger & operator=(const ACS_CS_Logger &right);


      void createDirectory (std::string pathName);

      void printToFile (std::string message);

      bool readRegistry ();

      bool getPath ();

      //	This function creates backup files for the log file once
      //	its size exceeds MAX_LOG_FILE_SIZE. The maximal number
      //	of backup files created is defined by MAX_NUM_OF_BACKUP_
      //	FILES. For example, if the max number is 99, the backup
      //	files would be named as log_file_name.01, log_file_
      //	name.02, ..., log_file_name.99. This function first
      //	deletes the oldest backup file log_file_name.99, then
      //	moves each backup file to the next one, i.e., log_file_
      //	name.98 -> log_file_name.99, ..., log_file_name.01 -> log
      //	_file_name.02. Finally, it moves the current log file to
      //	log_file_name.01 and creates a new empty log file.
      bool switchFileOnSize ();

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

      static ACS_CS_Logger* _instance;

       std::fstream logFile;

       std::string logFileName;

       std::string dataPath;

       std::string systemPath;

       std::string logPath;

      //	This lock is used to synchronize the access to _instance.
      static  ACS_CS_ReaderWriterLock instanceLock;

      //	This lock is used to synchronize the access to logFile.
      static  ACS_CS_ReaderWriterLock logFileLock;

    // Additional Implementation Declarations

};


// Class ACS_CS_Logger 



#endif

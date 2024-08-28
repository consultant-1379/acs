//	Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_HostFile_h
#define ACS_CS_HostFile_h 1

#include <string>
#include "ace/Singleton.h"
#include "ace/Recursive_Thread_Mutex.h"


class ACS_CS_HostFile {
    public:
        virtual ~ACS_CS_HostFile();

        static uint32_t getIPAddress(std::string hostName);

        friend class ACE_Singleton<ACS_CS_HostFile, ACE_Recursive_Thread_Mutex> ;

    protected:
        ACS_CS_HostFile();

    private:
        ACS_CS_HostFile(const ACS_CS_HostFile &right);

        ACS_CS_HostFile & operator=(const ACS_CS_HostFile &right);

        static ACS_CS_HostFile * getInstance();

        uint32_t readFromHostFile(std::string hostName);

        std::string getHostFilePath();

    private:
        std::string hostFilePath;
        bool isTestEnvironment;

        static uint32_t ipA1;
        static uint32_t ipA2;
        static uint32_t ipB1;
        static uint32_t ipB2;
        static uint32_t scb_rp_for_test;

        typedef ACE_Singleton<ACS_CS_HostFile, ACE_Recursive_Thread_Mutex> instance;
};


#endif

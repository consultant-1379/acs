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

#ifndef ACS_CS_Registry_h
#define ACS_CS_Registry_h 1

#include <string>

namespace ACS_CS_Registry_NS
{
    static const bool ACS_CS_DEFAULT_TEST_ENVIRONMENT   = false;
    static const bool ACS_CS_DEFAULT_MULTIPLE_CP_SYSTEM = false;
    static const unsigned short ACS_CS_DEFAULT_FRONTAP  = 2001;
    static const unsigned short ACS_CS_DEFAULT_APT_TYPE = 0;
}


class ACS_CS_Registry // NOTE: This methods are static. Synchronization in multi-thread environment is required.
{
  // Internal public class
  public:
    class TImmAccess
    {
      // Costructor/distructor
      public:
        TImmAccess(){};
        virtual ~TImmAccess(){};
      // Access methods
      public:
        // Get int attribute from IMM
        bool getImmAttributeInt(const std::string &strDn, const std::string &strAttr, int *iVal);
        bool getImmAttributeString(const std::string &strDn, const std::string &strAttr, std::string *strVal);
    };
  
public:
    //static bool getImmAttributeInt(const std::string &strDn, const std::string &strAttr, int *iVal);
    static unsigned short getFrontAPG();
    static bool isMultipleCPSystem(); //now uses a PHA parameter instead of a registry value
    static bool isTestEnvironment();
    static bool isDebug();
    static void setDebug(bool debug);
    static std::string getAPTType ();
    static int getApzSystem();
    static bool IsCba();
    static bool getNodeArchitecture(int &architecture);
    static bool getApgNumber(int &apgNumber);
    static bool getBGCIVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2]);
    static bool getSOLVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2]);
    static bool getDmxcAddress(uint32_t &addressBgciA, uint32_t &addressBgciB);
    static bool getApgBgciAddress(uint16_t apgNr, uint16_t side, uint32_t &ipA, uint32_t &ipB);
    static bool getApgHwVersion(int &hwType);
    static int getAPZType();
		static int getDHCPOnOffFlag();

    static int hexMACtoDecMAC(const char *input, char *out = 0);
	static void loadDefaultMauType ();
	static int getDefaultMauType ();

  private:
      ACS_CS_Registry();

      //Cache for systemType parameter stored in "axeFunctionsId=1"
      static int s_isBladeCluster;
      static uint32_t s_dmxcAddress[2];
      static uint32_t s_apgBgciAddress[2][2][2];

      static bool isDebugEnabled;
      static int s_hwVersion;
      static int s_nodeArchitecture;
      static int s_apgNumber;
      static int s_aptType;
      static int s_apzType;
	  	static int mau_type;
			static int s_enableDHCP;
};

#endif



#ifndef ACS_CS_API_Common_R1_h
#define ACS_CS_API_Common_R1_h 1

#include <string>
#include <stdint.h>


class ACS_CS_API_Name_Implementation;
class ACS_CS_API_NameList_Implementation;
class ACS_CS_API_IdList_Implementation;
class ACS_CS_API_MacList_Implementation;

class ACS_CS_API_Name_R1;

typedef uint16_t CPID;
typedef uint16_t APID;
typedef uint16_t SysID;
typedef uint16_t BoardID;

namespace ACS_CS_API_NS
{
    // CS API Result Codes
    enum CS_API_Result
    {
        Result_Success =            0,
        Result_NoEntry =            3,
        Result_NoValue =            4,
        Result_NoAccess =           12,
        Result_Failure =            15
    };

    // APZ type identifiers
    enum CS_API_APZ_Type
    {
        APZ21255 =     1,
        APZ21401 =     2
    };


    enum MauType
    {
    	MAU_UNDEFINED 	=	0,
    	MAUB 			= 	1,
    	MAUS 			=	2
    };

   // See the Cluster Handler IWD 1/155 19-ANZ 250 03
   // for detailed descriptions of values that can be
   // assigned to CpState, AppliationId, ApzSubstate, 
   // State Transition, and AptSubstate.

   // Quorum state
   typedef int CpState;
   // Application identity
   typedef int ApplicationId;
   // APZ substate
   typedef int ApzSubstate;
   // State transition
   typedef int StateTransition;
   // APT substate
   typedef int AptSubstate;
   // Blocking Information
   typedef uint16_t BlockingInfo;
   // CP Capacity
   typedef uint32_t CpCapacity;


   static const uint16_t CPID_Unspecified = 255;

   static const uint16_t BoardID_Unspecified = 65535;
}







class ACS_CS_API_NameList_R1
{

  public:
      ACS_CS_API_NameList_R1();

      virtual ~ACS_CS_API_NameList_R1();

      ACS_CS_API_Name_R1 operator[](size_t index) const;


      size_t size () const;

      void setValue (const ACS_CS_API_Name_R1 &value, size_t index);

      void setSize (size_t newSize);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_API_NameList_R1(const ACS_CS_API_NameList_R1 &right);

      ACS_CS_API_NameList_R1 & operator=(const ACS_CS_API_NameList_R1 &right);

    // Additional Private Declarations

    // Data Members for Class Attributes

       ACS_CS_API_NameList_Implementation *implementation;

    // Data Members for Associations

    // Additional Implementation Declarations

};






class ACS_CS_API_Name_R1
{

  public:
      ACS_CS_API_Name_R1();

      ACS_CS_API_Name_R1(const ACS_CS_API_Name_R1 &right);

      ACS_CS_API_Name_R1 (const char *name);

      virtual ~ACS_CS_API_Name_R1();

      ACS_CS_API_Name_R1 & operator=(const ACS_CS_API_Name_R1 &right);


      ACS_CS_API_NS::CS_API_Result getName (char *name, size_t &nameLength) const;

      void setName (const char *newName);

      size_t length () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
    // Additional Private Declarations

    // Data Members for Class Attributes

       ACS_CS_API_Name_Implementation *implementation;

    // Data Members for Associations

    // Additional Implementation Declarations

};






class ACS_CS_API_IdList_R1
{

  public:
      ACS_CS_API_IdList_R1();

      ACS_CS_API_IdList_R1(const ACS_CS_API_IdList_R1 &right);

      virtual ~ACS_CS_API_IdList_R1();

      ACS_CS_API_IdList_R1 & operator=(const ACS_CS_API_IdList_R1 &right);

      uint16_t operator[](const size_t index) const;


      size_t size () const;

      void setValue (uint16_t value, size_t index);

      void setSize (size_t newSize);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
    // Additional Private Declarations

    // Data Members for Class Attributes

       ACS_CS_API_IdList_Implementation *implementation;

    // Data Members for Associations

    // Additional Implementation Declarations

};


class ACS_CS_API_MacList_R1
{

  public:
	ACS_CS_API_MacList_R1();

	ACS_CS_API_MacList_R1(const ACS_CS_API_MacList_R1 &right);

      virtual ~ACS_CS_API_MacList_R1();

      ACS_CS_API_MacList_R1 & operator=(const ACS_CS_API_MacList_R1 &right);

      std::string operator[](const size_t index) const;


      size_t size () const;

      void setValue (std::string value, size_t index);

      void setSize (size_t newSize);

    // Additional Public Declarations


  private:

       ACS_CS_API_MacList_Implementation *implementation;

};






class ACS_CS_API_BoardSearch_R1
{

  public:
      virtual ~ACS_CS_API_BoardSearch_R1();


      virtual void setMagazine (uint32_t magazine) = 0;

      virtual void setSlot (uint16_t slot) = 0;

      virtual void setSysType (uint16_t sysType) = 0;

      virtual void setSysNo (uint16_t sysNo) = 0;

      virtual void setFBN (uint16_t fbn) = 0;

      virtual void setSide (uint16_t side) = 0;

      virtual void setSeqNo (uint16_t seqNo) = 0;

      virtual void setIPEthA (uint32_t address) = 0;

      virtual void setIPEthB (uint32_t address) = 0;

      virtual void setAliasEthA (uint32_t address) = 0;

      virtual void setAliasEthB (uint32_t address) = 0;

      virtual void setAliasNetmaskEthA (uint32_t mask) = 0;

      virtual void setAliasNetmaskEthB (uint32_t mask) = 0;

      virtual void setDhcpMethod (uint16_t method) = 0;

      virtual void setSysId (uint16_t sysId) = 0;

      virtual void reset () = 0;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
    // Additional Private Declarations

    // Additional Implementation Declarations

};


class ACS_CS_API_BoardSearch_R2: public ACS_CS_API_BoardSearch_R1
{
public:
	virtual void setSwVerType (uint16_t swVerType) = 0;

	virtual void setUuid (const std::string & uuid) = 0;
};


class ACS_CS_API_NWT_BoardSearch_R1
{

  public:
      virtual ~ACS_CS_API_NWT_BoardSearch_R1();

      virtual void setNtwName (std::string ntwName) = 0;

      virtual void setAdminState (std::string adminState) = 0;

      virtual void setExtNetwID (std::string extNtwId) = 0;

      virtual void setIntNetwID (std::string intNtwId) = 0;

      virtual void setMAC (std::string mac) = 0;

      virtual void setInterfaceID (std::string interfaceId) = 0;

      virtual void setBoardID (BoardID boardId) = 0;

      virtual void reset () = 0;

};


//	Cluster operation mode values and static methods to
//	manipulate them.



class ACS_CS_API_ClusterOpMode
{

  public:

    //	Values for ClusterOperationMode that can be supplied to
    //	CSAPI clients.  If updating this enum after the CSAPI
    //	interface has been published, you must not remove or
    //	change the meaning of existing enumeration items.  Only
    //	new values can be added.



    typedef enum { Normal = 0, SwitchingToNormal = 1, Expert = 2, SwitchingToExpert = 3 } Value;


  public:
    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
    // Additional Private Declarations

    // Additional Implementation Declarations

};



//	Settable cluster operation mode values and static
//	methods to manipulate them.



class ACS_CS_API_RequestedClusterOpMode 
{

  public:

    //	Values for CSAPI clients can request for ClusterOperation
    //	Mode.  If updating this enum after the CSAPI interface
    //	has been published, you must not remove or change the
    //	meaning of existing enumeration items.  Only new values
    //	can be added.



    typedef enum { Normal = 0, Expert = 1 } RequestedValue;


  public:
    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
    // Additional Private Declarations

    // Additional Implementation Declarations

};



//	Operation type of for table change notifications.



class ACS_CS_API_TableChangeOperation 
{

  public:




    typedef enum { Unspecified = 0, Add = 1, Delete = 2, Change = 3 } OpType;


  public:
    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
    // Additional Private Declarations

    // Additional Implementation Declarations

};



//	Enum values associated with the OmProfile Phase changes.



class ACS_CS_API_OmProfilePhase
{

  public:

    //	Values for the Profile Phases.



    typedef enum { Idle = -1, Validate = 0, ApNotify = 1, CpNotify = 2, Commit = 3 } PhaseValue;


  public:
    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
    // Additional Private Declarations

    // Additional Implementation Declarations

};

class ACS_CS_API_CommonBasedArchitecture
{

  public:
        typedef enum {SCB = 0, SCX = 1, DMX = 2, VIRTUALIZED = 3, SMX = 4} ArchitectureValue;
        typedef enum {AP1 = 1, AP2 = 2} ApgNumber;

  public:
    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_API_NameList_R1 

// Class ACS_CS_API_Name_R1 

// Class ACS_CS_API_IdList_R1 

// Class ACS_CS_API_BoardSearch_R1 

// Class ACS_CS_API_ClusterOpMode 

// Class ACS_CS_API_RequestedClusterOpMode 

// Class ACS_CS_API_TableChangeOperation 

// Class ACS_CS_API_OmProfilePhase 

// Class ACS_CS_API_ClusterOpMode 

// Additional Declarations

// Class ACS_CS_API_RequestedClusterOpMode 

// Additional Declarations

// Class ACS_CS_API_TableChangeOperation 

// Additional Declarations

// Class ACS_CS_API_OmProfilePhase 

// Additional Declarations



#endif

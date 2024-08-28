//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%464FFAAA01C3.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%464FFAAA01C3.cm

//## begin module%464FFAAA01C3.cp preserve=no
//	© Copyright Ericsson AB 2007. All rights reserved.
//## end module%464FFAAA01C3.cp

//## Module: ACS_CS_TableSignature%464FFAAA01C3; Package specification
//## Subsystem: ACS_CS::cssrv_caa::src%459B6F040389
//## Source file: Z:\ntacs\cs_cnz\cssrv_caa\src\ACS_CS_TableSignature.h

#ifndef ACS_CS_TableSignature_h
#define ACS_CS_TableSignature_h 1

//## begin module%464FFAAA01C3.additionalIncludes preserve=no
//## end module%464FFAAA01C3.additionalIncludes

//## begin module%464FFAAA01C3.includes preserve=yes

#include "ACS_CS_Protocol.h"

#include <string>

//## end module%464FFAAA01C3.includes

//## begin module%464FFAAA01C3.declarations preserve=no
//## end module%464FFAAA01C3.declarations

//## begin module%464FFAAA01C3.additionalDeclarations preserve=yes

class ACS_CS_ReaderWriterLock;

//## end module%464FFAAA01C3.additionalDeclarations


//## begin ACS_CS_TableSignature%464FF9F3034A.preface preserve=yes
//## end ACS_CS_TableSignature%464FF9F3034A.preface

//## Class: ACS_CS_TableSignature%464FF9F3034A
//## Category: cssrv%458BCEF400CD
//## Subsystem: ACS_CS::cssrv_caa::src%459B6F040389
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class ACS_CS_TableSignature 
{
  //## begin ACS_CS_TableSignature%464FF9F3034A.initialDeclarations preserve=yes

   struct CS_SIGNATURE_BLOCK			// Struct to hold file signatures
	{
		unsigned long hwcSignature;
		unsigned long cpIdSignature;
		unsigned long cpGroupNameSignature;
      unsigned long cpGroupMemberSignature;
      unsigned long fdProviderSignature;
      unsigned long fdUserSignature;
      unsigned long neSignature;
      unsigned long unusedSignature1;
      unsigned long unusedSignature2;
      unsigned long unusedSignature3;
	};

   struct CS_SIGNATURE_UPDATE
   {
      bool hwcSignature;
		bool cpIdSignature;
		bool cpGroupNameSignature;
      bool cpGroupMemberSignature;
      bool fdProviderSignature;
      bool fdUserSignature;
      bool neSignature;
      bool unusedSignature1;
      bool unusedSignature2;
      bool unusedSignature3;
   };

  //## end ACS_CS_TableSignature%464FF9F3034A.initialDeclarations

  public:
    //## Destructor (generated)
      virtual ~ACS_CS_TableSignature();


    //## Other Operations (specified)
      //## Operation: getInstance%464FFA6900BA
      static ACS_CS_TableSignature * getInstance ();

      //## Operation: sign%464FFD3402FC
      void sign (unsigned long signature, std::string signedFile, ACS_CS_Protocol::CS_Service_Type state);

      //## Operation: getSignature%464FFD4B02CD
      unsigned long getSignature (std::string signedFile);

    // Additional Public Declarations
      //## begin ACS_CS_TableSignature%464FF9F3034A.public preserve=yes
      //## end ACS_CS_TableSignature%464FF9F3034A.public

  protected:
    //## Constructors (generated)
      ACS_CS_TableSignature();

    // Additional Protected Declarations
      //## begin ACS_CS_TableSignature%464FF9F3034A.protected preserve=yes
      //## end ACS_CS_TableSignature%464FF9F3034A.protected

  private:
    //## Constructors (generated)
      ACS_CS_TableSignature(const ACS_CS_TableSignature &right);

    //## Assignment Operation (generated)
      ACS_CS_TableSignature & operator=(const ACS_CS_TableSignature &right);


    //## Other Operations (specified)
      //## Operation: readSignature%4650009001C3
      void readSignature (std::string fileName, CS_SIGNATURE_BLOCK &signatureBlock);

      //## Operation: writeSignature%4650009B03B7
      void writeSignature (std::string fileName, CS_SIGNATURE_BLOCK &signatureBlock, CS_SIGNATURE_UPDATE &signatureUpdate);

    // Additional Private Declarations
      //## begin ACS_CS_TableSignature%464FF9F3034A.private preserve=yes
      //## end ACS_CS_TableSignature%464FF9F3034A.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: instance%464FFA4B0156
      //## begin ACS_CS_TableSignature::instance%464FFA4B0156.attr preserve=no  private: static  ACS_CS_TableSignature * {U} 0
      static  ACS_CS_TableSignature *instance;
      //## end ACS_CS_TableSignature::instance%464FFA4B0156.attr

      //## Attribute: lock%464FFBB300C9
      //## begin ACS_CS_TableSignature::lock%464FFBB300C9.attr preserve=no  private:  ACS_CS_ReaderWriterLock * {U} 0
       ACS_CS_ReaderWriterLock *lock;
      //## end ACS_CS_TableSignature::lock%464FFBB300C9.attr

      //## Attribute: currentSignatures%464FFF17034A
      //## begin ACS_CS_TableSignature::currentSignatures%464FFF17034A.attr preserve=no  private:  CS_SIGNATURE_BLOCK {U} 
       CS_SIGNATURE_BLOCK currentSignatures;
      //## end ACS_CS_TableSignature::currentSignatures%464FFF17034A.attr

      //## Attribute: currentUpdates%465139EC0346
      //## begin ACS_CS_TableSignature::currentUpdates%465139EC0346.attr preserve=no  private:  CS_SIGNATURE_UPDATE {U} 
       CS_SIGNATURE_UPDATE currentUpdates;
      //## end ACS_CS_TableSignature::currentUpdates%465139EC0346.attr

      //## Attribute: fileNameMaster%465001450202
      //## begin ACS_CS_TableSignature::fileNameMaster%465001450202.attr preserve=no  private:  std::string {U} 
       std::string fileNameMaster;
      //## end ACS_CS_TableSignature::fileNameMaster%465001450202.attr

      //## Attribute: fileNameSlave%465002130231
      //## begin ACS_CS_TableSignature::fileNameSlave%465002130231.attr preserve=no  private:  std::string {U} 
       std::string fileNameSlave;
      //## end ACS_CS_TableSignature::fileNameSlave%465002130231.attr

    // Additional Implementation Declarations
      //## begin ACS_CS_TableSignature%464FF9F3034A.implementation preserve=yes
      //## end ACS_CS_TableSignature%464FF9F3034A.implementation

};

//## begin ACS_CS_TableSignature%464FF9F3034A.postscript preserve=yes
//## end ACS_CS_TableSignature%464FF9F3034A.postscript

// Class ACS_CS_TableSignature 

//## begin module%464FFAAA01C3.epilog preserve=yes
//## end module%464FFAAA01C3.epilog


#endif

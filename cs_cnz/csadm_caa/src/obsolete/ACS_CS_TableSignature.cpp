//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%464FFAAC0212.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%464FFAAC0212.cm

//## begin module%464FFAAC0212.cp preserve=no
//	© Copyright Ericsson AB 2007. All rights reserved.
//## end module%464FFAAC0212.cp

//## Module: ACS_CS_TableSignature%464FFAAC0212; Package body
//## Subsystem: ACS_CS::cssrv_caa::src%459B6F040389
//## Source file: Z:\ntacs\cs_cnz\cssrv_caa\src\ACS_CS_TableSignature.cpp

//## begin module%464FFAAC0212.additionalIncludes preserve=no
//## end module%464FFAAC0212.additionalIncludes

//## begin module%464FFAAC0212.includes preserve=yes

#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_Logger.h"
#include "ACS_CS_EventReporter.h"
#include <fstream>

//## end module%464FFAAC0212.includes

// ACS_CS_TableSignature
#include "ACS_CS_TableSignature.h"
//## begin module%464FFAAC0212.declarations preserve=no
//## end module%464FFAAC0212.declarations

//## begin module%464FFAAC0212.additionalDeclarations preserve=yes

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_TableSignature_TRACE);

using namespace ACS_CS_Protocol;
using namespace ACS_CS_NS;
using std::string;
using std::fstream;
using std::ios_base;

//## end module%464FFAAC0212.additionalDeclarations


// Class ACS_CS_TableSignature 

//## begin ACS_CS_TableSignature::instance%464FFA4B0156.attr preserve=no  private: static  ACS_CS_TableSignature * {U} 0
 ACS_CS_TableSignature *ACS_CS_TableSignature::instance = 0;
//## end ACS_CS_TableSignature::instance%464FFA4B0156.attr

ACS_CS_TableSignature::ACS_CS_TableSignature()
  //## begin ACS_CS_TableSignature::ACS_CS_TableSignature%464FF9F3034A_const.hasinit preserve=no
      : lock(0)
  //## end ACS_CS_TableSignature::ACS_CS_TableSignature%464FF9F3034A_const.hasinit
  //## begin ACS_CS_TableSignature::ACS_CS_TableSignature%464FF9F3034A_const.initialization preserve=yes
  //## end ACS_CS_TableSignature::ACS_CS_TableSignature%464FF9F3034A_const.initialization
{
  //## begin ACS_CS_TableSignature::ACS_CS_TableSignature%464FF9F3034A_const.body preserve=yes

   lock = new ACS_CS_ReaderWriterLock();
   memset(&currentSignatures, 0, sizeof(currentSignatures));
   memset(&currentUpdates, 0, sizeof(currentUpdates));

   ACS_CS_Protocol::CS_Side_Identifier side = CS_ProtocolChecker::getNode();
   string dataDiskPath = ACS_CS_Logger::getDataDir();


   if (side == ACS_CS_Protocol::Side_A)
   {
      fileNameMaster.append(dataDiskPath);
      fileNameMaster.append("\\");
      fileNameMaster.append(FILE_NAME_NODE_A_SIG);
      fileNameMaster.append(ACS_CS_NS::FILE_TYPE_STD);

      fileNameSlave.append(dataDiskPath);
      fileNameSlave.append("\\");
      fileNameSlave.append(FILE_NAME_NODE_B_SIG);
      fileNameSlave.append(ACS_CS_NS::FILE_TYPE_STD);
   }
   else if (side == ACS_CS_Protocol::Side_B)
   {
      fileNameMaster.append(dataDiskPath);
      fileNameMaster.append("\\");
      fileNameMaster.append(FILE_NAME_NODE_B_SIG);
      fileNameMaster.append(ACS_CS_NS::FILE_TYPE_STD);

      fileNameSlave.append(dataDiskPath);
      fileNameSlave.append("\\");
      fileNameSlave.append(FILE_NAME_NODE_A_SIG);
      fileNameSlave.append(ACS_CS_NS::FILE_TYPE_STD);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_TableSignature_TRACE,
			"(%t) ACS_CS_TableSignature::ACS_CS_TableSignature()\n"
			"Error: Unknown side.\n"));
   }

  //## end ACS_CS_TableSignature::ACS_CS_TableSignature%464FF9F3034A_const.body
}


ACS_CS_TableSignature::~ACS_CS_TableSignature()
{
  //## begin ACS_CS_TableSignature::~ACS_CS_TableSignature%464FF9F3034A_dest.body preserve=yes

   if (lock)
      delete lock;

  //## end ACS_CS_TableSignature::~ACS_CS_TableSignature%464FF9F3034A_dest.body
}



//## Other Operations (implementation)
 ACS_CS_TableSignature * ACS_CS_TableSignature::getInstance ()
{
  //## begin ACS_CS_TableSignature::getInstance%464FFA6900BA.body preserve=yes

   if (ACS_CS_TableSignature::instance == 0)
      ACS_CS_TableSignature::instance = new ACS_CS_TableSignature();

   return ACS_CS_TableSignature::instance;

  //## end ACS_CS_TableSignature::getInstance%464FFA6900BA.body
}

 void ACS_CS_TableSignature::sign (unsigned long signature, std::string signedFile, ACS_CS_Protocol::CS_Service_Type state)
{
  //## begin ACS_CS_TableSignature::sign%464FFD3402FC.body preserve=yes

   ACS_CS_TRACE((ACS_CS_TableSignature_TRACE,
			"(%t) ACS_CS_TableSignature::sign()\n"
			"Timestamp = %d, file name = %s, state = %d\n",
         signature, signedFile.c_str(), state));

   ACS_CS_TRACE((ACS_CS_TableSignature_TRACE,
				"(%t) ACS_CS_TableSignature::sign()\n"
				"Calling lock->start_writing()\n"));

 if (lock) lock->start_writing();

   if (signedFile == ACS_CS_NS::FILE_NAME_HWC_TABLE)
   {
      currentSignatures.hwcSignature = signature;
      currentUpdates.hwcSignature = true;
   }
   else if (signedFile == ACS_CS_NS::FILE_NAME_CPID_TABLE)
   {
      currentSignatures.cpIdSignature = signature;
      currentUpdates.cpIdSignature = true;
   }
   else if (signedFile == ACS_CS_NS::FILE_NAME_CPGROUP_NAME_TABLE)
   {
      currentSignatures.cpGroupNameSignature = signature;
      currentUpdates.cpGroupNameSignature = true;
   }
   else if (signedFile == ACS_CS_NS::FILE_NAME_CPGROUP_MEMBER_TABLE)
   {
      currentSignatures.cpGroupMemberSignature = signature;
      currentUpdates.cpGroupMemberSignature = true;
   }
   else if (signedFile == ACS_CS_NS::FILE_NAME_FUNCDIST_PROVIDER_TABLE)
   {
      currentSignatures.fdProviderSignature = signature;
      currentUpdates.fdProviderSignature = true;
   }
   else if (signedFile == ACS_CS_NS::FILE_NAME_FUNCDIST_USER_TABLE)
   {
      currentSignatures.fdUserSignature = signature;
      currentUpdates.fdUserSignature = true;
   }
   else if (signedFile == ACS_CS_NS::FILE_NAME_NE_TABLE)
   {
      currentSignatures.neSignature = signature;
      currentUpdates.neSignature = true;
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_TableSignature_TRACE,
			"(%t) ACS_CS_TableSignature::sign()\n"
			"Error: Unkown signature\n"));
   }

   if (state == ACS_CS_Protocol::ServiceType_MASTER)
      writeSignature(fileNameMaster, currentSignatures, currentUpdates);
   else if (state == ACS_CS_Protocol::ServiceType_SLAVE)
      writeSignature(fileNameSlave, currentSignatures, currentUpdates);

      ACS_CS_TRACE((ACS_CS_TableSignature_TRACE,
				"(%t) ACS_CS_TableSignature::sign()\n"
				"Calling lock->stop_writing()\n"));

 if (lock) lock->stop_writing();

  //## end ACS_CS_TableSignature::sign%464FFD3402FC.body
}

 long unsigned ACS_CS_TableSignature::getSignature (std::string signedFile)
{
  //## begin ACS_CS_TableSignature::getSignature%464FFD4B02CD.body preserve=yes

   ACS_CS_TRACE((ACS_CS_TableSignature_TRACE,
		"(%t) ACS_CS_TableSignature::getSignature()\n"
		"Entering function\n"));

   CS_SIGNATURE_BLOCK tempSignatures;
   memset(&tempSignatures, 0, sizeof(tempSignatures));

 if (lock) lock->start_reading();
   readSignature(fileNameMaster, tempSignatures);
 if (lock) lock->stop_reading();

   unsigned long signature = 0;

   if (signedFile == ACS_CS_NS::FILE_NAME_HWC_TABLE)
      signature = tempSignatures.hwcSignature;
   else if (signedFile == ACS_CS_NS::FILE_NAME_CPID_TABLE)
      signature = tempSignatures.cpIdSignature;
   else if (signedFile == ACS_CS_NS::FILE_NAME_CPGROUP_NAME_TABLE)
      signature = tempSignatures.cpGroupNameSignature;
   else if (signedFile == ACS_CS_NS::FILE_NAME_CPGROUP_MEMBER_TABLE)
      signature = tempSignatures.cpGroupMemberSignature;
   else if (signedFile == ACS_CS_NS::FILE_NAME_FUNCDIST_PROVIDER_TABLE)
      signature = tempSignatures.fdProviderSignature;
   else if (signedFile == ACS_CS_NS::FILE_NAME_FUNCDIST_USER_TABLE)
      signature = tempSignatures.fdUserSignature;
   else if (signedFile == ACS_CS_NS::FILE_NAME_NE_TABLE)
      signature = tempSignatures.neSignature;
   else
   {
      ACS_CS_TRACE((ACS_CS_TableSignature_TRACE,
			"(%t) ACS_CS_TableSignature::getSignature()\n"
			"Error: Unkown signature\n"));
   }

   ACS_CS_TRACE((ACS_CS_TableSignature_TRACE,
			"(%t) ACS_CS_TableSignature::getSignature()\n"
			"Signature = %d, file name = %s\n", signature, signedFile.c_str()));

   return signature;

  //## end ACS_CS_TableSignature::getSignature%464FFD4B02CD.body
}

 void ACS_CS_TableSignature::readSignature (std::string fileName, CS_SIGNATURE_BLOCK &signatureBlock)
{
  //## begin ACS_CS_TableSignature::readSignature%4650009001C3.body preserve=yes

   ACS_CS_TRACE((ACS_CS_TableSignature_TRACE,
		"(%t) ACS_CS_TableSignature::readSignature()\n"
      "File name = %s\n", fileName.c_str()));

   fstream file;
   file.open(fileName.c_str(), ios_base::binary | ios_base::in);

	if (file.is_open() )
	{
		(void) file.seekg(0, ios_base::end);
		int dataLength = file.tellg();

      if (dataLength == sizeof(signatureBlock))
      {
		   (void) file.seekg(0);

		   // Read all data except checksum (the last 4 bytes)
		   (void) file.read(reinterpret_cast<char *>(&signatureBlock), dataLength);

      }

      file.close();
   }

  //## end ACS_CS_TableSignature::readSignature%4650009001C3.body
}

 void ACS_CS_TableSignature::writeSignature (std::string fileName, CS_SIGNATURE_BLOCK &signatureBlock, CS_SIGNATURE_UPDATE &signatureUpdate)
{
  //## begin ACS_CS_TableSignature::writeSignature%4650009B03B7.body preserve=yes

   ACS_CS_TRACE((ACS_CS_TableSignature_TRACE,
		"(%t) ACS_CS_TableSignature::writeSignature()\n"
      "File name = %s\n", fileName.c_str()));

   CS_SIGNATURE_BLOCK tempSignatures;
   memset(&tempSignatures, 0, sizeof(tempSignatures) );

   fstream inFile;
	inFile.open(fileName.c_str(), ios_base::binary | ios_base::in);

   if ( inFile.is_open() )
   {
      (void) inFile.seekg(0, ios_base::end);
      size_t dataLength = inFile.tellg();
      (void) inFile.seekg(0);

      if (dataLength >= sizeof(tempSignatures) )
      {
         (void) inFile.read( reinterpret_cast<char *> (&tempSignatures), sizeof(tempSignatures) );
      }

      inFile.close();
   }

   fstream outFile;
   outFile.open(fileName.c_str(), ios_base::binary | ios_base::trunc | ios_base::out | ios_base::in);

   if (signatureUpdate.hwcSignature)
      tempSignatures.hwcSignature = signatureBlock.hwcSignature;
   if (signatureUpdate.cpIdSignature)
      tempSignatures.cpIdSignature = signatureBlock.cpIdSignature;
   if (signatureUpdate.cpGroupNameSignature)
      tempSignatures.cpGroupNameSignature = signatureBlock.cpGroupNameSignature;
   if (signatureUpdate.cpGroupMemberSignature)
      tempSignatures.cpGroupMemberSignature = signatureBlock.cpGroupMemberSignature;
   if (signatureUpdate.fdProviderSignature)
      tempSignatures.fdProviderSignature = signatureBlock.fdProviderSignature;
   if (signatureUpdate.fdUserSignature)
      tempSignatures.fdUserSignature = signatureBlock.fdUserSignature;
   if (signatureUpdate.neSignature)
      tempSignatures.neSignature = signatureBlock.neSignature;


	if ( outFile.is_open() )
	{
      (void) outFile.seekg(0);

      (void) outFile.write( reinterpret_cast<const char *> (&tempSignatures), sizeof(tempSignatures) );
     
      outFile.close();
   }

  //## end ACS_CS_TableSignature::writeSignature%4650009B03B7.body
}

// Additional Declarations
  //## begin ACS_CS_TableSignature%464FF9F3034A.declarations preserve=yes
  //## end ACS_CS_TableSignature%464FF9F3034A.declarations

//## begin module%464FFAAC0212.epilog preserve=yes
//## end module%464FFAAC0212.epilog

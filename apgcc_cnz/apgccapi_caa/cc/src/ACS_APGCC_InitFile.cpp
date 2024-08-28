// COPYRIGHT Ericsson Utvecklings AB, Sweden 1997.
// All rights reserved.
//
// The Copyright to the computer program(s) herein
// is the property of Ericsson Utvecklings AB, Sweden.
// The program(s) may be used and/or copied only with
// the written permission from Ericsson Utvecklings AB or in
// accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been
// supplied.




#include <iostream>
#include <fstream>
#include <algorithm>
#include <ace/OS.h>
#include "ACS_APGCC_InitFile.H"





ACS_APGCC_InitFile::ACS_APGCC_InitFile()
:changed_(false)
{}

ACS_APGCC_InitFile::~ACS_APGCC_InitFile()
{
   SectionMap::iterator sit;
   KeyValueMap* km;

   // if changed, save to config file
   if (changed_)
      this->save();

   // Clear and delete entries
   for (sit=sect_.begin(); sit!=sect_.end(); sit++) {
      km = sit->second;
      if (km)
         delete km;
   }

   sect_.erase(sect_.begin(),sect_.end());
}

bool ACS_APGCC_InitFile::open(const std::string& fname)
{
	ifstream fin;

   // Save file name
   fname_ = fname;

   // Clear section map
   sect_.erase(sect_.begin(),sect_.end());

   // Open the given file
	fin.open(fname_.c_str(),ios::in);
	if (!fin)
		return false;

	std::string str;
	std::string sect;
   KeyValueMap* km = NULL;

   // Construct the section map by reading file entries
	while (fin) {

		getline(fin,str,'\n');

		if (!fin)
			break;

      ACS_APGCC::trim(str);

      if ( ACS_APGCC::match(str,"[*]") ) {        // New section

         if ( !ACS_APGCC::item(sect,str,"[]",0) ) {
            //cout << "Error: item(section) failed!" << endl;
		    sect = "";
            continue;
         }

         ACS_APGCC::trim(sect);
         ACS_APGCC::toUpper(sect);

         km = new KeyValueMap;
		 if (km) {
			sect_[sect] = km;
		 }
		 else {
			 sect = "";
		 }

         //cout << "New section: " << sect.c_str() << endl;

      }
      else if ( sect.length() > 0 && ACS_APGCC::match(str,"*=*") ) {   // New key-value pair

         std::string key;
         std::string val;
         KeyValue kv;

         key = ACS_APGCC::before(str,"=");
         val = ACS_APGCC::after(str,"=");

         ACS_APGCC::trim(key);
         ACS_APGCC::trim(val);

         // Store key-value pair
         kv.first  = key;
         kv.second = val;

         km = sect_[sect];

         // Search keys are always upper case
         ACS_APGCC::toUpper(key);

         // Insert key and value
         (*km)[key] = kv;

         //cout << "New key: " << kv.first.c_str() << ", value: " << kv.second.c_str() << endl;

      }
      else {
         //cout << "ERROR: Skipping line: " << str.c_str() << endl;
      }

	}  // while

   fin.close();

   changed_ = false;

	return true;
}


void ACS_APGCC_InitFile::close()
{
   if (changed_)
      this->save();
}


bool ACS_APGCC_InitFile::save(const std::string& newFname)
{
	ofstream fout;

   if ( newFname.length() > 0)
      fname_ = newFname;

   // Open/Truncate the given file
   fout.open(fname_.c_str(),ios::out | ios::trunc);
	if (!fout)
		return false;

   SectionMap::iterator sit;
   KeyValueMap::iterator kit;
   KeyValueMap* km;

   fout << "# Note that changes made to this file may be overwritten" << std::endl;
   fout << "# if the service(s) decides to save internally made changes." << std::endl;

   for (sit=sect_.begin(); sit!=sect_.end(); sit++) {

      fout << std::endl << "[" << sit->first.c_str() << "]" << std::endl;

      km = sit->second;

      for (kit=km->begin(); kit!=km->end(); kit++) {
         fout << kit->second.first.c_str() << "=" << kit->second.second.c_str() << std::endl;
      }

   } // for all sections

   fout.close();

   changed_ = false;

   return true;
}


void ACS_APGCC_InitFile::section(const std::string& Section)
{
   currSection_ = Section;
   ACS_APGCC::trim(currSection_);
   ACS_APGCC::toUpper(currSection_);
}


void ACS_APGCC_InitFile::eraseSection(const std::string& Section)
{
   KeyValueMap* km;
   std::string sect = Section;

   ACS_APGCC::trim(sect);
   ACS_APGCC::toUpper(sect);

   SectionMap::iterator it = sect_.find(sect);

   if ( it != sect_.end() ) {

      // Delete key-value map
      km = it->second;

      if (km)
         delete km;

      sect_.erase(it);

      changed_ = true;
   }
}


void ACS_APGCC_InitFile::eraseKey(const std::string& Section, const std::string& Key)
{
   KeyValueMap* km;
   std::string sect = Section;
   std::string key  = Key;

   ACS_APGCC::trim(sect);
   ACS_APGCC::trim(key);
   ACS_APGCC::toUpper(sect);
   ACS_APGCC::toUpper(key);

   SectionMap::iterator it;

   it = sect_.find(sect);

   if ( it != sect_.end() ) {

      km = it->second;

      if (km) {

         KeyValueMap::iterator kit;

         kit = km->find(key);

         if ( kit != km->end() ) {
            km->erase(kit);

            changed_ = true;
         }
      }
   }
}


std::string ACS_APGCC_InitFile::getStr(const std::string& Key, const std::string& DefaultStr)
{
   return this->getStr(currSection_,Key,DefaultStr);
}


std::string ACS_APGCC_InitFile::getStr(const std::string& Section, const std::string& Key, const std::string& DefaultStr)
{
	std::string sect = Section;
	std::string key  = Key;

   ACS_APGCC::trim(sect);
   ACS_APGCC::trim(key);
   ACS_APGCC::toUpper(sect);
   ACS_APGCC::toUpper(key);

   // Check for section
   if ( sect_.count(sect) > 0 ) {

      KeyValueMap* km = sect_[sect];

      // Check for key-value pair
      if ( km && km->count(key) > 0 ) {
         return (*km)[key].second;
      }
   }

   return DefaultStr;
}


int ACS_APGCC_InitFile::getVal(const std::string& Key, int DefaultVal)
{
   return this->getVal(currSection_,Key,DefaultVal);
}


int ACS_APGCC_InitFile::getVal(const std::string& Section, const std::string& Key, int DefaultVal)
{
   std::string sect = Section;
   std::string key  = Key;

   ACS_APGCC::trim(sect);
   ACS_APGCC::trim(key);
   ACS_APGCC::toUpper(sect);
   ACS_APGCC::toUpper(key);

   // Check for section
   if ( sect_.count(sect) > 0 ) {

      KeyValueMap* km = sect_[sect];

      // Check for key-value pair
      if ( km && km->count(key) > 0 ) {
         return ::atoi( (*km)[key].second.c_str() );
      }
   }

   return DefaultVal;
}


bool ACS_APGCC_InitFile::setStr(const std::string& Key, const std::string& Value)
{
   return this->setStr(currSection_,Key,Value);
}


bool ACS_APGCC_InitFile::setStr(const std::string& Section, const std::string& Key, const std::string& Value)
{
   std::string sect = Section;
   std::string key  = Key;
   std::string val  = Value;
   KeyValue kv;
   KeyValueMap* km;

   // Save in key-value pair
   ACS_APGCC::trim(key);
   ACS_APGCC::trim(val);

   kv.first  = key;
   kv.second = val;

   // Construct the search keys
   ACS_APGCC::trim(sect);
   ACS_APGCC::toUpper(sect);
   ACS_APGCC::toUpper(key);

   // Check if section exists, otherwise create it
   if ( sect_.count(sect) == 0 ) {

      km = new KeyValueMap;

      sect_[sect] = km;

      changed_ = true;
   }
   else
      km = sect_[sect];


   // Insert key-value pair

   if ( km ) {
      // Insert or change key-value pair
      (*km)[key] = kv;

      changed_ = true;
   }
   else
      return false;

   return true;
}



bool ACS_APGCC_InitFile::setVal(const std::string& Key, int Value)
{
   return this->setVal(currSection_,Key,Value);
}


bool ACS_APGCC_InitFile::setVal(const std::string& Section, const std::string& Key, int Value)
{
	std::string sect = Section;
	std::string key  = Key;
   char val[128];
   KeyValue kv;
   KeyValueMap* km;

   // Save in key-value pair
   ACS_APGCC::trim(key);
   ::sprintf(val,"%i",Value);

   kv.first  = key;
   kv.second = val;

   // Construct the search keys
   ACS_APGCC::trim(sect);
   ACS_APGCC::toUpper(sect);
   ACS_APGCC::toUpper(key);

   // Check if section exists, otherwise create it
   if ( sect_.count(sect) == 0 ) {

      km = new KeyValueMap;

      sect_[sect] = km;

      changed_ = true;
   }
   else
      km = sect_[sect];


   // Insert key-value pair

   if ( km ) {
      // Insert or change key-value pair
      (*km)[key] = kv;

      changed_ = true;
   }
   else
      return false;

   return true;
}


void ACS_APGCC_InitFile::changed(bool TrueFalse)
{
   changed_ = TrueFalse;
}


void ACS_APGCC_InitFile::dump()
{
   SectionMap::iterator sit;
   KeyValueMap::iterator kit;
   KeyValueMap* km;

   std::cout << "- - - Start of Section Map - - -" << std::endl;

   for (sit=sect_.begin(); sit!=sect_.end(); sit++) {

	   std::cout << "[" << sit->first.c_str() << "]" << std::endl;

      km = sit->second;

      for (kit=km->begin(); kit!=km->end(); kit++) {
    	  std::cout << kit->second.first.c_str() << "=" << kit->second.second.c_str() <<std::endl;
      }
   } // for all sections

   std::cout << "- - - End of Section Map - - -" << std::endl;
}



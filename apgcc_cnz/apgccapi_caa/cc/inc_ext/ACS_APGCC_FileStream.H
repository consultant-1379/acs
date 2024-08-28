#ifndef ACS_APGCC_FILESTREAM_H
#define ACS_APGCC_FILESTREAM_H


#  include "ACS_APGCC_Serialize.H"



#  include "ACS_APGCC_OS.H"


#ifndef INCLUDED_IOSTREAM
#  include <iostream>
#endif



// Writer for files

class ACS_APGCC_FileWriter: public ACS_APGCC_StreamWriter
{
public:

	ACS_APGCC_FileWriter(const char* filename);
	ACS_APGCC_FileWriter(std::ostream* os);
	~ACS_APGCC_FileWriter();

	// write the whole object
	virtual int writeObject(ACS_APGCC_Serializable& ss);

	// primitive "built-in" value types
	virtual int writeChar(const char ch);
	virtual int writeChar(const unsigned char ch);
	virtual int writeInt(const int n);
	virtual int writeByte(const int n);
	virtual int writeWord(const int n);
	virtual int writeString(const char* s);
	virtual int writeBuffer(const void* buf, size_t len);

private:

   std::ostream* fout;
	bool delete_on_exit;
};


// Reader for files

class ACS_APGCC_FileReader: public ACS_APGCC_StreamReader
{
public:

	ACS_APGCC_FileReader(const char* filename);
	ACS_APGCC_FileReader(std::istream* is);
	virtual ~ACS_APGCC_FileReader();

	// read the whole object
	virtual int readObject(ACS_APGCC_Serializable& ss);

	// primitive "built-in" value types
	virtual int readChar(char& ch);
	virtual int readChar(unsigned char& ch);
	virtual int readInt(int& n);
	virtual int readByte(int& n);
	virtual int readWord(int& n);
	virtual int readString(char* s, const size_t maxLen);
	virtual int readBuffer(void* buf, size_t len);

   virtual int readBuffer(void* buf);
   // Returns size of allocated buffer, or -1 if failed.
   // Caller is responsible for deallocating the returned buffer.

private:

   std::istream* fin;
	bool delete_on_exit;
};



#endif //ACS_APGCC_FILESTREAM_H


#include <fstream>
#include <string>
#include "ACS_APGCC_FileStream.H"


#ifdef WIN32
using namespace std;
#endif


ACS_APGCC_FileWriter::ACS_APGCC_FileWriter(const char* filename)
:fout(new ofstream(filename)), delete_on_exit(false)
{
}

ACS_APGCC_FileWriter::ACS_APGCC_FileWriter(ostream* os)
:fout(os), delete_on_exit(false)
{
}

ACS_APGCC_FileWriter::~ACS_APGCC_FileWriter()
{
	//fout->close();
	/*if (delete_on_exit)
		delete fout;*/
}

// write the whole object
int ACS_APGCC_FileWriter::writeObject(ACS_APGCC_Serializable& ss)
{
	if (*fout)
		return ss.writeTo(this);
	else
		return -1;
}

// primitive "built-in" value types
int ACS_APGCC_FileWriter::writeChar(const char ch)
{
	if (*fout)
      *fout << "ch:" << ch << std::endl;
	else
		return -1;
	return 0;
}

int ACS_APGCC_FileWriter::writeChar(const unsigned char ch)
{
	if (*fout)
      *fout << "uch:" << ch << std::endl;
	else
		return -1;
	return 0;
}

int ACS_APGCC_FileWriter::writeInt(const int n)
{
	if (*fout)
      *fout << "int:" << n << std::endl;
	else
		return -1;
	return 0;
}

int ACS_APGCC_FileWriter::writeByte(const int n)
{
	if (!fout)
		return -1;

	*fout << "byte:" << (n % 256) << std::endl;
	return 0;
}

int ACS_APGCC_FileWriter::writeWord(const int n)
{
	if (!fout)
		return -1;

	*fout << "word:" << (n % 0xffff) << std::endl;
	return 0;
}

int ACS_APGCC_FileWriter::writeString(const char* s)
{
	if (!fout)
		return -1;
	*fout << "str:" << ::strlen(s) << ":" << s << std::endl;
	return 0;
}

int ACS_APGCC_FileWriter::writeBuffer(const void* buf, size_t len)
{
	if (!fout)
		return -1;

	*fout << "buf:" << len << ":";

	for (size_t i=0; i<len; i++)
		*fout << (char)((const char*)buf)[i];

	*fout << std::endl;

	return 0;
}




ACS_APGCC_FileReader::ACS_APGCC_FileReader(const char* filename)
:fin(new ifstream(filename)), delete_on_exit(true)
{
}

ACS_APGCC_FileReader::ACS_APGCC_FileReader(istream* is)
:fin(is), delete_on_exit(false)
{
}

ACS_APGCC_FileReader::~ACS_APGCC_FileReader()
{
	if (delete_on_exit)
		delete fin;
}

// read the whole object
int ACS_APGCC_FileReader::readObject(ACS_APGCC_Serializable& ss)
{
	if (*fin)
		return ss.readFrom(this);
	else
		return -1;
}

// primitive "built-in" value types
int ACS_APGCC_FileReader::readChar(char& ch)
{
	if (!fin)
		return -1;

	char type[20];

	fin->getline(type,20,':');
	*fin >> ch;

	return 0;
}

int ACS_APGCC_FileReader::readChar(unsigned char& ch)
{
	if (!fin)
		return -1;

	char type[20];

	fin->getline(type,20,':');
	*fin >> ch;

	return 0;
}

int ACS_APGCC_FileReader::readInt(int& n)
{

	if (!fin)
		return -1;

	char type[20];
	std::cout<<"ACS_APGCC_FileReader::readInt"<<std::endl;
	fin->getline(type,20,':');
	*fin >> n;
	std::cout<<"N:"<<n<<std::endl;
	return 0;
}

int ACS_APGCC_FileReader::readByte(int& n)
{
	if (!fin)
		return -1;

	char type[20];

	fin->getline(type,20,':');
	*fin >> n;

	return 0;
}

int ACS_APGCC_FileReader::readWord(int& n)
{
	if (!fin)
		return -1;

	char type[20];

	fin->getline(type,20,':');
	*fin >> n;

	return 0;
}

int ACS_APGCC_FileReader::readString(char* s, const size_t maxLen)
{
	if (!fin)
		return -1;

	char type[20];

	fin->getline(type,20,':');
	fin->getline(type,20,':');
	fin->getline(s,maxLen);

	return 0;
}

int ACS_APGCC_FileReader::readBuffer(void* buf, size_t len)
{
	if (!fin)
		return -1;

	char type[20];

	fin->getline(type,20,':');	// type
	fin->getline(type,20,':');	// number of data
	fin->getline((char*)buf,len+1);	// data

	return 0;
}

int ACS_APGCC_FileReader::readBuffer(void* buf)
{
	if (!fin)
		return -1;

	char type[20];
   int len;

	fin->getline(type,20,':');	// type
	fin->getline(type,20,':');	// number of data
   len = ::atoi(type);
	fin->getline((char*)buf,len+1);	// data

	return 0;
}

/*
 * This application is used to demo the Valgrind memcheck tool.
 * The code is intentionally littered with memory errors. Use
 * with care.
 */

#include <iostream>
#include <unistd.h>
#include <string.h>

using std::cout;
using std::endl;

void invalidRead();
void invalidWrite();
int useUninitializedValue();
void freeDeleteMismatch();
void memoryOverlap();
unsigned char * memoryLeak();
void deleteMismatch();
void pointerDereferenceError();
int segmentationFault();


/**
 * -a invalid read
 * -b invalid write
 * -c use uninitialized value
 * -d free delete mismatch
 * -e memory overlap
 * -f memory leak
 * -g delete mismatch
 * -h segmentation fault
 */
int main(int argc, char * argv[]) {

	// Valid options
	char validOptions[] = "abcdefgh";
	int option = 0, error = 0;

	// Check that there is exactly one option (argc == 2)
	if (argc != 2)
		error = 1;

	// Check for a valid option
	if ( (option = getopt (argc, argv, validOptions)) == '?')
		error = 1;

	// Check that there are no more options (i.e. no -abcd)
	if (getopt (argc, argv, validOptions) != -1)
		error = 1;

	// Print error message
	if (error) {
		cout << "Usage: " << argv[0] << " [-a] [-b] [-c] [-d] [-e] [-f] [-g] [-h]" << endl;
		cout << "-a invalid read" << endl;
		cout << "-b invalid write" << endl;
		cout << "-c use uninitialized value" << endl;
		cout << "-d free delete mismatch" << endl;
		cout << "-e memory overlap" << endl;
		cout << "-f memory leak" << endl;
		cout << "-g delete mismatch" << endl;
		cout << "-h segmentation fault" << endl;
		exit(2);
	}

	// Call proper function depending on option
	switch (option) {
		case 'a':
			invalidRead();
			break;
		case 'b':
			invalidWrite();
			break;
		case 'c':
			(void) useUninitializedValue();
			break;
		case 'd':
			freeDeleteMismatch();
			break;
		case 'e':
			memoryOverlap();
			break;
		case 'f':
			(void) memoryLeak();
			break;
		case 'g':
			deleteMismatch();
			break;
		case 'h':
			(void) segmentationFault();
			break;
		default:
			break;
	}

	return 0;

}

/**
 * Reads memory outside allocated region
 */
void invalidRead() {

	unsigned char readArray[256];
	unsigned char writeArray[256];
	memcpy(writeArray, readArray - 4096, 256);
}

/**
 * Writes memory outside allocated region
 */
void invalidWrite() {

	unsigned char * byteArray = new unsigned char[256];
	memset(byteArray, 0, 260);
	delete [] byteArray;
}

/**
 * Use uninitialized pointer
 */
int useUninitializedValue() {

	int * x;
	int y;

	if (x)
		y = *x;

	return y;
}

/**
 * Allocate with new, deallocate with free
 */
void freeDeleteMismatch() {

	unsigned char * array = new unsigned char[256];
	free(array);
}

/**
 * Copying data between overlapping memory regions
 */
void memoryOverlap() {

	unsigned char writeArray[256];
	memcpy(writeArray, writeArray + 128, 256);
}

/**
 * Forgetting to deallocating memory
 */
unsigned char * memoryLeak() {

	unsigned char * byteArray = new unsigned char[256];
	return byteArray;
}

/**
 * Allocate with [] but forgetting [] when deleting
 */
void deleteMismatch() {

	unsigned char * byteArray = new unsigned char[256];
	delete byteArray;
}

/*
 * Dereferencing NULL-pointer, causing segmentation fault
 */
int segmentationFault() {

	int * x = NULL;
	int y = *x;
	return y;
}

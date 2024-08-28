#include <iostream.h>
#include <windows.h>
#include <winbase.h>

#include <rw/regexp.h>
#include <rw/cstring.h>

void main ()
{
	RWCString stri;

	// Skapa RegExp som matchar text som börjar på "e"
	RWCRegexp re("e[a-z]*"); 
	
	cout << "Enter string (must contain an 'e') >" << flush;
	cin >> stri;

	// Matcha RegExp:et mot inlästa stri-strängen
	cout << endl << "RegExp:" << stri(re) << endl; 
	
	// Hur gick det ?
	cout << "RegExp statVal = " << re.status() << endl;
	
	Sleep(1000);  // wait 1 second
}
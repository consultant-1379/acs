//============================================================================
// Name        : Prova_TRA.cpp
// Author      : XGIOPAP
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "/vobs/cm4ap/ntacs/tra_cnz/traapi_caa/inc_ext/ACS_TRA_trace.h"

using namespace std;

int main()
{
	int n = 1;

	ACS_TRA_trace * objComplex = new ACS_TRA_trace("ComplexPoint", "b, B2, s2, S, i, I2, l2, L, c10, C, p");
	//ACS_TRA_trace objComplex("ComplexPoint", "b, B2, s2, S, i, I2, l2, L, c10, C, p");
	while (n <= 100)
	{
		if (objComplex->ACS_TRA_ON())
		{
			char b = 98;
			char B2[2] = {66, 66};
			short s2[2] = {-126, 126};
			unsigned short S = 255;
			int i = -32767;
			unsigned int I2[2] = {0, 65535};
			long l2[2] ={-2147483647, 2147483647};
			unsigned long L = 4294967295;
			char C[100] = "This is a long character string";
			char c10[10] = "wwwwwwwww";
			char *p = C;
			objComplex->ACS_TRA_event(11, b, B2, s2, S, i, I2, l2, L, c10, C, p);
			cout << "complex " << b << endl;
			cout << "complex " << B2 << endl;
			cout << "complex " << s2 << endl;
			cout << "complex " << S << endl;
			cout << "complex " << i << endl;
			cout << "complex " << I2 << endl;
			cout << "complex " << l2 << endl;
			cout << "complex " << L << endl;
			cout << "complex " << c10 << endl;
			cout << "complex " << C << endl;
			cout << "complex " << p << endl;
			cout << "time " << n++ << endl;
			sleep(3);
		}
	}

	delete objComplex;

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
//#include <winbase.h>
#include <iostream.h>
#include "ACS_AEH_EvReport.H"
#include <string.h>
#include "getopt.h"

char* deleteNewLine(char *line)
{
	if ( line[ strlen(line)-1 ] == '\n' )
	{
		line[ strlen(line)-1 ] = '\0';
	}
	return line;
}
		

int main (int argc, char *argv[])
{
	int nrOfTimes (1);
	char fileId[256];
	bool enumerateSpecificProblem = false;

	FILE *eventFile;

	int	c;	
	int errflag (0);

	// Parse options and parameters

	while ((c = getopt (argc, argv, "n:s")) != EOF)
    {   
		switch (c) 
		{
		case 'n':				// Number of times 
			nrOfTimes = atoi(optarg);
			break;
		case 's':
			enumerateSpecificProblem = true;
			break;
		case '?':				// Other junk
		default:
			errflag = 1;
			break;
			
		} // end switch (arg)
	}  

	if (optind < argc)	// filename specified ?
	{
		strcpy(fileId, argv [optind]);
	}
	else
	{
		errflag = 1;
    }

	if (errflag)
    {
		// Usage
		cerr << endl;
		cerr << "Usage: " << argv [0] 
			 << " [-n number_of_times] [-s] file" << endl;
		return 1;
    }

	if((eventFile = fopen(fileId, "r")) == NULL)
	{
      cout << "Cannot open file: " << fileId << endl;
	  return(2);
	}

	char processName[32];	
    long specificProblem;			
	char percSeverity[8];		
    char probableCause[128];		
	char objClassOfReference[8];	
    char objectOfReference[64];				
    char problemData[1024];		
    char problemText[1024];	

	ACS_AEH_EvReport eventReport;
	char buf[1025];

	for (int i=1; i<= nrOfTimes; i++)
	{
		while (fgets( buf, 1025, eventFile ) != NULL)  // empty line
		{
			fgets( buf, 1025, eventFile );
			strcpy(processName, deleteNewLine(buf));
			fgets( buf, 1025, eventFile );
			specificProblem = atol(deleteNewLine(buf));
			
			if (enumerateSpecificProblem) {
				specificProblem += i-1;   
			}
			fgets( buf, 1025, eventFile );
			strcpy(percSeverity, deleteNewLine(buf));
			fgets( buf, 1025, eventFile );
			strcpy(probableCause, deleteNewLine(buf));
			fgets( buf, 1025, eventFile );
			strcpy(objClassOfReference, deleteNewLine(buf));
			fgets( buf, 1025, eventFile );
			strcpy(objectOfReference, deleteNewLine(buf));
			fgets( buf, 1025, eventFile );
			strcpy(problemData, deleteNewLine(buf));
			fgets( buf, 1025, eventFile );
			strcpy(problemText, deleteNewLine(buf));

			if ((eventReport.sendEventMessage(
				processName,
				specificProblem,
				percSeverity,
				probableCause,
				objClassOfReference,
				objectOfReference,
				problemData,
				problemText)) != ACS_AEH_ok)
			{
				// Display error info.
				cout << "Error when reporting event" << endl;;
				cout << "getError()=" << eventReport.getError() << endl;
		        cout << "getErrorText()=" << eventReport.getErrorText() << endl;
			    cout << endl;  
			}
		}
		rewind(eventFile);
	}
	if( fclose( eventFile ) ) {      
		cout << "Cannot close file: " << fileId << endl;
		return(3);
	}
	return(0);
}

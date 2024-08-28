/****************************************************************
*			SFGEN.H FILE WAS CREATED ON JULY 1, 2008			*
*			AS A PART OF THE IMPLEMENTATION OF THE O.S.			*
*			HARDENING FEATURE IP IMPLEMENTATION.				*
*			IMPLEMENTATION EXECUTED BY SUJANA AND KALYAN.		*
*****************************************************************/

#ifndef SFGEN_CLASS_H
#define SFGEN_CLASS_H

#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <list>
#include <stdlib.h>
#include <cstring>
#include <stdio.h>
#include "clusapi.h"
#include "atlconv.h"

using namespace std;

#define MAX_INITAB 100 //Maximum number of rows in INItable
#define MAX_INFTAB 100 //Maximum number of rows in INFtable
#define MAX_REGTAB 50  //Maximum number of rows in REGtable

/******************************************************************************************************************/

/************************************************************************
*																		*
*		INFTABLE: CONTAINS THE KEYS AND VALUES READ FROM THE INF FILE	*
*																		*
*************************************************************************/

typedef struct{
	string key, value;
}INFtable;


/************************************************************************
*																		*
*		INITABLE: CONTAINS THE KEYS AND VALUES READ FROM THE INI FILE	*
*																		*
*************************************************************************/

typedef struct
{
	string tkey,def,lo_threshold,up_threshold,sect,tvalue;
}INItable;

/********************************************************************************
*																				*
*		REGTABLE: CONTAINS THE REGISTRY KEYS AND THEIR CORRESPONDING SLOGANS	*
*																				*
*********************************************************************************/

typedef struct
{
	string exp, slogan;
}REGtable;

const char CSTR_WHITESPACE[3] = {32,'\t',0};
const char INIfilePath[] = "C:\\Winnt\\System32\\APG_Security_Custom_Conf.ini";
const char INFfilePath[] = "C:\\Winnt\\System32\\APG_Security_Custom.inf";
const char oldINFfilePath[] = "C:\\Winnt\\System32\\APG_Security_Custom_old.inf";
const char othernode_INFfilePath[] = "\\C$\\Winnt\\System32\\APG_Security_Custom.inf";
const char othernode_oldINFfilePath[] = "\\C$\\Winnt\\System32\\APG_Security_Custom_old.inf";
const string sub_run = "{Subsequent Runs}";
const string first_run = "{First Run}";

bool f_adv,f_rmv,f_def,f_imp;

/******************************************************************************************************************/

/**************************************************
*												  *
*				GLOBAL METHODS					  *
*												  *
**************************************************/

void usage(const string);

/**********************************************************
*	THIS METHOD IS USED TO DISPLAY PROPER OPTIONS.		  *
***********************************************************/

bool isNumeric(const char *);

/**********************************************************************
*	THIS METHOD IS USED TO CHECK IF THE USER-INPUT IN NUMERICAL.	  *
***********************************************************************/

list<string>  tokenize(const string& str,const string& delim = ":");

/********************************************************************************************************
*	THIS METHOD IS USED TO TOKENIZE ANY STRING VARIABLE ON BOTH SIDES WRT A DELIMITER.					*
*	THIS METHOD TAKES THE STRING VARIABLE TO BE TOKENIZED AS THE FIRST ARGUMENT,						*
*	AND THE STRING VARIABLE TO BE USED AS A DELIMITER AS THE SECOND ARGUMENT.							*
*	THE DEFAULT VALUE FOR THE SECOND ARGUMENT IS ':'.													*
*********************************************************************************************************/

void trim(string& str, const string& wipeOut = string(CSTR_WHITESPACE));

/************************************************************************************
*	THIS METHOD IS USED TO TRIM ANY STRING VARIABLE ON BOTH SIDES.					*
*	THIS METHOD TAKES THE STRING VARIABLE TO BE TRIMMED AS THE FIRST ARGUMENT,		*
*	AND THE STRING VARIABLE TO BE WIPED OFF AS THE SECOND ARGUMENT.					*
*	THE DEFAULT VALUE FOR THE SECOND ARGUMENT IS WHITESPACE.						*
*************************************************************************************/

/******************************************************************************************************************/

/**************************************************
*												  *
*					CLASSES						  *
*												  *
**************************************************/

class ACS_INIfile
{
	private:

		int c_INI_SR_rc, c_INIloc, c_RegLoc, c_Regtab_rc;
		FILE* c_fINI;
		INItable c_INItab[MAX_INITAB];
		REGtable c_regtab[MAX_REGTAB];

		void createINItable();
		void createRegtab();
		void setINI_SRrowcount();

	public:
		
		ACS_INIfile();
		INItable* searchINItable(string SearchKey);
		REGtable* searchRegtable(const string SearchKey);
		~ACS_INIfile();
};

class ACS_INFfile
{
	private:

		int c_INFrc,c_INFloc;
		FILE* c_fINF;
		INFtable c_INFtab[MAX_INFTAB];
		ACS_INIfile c_INIfile;
		bool f_sys,f_pri,f_evt,f_reg;

		void createINFtable();
		void searchINFtable(string SearchKey);
		void setINFrowcount();
		string validateUserValue(string SearchKey,string UserValue);
		BOOL getOtherNodeName(string& sRemoteMachine);
		int findSubstring(char* str, char* substr);
		void writeINFFile();
		void checkINFFile();

	public:

		ACS_INFfile();
		void createINFfile();
		void removeValue(string SearchKey,string UserValue);
		void addValue(string SearchKey,string UserValue);
		void restoreDefault(string SearchKey);
		void replaceDomName(const string old_dom,const string new_dom);
		~ACS_INFfile();
};

#endif
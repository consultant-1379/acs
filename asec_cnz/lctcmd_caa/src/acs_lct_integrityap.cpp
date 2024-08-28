/*=================================================================== */
/**
   @file acs_asec_integrityap.cpp

   Class method implementation.

   This module contains the implementation of class declared in
   the acs_asec_integrityap.h module

   @version N.N.N

   HISTORY

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       25/02/2011     NS       Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "acs_lct_integrityap.h"
#include <regex.h>
#include <syslog.h>
#include <linux/limits.h>
/*===================================================================
   			Default initialization.
=================================================================== */


char* ACS_SEC_Integrity::base_perm_arry[]={	(char *)"---",
		(char *)"--x",
		(char *)"-w-",
		(char *)"-wx",
		(char *)"r--",
		(char *)"r-x",
		(char *)"rw-",
		(char *)"rwx"};

char* ACS_SEC_Integrity::spcl_perm_arry[]={	(char *)"---",
		(char *)"--t",
		(char *)"-s-",
		(char *)"-st",
		(char *)"s--",
		(char *)"s-t",
		(char *)"ss-",
		(char *)"sst"};


char szLog[LOG_SIZE];
char sha1_bool[LINE_SIZE];
char *flist[50];
char bool_true[] = "true";
char bool_false[] = "false";
int flist_size=0;
int acl_status = 0;
int f_owner_flag = 0;
int f_group_flag = 0;
int f_b_perm_flag = 0;
int f_s_perm_flag = 0;
int f_ignored_files_flag=0; 
ACS_SEC_Integrity::ACS_SEC_Integrity() {

	basic_acl.f_file=NULL;
	basic_acl.f_owner=NULL;
	basic_acl.f_group=NULL;
	basic_acl.f_base_perm=NULL;
	basic_acl.f_spcl_perm=NULL;
	basic_acl.f_files_ignored=NULL;
	g_acl_str=NULL;
	log.Open("LCTBIN");
	includeFile=false;

	// below initializations done as part of coverity fixes	
	grp=NULL;
	pwd=NULL;
	stat_buff.st_gid=0;
	stat_buff.st_mode=0;
	stat_buff.st_uid=0;
}

ACS_SEC_Integrity::~ACS_SEC_Integrity() {


}

FILE *lctLogFile = NULL;
FILE *lctDataFile = NULL;
FILE *lctReportFile = NULL;
FILE *lctAclFile = NULL;



/*===================================================================
   ROUTINE: launchCmd
=================================================================== */


int ACS_SEC_Integrity::launchCmd(string cmd, const char *mode, char *str) {

	FILE *fp;
	char buff[LINE_SIZE];
	char buff1[LINE_SIZE];
	char line[LINE_SIZE];
	char *tmp;
	int rc=0;

	fp = popen(cmd.c_str(),mode);
	if (fp == NULL){
		//cout << "Error launching: ["<<cmd.c_str() <<"]"<< endl;
		sprintf(buff1,"popen failed for command [%s]",cmd.c_str());
		memcpy(str,buff1,strlen(buff1));
		str[strlen(buff1)]='\0';
		return -1;
	}
	fgets(line, LINE_SIZE, fp);
	strncpy(buff,line,LINE_SIZE);
	tmp = strtok (line," ");
	if (tmp == NULL)
		rc= -1;

	int status = pclose(fp);

	if ( rc != 0)
		return rc; 

	if (WIFEXITED(status) ){
		rc=WEXITSTATUS(status);
		if (rc == 0){
			memcpy(str,tmp,strlen(tmp));
			str[strlen(tmp)]='\0';
		}
		else {
			memcpy(str, buff, strlen(buff));
			str[strlen(buff)]='\0';
		}
		return rc;
	}
	return rc;
}

/*===================================================================
   ROUTINE: count_tokens
=================================================================== */


int ACS_SEC_Integrity::count_tokens(const char *str){

	int i=0;
	const char* substr;
	if (str == NULL)
		return i;
	substr = strchr( str, '/');

	if (substr == NULL)
		return i;

	while( *substr != '\0'){
		if ( *substr == ':')
			i++;
		substr++;
	}
	return i;
}

/*===================================================================
   ROUTINE: gettoken 
=================================================================== */

char* ACS_SEC_Integrity::gettoken(){

	char *p;
	int i =0;
	int is_eos=0;

	p = (char *)malloc(80);	
	while ( *g_acl_str != '\0') {
		if ( *g_acl_str == ':') {
			break;
		}	
		p[i] = *g_acl_str;
		i++;
		g_acl_str++;
		if (iscntrl(*g_acl_str)){
			is_eos=1;
			break;
		}
	}
	p[i] = '\0';
	if (!is_eos)
		g_acl_str++;
	return p;
}

/*===================================================================
   ROUTINE: extractAcls
=================================================================== */


int ACS_SEC_Integrity::extractAcls(char* acl_str){

	char *next_tok = NULL;
	int i=0;
	f_owner_flag = 0;
	f_group_flag = 0;
	f_b_perm_flag = 0;
	f_s_perm_flag = 0;
	f_ignored_files_flag = 0;

	/* Get number of tokens in the string */
	int count=count_tokens(acl_str);
	while ( acl_str != NULL) {
		g_acl_str = acl_str;
		for (i=0;i<=count;i++) {

			next_tok=gettoken();

			/* If file name */
			if ( i == 0 ){
				if (next_tok[0] == '\0'){
					//cout<<"Error! File name NULL found"<<endl;
					log.Write("Error! File name NULL found", LOG_LEVEL_ERROR);
					free(next_tok);
					next_tok=NULL;
					return -1;
				}
				else {		
					basic_acl.f_file = next_tok;
				}
			}		

			/* If Owner name */
			if ( i == 1 ){
				if (next_tok[0] == '\0'){
					//cout<<"Error! Owner name NULL found"<<endl;
					//log.Write("Error! Owner name NULL found", LOG_LEVEL_ERROR);
					//return -1;
					f_owner_flag = 1;
				}
				else {
					basic_acl.f_owner = next_tok;
				}
			}

			/* If group name */
			if ( i == 2 ){
				if (next_tok[0] == '\0'){
					//cout<<"Error! Group name NULL found"<<endl;
					//log.Write("Error! Group name NULL found", LOG_LEVEL_ERROR);
					//return -1;
					f_group_flag = 1;
				}
				else {
					basic_acl.f_group = next_tok;
				}
			}
			/* If base permissions */
			if ( i == 3 ){
				//cout << "base perms" << endl;
				if (next_tok[0] == '\0'){
					//cout<<"Error! Basic permission NULL found"<<endl;
					//log.Write("Error! Basic permission NULL found", LOG_LEVEL_ERROR);
					//return -1;
					f_b_perm_flag = 1;
				}
				else {
					basic_acl.f_base_perm = next_tok;
					basic_acl.f_spcl_perm= (char *)"0";
				}
			}

			/* If sticky bit/sid/gid */
			if ( i == 4 ){
				//cout << "sticky bit present" << endl;
				if (next_tok[0] == '\0'){
					//cout<<"Error! stickbit/setuid/setgid field NULL found"<<endl;
					//log.Write("Error! stickbit/setuid/setgid field NULL found", LOG_LEVEL_ERROR);
					f_s_perm_flag = 1;
					basic_acl.f_spcl_perm= (char *)"0";
				}
				else if ( atoi(next_tok) > 7 ) {
					basic_acl.f_spcl_perm= (char *)"0";
					//cout <<"Error! Wrong stickybit/sid/gid Found. Ignoring acl check..."<<endl;
					log.Write("Error! Wrong stickybit/sid/gid Found. Ignoring acl check...", LOG_LEVEL_ERROR);
					free(next_tok);
					next_tok=NULL;
					return -1;
				}
				else {	
					basic_acl.f_spcl_perm=next_tok;
				}

			}
			/* If ignored files present */
			if ( i == 5 ){
				//cout << "ignored files present" << endl;
				if (next_tok[0] == '\0'){
					//cout<<"Error! Basic permission NULL found"<<endl;
					//log.Write("Error! Basic permission NULL found", LOG_LEVEL_ERROR);
					//return -1;
					//cout << "ignored files present" << endl;
				}
				else {
					f_ignored_files_flag = 1;
					basic_acl.f_files_ignored = next_tok;
				}
			}
		}
		g_acl_str = NULL;
		break;
	}		
	next_tok = NULL;
	free(next_tok);
	return 0;
}

/*===================================================================
   ROUTINE: seek_files
=================================================================== */

int ACS_SEC_Integrity::seek_files(vector<string>& mylist, string path)
{

	char buff[LINE_SIZE];
	DIR* dir = opendir(path.c_str());
	if(dir == 0){
		sprintf(buff,"Can not open directory '%s'",path.c_str());
		log.Write(buff,LOG_LEVEL_ERROR);
		printf("%s\n",buff);
		return 1;
	}

	struct dirent *d;
	while( (d = readdir(dir)) != 0)
	{
		if(d->d_type == DT_REG)
		{
			string temp = path + d->d_name;
			mylist.push_back(temp);
		}

	}
	closedir(dir);
	return 0;
}

/*===================================================================
   ROUTINE: scan_files
=================================================================== */


int ACS_SEC_Integrity::scan_files(vector<string>& mylist, string path,char* str,int len)
{

	char buff[LINE_SIZE];
	DIR* dir = opendir(path.c_str());
	if(dir == 0){
		sprintf(buff,"Can not open directory '%s'",path.c_str());
		log.Write(buff,LOG_LEVEL_ERROR);
		printf("%s\n",buff);
		return 1;
	}

	struct dirent *d;
	while( (d = readdir(dir)) != 0)
	{
		if(d->d_type == DT_REG)
		{
			string temp = d->d_name;
			if (strncmp(temp.c_str(),str,len) == 0){ 
				string temp1 = path + d->d_name;
				mylist.push_back(temp1);
			}
		}

	}
	closedir(dir);
	return 0;
}






/*===================================================================
   ROUTINE: verify_stat_func
=================================================================== */

int ACS_SEC_Integrity::verify_stat_func(){

	char * p1;
	char * p2;
	char *p;
	char *ig;
	vector<string> mylist;
	vector<string> ignore_files_list;
	char path[LINE_SIZE];
	char str[LINE_SIZE];
	mylist.clear();
	ignore_files_list.clear();
	int wild_status = 0;
	int status = 0;

	/*check if the filepath has a wild character*/
	/*check if ignored files are present*/
	if (f_ignored_files_flag == 1){
		char *temp;
		int len=0;
		string s1;
		ig = basic_acl.f_files_ignored;
		temp=ig;
		//cout << "ignore:" << ig << endl;
		while( *temp != '\0'){
			len++;
			temp++;
		}
		len=len+1;
		for (int j=0;j<len;j++)
		{
			if (j==len-1){
				//cout << "string:" << s1 << endl;
				ignore_files_list.push_back(s1);
				s1="\0";
			}
			if (*ig!=','){
				s1+=*ig;
				ig++;
				continue;
			}
			//cout << "string:" << s1 << endl;
			ignore_files_list.push_back(s1);
			s1="\0";
			ig++;
		}

	}
	p = basic_acl.f_file;
	p2 = strrchr(basic_acl.f_file,'*');
	if (p2 != NULL){
		int len = 0;
		p1 = strrchr(basic_acl.f_file,'/');
		int l = p2 - p1 - 1;

		wild_status = 1;

		if ( l == 0 ){
			len = p1 - p + 1;
			strncpy(path,p,len);
			path[len] = '\0';
			if (seek_files(mylist,path) != 0){
				acl_status = 1;
				return -1;
			}		
		}
		else
		{
			len = p1 - p + 1;
			p1++;
			strncpy(str,p1,l);
			str[l] = '\0';
			strncpy(path,p,len);
			if (scan_files(mylist,path,str,l) != 0){
				acl_status = 1;
				return -1;
			}

		}

	} 

	if (wild_status == 1){


		if ( (int)mylist.size() == 0 ){
			sprintf(szLog,"ERROR: File \"%s\" has been removed: ACLs cannot be processed \n",basic_acl.f_file);
			WriteReport();
			printf("%s\n",szLog);
			szLog[0]='\0';
			acl_status = 1;
			return -1;
		}


		for(int k=0;k<(int)mylist.size();k++)
		{
			int ignore_flag=0;
			char file_path[500];
			strcpy(file_path,mylist[k].c_str());
			int len1 = strlen(file_path);
			file_path[len1]='\0';
			for(int q=0;q<(int)ignore_files_list.size();q++)
			{
				if (strcmp(file_path,ignore_files_list[q].c_str())==0){
					//cout<<"ignored file:" << file_path <<endl;
					ignore_flag=1;
					break;
				}
			}		
			if(ignore_flag==1){
				continue;
			}	
			status = stat(file_path,&stat_buff);		

			if ( check_acls(file_path) != 0) {
				WriteReport();
				printf("%s\n",szLog);
				szLog[0] = '\0';
				acl_status = 1;
			}
			else
			{
				//do nothing
			}



		}

	}

	else
	{
		status = stat(basic_acl.f_file,&stat_buff);

		if (status == -1)
		{
			sprintf(szLog,"ERROR: File \"%s\" has been removed: ACLs cannot be processed \n",basic_acl.f_file);
			WriteReport();
			printf("%s\n",szLog);
			szLog[0]='\0';
			acl_status = 1;
			return -1;
		}
		else
		{
			if ( check_acls(basic_acl.f_file) != 0) {
				WriteReport();
				printf("%s\n",szLog);
				szLog[0] = '\0';
				acl_status = 1;

			}
			else
			{
				//do nothing
			}

		}
	}

	return 0;

}

/*===================================================================
   ROUTINE: check_acls
=================================================================== */


int ACS_SEC_Integrity::check_acls(char* f){

	int status = 0;
	char owner[] = "---";
	char group[] = "---";
	char others[] = "---";
	char special[] = "---";
	char perm[4];
	char owner_str[LINE_SIZE];
	char group_str[LINE_SIZE];
	char file_perm[3];
	char spl_perm_str[LINE_SIZE];
	char stat_spl_perm_str[LINE_SIZE];
	char *p[3] = {0};
	char printstr[1000];
	szLog[0] = '\0';
	printstr[0] = '\0';

	strncpy(file_perm,basic_acl.f_base_perm,strlen(basic_acl.f_base_perm));


	/* Retrieve owner's name if it is found using getpwuid(). */
	if (f_owner_flag == 0) {
		if ((pwd = getpwuid(stat_buff.st_uid)) != NULL){
			strncpy(owner_str,pwd->pw_name,strlen(pwd->pw_name)+1);
			if (strncmp(owner_str,basic_acl.f_owner,strlen(basic_acl.f_owner)) != 0){
				status = -1;
			}

		}
		else
			status = -1;
	}

	/* Retrieve group name if it is found using getgrgid(). */
	if (f_group_flag == 0){
		if ((grp = getgrgid(stat_buff.st_gid)) != NULL){
			strncpy(group_str,grp->gr_name,strlen(grp->gr_name)+1);
			if (strncmp(group_str,basic_acl.f_group,strlen(basic_acl.f_group)) != 0){
				status = -1;
			}
		}
		else
			status = -1;
	}


	if (f_b_perm_flag == 0){
		/* Retrieve base permissions of the file. */
		sprintf(perm,"%3o",0777 & stat_buff.st_mode);

		if (strncmp(perm,basic_acl.f_base_perm,strlen(basic_acl.f_base_perm)) != 0){
			status = -1;
		}


		/* Retrieve individual permissions for owner, group and others */
		for (int i = 0; i <= 2; i++){
			char tmp = file_perm[i];
			int a = tmp - '0';
			p[i] = base_perm_arry[a];
		}


		if( S_IRUSR & stat_buff.st_mode ) owner[0] = 'r';
		if( S_IWUSR & stat_buff.st_mode ) owner[1] = 'w';
		if( S_IXUSR & stat_buff.st_mode ) owner[2] = 'x';

		if( S_IRGRP & stat_buff.st_mode ) group[0] = 'r';
		if( S_IWGRP & stat_buff.st_mode ) group[1] = 'w';
		if( S_IXGRP & stat_buff.st_mode ) group[2] = 'x';

		if( S_IROTH & stat_buff.st_mode ) others[0] = 'r';
		if( S_IWOTH & stat_buff.st_mode ) others[1] = 'w';
		if( S_IXOTH & stat_buff.st_mode ) others[2] = 'x';


		if (strncmp(p[0],owner,strlen(owner)) != 0){
			status = -1;
		}

		if (strncmp(p[1],group,strlen(group)) != 0){
			status = -1;
		}

		if (strncmp(p[2],others,strlen(others)) != 0){
			status = -1;
		}
	}

	if (f_s_perm_flag == 0){ 
		/* Retrieve special permissions for the file */
		if ( S_ISUID & stat_buff.st_mode ){
			special[0] = 's';
		}
		if ( S_ISGID & stat_buff.st_mode ){
			special[1] = 's';
		}
		if ( S_ISVTX & stat_buff.st_mode ){
			special[2] = 't';
		}

		int spcl_perm = atoi(basic_acl.f_spcl_perm);
		if (strncmp(spcl_perm_arry[spcl_perm],special,strlen(special)) != 0){
			status = -1;
		}	



		//char spl_perm_str[LINE_SIZE];
		spl_perm_str[0] = '\0';

		switch (spcl_perm){

		case 1 :
			sprintf(spl_perm_str,"sticky-bit");
			break;

		case 2 :
			sprintf(spl_perm_str,"set-Group-Id bit");
			break;

		case 3 :
			sprintf(spl_perm_str,"sticky-bit,set-Group-ID bit");
			break;
		case 4 :
			sprintf(spl_perm_str,"setUID bit");
			break;
		case 5 :
			sprintf(spl_perm_str,"sticky-bit,setUID bit");
			break;
		case 6 :
			sprintf(spl_perm_str,"setUID bit,set-Group-ID bit");
			break;
		case 7 :
			sprintf(spl_perm_str,"sticky-bit,setUID bit,set-Group-ID bit");
			break;
		default:
			sprintf(spl_perm_str,"none"); 

		}	

		//char stat_spl_perm_str[LINE_SIZE];
		stat_spl_perm_str[0] = '\0';


		if (strcmp(special,spcl_perm_arry[1]) == 0)
			sprintf(stat_spl_perm_str,"sticky-bit");
		else if (strcmp(special,spcl_perm_arry[2]) == 0) 
			sprintf(stat_spl_perm_str,"set-Group-Id bit");
		else if (strcmp(special,spcl_perm_arry[3]) == 0) 
			sprintf(stat_spl_perm_str,"sticky-bit,set-Group-ID bit");
		else if (strcmp(special,spcl_perm_arry[4]) == 0) 
			sprintf(stat_spl_perm_str,"setUID bit");
		else if (strcmp(special,spcl_perm_arry[5]) == 0) 
			sprintf(stat_spl_perm_str,"sticky-bit,setUID bit");
		else if (strcmp(special,spcl_perm_arry[6]) == 0) 
			sprintf(stat_spl_perm_str,"setUID bit,set-Group-ID bit");
		else if (strcmp(special,spcl_perm_arry[7]) == 0) 
			sprintf(stat_spl_perm_str,"sticky-bit,setUID bit,set-Group-ID bit");
		else 
			sprintf(stat_spl_perm_str,"none");

		if (strcmp(spl_perm_str, stat_spl_perm_str) != 0){
			status = -1;
		}		
	}


	if (status == -1){
		sprintf(szLog,"WARNING: Wrong ACL found for file \"%s\" \n", f);
		sprintf(printstr,"Was ");
		strcat(szLog,printstr);
		printstr[0] = '\0';
		if (f_owner_flag == 0){
			sprintf(printstr,"[owner: %s] ,",basic_acl.f_owner);
			strcat(szLog,printstr);
			printstr[0] = '\0';
		}
		if (f_group_flag == 0){
			sprintf(printstr,"[groupowner: %s] ,",basic_acl.f_group);
			strcat(szLog,printstr);
			printstr[0] = '\0';
		}

		if (f_b_perm_flag == 0){
			sprintf(printstr,"[owner permission: %s] , [group permission: %s] , [others permission: %s] ,",p[0],p[1],p[2]);
			strcat(szLog,printstr);
			printstr[0] = '\0';
		}

		if (f_s_perm_flag == 0){
			sprintf(printstr,"[special permission: %s]",spl_perm_str);
			strcat(szLog,printstr);
			printstr[0] = '\0';
		}

		//sprintf(printstr,"Was [owner: %s] , [groupowner: %s] , [owner permission: %s] , [group permission: %s] , [others permission: %s] , [special permission: %s] \n",basic_acl.f_owner,basic_acl.f_group,p[0],p[1],p[2],spl_perm_str); 
		//strcat(szLog,printstr);
		//printstr[0] = '\0';
		sprintf(printstr,"\nIs ");
		strcat(szLog,printstr);
		printstr[0] = '\0';
		if (f_owner_flag == 0){
			sprintf(printstr,"[owner: %s] ,",owner_str);
			strcat(szLog,printstr);
			printstr[0] = '\0';
		}
		if (f_group_flag == 0){
			sprintf(printstr,"[groupowner: %s] ,",group_str);
			strcat(szLog,printstr);
			printstr[0] = '\0';
		}

		if (f_b_perm_flag == 0){
			sprintf(printstr,"[owner permission: %s] , [group permission: %s] , [others permission: %s] ,",owner,group,others);
			strcat(szLog,printstr);
			printstr[0] = '\0';
		}

		if (f_s_perm_flag == 0){
			sprintf(printstr,"[special permission: %s]",stat_spl_perm_str);
			strcat(szLog,printstr);
			printstr[0] = '\0';
		}

		return -1;
	}

	return 0;


}

/*===================================================================
   ROUTINE: verify_sha1sum
=================================================================== */


void ACS_SEC_Integrity::verify_sha1sum(){

	int status = 0;
	char line[LINE_SIZE]={'\0'};
	char sha1_key[LINE_SIZE]={'\0'};
	char sha1_file[LINE_SIZE]={'\0'};
	char sha1_generated_key[LINE_SIZE]={'\0'};

	const char* mode = "r";

	//	char * p1;
	char *p2 = NULL;
	char *p = NULL;

	while (fgets(line,LINE_SIZE,lctDataFile) != NULL){
		memset(sha1_key,'\0',LINE_SIZE);
		memset(sha1_file,'\0',LINE_SIZE);
		memset(sha1_generated_key,'\0',LINE_SIZE);
		p = line;
		szLog[0] = '\0';
		//		p1 = strchr(line,'\"');
		p2 = strrchr(line,'\"');
		if (p2 == NULL){
			memset(line,'\0',LINE_SIZE);
			continue;
		}
		int len = p2 - p - 1;
		p++;
		strncpy(sha1_file,p,len);
		sha1_file[len] = '\0';
		int exist_status = stat(sha1_file,&stat_sha1_buff);
		p2++;
		if ( *p2 == '\n' ){
			if (exist_status == -1){
				sprintf(szLog, "ERROR: Directory %s has been removed \n", sha1_file);
				WriteReport();
				printf("%s\n",szLog);
				szLog[0]='\0';
				memset(line,'\0',LINE_SIZE);
				continue;
			}
		}
		else {
			int key_len = strlen(p2);
			if (p2[key_len-1] == '\n')
				p2[key_len-1] = '\0';

			strncpy(sha1_key,p2,key_len);
			sha1_key[key_len] = '\0';

			if (exist_status == -1){
				sprintf(szLog, "ERROR: File %s has been removed \n", sha1_file);
				WriteReport();
				printf("%s\n",szLog);
				szLog[0]='\0';
				status = 1;
				memset(line,'\0',LINE_SIZE);
				continue;
			}
			//skip check for .pyc files
			if (strstr(sha1_file,".pyc") != 0)
    			{
        			continue;
    			}
			string cmdToLaunch("md5sum ");
			cmdToLaunch.append(sha1_file);
			cmdToLaunch.append("  2>&1 ");
			if (launchCmd(cmdToLaunch,mode,sha1_generated_key) != 0 ) {
				int len = strlen(sha1_generated_key);
				sha1_generated_key[len - 1] = '\0';
				sprintf(szLog, "sha1sum Failed:[%s] \n", sha1_generated_key);
				WriteReport();
				printf("%s\n",szLog);
				szLog[0]='\0';
				status = 1;

				memset(line,'\0',LINE_SIZE);
				continue;
			}

			if (strncmp(sha1_key,sha1_generated_key,strlen(sha1_key)) != 0 ){
				sprintf(szLog, "WARNING: File %s has been changed.\n", sha1_file);
				WriteReport();
				printf("%s\n",szLog);
				szLog[0]='\0';
				status = 1;
			}

		}
		memset(line,'\0',LINE_SIZE);
	}

	if (status == 0)
		cout<<"All files checked out OK"<<endl;

}  

/*===================================================================
   ROUTINE: verify_acls 
=================================================================== */

void  ACS_SEC_Integrity::verify_acls(){


	char line[LINE_SIZE];
	char *ch;

	while (fgets(line,sizeof(line),(FILE *)lctAclFile) != NULL){

		ch = line;
		while (*ch == ' ' || *ch == '\t')
			ch++;

		if (*ch == '#' || *ch == '\n' || *ch == '\0')
			continue;

		if ( strlen(ch) <=2)
			continue;

		if ( extractAcls(ch) != 0 ) {			
			sprintf(szLog,"WARNING: Wrong ACL format found : [%s]",line);
			WriteReport();
			printf("%s\n",szLog);
			szLog[0] = '\0';
			continue;
		}

		else
		{	if ( verify_stat_func() != 0 ) {
			continue;
		}
		}

	}

	if ( acl_status == 0 ){
		cout<<"ACL check OK"<<endl;
	}

}


/*===================================================================
   ROUTINE: Usage 
=================================================================== */

void ACS_SEC_Integrity::Usage(void)
{
	printf("\nintegrityap uses the Secure Hash Algorithm, SHA-1, for computing a condensed\n");
	printf("160 bit representation of a data file and stores this digest in a file\n");
	printf("together with the filename. SHA-1 is described in FIPS-180-1.\n");
	printf("\nUsage: integrityap \n");
	printf("\t\t-h  Prints this help\n");
}

/*===================================================================
   ROUTINE: OpenFiles 
=================================================================== */


int ACS_SEC_Integrity::OpenFiles(void){

	lctDataFile=fopen(SHA1_FILE,"r");
	if (lctDataFile == NULL){
		//printf("\nFailed to open checksum data file %s\n", SHA1_FILE);
		printf("Baseline file not found. Execute acs_lct_baseline_create to create baseline file.\n\n");
		return 3;
	}

	lctAclFile=fopen(ACL_FILE,"r");
	if (lctAclFile == NULL){
		printf("\nFailed to open ACL data file %s\n", ACL_FILE);
		return 4;	
	}

	lctReportFile=fopen(LCTREPORT_FILE,"w");
	if (lctReportFile == NULL){
		printf("\nFailed to create report file %s\n", LCTREPORT_FILE);
		return 5;
	}
	return 0;

}

/*===================================================================
   ROUTINE: CloseFiles 
=================================================================== */


int ACS_SEC_Integrity::CloseFiles(void){

	char buff[LINE_SIZE];

	if (lctDataFile != NULL) {
		if (fclose(lctDataFile) != 0){
			//cout<<"Error! fclose failed for file "<<SHA1_FILE<<endl;
			sprintf(buff,"Error! fclose failed for file '%s'",SHA1_FILE);
			log.Write(buff,LOG_LEVEL_ERROR);
			return 1;
		}
	}

	if (lctAclFile != NULL){
		if (fclose(lctAclFile) != 0){
			//cout<<"Error! fclose failed for file "<<ACL_FILE<<endl;
			sprintf(buff,"Error! fclose failed for file '%s'",ACL_FILE);
			log.Write(buff,LOG_LEVEL_ERROR);
			return 1;
		}
	}

	if (lctReportFile != NULL){
		if (fclose(lctReportFile) != 0){
			//cout<<"Error! fclose failed for file "<<LCTREPORT_FILE<<endl;
			sprintf(buff,"Error! fclose failed for file '%s'",LCTREPORT_FILE);			
			log.Write(buff,LOG_LEVEL_ERROR);
			return 1;
		}
	}
	return 0;

}

/*===================================================================
   ROUTINE: WriteReport 
=================================================================== */


void ACS_SEC_Integrity::WriteReport(void){

	char outstr[200];
	time_t t;
	struct tm *tmp;
	t = time(NULL);
	tmp = localtime(&t);

	if (strftime(outstr, sizeof(outstr),"[%H:%M:%S %D]",tmp) == 0) {
		fprintf(stderr, "strftime returned 0");
	}




	if (lctReportFile != NULL)
	{
		fprintf(lctReportFile, "%s integrityap: %s\n",outstr,szLog);
	}
	else
		fprintf(stderr, "integrityap: %s\n",szLog);	

	fflush(lctReportFile);


}


/*===================================================================
   ROUTINE: check_new_files
=================================================================== */



void ACS_SEC_Integrity::check_new_files(void){
	FILE *fp;
	char line[LINE_SIZE];
	char sha1_base_file[LINE_SIZE];
	char *ch;
	//	char *p1;
	char *p2;
	char fname[50][PATH_MAX];
	vector<string> mylist;
	int new_file_status = 0;
	bool ignoreDir=false;
	int new_dir_status = 0;

	for (int k=0; k < 50; k++)
		flist[k]='\0';

	fp=fopen(SHA1_BASE_FILE,"r");
	if (fp == NULL){
		printf("\nFailed to open file %s\n", SHA1_BASE_FILE);
		return;
	}

	while (fgets(line,sizeof(line),fp) != NULL){

		for (int k=0; k < 50; k++)
			flist[k]='\0';
		ignoreDir=false;
		includeFile=false;
		ch = line;
		int len;
		int flist_stat = 0;
		flist_size = 0;

		while (*ch == ' ' || *ch == '\t')
			ch++;

		if (*ch == '#' || *ch == '\n' || *ch == '\0')
			continue;

		if ( strlen(ch) <=2)
			continue;

		//		p1 = strchr(line,'\"');
		p2 = strrchr(line,'\"');
		len = p2 - ch - 1;
		ch++;
		strncpy(sha1_base_file,ch,len);
		sha1_base_file[len] = '\0';

		p2++;

		while (*p2 == ' ')
			p2++;

		if (strncmp(p2,bool_true,4) == 0 ){
			strncpy(sha1_bool,bool_true,4);
			len=strlen(bool_true);
			sha1_bool[len] = '\0';
		}

		if (strncmp(p2,bool_false,5) == 0 ){
			strncpy(sha1_bool,bool_false,5);
			len=strlen(bool_false);
			sha1_bool[len] = '\0';
		}

		if ((p2 = strchr(p2,' '))!=NULL){ // Search space

			while (*p2 == ' ')
				p2++;

			if  ( *p2 == '-' ){

				p2++;
				while (*p2 == ' ')
					p2++;
				int i = 0;

				while(*p2 != '\n') // End of line
				{
					int j=0;
					while (*p2 != ' ' && *p2 != '\n') //Max size of extension to be compared is 10 char (fx<10)
					{
						fname[i][j] = *p2;
						j++;
						p2++;
					}

					fname[i][j] = '\0';
					flist[i]=fname[i];
					flist_stat = 1;
					char filePath[LINE_SIZE]={'\0'};
					strcpy(filePath,sha1_base_file);
					strcat(filePath,"/");
					strcat(filePath,flist[i]);

					struct stat fileStat;
					if (stat(filePath, &fileStat) == 0 && S_ISDIR(fileStat.st_mode))
					{
						skipDir_list.push_back(filePath);
						ignoreDir=true;
					}
					i++;
					while (*p2 == ' ')
						p2++;
				}
				flist_size=i;
			}
			else if  ( *p2 == '+' ){

				p2++;
				while (*p2 == ' ')
					p2++;
				int i = 0;

				while(*p2 != '\n') // End of line
				{
					int j=0;
					while (*p2 != ' ' && *p2 != '\n') //Max size of extension to be compared is 10 char (fx<10)
					{
						fname[i][j] = *p2;
						j++;
						p2++;
					}

					fname[i][j] = '\0';
					flist[i]=fname[i];
					flist_stat = 1;
					char filePath[LINE_SIZE]={'\0'};
					strcpy(filePath,sha1_base_file);
					strcat(filePath,"/");
					strcat(filePath,flist[i]);
					AddFile_list.push_back(filePath);
					includeFile=true;
					i++;
					while (*p2 == ' ')
						p2++;
				}
				flist_size=i;
			}

		}	

		int retval = seek_dir(mylist,sha1_base_file,ignoreDir);

		if ( retval == 0 ){

			for(int k=0;k<(int)mylist.size();k++)
			{
				int exl_stat = 0;
				char p[LINE_SIZE];
				strcpy(p,mylist[k].c_str());
				int len1 = strlen(p);
				p[len1]='\0';

				if (flist_stat == 1){
					for(int k=0;k<flist_size;k++){
						char f[LINE_SIZE];
						strcpy(f,sha1_base_file);
						strcat(f,"/");
						strcat(f,flist[k]);

						len1 = strlen(f);
						f[len1]='\0';


						if (strcmp(p,f) == 0)
							exl_stat = 1;
						else if(!strcmp(sha1_base_file,"/opt/com/lib") || !strcmp(sha1_base_file,"/usr/lib")) //ENM OAM_SA support, Check for string matches with Regular expression	//Fix for TR HY43823 - BSC Tool Impacts on APG
						{
							int status = REG_NOMATCH;
							regex_t re;
							if (regcomp(&re, f, REG_EXTENDED|REG_NOSUB) == 0)
							{
								status = regexec(&re, p, (size_t)0, NULL, 0);
								regfree(&re);
							}
							if(status == 0)
							{
								syslog(LOG_INFO,"IntegrityAP : Matching string %s with regular expression %s", p,f);
								exl_stat = 1;
							}
						}
					}
				}


				if (flist_size == 0 || exl_stat == 0){

					if (CheckIfNew(p))
					{
						string temp=mylist[k];
						char f_type[LINE_SIZE];
						char f_dir[]="directory";
						char f_file[]="file";

						int f_status = stat(temp.c_str(),&stat_buff);

						if (f_status != -1)
						{
							if( S_IFDIR & stat_buff.st_mode ) {
								strcpy(f_type,f_dir);
								int n = strlen(f_dir);
								f_type[n] = '\0';
								new_dir_status = 1;
							}

							if( S_IFREG & stat_buff.st_mode ) {
								strcpy(f_type,f_file);
								int m = strlen(f_file);
								f_type[m] = '\0';
								new_file_status = 1;
							}


						}


						sprintf(szLog, "WARNING: New %s found: %s",f_type,temp.c_str());
						WriteReport();
						printf("\n%s\n", szLog);
						szLog[0] = '\0';
					}
				}
			}

		}

		mylist.clear();
		skipDir_list.clear();	
	}

	if ( new_file_status == 0 ){
		cout<<"No new files found"<<endl;
	}



	if ( new_dir_status == 0 ){
		cout<<"No new directories found"<<endl;
	}

	fclose(fp);

}


/*===================================================================
   ROUTINE: CheckIfNew 
=================================================================== */

bool ACS_SEC_Integrity::CheckIfNew(string filename)
{
	char line[LINE_SIZE]={'\0'};
	char *p = NULL;
	char *p2 = NULL;
	char sha1_file[LINE_SIZE]={'\0'};

	fseek(lctDataFile,0L,SEEK_SET);
	while (fgets(line,LINE_SIZE,lctDataFile) != NULL){
		p = line;
		//szLog[0] = '\0';
		p2 = strrchr(line,'\"');
		if (p2 == NULL){
			memset(line,'\0',LINE_SIZE);
			memset(sha1_file,'\0',LINE_SIZE);
			continue;
		}	
		int len = p2 - p - 1;
		p++;
		memset(sha1_file,'\0',LINE_SIZE);
		strncpy(sha1_file,p,len);
		//sha1_file[len] = '\0';
		if (!strcmp(sha1_file,filename.c_str()))
		{
			return 0;
		}
		memset(line,'\0',LINE_SIZE);
	}
	return 1;
} 

/*===================================================================
   ROUTINE: seek_dir
=================================================================== */


int ACS_SEC_Integrity::seek_dir(vector<string>& mylist, string path, bool ignoreDir)
{
	char buff[LINE_SIZE];
	//bool skipDir_check = false;
	DIR* dir = opendir(path.c_str());
	if(dir == 0){
		//cout << "Can not open directory '" << path << "'\n";
		sprintf(buff,"Can not open directory '%s'",path.c_str());
		log.Write(buff,LOG_LEVEL_ERROR);
		printf("%s\n",buff);		
		return 1;
	}

	struct dirent *d;
	while( (d = readdir(dir)) != 0)
	{
		if( d->d_type == DT_DIR && strcmp(d->d_name,".") != 0 &&
				strcmp(d->d_name,"..") != 0)
		{
			bool skipDir_check = false;
			string p = path + "/" + d->d_name;
			if(true == ignoreDir)
			{
				for(int q=0;q<(int)skipDir_list.size();q++)
				{
					if (0 == p.compare(skipDir_list[q]))
					{
						skipDir_check = true;
						break;
					}
				}
				if(true != skipDir_check){
					mylist.push_back(p);
					if (strcmp(sha1_bool,bool_true) == 0){
						seek_dir(mylist,p,true);
					}
				}	
			}
			else { 
				if (true == includeFile){
					for(int q=0;q<(int)AddFile_list.size();q++)
					{
						if (0 == p.compare(AddFile_list[q]))
						{
							mylist.push_back(p);
							if (strcmp(sha1_bool,bool_true) == 0){
								seek_dir(mylist,p);
							}
						}	
					}
				}
				else{
					mylist.push_back(p);
					if (strcmp(sha1_bool,bool_true) == 0){
						seek_dir(mylist,p);
					}
				}
			}
		}
		else if(d->d_type == DT_REG)
		{
			string temp = path + "/" + d->d_name;
			if (true == includeFile){
				for(int q=0;q<(int)AddFile_list.size();q++)
				{
					if (0 == temp.compare(AddFile_list[q]))
					{
						mylist.push_back(temp);
						break;
					}	
				}
			}
			else {
				mylist.push_back(temp);
			}	
		}

	}
	closedir(dir);
	return 0;
}
int ACS_SEC_Integrity::verify_baseline_file(void)
{
	struct stat st;
	int fd = open(SHA1_FILE,O_RDONLY); 
	if (-1 == fd)
	{
		printf("Baseline file not found. Execute acs_lct_baseline_create to create baseline file.\n\n");
		return 3;
	}
	if (fstat(fd, &st))
	{
		log.Write("Error! unable to open SHA1_FILE", LOG_LEVEL_ERROR);
		close(fd);
		return 3;
	}
	long sz=st.st_size;
	if (sz == 0)
	{
		printf("Baseline file not found. Execute acs_lct_baseline_create to create baseline file.\n\n");
		close(fd);
		return 3;
	}
	close(fd);
	return 0;
}
/*=================================================================== */
/**
   @brief           Main method for entire functionality of IntegrityAP

   main:       Main Method


   @return          int

 **/
/*=================================================================== */


int main(int argc, char **argv){

	int rCode=0;
	ACS_SEC_Integrity *IntegrityAp= new ACS_SEC_Integrity;

	if (argc > 2){
		cout<<endl<<"Incorrect usage";
		for(int argcItr=1;argcItr<argc;argcItr++)
			cout<<" "<<argv[argcItr];
		cout<<endl;
		return 2;
	}

	if (argc > 1){
		if (strcmp(argv[1],"-h") == 0){
			IntegrityAp->Usage();
			return 0;
		}
		else
		{
			cout<<endl<<"Incorrect usage "<<argv[1]<<endl;
			return 2;
		}
	}	

	rCode = IntegrityAp->OpenFiles();
	if(rCode != 0)
		return rCode;
	int baseline_result=IntegrityAp->verify_baseline_file();
	if(baseline_result != 0)
		return baseline_result;
	IntegrityAp->verify_sha1sum();
	cout<<endl<<"Checking for new files and directories..."<<endl;
	IntegrityAp->check_new_files();
	cout<<endl<<"Verifying Access Control Lists....."<<endl;
	IntegrityAp->verify_acls();

	if((IntegrityAp->CloseFiles()) != 0)
	{
		cout<<endl<<"Error when executing (General fault)"<<endl;
		return 1;
	}
	return 0;
}


//============================================================================
// Name        : immDemo_2.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <OmHandler.h>


using namespace std;

int printMenu();
int printAttr();
int printAttrToRetrieve();
std::string getAttributeTypeStr(int );

int printMenu(){
	int value;

	cout<<"Select an operation: "
		<<"\n 1. create a new IMM object "
		<<"\n 2. retrieve the value of an attribute to an IMM object"
		<<"\n 3. modify the value of an attribute to an IMM object"
		<<"\n 4. remove a IMM object"
		<<"\n - ";
	cin>>value;

	return value;
}

int printAttr(){

	int value;
	cout<<endl;
	cout<<"Select an attribute: "
		<<"\n*----------------------------------------------*"
		<<"\n*\t Name Attribute\t\t Type          *"
		<<"\n*----------------------------------------------*"
		<<"\n 1.\t ATTRIBUTE_1\t\t SA_FLOAT_T"
		<<"\n 2.\t ATTRIBUTE_2\t\t SA_INT64_T"
		<<"\n 3.\t ATTRIBUTE_3\t\t SA_INT32_T"
		<<"\n 4.\t ATTRIBUTE_4\t\t SA_DOUBLE_T"
		<<"\n 5.\t ATTRIBUTE_5\t\t SA_UINT64_T"
		<<"\n 6.\t ATTRIBUTE_6\t\t SA_UINT32_T"
		<<"\n 7.\t ATTRIBUTE_7\t\t SA_NAME_T"
		<<"\n 8.\t ATTRIBUTE_8\t\t SA_STRING_T"
		<<"\n - ";
	cin>>value;

	return value;
}

int printAttrToRetrieve(){

	int value;
	cout<<endl;
	cout<<"Select an attribute: "

		<<"\n 1.\t ATTRIBUTE_1\t 5.\t ATTRIBUTE_5"
		<<"\n 2.\t ATTRIBUTE_2\t 6.\t ATTRIBUTE_6"
		<<"\n 3.\t ATTRIBUTE_3\t 7.\t ATTRIBUTE_7"
		<<"\n 4.\t ATTRIBUTE_4\t 8.\t ATTRIBUTE_8"
		<<"\n - ";
	cin>>value;
	cout<<endl;

	return value;
}

std::string getAttributeTypeStr(int value){

	std::string str;

	switch(value){
		case 1:
			str = "ATTR_INT32T\0";
		break;

		case 2:
			str = "ATTR_UINT32T\0";
		break;

		case 3:
			str = "ATTR_INT64T\0";
		break;

		case 4:
			str = "ATTR_UINT64T\0";
		break;

		case 5:
			str = "ATTR_TIMET\0";
		break;

		case 6:
			str = "ATTR_NAMET\0";
		break;

		case 7:
			str = "ATTR_FLOATT\0";
		break;

		case 8:
			str = "ATTR_DOUBLET\0";
		break;

		case 9:
			str = "ATTR_STRINGT\0";
		break;

		case 10:
			str = "ATTR_ANYT\0";
		break;

		default:
			str = "ERROR\0";
		break;

	}
	return str;

}

int main() {



	cout<<"*--------------------------------------------------------------------*\n"
		<<"*                                                                    *\n"
		<<"*             IMM CREATE/DELETE/SET/GET OBJECT DEMO                  *\n"
		<<"*                                                                    *\n"
		<<"*--------------------------------------------------------------------*\n"
		<<endl;

	int choice = 0;
	int choice2 = 0;
	int numAttr = 9;
	char cont;
	ACS_CC_ReturnType returnCode;

	char attrdn[] = "RDN_Attrbute";
	char attrFloat[] = "ATTRIBUTE_1";
	char name_attint64[] = "ATTRIBUTE_2";
	char name_attint32[] = "ATTRIBUTE_3";
	char name_attDouble[] = "ATTRIBUTE_4";
	char name_attUint64[] = "ATTRIBUTE_5";
	char name_attUint32[] = "ATTRIBUTE_6";
	char name_attnameT[] = "ATTRIBUTE_7";
	char name_attString[] = "ATTRIBUTE_8";

	/*parent name*/
	char* parentName = const_cast<char*>("safApp=safImmService");

	/*class name*/
	//char* className = const_cast<char*>("Test_config");
	char className[30];

	/*the vector*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;

	//View the start menu
	choice = printMenu();

	ACS_CC_ValuesDefinitionType *attributes;
	attributes = new ACS_CC_ValuesDefinitionType[numAttr];

	OmHandler omHandler;

	if(choice == 1){

		char dn[50];
		float valueflo;
		long long valueI64;
		int valueI32;
		double valueDo;
		unsigned long long valueUnsInt64;
		unsigned int valueUnsInt32;
		char valueName[250];
		char valueStr[250];
		char* provaStr;

		cout << "\nInsert the name of class \n-";
		cin>>className;

		cout<<"\nInsert the relative distinguished name of the Object: \n-";
		cin>>dn;

		attributes[0].attrName = attrdn;
		attributes[0].attrType = ATTR_NAMET;
		attributes[0].attrValuesNum = 1;
		attributes[0].attrValues = new void*[1];
		attributes[0].attrValues[0] = reinterpret_cast<void*>(dn);
		AttrList.push_back(attributes[0]);


		do{

			choice2 = printAttr();
			switch(choice2){

				case 1:
					cout<<"\n Insert the value for attribute "<<attrFloat<<":\n - ";
					cin>>valueflo;

					attributes[1].attrName = attrFloat;
					attributes[1].attrType = ATTR_FLOATT;
					attributes[1].attrValuesNum = 1;
					attributes[1].attrValues = new void*[1];
					attributes[1].attrValues[0] = reinterpret_cast<void*>(&valueflo);
					AttrList.push_back(attributes[1]);
				break;

				case 2:
					cout<<"\n Insert the value for attribute "<<name_attint64<<":\n - ";
					cin>>valueI64;

					attributes[2].attrName = name_attint64;
					attributes[2].attrType = ATTR_INT64T;
					attributes[2].attrValuesNum = 1;
					attributes[2].attrValues = new void*[1];
					attributes[2].attrValues[0] = reinterpret_cast<void*>(&valueI64);
					AttrList.push_back(attributes[2]);
				break;

				case 3:
					cout<<"\n Insert the value for attribute "<<name_attint32<<":\n - ";
					cin>>valueI32;

					attributes[3].attrName = name_attint32;
					attributes[3].attrType = ATTR_INT32T;
					attributes[3].attrValuesNum = 1;
					attributes[3].attrValues = new void*[1];
					attributes[3].attrValues[0] = reinterpret_cast<void*>(&valueI32);
					AttrList.push_back(attributes[3]);
				break;

				case 4:
					cout<<"\n Insert the value for attribute "<<name_attDouble<<":\n - ";
					cin>>valueDo;

					attributes[4].attrName = name_attDouble;
					attributes[4].attrType = ATTR_DOUBLET;
					attributes[4].attrValuesNum = 1;
					attributes[4].attrValues = new void*[1];
					attributes[4].attrValues[0] = reinterpret_cast<void*>(&valueDo);
					AttrList.push_back(attributes[4]);
				break;

				case 5:
					cout<<"\n Insert the value for attribute "<<name_attUint64<<":\n - ";
					cin>>valueUnsInt64;

					attributes[5].attrName = name_attUint64;
					attributes[5].attrType = ATTR_UINT64T;
					attributes[5].attrValuesNum = 1;
					attributes[5].attrValues = new void*[1];
					attributes[5].attrValues[0] = reinterpret_cast<void*>(&valueUnsInt64);
					AttrList.push_back(attributes[5]);
				break;

				case 6:
					cout<<"\n Insert the value for attribute "<<name_attUint32<<":\n - ";
					cin>>valueUnsInt32;

					attributes[6].attrName = name_attUint32;
					attributes[6].attrType = ATTR_UINT32T;
					attributes[6].attrValuesNum = 1;
					attributes[6].attrValues = new void*[1];
					attributes[6].attrValues[0] = reinterpret_cast<void*>(&valueUnsInt32);
					AttrList.push_back(attributes[6]);
				break;

				case 7:
					cout<<"\n Insert the value for attribute "<<name_attnameT<<":\n - ";
					cin.ignore();
					cin.getline(valueName, 250);

					attributes[7].attrName = name_attnameT;
					attributes[7].attrType = ATTR_NAMET;
					attributes[7].attrValuesNum = 1;
					attributes[7].attrValues = new void*[1];
					attributes[7].attrValues[0] = reinterpret_cast<void*>(valueName);
					AttrList.push_back(attributes[7]);
				break;

				case 8:
					cout<<"\n Insert the value for attribute "<<name_attString<<":\n - ";
					cin.ignore();
					cin.getline(valueStr, 250);

					cout<<"valueStr: "<<valueStr<<endl;
					int len=strlen(valueStr);
					provaStr=(char*)malloc(len+1);
					memcpy(provaStr, valueStr, len);
					attributes[8].attrName = name_attString;
					attributes[8].attrType = ATTR_STRINGT;
					attributes[8].attrValuesNum = 1;
					attributes[8].attrValues = new void*[1];
					attributes[8].attrValues[0] = reinterpret_cast<void*>(provaStr);
					AttrList.push_back(attributes[8]);
				break;

			}
			cout<<"\nInsert the value for another attribute? (y/n)\n - ";
			do{
				cin.clear();
				cin>>cont;
				cin.clear();
				if(cont != 'y' && cont != 'n')
					cout<<"Incorrect Value. Retry\n";
			}while(cont != 'y' && cont != 'n');


		}while(cont == 'y');

		cout<<endl;

		if (omHandler.Init() == ACS_CC_FAILURE) {
			cout<<"ERROR: Initialization FAILURE\n";
			return -1;
		}

		if (omHandler.createObject(className, parentName, AttrList ) == ACS_CC_FAILURE){
			cout<<"ERROR: createObject FAILURE\n";
			return -1;
		} else {
			cout<<"Object "<<dn<<","<<parentName<<" created with success"<<endl;
		}

		if (omHandler.Finalize() == ACS_CC_FAILURE){
			cout<<"ERROR: Finalization FAILURE\n";
			return -1;
		}

	}else if(choice == 2){

		ACS_CC_ImmParameter paramToFind;
		int value = 0;
		char dn[50];

		//omHandler.Init();
		if (omHandler.Init() == ACS_CC_FAILURE) {
			cout<<"ERROR: Initialization FAILURE\n";
			return -1;
		}

		cout<<"\nInsert DN of the object containing the value to be retrieved\n - ";
		cin>>dn;

		do{

			value = printAttrToRetrieve();

			switch(value){

				case 1:
					paramToFind.attrName = attrFloat;
					if (omHandler.getAttribute(dn, &paramToFind ) != 0){
						cout<<"getAttribute FAILURE\n";
						return -1;
					}else{
						cout<<endl;
						cout<<"Attribute name: "<<paramToFind.attrName<<endl;
						cout<<"Attribute type: "<<getAttributeTypeStr(paramToFind.attrType)<<endl;
						cout<<"Attribute number values: "<<paramToFind.attrValuesNum<<endl;
						if(paramToFind.attrValuesNum == 0){
							cout<<"The attribute has no value set\n";
						}else{
							cout<<"Attribute value: "<<*(reinterpret_cast<float*>(*(paramToFind.attrValues)))<<endl;

						}
						cout<<endl;
					}
				break;

				case 2:

					paramToFind.attrName = name_attint64;
					if (omHandler.getAttribute(dn, &paramToFind ) != 0){
						cout<<"getAttribute FAILURE\n";
						return -1;
					}else{
						cout<<endl;
						cout<<"Attribute name: "<<paramToFind.attrName<<endl;
						cout<<"Attribute type: "<<getAttributeTypeStr(paramToFind.attrType)<<endl;
						cout<<"Attribute number values: "<<paramToFind.attrValuesNum<<endl;
						if(paramToFind.attrValuesNum == 0){
							cout<<"The attribute has no value set\n";
						}else{
							cout<<"Attribute value: "<<*(reinterpret_cast<long long*>(*(paramToFind.attrValues)))<<endl;

						}
						cout<<endl;
					}
				break;

				case 3:

					paramToFind.attrName=name_attint32;
					if (omHandler.getAttribute(dn, &paramToFind ) != 0){
						cout<<"getAttribute FAILURE\n";
						return -1;
					}else{
						cout<<endl;
						cout<<"Attribute name: "<<paramToFind.attrName<<endl;
						cout<<"Attribute type: "<<getAttributeTypeStr(paramToFind.attrType)<<endl;
						cout<<"Attribute number values: "<<paramToFind.attrValuesNum<<endl;
						if(paramToFind.attrValuesNum == 0){
							cout<<"The attribute has no value set\n";
						}else{
							cout<<"Attribute value: "<<*(reinterpret_cast<int*>(*(paramToFind.attrValues)))<<endl;

						}
						cout<<endl;
					}

				break;

				case 4:

					paramToFind.attrName = name_attDouble;
					if (omHandler.getAttribute(dn, &paramToFind ) != 0){
						cout<<"getAttribute FAILURE\n";
						return -1;
					}else{
						cout<<endl;
						cout<<"Attribute name: "<<paramToFind.attrName<<endl;
						cout<<"Attribute type: "<<getAttributeTypeStr(paramToFind.attrType)<<endl;
						cout<<"Attribute number values: "<<paramToFind.attrValuesNum<<endl;
						if(paramToFind.attrValuesNum == 0){
							cout<<"The attribute has no value set\n";
						}else{
							cout<<"Attribute value: "<<*(reinterpret_cast<double*>(*(paramToFind.attrValues)))<<endl;

						}
						cout<<endl;
					}

				break;

				case 5:

					paramToFind.attrName = name_attUint64;
					if (omHandler.getAttribute(dn, &paramToFind ) != 0){
						cout<<"getAttribute FAILURE\n";
						return -1;
					}else{
						cout<<endl;
						cout<<"Attribute name: "<<paramToFind.attrName<<endl;
						cout<<"Attribute type: "<<getAttributeTypeStr(paramToFind.attrType)<<endl;
						cout<<"Attribute number values: "<<paramToFind.attrValuesNum<<endl;
						if(paramToFind.attrValuesNum == 0){
							cout<<"The attribute has no value set\n";
						}else{
							cout<<"Attribute value: "<<*(reinterpret_cast<unsigned long long*>(*(paramToFind.attrValues)))<<endl;

						}
						cout<<endl;
					}

				break;

				case 6:

					paramToFind.attrName = name_attUint32;
					if (omHandler.getAttribute(dn, &paramToFind ) != 0){
						cout<<"getAttribute FAILURE\n";
						return -1;
					}else{
						cout<<endl;
						cout<<"Attribute name: "<<paramToFind.attrName<<endl;
						cout<<"Attribute type: "<<getAttributeTypeStr(paramToFind.attrType)<<endl;
						cout<<"Attribute number values: "<<paramToFind.attrValuesNum<<endl;
						if(paramToFind.attrValuesNum == 0){
							cout<<"The attribute has no value set\n";
						}else{
							cout<<"Attribute value: "<<*(reinterpret_cast<unsigned int*>(*(paramToFind.attrValues)))<<endl;

						}
						cout<<endl;
					}

				break;

				case 7:

					paramToFind.attrName = name_attnameT;
					if (omHandler.getAttribute(dn, &paramToFind ) != 0){
						cout<<"getAttribute FAILURE\n";
						return -1;
					}else{
						cout<<endl;
						cout<<"Attribute name: "<<paramToFind.attrName<<endl;
						cout<<"Attribute type: "<<getAttributeTypeStr(paramToFind.attrType)<<endl;
						cout<<"Attribute number values: "<<paramToFind.attrValuesNum<<endl;
						if(paramToFind.attrValuesNum == 0){
							cout<<"The attribute has no value set\n";
						}else{
							cout<<"Attribute value: "<<(reinterpret_cast<char*>(*(paramToFind.attrValues)))<<endl;

						}
						cout<<endl;
					}

				break;

				case 8:

					paramToFind.attrName = name_attString;
					if (omHandler.getAttribute(dn, &paramToFind ) != 0){
						cout<<"getAttribute FAILURE\n";
						return -1;
					}else{
						cout<<endl;
						cout<<"Attribute name: "<<paramToFind.attrName<<endl;
						cout<<"Attribute type: "<<getAttributeTypeStr(paramToFind.attrType)<<endl;
						cout<<"Attribute number values: "<<paramToFind.attrValuesNum<<endl;
						if(paramToFind.attrValuesNum == 0){
							cout<<"The attribute has no value set\n";
						}else{
							cout<<"Attribute value: "<<(reinterpret_cast<char*>(*(paramToFind.attrValues)))<<endl;

						}
						cout<<endl;
					}

				break;

				default:
					cout<<"Incorrect value\n";
				break;
			}

			cout<<"Retrieve the value for another attribute? (y/n)\n - ";
			do{
				cin.clear();
				cin>>cont;
				cin.clear();
				if(cont != 'y' && cont != 'n')
					cout<<"Incorrect Value. Retry\n - ";
			}while(cont != 'y' && cont != 'n');


		}while(cont == 'y');

		omHandler.Finalize();

	}else if(choice == 3){

		ACS_CC_ImmParameter paramToChange;
		char dn[50];
		int value;

		//omHandler.Init();
		if (omHandler.Init() == ACS_CC_FAILURE) {
			cout<<"ERROR: Initialization FAILURE\n";
			return -1;
		}
		cout<<"\nInsert DN of the object containing the value to be retrieved\n - ";
		cin>>dn;

		do{

			value = printAttr();
			cout<<endl;
			switch(value){

				case 1:
					float valueFlo;
					cout<<"Insert the new value for the attribute "<<attrFloat<<"\n - ";
					cin>>valueFlo;

					paramToChange.attrName = attrFloat;
					paramToChange.attrType = ATTR_FLOATT;
					paramToChange.attrValuesNum = 1;
					paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
					paramToChange.attrValues[0] = reinterpret_cast<void*>(&valueFlo);

					returnCode = omHandler.modifyAttribute(dn, &paramToChange);
					cout<<endl;
					if(returnCode == ACS_CC_SUCCESS){
						cout<<"attribute successfully changed\n";
					}else {
						cout<<"modifyAttribute FAILURE\n";
					}


				break;

				case 2:

					long long valueInt64;
					cout<<"Insert the new value for the attribute "<<name_attint64<<"\n - ";
					cin>>valueInt64;

					paramToChange.attrName = name_attint64;
					paramToChange.attrType = ATTR_INT64T;
					paramToChange.attrValuesNum = 1;
					paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
					paramToChange.attrValues[0] = reinterpret_cast<void*>(&valueInt64);

					returnCode = omHandler.modifyAttribute(dn, &paramToChange);
					cout<<endl;
					if(returnCode == ACS_CC_SUCCESS){
						cout<<"attribute successfully changed\n";
					}else {
						cout<<"modifyAttribute FAILURE\n";
					}


				break;

				case 3:

					int valueInt32;
					cout<<"Insert the new value for the attribute "<<name_attint32<<"\n - ";
					cin>>valueInt32;

					paramToChange.attrName = name_attint32;
					paramToChange.attrType = ATTR_INT32T;
					paramToChange.attrValuesNum = 1;
					paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
					paramToChange.attrValues[0] = reinterpret_cast<void*>(&valueInt32);

					returnCode = omHandler.modifyAttribute(dn, &paramToChange);
					cout<<endl;
					if(returnCode == ACS_CC_SUCCESS){
						cout<<"attribute successfully changed\n";
					}else {
						cout<<"modifyAttribute FAILURE\n";
					}

				break;

				case 4:

					double valueDouble;
					cout<<"Insert the new value for the attribute "<<name_attDouble<<"\n - ";
					cin>>valueDouble;

					paramToChange.attrName = name_attDouble;
					paramToChange.attrType = ATTR_DOUBLET;
					paramToChange.attrValuesNum = 1;
					paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
					paramToChange.attrValues[0] = reinterpret_cast<void*>(&valueDouble);

					returnCode = omHandler.modifyAttribute(dn, &paramToChange);
					cout<<endl;
					if(returnCode == ACS_CC_SUCCESS){
						cout<<"attribute successfully changed\n";
					}else {
						cout<<"modifyAttribute FAILURE\n";
					}

				break;

				case 5:

					unsigned long long valueUint64;
					cout<<"Insert the new value for the attribute "<<name_attUint64<<"\n - ";
					cin>>valueUint64;

					paramToChange.attrName = name_attUint64;
					paramToChange.attrType = ATTR_UINT64T;
					paramToChange.attrValuesNum = 1;
					paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
					paramToChange.attrValues[0] = reinterpret_cast<void*>(&valueUint64);

					returnCode = omHandler.modifyAttribute(dn, &paramToChange);
					cout<<endl;
					if(returnCode == ACS_CC_SUCCESS){
						cout<<"attribute successfully changed\n";
					}else {
						cout<<"modifyAttribute FAILURE\n";
					}

				break;

				case 6:

					unsigned int valueUint32;
					cout<<"Insert the new value for the attribute "<<name_attUint32<<"\n - ";
					cin>>valueUint32;

					paramToChange.attrName = name_attUint32;
					paramToChange.attrType = ATTR_UINT32T;
					paramToChange.attrValuesNum = 1;
					paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
					paramToChange.attrValues[0] = reinterpret_cast<void*>(&valueUint32);

					returnCode = omHandler.modifyAttribute(dn, &paramToChange);
					cout<<endl;
					if(returnCode == ACS_CC_SUCCESS){
						cout<<"attribute successfully changed\n";
					}else {
						cout<<"modifyAttribute FAILURE\n";
					}

				break;

				case 7:
				{
					char valueNameT[250];

					cout<<"Insert the new value for the attribute "<<name_attnameT<<"\n - ";
					//cin>>valueNameT;
					cin.ignore();
					cin.getline(valueNameT, 250);

					paramToChange.attrName = name_attnameT;
					paramToChange.attrType = ATTR_NAMET;
					paramToChange.attrValuesNum = 1;
					paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
					paramToChange.attrValues[0] = reinterpret_cast<void*>(valueNameT);

					returnCode = omHandler.modifyAttribute(dn, &paramToChange);
					cout<<endl;
					if(returnCode == ACS_CC_SUCCESS){
						cout<<"attribute successfully changed\n";
					}else {
						cout<<"modifyAttribute FAILURE\n";
					}

				break;
				}
				case 8:
					ACS_CC_ImmParameter paramToChange2;
					char  valueString[250];
					cout<<"Insert the new value for the attribute "<<name_attString<<"\n - ";
					//cin>>valueString;
					cin.ignore();
					cin.getline(valueString, 250);

					paramToChange2.attrName = name_attString;
					paramToChange2.attrType = ATTR_STRINGT;
					paramToChange2.attrValuesNum = 1;
					paramToChange2.attrValues = new void*[paramToChange2.attrValuesNum];
					paramToChange2.attrValues[0] = reinterpret_cast<void*>(valueString);

					returnCode=omHandler.modifyAttribute(dn, &paramToChange2);
					cout<<endl;
					if(returnCode == ACS_CC_SUCCESS){
						cout<<"attribute successfully changed\n";
					}else {
						cout<<"modifyAttribute FAILURE\n";
					}

				break;
			}

			cout<<"\nChange the value of another attribute? (y/n)\n - ";
			do{
				cin.clear();
				cin>>cont;
				cin.clear();
				if(cont != 'y' && cont != 'n')
					cout<<"Incorrect Value. Retry\n - ";
			}while(cont != 'y' && cont != 'n');


		}while(cont=='y');

		//omHandler.Finalize();
		if (omHandler.Finalize() == ACS_CC_FAILURE){
			cout<<"Finalization FAILURE\n";
			return -1;
		}


	}else if(choice == 4){
		//Delete object section
		char dn[256];
		cout << "\nInsert DN of class to be deleted\n - ";
		cin >> dn;
		cout << endl;

		if (omHandler.Init() == ACS_CC_FAILURE) {

			cout<<"initialization FAILURE\n";
			return -1;

		}


		if(omHandler.deleteObject(dn) == ACS_CC_FAILURE){
			cout<<"deleteObject FAILURE\n";
			return -1;
		}else{
			cout<<"Object "<<dn<<" successfully deleted\n";
		}


		if (omHandler.Finalize() == ACS_CC_FAILURE){
			cout<<"Finalization FAILURE\n";
			return -1;
		}


	}else{
		cout<<"ERROR: Incorrect value\n";
		return -1;
	}

	delete[] attributes;


	return 0;
}

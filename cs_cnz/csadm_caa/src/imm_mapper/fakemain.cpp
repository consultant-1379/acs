#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>


using namespace std;


int main(int argv, char** argc)
{

   if (1 && 1 && 1)
      cout << "1 && 1 && 1 == true" << endl;

   if (1 && 0 && 1)
      cout << "1 && 0 && 1 == true" << endl;

   if (0 && 0 && 0)
      cout << "0 && 0 && 0 == true" << endl;

   return 0;
}

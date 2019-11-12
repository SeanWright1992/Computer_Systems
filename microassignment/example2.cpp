#include <iostream>
#include <cstring>

using namespace std;

#define BUFFERSIZE 7


void setint(int* ip, int i)
{
  *ip = i;
} 


void write_message(const char* message)
{
  const short number = 0;
  char buffer[BUFFERSIZE];
  int length = strlen(message);
  if(length > BUFFERSIZE){
	  for(int i = 0; i<BUFFERSIZE;i++)
		  cout<<message[i];
	  cout <<endl;
  }
  else{
  memset(buffer, '\0', BUFFERSIZE);
  strcpy(buffer, message);
  cout << buffer << endl;
  }
	  
}


int main(int argc, char** argv)
{
  int a = -1;
  int* b = NULL;
  const char* message = NULL;

  if (argc < 2)
  {
    // If not command line arguments are given
    // then print an error message and exit
    cout << "Error: missing command line argument" << endl;
    cout << "Usage: ./example2 message" << endl;
    return 1;
  }
  else
  message = argv[1]; // message is the 1st command line argument
  
  setint(&a, 10);
  cout << a << endl;
  b = &a;
  setint(b, 20);
  cout << b << endl;

  write_message(message);
  return 0;
}

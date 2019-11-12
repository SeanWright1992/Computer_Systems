#include <iostream>

using namespace std;


void print_scrambled(const char* message)
{
  int scramble = 3;
  if(!message)
	  return;
  for (int i = 0; message[i] != '\0'; ++i) {
    char scrambled_char = message[i] + scramble;
    cout << scrambled_char;
  }
  cout << endl;
}


int main(int argc, char** argv)
{
  const char* bad_message = NULL;
  const char* good_message = "Hello, world.";

  print_scrambled(good_message);
  print_scrambled(bad_message);

  return 0;
}

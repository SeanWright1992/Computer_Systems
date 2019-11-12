#include <iostream>
#include <climits>

using namespace std;

#define ARRAY_SIZE 4


double slidingAverage(const int* numbers, int size)
{
  // Make a copy so we don't modify the original
  int* numCopy = new int[size];

  // Calculate sliding totals
  numCopy[0] = 0;
  for (int i = 1; i < size; ++i)
  {
    numCopy[i] = numbers[i] + numbers[i-1];
  }
	
  // Print the average of the sliding totals
  int sum = 0, count;
  for (count = 0; count < size; ++count)
  {
    sum += numCopy[count];
  }
  delete[] numCopy;
  return static_cast<double>(sum) / count;
}


int main(int argc, char** argv)
{
  double avg;
  int madeUpNumbers[ARRAY_SIZE] = { 9, 42, 256, (SHRT_MAX/256) };
  avg =slidingAverage(madeUpNumbers, ARRAY_SIZE);
  cout << "Sliding Average: " << avg << endl;
  return 0;
}

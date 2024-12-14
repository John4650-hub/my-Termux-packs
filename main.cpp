#include <iostream>
#include <limits>

using namespace std;

int main()
{
  int x;
  cout << "pls enter an integer: ";

  while(!(cin >> x))
  {
    cout << "Bad input, pls try again"<< endl;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
  }
  cout << "You entered "<< x << endl;
  return EXIT_SUCCESS;
}

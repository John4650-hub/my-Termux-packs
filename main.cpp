#include <iostream>
#include <string>

using namespace std;

int main()
{
  string line;
  cout << "plis enter a line of text: ";
  getline(cin, line);
  cout << "you entered: \"" << line << "\""<<endl;
  return 0;
}

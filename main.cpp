#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

void print_vector(const vector<int>& vec)
{
  cout << '{';
  int len = vec.size;
  if (len > 0){
    for (int i = 0; i < len - 1; i++)
      cout << vec[i]<<",";
    cout << vec[len-1];

  }
  cout << "}\n";
}

void saveVector(const string& filename,const vector<int>& vec){
  ofstream out(filename);
  if(out.good()){
    int n=vec.size()
    for (int i=0;i<n;i++)
      out << vec[i] << " ";
    out << '\n';
  }
  else
    cout << "Unable to save file \n";

}

void loadVector(const& filename, const vector<int>& vec){
  ifstream in(filename);
  if(in.good()){
    vec.clear();
    int value;
    while(in >> value)
      vec.push_back(value);
  }
  else
    cout  << "Unable to load file\n";
}

int main()
{
  vector<int> list;
  bool done=false;
  char command;
  while(!done){
    cout << "I)nsert <item> P)rint " << "S)ave <filename> L)oad <filename> " << "E)rase Q)uit: ";
    cin >> command;
    int value;
    string filename;
    switch(command){
      case 'I':
      case 'j':
	cin >> value;
	list.push_back;
	break;
      case 'P':
      case 'p':
	print_vector(list);
	break;
      case 'S':
      case 's':
	cin >> filename;
	saveVector(filename, list);
	break;
      case 'E':
      case 'e':
	list.clear();
	break;
      case 'L':
      case 'l':
	cin >> filename;
	loadVector(filename,list);
	break;
      case 'Q':
      case 'q':
	done = true;
	break;

    }
  }
  return EXIT_SUCCESS;
}

#include <iostream>

int add(int x, int y);

int main()
{
  int a{},b{};
  std::cin >> a;
  std::cin >> b;
  std::cout << add(a,b)<<"\n";
  return 0;
}

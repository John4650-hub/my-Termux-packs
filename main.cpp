#include <iostream>
#include <vector>

using std::vector;

void vectors(){
    vector<int> vec_a;//defines a vector for integers 
    vector<int> vec_b(10); // with size 10
    vector<int> vec_c(10,8);// with size 10 and has 8 as initial value
    vector<int> vec_d{1,2,3,4,5,6,7,8,9};
}

int main()
{
    vector<int> vec(10,4);
    std::cout << vec[5] << std::endl;
    for(auto i=0;i<10;i++){
        std::cout << vec[i] << std::endl;
    }
    return 0;
}

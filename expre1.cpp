#include <iostream>

using namespace std;

int main(){

    float a , b , result = 0.0 ;

    cout << "Enter the number a :\n "; cin >> a;
    cout << "Enter the number b :\n "; cin >> b;

    result = (a/b) + 1;

    cout.precision(3);
    cout << "The sum of a and b divided by b and 1 is : " << result << endl;

    return 0;
}
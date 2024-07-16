#include <iostream>

using namespace std;

int main()
{
    int num1, num2, sum1=0, rest=0, mult=0, div=0;

    cout << "Enter integers 1 = ";
    cin >> num1; 
    cout << "Enter integers 2 = ";
    cin >> num2;

    sum1 = num1 + num2;
    rest = num1 - num2;
    mult = num1 * num2;
    div = num1 / num2;

    cout << "\nSum of integers: " << sum1 << endl;
    cout << "Difference of integers: " << rest << endl;
    cout << "Product of integers: " << mult << endl;
    cout << "Quotient of integers: " << div << endl;
    
    return 0;
}

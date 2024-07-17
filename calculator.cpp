#include <iostream>

using namespace std;

int main() {
    int year;
    char sex[10];
    float height;

    cout << "Enter year:\n"; cin >> year;

    cout << "Enter sex male, female or other):\n"; cin >> sex;

    cout << "Enter height in meter:\n"; cin >> height;

    cout << "Your data has been saved: " << year << " year old " << sex << " " << height << " meter " << endl;

    return 0;
}

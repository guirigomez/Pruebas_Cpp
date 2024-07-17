#include <iostream>
using namespace std;

int main() {
    int rows, cols;

    cout << "Enter rows and columns for the matrix: ";
    cin >> rows >> cols;

    // Dynamically allocate memory for the matrix
    int** matrix = new int*[rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new int[cols];
    }

    cout << "Enter matrix elements:" << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cin >> matrix[i][j];
        }
    }

    cout << "Transpose of the matrix:" << endl;
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            cout << matrix[j][i] << " ";
        }
        cout << endl;
    }

    // Deallocate memory
    for (int i = 0; i < rows; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;

    return 0;
}

#include <iostream>
using namespace std;

int main() {
    bool* test = new bool[3];
    
    cout << test << endl;

    void *hmmm = test;

    cout << (int)hmmm + 1 << endl;

    delete[] test;

    return 0;
}
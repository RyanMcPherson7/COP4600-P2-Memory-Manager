#include <iostream>
using namespace std;

int main() {
    bool test[3];
    test[0] = 5;

    for (bool i : test) {
        cout << i << endl;
    }

    return 0;
}
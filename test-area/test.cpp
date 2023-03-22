#include <iostream>
#include "../MemoryManager/MemoryManager.h"
using namespace std;

int main() {
    // initialize
    // unsigned int  wordSize = 8;
    // size_t numberOfWords = 26;
    // MemoryManager memoryManager(wordSize, bestFit);
    // memoryManager.initialize(numberOfWords);

    // // do stuff
    // uint64_t* testArray1 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 10));
    // uint64_t* testArray2 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 2));
    // uint64_t* testArray3 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 2));
    // uint64_t* testArray4 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 6));
    // memoryManager.free(testArray1);
    // memoryManager.free(testArray3);

    // // gen bitmap
    // uint8_t* bitmap = static_cast<uint8_t*>(memoryManager.getBitmap());

    // for (int i = 0; i < 6; i++) {
    //     cout << std::hex <<  (int)bitmap[i] << endl;
    // }

    bool* ptr = new bool[3];
    ptr[1] = 7;
    ptr[2] = 8;

    cout << ptr[0] << " " << ptr[1] << " " << ptr[2] << endl;

    delete ptr;

    return 0;
}
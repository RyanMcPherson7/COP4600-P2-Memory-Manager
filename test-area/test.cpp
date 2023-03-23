#include <iostream>
#include <vector>
#include "../MemoryManager/MemoryManager.h"
using namespace std;

unsigned int testReadingUsingGetMemoryStart()
{
    std::cout << "Test Case: Reading memory using GetMemoryStart" << std::endl;
    unsigned int wordSize = 8;
    size_t numberOfWords = 80;
    MemoryManager memoryManager(wordSize, worstFit);
    memoryManager.initialize(numberOfWords);

    std::vector<uint64_t> arrayContent{1, 12, 13, 14, 15, 2, 22, 23, 24, 25, 3, 32, 33, 34, 35, 4, 42, 43, 44, 45, 5, 52, 53, 54, 55};

    std::vector<uint64_t>::const_iterator arrayContentItr = arrayContent.begin();
    std::vector<uint64_t*> testArrays;

    testArrays.push_back(static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 5)));
    testArrays.push_back(static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 5)));
    testArrays.push_back(static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 5)));
    testArrays.push_back(static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 5)));
    testArrays.push_back(static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 5)));
    
    for(auto testArray: testArrays) {
        for(uint16_t i = 0; i < 5; ++i) {
            testArray[i] = *arrayContentItr++;
        }
    }
    cout << "post assignment" << endl;

    unsigned int score = 0;

    arrayContentItr = arrayContent.begin();

    uint64_t* MemoryManagerContents = static_cast<uint64_t*>(memoryManager.getMemoryStart());
    cout << "before " << memoryManager.getMemoryStart() << endl;
    
    for(auto content: arrayContent) {
        if(content != *MemoryManagerContents++) {
            std::cout << "Expected: " << content << std::endl;
            std::cout << "Got: "  << *(MemoryManagerContents - 1) << std::endl;
            std::cout << "[INCORRECT]\n" << std::endl;
            return score;
        }
    }

    cout << "after " << memoryManager.getMemoryStart() << endl;

    ++score;

    // cout << "stuff" << endl;
    // cout << "test size " << sizeof(MemoryManagerContents) << endl;
    // MemoryManagerContents -= 25;
    // for (int i = 0; i < 25; i++) {
    //     cout << MemoryManagerContents[i] << endl;
    // }
    // cout << "end stuff" << endl;

    memoryManager.shutdown();
    std::cout << "[CORRECT]\n" << std::endl;
    return score;
}

int main() {
    testReadingUsingGetMemoryStart();

    return 0;
}
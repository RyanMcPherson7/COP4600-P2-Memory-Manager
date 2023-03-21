#include "MemoryManager.h"
#include <vector>

#include <iostream>

MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator) {
    wordSizeInBytes = wordSize;
    allocatorFunc = allocator;
    memorySizeInWords = 0;
    memoryList = nullptr;
}


MemoryManager::~MemoryManager() {
    shutdown();
}


void MemoryManager::initialize(size_t sizeInWords) {
    shutdown();

    if (sizeInWords > 65536) {
        sizeInWords = 65536;
    }

    this->memorySizeInWords = sizeInWords;
    memoryList = new bool[sizeInWords];

    /*
     * initialize all words to be free
     * 0 or false = free
     * 1 or true = in use
    */
    for (int i = 0; i < sizeInWords; i++) {
        memoryList[i] = 0;
    }
}


void MemoryManager::shutdown() {
    if (memoryList == nullptr) return;

    memorySizeInWords = 0;
    delete[] memoryList;
    memoryList = nullptr;

    // FIXME: remove all allocated addresses from hash set
}


void *MemoryManager::allocate(size_t sizeInBytes) {
    unsigned int neededWords =  sizeInBytes / wordSizeInBytes;

    if (sizeInBytes % wordSizeInBytes != 0 || neededWords > memorySizeInWords) {
        return nullptr;
    }

    int wordOffset = allocatorFunc(neededWords, getList());

    if (wordOffset == -1) {
        return nullptr;
    }

    // marking memory as in use
    for (int i = 0; i < neededWords; i++) {
        memoryList[wordOffset + i] = 1;
    }

    // FIXME: memory leak, use some structure (hash set) to store each tuple and remove them accordingly
    int* startAddrAndLen = new int[2];
    startAddrAndLen[0] = (int)memoryList + wordOffset * wordSizeInBytes; // start address
    startAddrAndLen[1] = sizeInBytes / wordSizeInBytes; // length
    return startAddrAndLen;
}


void MemoryManager::free(void *address) {
    int startAddress = ((int*)address)[0];
    int length = ((int*)address)[1];
    unsigned int targetAddressMapping = (startAddress - (int)memoryList) / wordSizeInBytes;

    for (int i = 0; i < length; i++) {
        memoryList[targetAddressMapping + i] = 0;
    }

    // FIXME: memory leak, delete the address pointer when done using delete[]
    // and update hash set
}


void MemoryManager::setAllocator(std::function<int(int, void *)> allocator) {
    allocatorFunc = allocator;
}


int MemoryManager::dumpMemoryMap(char *filename) {
    return 69;
}


void *MemoryManager::getList() {
    if (memoryList == nullptr) {
        return nullptr;
    }

    unsigned int numHoles = 0;
    vector<unsigned int> holesInfoVector;

    // temporary placeholder
    holesInfoVector.push_back(0);

    // find holes
    for (unsigned int i = 0; i < memorySizeInWords; i++) {
        if (memoryList[i] == 0) {
            unsigned int startOffset = i;
            while (i < memorySizeInWords && memoryList[i] == 0) {
                i++;
            }

            holesInfoVector.push_back(startOffset);
            holesInfoVector.push_back(i - startOffset);
            numHoles++;
        }
    }

    // add back final number of holes
    holesInfoVector.at(0) = numHoles;

    // convert vector to standard array
    int* holesInfoArray = new int[numHoles * 2 + 1];
    for (unsigned int i = 0; i < holesInfoVector.size(); i++) { 
        holesInfoArray[i] = holesInfoVector.at(i);
    }

    // FIXME: REMOVE FOR TESTING PURPOSES
    // for (int i = 0; i < numHoles * 2 + 1; i++) {
    //     cout << holesInfoArray[i] << " ";
    // }
    // cout << "from getList()" << endl;

    return holesInfoArray;
}


void *MemoryManager::getBitmap() {
    return nullptr;
}


unsigned MemoryManager::getWordSize() {
    return wordSizeInBytes;
}


void *MemoryManager::getMemoryStart() {
    return memoryList;
}


unsigned MemoryManager::getMemoryLimit() {
    return (int)memoryList + wordSizeInBytes * memorySizeInWords;
}


int bestFit(int sizeInWords, void *list) {
    int* holeList = (int*)list;
    int numHoles = holeList[0];
    int chosenHoleOffset = -1;
    int minSpaceLeft = INT32_MAX;

    // consider each hole
    // start addresses are odd, lengths are even
    for (int i = 1; i < numHoles * 2; i += 2) {
        int currSpaceLeft = holeList[i + 1] - sizeInWords;
        if (currSpaceLeft > 0 && currSpaceLeft < minSpaceLeft) {
            minSpaceLeft = currSpaceLeft;
            chosenHoleOffset = holeList[i];
        }
    }

    return chosenHoleOffset;
}


int worstFit(int sizeInWords, void *list) {
    int* holeList = (int*)list;
    int numHoles = holeList[0];
    int chosenHoleOffset = -1;
    int maxSpaceLeft = INT32_MIN;

    // consider each hole
    // start addresses are odd, lengths are even
    for (int i = 1; i < numHoles * 2; i += 2) {
        int currSpaceLeft = holeList[i + 1] - sizeInWords;
        if (currSpaceLeft > 0 && currSpaceLeft > maxSpaceLeft) {
            maxSpaceLeft = currSpaceLeft;
            chosenHoleOffset = holeList[i];
        }
    }

    return chosenHoleOffset;
}

#include "MemoryManager.h"
#include <vector>
#include <string>
#include <fstream>

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
    memoryList = new char[sizeInWords * wordSizeInBytes];

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
    if (memoryList != nullptr) {
        delete[] memoryList;
    }

    memorySizeInWords = 0;
    memoryList = nullptr;
    startAddrToLenMap.clear();
}


void *MemoryManager::allocate(size_t sizeInBytes) {
    unsigned int neededWords =  sizeInBytes / wordSizeInBytes;

    if (sizeInBytes % wordSizeInBytes != 0 || neededWords > memorySizeInWords) {
        return nullptr;
    }

    uint16_t* list = (uint16_t*)getList();
    int wordOffset = allocatorFunc(neededWords, list);
    delete[] list;

    if (wordOffset == -1) {
        return nullptr;
    }

    // marking memory as in use with 1s
    for (int i = 0; i < neededWords; i++) {
        memoryList[wordOffset + i] = 1;
    }

    // calculating starting address and length
    char* startAddress = memoryList + wordOffset * wordSizeInBytes;
    int length = sizeInBytes / wordSizeInBytes;

    // store allocated memory for later deletion
    startAddrToLenMap.emplace(startAddress, length);

    return static_cast<void*>(startAddress);
}


void MemoryManager::free(void *address) {
    if (address == nullptr) {
        return;
    }

    char* startAddress = (char*)address;
    int length = startAddrToLenMap.at((char*)address);
    unsigned int targetAddressMapping = (startAddress - memoryList) / wordSizeInBytes;

    // marking memory as free with 0s
    for (int i = 0; i < length; i++) {
        memoryList[targetAddressMapping + i] = 0;
    }

    // free allocated address
    startAddrToLenMap.erase((char*)address);
}


void MemoryManager::setAllocator(std::function<int(int, void *)> allocator) {
    allocatorFunc = allocator;
}


int MemoryManager::dumpMemoryMap(char *filename) {
    ofstream file;
    file.open(filename);

    if (!file.is_open()) {
        return -1;
    }
    
    uint16_t* holesList = (uint16_t*)getList();
    uint16_t numHoles = holesList[0];

    // write memory map to file
    file << "[" + to_string(holesList[1]) + ", ";
    for (int i = 2; i < numHoles * 2 - 1; i += 2) {
        file << to_string(holesList[i]) + "] - [" + to_string(holesList[i + 1]) + ", ";
    }
    file << to_string(holesList[numHoles * 2]) + "]";

    delete[] holesList;

    return 0;
}


void *MemoryManager::getList() {
    if (memoryList == nullptr) {
        return nullptr;
    }

    uint16_t numHoles = 0;
    vector<uint16_t> holesInfoVec;

    // temporary placeholder
    holesInfoVec.push_back(0);

    // find holes
    for (unsigned int i = 0; i < memorySizeInWords; i++) {
        if (memoryList[i] == 0) {
            uint16_t startOffset = i;
            while (i < memorySizeInWords && memoryList[i] == 0) {
                i++;
            }

            holesInfoVec.push_back(startOffset);
            holesInfoVec.push_back(i - startOffset);
            numHoles++;
        }
    }

    // add back final number of holes
    holesInfoVec.at(0) = numHoles;

    // convert vector to standard array
    uint16_t* holesInfoArray = new uint16_t[numHoles * 2 + 1];
    for (unsigned int i = 0; i < holesInfoVec.size(); i++) { 
        holesInfoArray[i] = holesInfoVec.at(i);
    }

    return holesInfoArray;
}


void *MemoryManager::getBitmap() {
    vector<uint8_t> bitmapVec;
    unsigned int bitmapSize = 0;

    // placeholders
    bitmapVec.push_back(0);
    bitmapVec.push_back(0);

    // generate bitmap
    int i = 0;
    while (i < memorySizeInWords) {
        int entry = 0;
        int power = 1;
        int wordsLeftToView = 8;
        while (i < memorySizeInWords && wordsLeftToView > 0) {
            entry += memoryList[i] * power;
            power *= 2;
            wordsLeftToView--;
            i++;
        }

        bitmapSize++;
        bitmapVec.push_back(entry);
    }

    // include bitmap size
    if (bitmapSize == (uint8_t)bitmapSize) {
        bitmapVec.at(0) = (uint8_t)bitmapSize;
    } else {
        bitmapVec.at(0) = INT8_MAX;
        bitmapVec.at(1) = bitmapSize - INT8_MAX;
    }

    // convert vector to standard array
    uint8_t* bitmapArray = new uint8_t[bitmapVec.size()];
    for (unsigned int i = 0; i < bitmapVec.size(); i++) {
        bitmapArray[i] = bitmapVec.at(i);
    }

    return bitmapArray;
}


unsigned MemoryManager::getWordSize() {
    return wordSizeInBytes;
}


void *MemoryManager::getMemoryStart() {
    return (uint64_t*)memoryList;
}


unsigned MemoryManager::getMemoryLimit() {
    return wordSizeInBytes * memorySizeInWords;
}


int bestFit(int sizeInWords, void *list) {
    if (list == nullptr) {
        return -1;
    }

    uint16_t* holeList = (uint16_t*)list;
    int numHoles = holeList[0];
    int chosenHoleOffset = -1;
    int minSpaceLeft = INT32_MAX;

    // consider each hole
    // start addresses are odd, lengths are even
    for (int i = 1; i < numHoles * 2; i += 2) {
        int currSpaceLeft = holeList[i + 1] - sizeInWords;
        if (currSpaceLeft >= 0 && currSpaceLeft < minSpaceLeft) {
            minSpaceLeft = currSpaceLeft;
            chosenHoleOffset = holeList[i];
        }
    }

    return chosenHoleOffset;
}


int worstFit(int sizeInWords, void *list) {
    if (list == nullptr) {
        return -1;
    }

    uint16_t* holeList = (uint16_t*)list;
    int numHoles = holeList[0];
    int chosenHoleOffset = -1;
    int maxSpaceLeft = INT32_MIN;

    // consider each hole
    // start addresses are odd, lengths are even
    for (int i = 1; i < numHoles * 2; i += 2) {
        int currSpaceLeft = holeList[i + 1] - sizeInWords;
        if (currSpaceLeft >= 0 && currSpaceLeft > maxSpaceLeft) {
            maxSpaceLeft = currSpaceLeft;
            chosenHoleOffset = holeList[i];
        }
    }

    return chosenHoleOffset;
}

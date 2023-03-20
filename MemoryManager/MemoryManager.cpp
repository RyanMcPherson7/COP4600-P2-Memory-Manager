#include "MemoryManager.h"
#include <cmath>

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
}


void *MemoryManager::allocate(size_t sizeInBytes) {
    unsigned int neededWords = ceil((double)wordSizeInBytes / (double)sizeInBytes);

    if (neededWords > memorySizeInWords) {
        return nullptr;
    } 

    int wordOffset = allocatorFunc(neededWords, memoryList);

    if (wordOffset == -1) {
        return nullptr;
    }

    // marking memory as in use
    for (int i = 0; i < neededWords; i++) {
        memoryList[wordOffset + i] = 1;
    }
}


void MemoryManager::free(void *address) {
    unsigned int targetAddressMapping = (int)address - (int)memoryList;
    memoryList[targetAddressMapping] = 0;
}


void MemoryManager::setAllocator(std::function<int(int, void *)> allocator) {
    allocatorFunc = allocator;
}


int MemoryManager::dumpMemoryMap(char *filename) {
    return 69;
}


void *MemoryManager::getList() {
    return nullptr;
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
    return 69;
}


int worstFit(int sizeInWords, void *list) {
    return 69;
}

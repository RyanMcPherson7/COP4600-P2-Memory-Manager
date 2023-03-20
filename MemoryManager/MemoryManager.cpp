#include "MemoryManager.h"

MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator) {
    wordSizeInBytes = wordSize;
    allocatorFunc = allocator;
    sizeInWords = 0;
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

    this->sizeInWords = sizeInWords;
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

    sizeInWords = 0;
    delete[] memoryList;
    memoryList = nullptr;
}


void *MemoryManager::allocate(size_t sizeInBytes) {
    return nullptr;
}


void MemoryManager::free(void *address) {

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
    return (int)memoryList + wordSizeInBytes * sizeInWords;
}


int bestFit(int sizeInWords, void *list) {
    return 69;
}


int worstFit(int sizeInWords, void *list) {
    return 69;
}

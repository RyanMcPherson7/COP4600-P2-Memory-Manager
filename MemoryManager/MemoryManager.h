#pragma once
#include <functional>
using namespace std;

class MemoryManager {
    public:
        MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator);
        ~MemoryManager();
        void initialize(size_t sizeInWords);
        void shutdown();
        void *allocate(size_t sizeInBytes);
        void free(void *address);
        void setAllocator(std::function<int(int, void *)> allocator);
        int dumpMemoryMap(char *filename);
        void *getList();
        void *getBitmap();
        unsigned getWordSize();
        void *getMemoryStart();
        unsigned getMemoryLimit();
    private:
        unsigned wordSizeInBytes;
        std::function<int(int, void *)> allocatorFunc;
        size_t sizeInWords;
        bool* memoryList;
};

int bestFit(int sizeInWords, void *list);
int worstFit(int sizeInWords, void *list);

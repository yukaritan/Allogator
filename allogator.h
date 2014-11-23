#ifndef ALLOGATOR_H
#define ALLOGATOR_H

#include <stddef.h>

class Allogator
{
protected:
    // This tells us the size and address of any allocated object.
    // Pardon the name.
    struct ItemHandle
    {
        void* address = nullptr;
        size_t size = 0;
    };

    // These are references to any memory we've allocated
    size_t numberOfItemHandles = 1024;  // This is an obvious design flaw. As I said, it's a toy implementation
    ItemHandle* allocatedItemHandles;

    size_t size = 0;        // Amount of memory in use
    size_t maxSize = 1024;  // The amount of memory we have allocated
    char* memory;           // Actual memory

    // Transaction stuff
    bool inTransaction = false;

    // These variables will hold the original values in case we're doing a transaction
    ItemHandle* originalItemHandles = nullptr;
    char* originalMemory = nullptr;

    // Figures out how much we should pad before putting down the next object
    size_t GetNextAlignedAddress (size_t typeSize);

    // Throws away transaction backups
    void DeleteTransactionBackups ();

public:
    // Will just call the other constructor with size 1024
    Allogator ();

    Allogator (size_t size);

    virtual ~Allogator ();

    // Begins a transaction by creating a copy of the currently allocated memory
    bool BeginTransaction ();

    // Discard the copy we created it BeginTransaction
    bool CommitTransaction ();

    // Put the copy back
    bool RollBackTransaction ();

    // The implementation of a non-specialized template must be visible to a translation unit that uses it.
    // The compiler must be able to see the implementation in order to generate code for all specializations in the code.
    // In other words; I can't easily move this out of the header. (Read: I can't be assed to fix it)
    template <typename T>
    T* Allocate ()
    {
        size_t offset = GetNextAlignedAddress(sizeof(T));

        if (size + offset + sizeof(T) > maxSize)
        {
            return nullptr;
        }

        for (int i = 0; i < numberOfItemHandles; ++i)
        {
            if (allocatedItemHandles[i].address == nullptr)
            {
                char* address = memory + size + offset;
                this->size += sizeof(T) + offset;

                allocatedItemHandles[i].address = address;
                allocatedItemHandles[i].size = sizeof(T);

                return (T*) address;
            }
        }

        return nullptr;
    }

    // The implementation of a non-specialized template must be visible to a translation unit that uses it.
    // The compiler must be able to see the implementation in order to generate code for all specializations in the code.
    // In other words; I can't easily move this out of the header. (Read: I can't be assed to fix it)
    template <typename T>
    bool Deallocate(T* object)
    {
        for (int i = 0; i < numberOfItemHandles; ++i)
        {
            if(allocatedItemHandles[i].address == object)
            {
                allocatedItemHandles[i].address = nullptr;
                allocatedItemHandles[i].size = 0;
                return true;
            }
        }
        return false;

    }

    // Counts the ram we can currently allocate. Padding is not counted.
    size_t GetUsable ();

    // Counts the ram in use. Doesn't count padding.
    size_t GetUsed ();

    // Counts all the ram currently not in use, including padding.
    size_t GetFree ();

    // Counts all the elements we've currently allocated.
    size_t GetNumberOfObjects ();
};

#endif
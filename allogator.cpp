#include "allogator.h"
#include <iostream>

Allogator::Allogator ()
{
    Allogator(1024);
}

Allogator::Allogator (size_t size)
{
    maxSize = size;
    memory = new char[maxSize];
    allocatedItemHandles = new ItemHandle[numberOfItemHandles];
}

Allogator::~Allogator ()
{
    auto count = GetNumberOfObjects();
    if(count > 0)
    {
        std::cout << count << " objects were not cleaned up when the allocator destroyed itself. There may have been leaks." << std::endl;
    }

    for (int i = 0; i < numberOfItemHandles; ++i)
    {
        allocatedItemHandles[i].address = nullptr;
        allocatedItemHandles[i].size = 0;
    }

    RollBackTransaction();

    delete [] memory;
    memory = nullptr;
    delete [] allocatedItemHandles;
    allocatedItemHandles = nullptr;
}

void Allogator::DeleteTransactionBackups ()
{
    if(originalItemHandles != nullptr)
    {
        for(size_t i=0; i< numberOfItemHandles; ++i)
        {
            originalItemHandles[i].address = nullptr;
        }

        delete [] originalItemHandles;
    }

    if(originalMemory != nullptr)
    {
        delete [] originalMemory;
    }

    originalItemHandles = nullptr;
    originalMemory = nullptr;
}

size_t Allogator::GetNextAlignedAddress (size_t typeSize)
{
    return size % typeSize;
}

size_t Allogator::GetNumberOfObjects ()
{
    size_t count = 0;
    for (int i = 0; i < numberOfItemHandles; ++i)
    {
        if(allocatedItemHandles[i].address != nullptr)
        {
            ++count;
        }
    }
    return count;
}

size_t Allogator::GetFree ()
{
    return this->maxSize - GetUsed();
}

size_t Allogator::GetUsed ()
{
    size_t size = 0;
    for (int i = 0; i < numberOfItemHandles; ++i)
    {
        if(allocatedItemHandles[i].address != nullptr)
        {
            size += allocatedItemHandles[i].size;
        }
    }
    return size;
}

size_t Allogator::GetUsable ()
{
    return this->maxSize - size;
}

bool Allogator::RollBackTransaction ()
{
    if(!inTransaction)
    {
        // there's no transaction to work with
        return false;
    }

    inTransaction = false;

    // copy everything back, throw away the originals

    for(size_t i=0; i<maxSize; ++i)
    {
        memory[i] = originalMemory[i];
    }

    for(size_t i=0; i< numberOfItemHandles; ++i)
    {
        allocatedItemHandles[i] = originalItemHandles[i];
    }

    DeleteTransactionBackups();

    return true;
}

bool Allogator::CommitTransaction ()
{
    if(!inTransaction)
    {
        // there's no transaction to work with
        return false;
    }

    inTransaction = false;

    // simply throw away originalMemory and originalItemHandles

    DeleteTransactionBackups();

    return true;
}

bool Allogator::BeginTransaction ()
{
    if(inTransaction)
    {
        // we're already in a transaction, we can't have another
        return false;
    }

    inTransaction = true;

    originalMemory = new char[maxSize];
    originalItemHandles = new ItemHandle[numberOfItemHandles];

    for(size_t i=0; i<maxSize; ++i)
    {
        originalMemory[i] = memory[i];
    }

    for(size_t i=0; i< numberOfItemHandles; ++i)
    {
        originalItemHandles[i] = allocatedItemHandles[i];
    }

    return true;
}

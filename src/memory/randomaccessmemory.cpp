//
// Created by Piotr on 31.08.2022.
//

#include "memory/randomaccessmemory.h"

namespace emu::memory {

    template<typename T, typename U>
    RandomAccessMemory<T, U>::RandomAccessMemory(std::size_t size) :
            memory_(size) {

    }

    template<typename T, typename U>
    RandomAccessMemory<T, U>::RandomAccessMemory(const std::vector<T>& vector) :
            memory_(vector) {

    }

    template<typename T, typename U>
    RandomAccessMemory<T, U>::RandomAccessMemory(std::vector<T>&& vector) :
            memory_(std::move(vector)) {

    }

    template<typename T, typename U>
    T& RandomAccessMemory<T, U>::operator[](U address) {
        return memory_[address];
    }

    template<typename T, typename U>
    const T& RandomAccessMemory<T, U>::operator[](U address) const {
        return memory_[address];
    }

    template<typename T, typename U>
    T RandomAccessMemory<T, U>::read(U address) {
        return memory_[address];
    }

    template<typename T, typename U>
    void RandomAccessMemory<T, U>::write(U address, T value) {
        memory_[address] = value;
    }

} // emu
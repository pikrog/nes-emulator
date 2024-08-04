//
// Created by Piotr on 31.08.2022.
//

#ifndef EMU_RANDOMACCESSMEMORY_H
#define EMU_RANDOMACCESSMEMORY_H

#include <memory>
#include <vector>

#include "interfacememory.h"

namespace emu::memory {

    template<typename T, typename U = std::size_t>
    class RandomAccessMemory : public InterfaceMemory<T, U> {
        std::vector<T> memory_;

    public:
        explicit RandomAccessMemory(std::size_t size);
        explicit RandomAccessMemory(const std::vector<T>& vector);
        explicit RandomAccessMemory(std::vector<T>&& vector);

        [[nodiscard]] T& operator[](U address);

        [[nodiscard]] const T& operator[](U address) const;

        T read(U address) override;
        void write(U address, T value) override;
    };

} // emu

#endif //EMU_RANDOMACCESSMEMORY_H

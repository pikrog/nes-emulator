//
// Created by Piotr on 31.08.2022.
//

#ifndef EMU_CPU_H
#define EMU_CPU_H

#include <cstdint>
#include <functional>

#include "flag.h"
#include "opcode.h"
#include "addressingmode.h"
#include "cpu/bus/oamdmaport.h"
#include "cpu/bus/cpubus.h"

namespace emu::cpu {

    class Cpu {
        friend class OamDmaPort;
        using OperationExecutor = std::function<int(Cpu*)>;
        using ExecutorsMap = std::unordered_map<Operation, OperationExecutor>;

    public:
        static constexpr uint16_t STACK_BASE_ADDRESS = 0x0100;
        static constexpr uint16_t NMI_VECTOR_ADDRESS = 0xfffa;
        static constexpr uint16_t RESET_VECTOR_ADDRESS = 0xfffc;
        static constexpr uint16_t IRQ_VECTOR_ADDRESS = 0xfffe;

        static constexpr int INTERRUPT_PRE_POLLING_CYCLES = 4;
        static constexpr int INTERRUPT_POST_POLLING_CYCLES = 3;

    private:
        static const ExecutorsMap executors;

        uint8_t accumulator_;
        uint8_t indexX_;
        uint8_t indexY_;
        uint16_t programCounter_;
        uint8_t stackPointer_;
        uint8_t status_;

        int elapsedCycles_;
        int remainingCycles_;

        CpuBus& bus_;
        OamDmaPort oamDmaPort_;
        AddressingMode addressingMode_;
        uint16_t address_;
        bool pageCrossed_;

        bool isInterruptPending_;
        bool isIrqPending_;
        bool isNmiPending_;
        bool isResetPending_;

        enum class InterruptSequencePhase {
            PRE_POLLING,
            POST_POLLING,
        } interruptSequencePhase_;

    public:
        explicit Cpu(CpuBus& bus, uint16_t startAddress);
        explicit Cpu(CpuBus& bus);

        [[nodiscard]] uint8_t getAccumulator() const;
        [[nodiscard]] uint8_t getIndexX() const;
        [[nodiscard]] uint8_t getIndexY() const;
        [[nodiscard]] uint16_t getProgramCounter() const;
        [[nodiscard]] uint8_t getStackPointer() const;
        [[nodiscard]] uint8_t getStatus() const;
        [[nodiscard]] int getElapsedCycles() const;

        [[nodiscard]] bool isFlagSet(Flag flag) const;
        [[nodiscard]] bool isFlagClear(Flag flag) const;

        void clock();
        void fullStep();

        void interruptRequest();
        void nonMaskableInterrupt();
        void reset();

    private:
        /* flag modifiers */
        void setFlag(Flag flag);
        void clearFlag(Flag flag);
        void updateFlag(Flag flag, bool set);
        void updateZeroFlag();
        void updateZeroFlag(uint8_t value);
        void updateNegativeFlag();
        void updateNegativeFlag(uint8_t value);

        /* execution control */
        int executionStep();
        int interruptSequence();
        int executeNextOpcode();
        void setAddressing(AddressingMode addressingMode);
        void incrementProgramCounter();

        /* fetch data by program counter */
        [[nodiscard]] uint8_t fetchByteOperand() const;
        [[nodiscard]] uint16_t fetchWordOperand() const;
        [[nodiscard]] uint16_t fetchTwoBytes(uint16_t lowerBytePointer, uint16_t higherBytePointer) const;

        /* R/W data by established addressing mode */
        [[nodiscard]] uint8_t readByte() const;
        void writeByte(uint8_t byte);

        /* op bases and common functions */
        int addBase(uint8_t operand);
        int branchBase(bool takeBranch);
        int compareBase(uint8_t minuend);
        int incrementBase(uint8_t& variable, int direction);
        int incrementInMemoryBase(int direction);
        template<typename BinaryOperation> int bitwiseOperationBase(BinaryOperation operation);
        int loadBase(uint8_t& destination);
        int transferBase(uint8_t source, uint8_t& destination);

        void pushStack(uint8_t value);
        [[nodiscard]] uint8_t pullStack();

        /* official ops */
        int addWithCarry();
        int andWithAccumulator();
        int arithmeticShiftLeft();
        int branchOnCarryClear();
        int branchOnCarrySet();
        int branchOnEqual();
        int bitTest();
        int branchOnMinus();
        int branchOnNotEqual();
        int branchOnPlus();
        int forceBreak();
        int branchOnOverflowClear();
        int branchOnOverflowSet();
        int clearCarry();
        int clearDecimal();
        int clearInterruptDisable();
        int clearOverflow();
        int compareWithAccumulator();
        int compareWithX();
        int compareWithY();
        int decrement();
        int decrementX();
        int decrementY();
        int exclusiveOrWithAccumulator();
        int increment();
        int incrementX();
        int incrementY();
        int jump();
        int jumpSubroutine();
        int loadAccumulator();
        int loadX();
        int loadY();
        int logicalShiftRight();
        int noOperation();
        int orWithAccumulator();
        int pushAccumulator();
        int pushProcessorStatus();
        int pullAccumulator();
        int pullProcessorStatus();
        int rotateLeft();
        int rotateRight();
        int returnFromInterrupt();
        int returnFromSubroutine();
        int subtractWithCarry();
        int setCarry();
        int setDecimal();
        int setInterruptDisable();
        int storeAccumulator();
        int storeX();
        int storeY();
        int transferAccumulatorToX();
        int transferAccumulatorToY();
        int transferStackPointerToX();
        int transferXToAccumulator();
        int transferXToStackPointer();
        int transferYToAccumulator();

        /* todo: unofficial ops */
        int halt();

    };

} // emu

#endif //EMU_CPU_H

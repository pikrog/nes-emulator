//
// Created by Piotr on 31.08.2022.
//

#include "cpu/cpu.h"

#include "util/bin.h"

using emu::util::bin::makeWord;
using emu::util::bin::isPageCrossed;
using emu::util::bin::isNegativeU2;
using emu::util::bin::getLowerByte;
using emu::util::bin::getHigherByte;
using emu::util::bin::decodeU2SignedInt;

/*
 * todo: make executors return void instead. writeByte -> pageCrossedPenalty = 0. add penalty after execute().
 * todo: branchBase can add penalty manually
 *
 * todo: replace raw status with BitRegister
 */
#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
namespace emu::cpu {

    const Cpu::ExecutorsMap Cpu::executors = {
            {Operation::ADC, &Cpu::addWithCarry},
            {Operation::AND, &Cpu::andWithAccumulator},
            {Operation::ASL, &Cpu::arithmeticShiftLeft},
            {Operation::BCC, &Cpu::branchOnCarryClear},
            {Operation::BCS, &Cpu::branchOnCarrySet},
            {Operation::BEQ, &Cpu::branchOnEqual},
            {Operation::BIT, &Cpu::bitTest},
            {Operation::BMI, &Cpu::branchOnMinus},
            {Operation::BNE, &Cpu::branchOnNotEqual},
            {Operation::BPL, &Cpu::branchOnPlus},
            {Operation::BRK, &Cpu::forceBreak},
            {Operation::BVC, &Cpu::branchOnOverflowClear},
            {Operation::BVS, &Cpu::branchOnOverflowSet},
            {Operation::CLC, &Cpu::clearCarry},
            {Operation::CLD, &Cpu::clearDecimal},
            {Operation::CLI, &Cpu::clearInterruptDisable},
            {Operation::CLV, &Cpu::clearOverflow},
            {Operation::CMP, &Cpu::compareWithAccumulator},
            {Operation::CPX, &Cpu::compareWithX},
            {Operation::CPY, &Cpu::compareWithY},
            {Operation::DEC, &Cpu::decrement},
            {Operation::DEX, &Cpu::decrementX},
            {Operation::DEY, &Cpu::decrementY},
            {Operation::EOR, &Cpu::exclusiveOrWithAccumulator},
            {Operation::INC, &Cpu::increment},
            {Operation::INX, &Cpu::incrementX},
            {Operation::INY, &Cpu::incrementY},
            {Operation::JMP, &Cpu::jump},
            {Operation::JSR, &Cpu::jumpSubroutine},
            {Operation::LDA, &Cpu::loadAccumulator},
            {Operation::LDX, &Cpu::loadX},
            {Operation::LDY, &Cpu::loadY},
            {Operation::LSR, &Cpu::logicalShiftRight},
            {Operation::NOP, &Cpu::noOperation},
            {Operation::ORA, &Cpu::orWithAccumulator},
            {Operation::PHA, &Cpu::pushAccumulator},
            {Operation::PHP, &Cpu::pushProcessorStatus},
            {Operation::PLA, &Cpu::pullAccumulator},
            {Operation::PLP, &Cpu::pullProcessorStatus},
            {Operation::ROL, &Cpu::rotateLeft},
            {Operation::ROR, &Cpu::rotateRight},
            {Operation::RTI, &Cpu::returnFromInterrupt},
            {Operation::RTS, &Cpu::returnFromSubroutine},
            {Operation::SBC, &Cpu::subtractWithCarry},
            {Operation::SEC, &Cpu::setCarry},
            {Operation::SED, &Cpu::setDecimal},
            {Operation::SEI, &Cpu::setInterruptDisable},
            {Operation::STA, &Cpu::storeAccumulator},
            {Operation::STX, &Cpu::storeX},
            {Operation::STY, &Cpu::storeY},
            {Operation::TAX, &Cpu::transferAccumulatorToX},
            {Operation::TAY, &Cpu::transferAccumulatorToY},
            {Operation::TSX, &Cpu::transferStackPointerToX},
            {Operation::TXA, &Cpu::transferXToAccumulator},
            {Operation::TXS, &Cpu::transferXToStackPointer},
            {Operation::TYA, &Cpu::transferYToAccumulator},

            /* unofficial operations */
            {Operation::JAM, &Cpu::halt},
    };



    Cpu::Cpu(CpuBus& bus, uint16_t startAddress)
        : accumulator_(0x0),
          indexX_(0x0),
          indexY_(0x0),
          programCounter_(startAddress),
          stackPointer_(0xff),
          status_(0x0),
          elapsedCycles_(0),
          remainingCycles_(0),
          addressingMode_(AddressingMode::UNSPECIFIED),
          address_(),
          pageCrossed_(false),
          bus_(bus),
          oamDmaPort_(*this),
          isInterruptPending_(false),
          isIrqPending_(false),
          isNmiPending_(false),
          isResetPending_(false),
          interruptSequencePhase_(InterruptSequencePhase::PRE_POLLING) {
        bus_.attach(oamDmaPort_);
    }

    Cpu::Cpu(CpuBus &bus)
        : Cpu(bus, 0x0) {
        reset();
    }

    uint8_t Cpu::getAccumulator() const {
        return accumulator_;
    }

    uint8_t Cpu::getIndexX() const {
        return indexX_;
    }

    uint8_t Cpu::getIndexY() const {
        return indexY_;
    }

    uint16_t Cpu::getProgramCounter() const {
        return programCounter_;
    }

    uint8_t Cpu::getStackPointer() const {
        return stackPointer_;
    }

    uint8_t Cpu::getStatus() const {
        return status_;
    }

    int Cpu::getElapsedCycles() const {
        return elapsedCycles_;
    }

    void Cpu::setFlag(Flag flag) {
        auto bit = static_cast<uint8_t>(flag);
        status_ |= bit;
    }

    void Cpu::clearFlag(Flag flag) {
        auto mask = static_cast<uint8_t>(flag);
        status_ &= ~mask;
    }

    void Cpu::updateFlag(Flag flag, bool set) {
        set ? setFlag(flag) : clearFlag(flag);
    }

    void Cpu::updateZeroFlag() {
        updateZeroFlag(accumulator_);
    }

    void Cpu::updateZeroFlag(uint8_t value) {
        updateFlag(Flag::ZERO, value == 0);
    }

    void Cpu::updateNegativeFlag() {
        updateNegativeFlag(accumulator_);
    }

    void Cpu::updateNegativeFlag(uint8_t value) {
        updateFlag(Flag::NEGATIVE, isNegativeU2(value));
    }


    bool Cpu::isFlagSet(Flag flag) const {
        auto mask = static_cast<uint8_t>(flag);
        return status_ & mask;
    }

    bool Cpu::isFlagClear(Flag flag) const {
        return !isFlagSet(flag);
    }

    void Cpu::clock() {
        elapsedCycles_++;
        if(remainingCycles_ > 0) {
            remainingCycles_--;
            return;
        }
        int cycles;
        remainingCycles_ += executionStep() - 1;
    }

    void Cpu::fullStep() {
        elapsedCycles_ += executionStep() + remainingCycles_;
        remainingCycles_ = 0;
    }

    void Cpu::interruptRequest() {
        isInterruptPending_ = isIrqPending_ = true;
    }

    void Cpu::nonMaskableInterrupt() {
        isInterruptPending_ = isNmiPending_ = true;
    }

    void Cpu::reset() {
        isInterruptPending_ = isResetPending_ = true;
    }

    int Cpu::executionStep() {
        if(isInterruptPending_ && (!isIrqPending_ || isFlagClear(Flag::INTERRUPT_DISABLE)))
            return interruptSequence();
        else
            return executeNextOpcode();
    }

    int Cpu::interruptSequence() {
        // the interrupt sequence is divided into two phases
        // pre-polling phase
        if(interruptSequencePhase_ == InterruptSequencePhase::PRE_POLLING) {
            if (!isResetPending_) { // during reset, memory is write-protected
                pushStack(getHigherByte(programCounter_));
                pushStack(getLowerByte(programCounter_));
            } else {
                stackPointer_ -= 2;
            }

            interruptSequencePhase_ = InterruptSequencePhase::POST_POLLING;
            return INTERRUPT_PRE_POLLING_CYCLES;
        }

        // post-polling phase
        uint16_t interruptVector;
        if(isResetPending_) // prioritize reset
            interruptVector = RESET_VECTOR_ADDRESS;
        else if(isNmiPending_) // allow IRQ hijacking
            interruptVector = NMI_VECTOR_ADDRESS;
        else
            interruptVector = IRQ_VECTOR_ADDRESS;

        if(!isResetPending_)
            // push status with 5th & 4th bit set
            // clear 4th bit status if it is IRQ or NMI
            // leave 4th bit set if it is BRK
            pushStack((status_ | 0x30) & ~((isNmiPending_ || isIrqPending_) << 4));
        else
            stackPointer_--;

        programCounter_ = fetchTwoBytes(interruptVector, interruptVector + 1);
        setFlag(Flag::INTERRUPT_DISABLE);

        interruptSequencePhase_ = InterruptSequencePhase::PRE_POLLING;
        isInterruptPending_ = isResetPending_ = isNmiPending_ = isIrqPending_ = false;
        return INTERRUPT_POST_POLLING_CYCLES;
    }

    int Cpu::executeNextOpcode() {
        auto opcode = bus_.read(programCounter_);
        auto& attributes = opcodes[opcode];
        setAddressing(attributes.addressingMode);
        incrementProgramCounter();
        auto& executeOperation = executors.at(attributes.operation);
        auto penaltyCycles = executeOperation(this);
        auto cycles = attributes.cycles + penaltyCycles;
        return cycles;
    }

    uint8_t Cpu::fetchByteOperand() const {
        return bus_.read(programCounter_ + 1);
    }

    uint16_t Cpu::fetchWordOperand() const {
        return fetchTwoBytes(programCounter_ + 1, programCounter_ + 2);
    }

    uint16_t Cpu::fetchTwoBytes(uint16_t lowerBytePointer, uint16_t higherBytePointer) const {
        auto lowerByte = bus_.read(lowerBytePointer);
        auto higherByte = bus_.read(higherBytePointer);
        return makeWord(lowerByte, higherByte);
    }

    void Cpu::setAddressing(AddressingMode addressingMode) {
        addressingMode_ = addressingMode;
        pageCrossed_ = false;
        switch(addressingMode_) {
            case AddressingMode::IMMEDIATE:
            case AddressingMode::RELATIVE:
                address_ = programCounter_ + 1;
                break;
            case AddressingMode::ZERO_PAGE:
                address_ = fetchByteOperand();
                break;
            case AddressingMode::ZERO_PAGE_X:
                address_ = (fetchByteOperand() + indexX_) & 0xff;
                break;
            case AddressingMode::ZERO_PAGE_Y:
                address_ = (fetchByteOperand() + indexY_) & 0xff;
                break;
            case AddressingMode::ABSOLUTE:
                address_ = fetchWordOperand();
                break;
            case AddressingMode::ABSOLUTE_X:
            case AddressingMode::ABSOLUTE_Y: {
                auto operandAddress = fetchWordOperand();
                auto index = addressingMode == AddressingMode::ABSOLUTE_X ? indexX_ : indexY_;
                address_ = operandAddress + index;
                pageCrossed_ = isPageCrossed(operandAddress, address_);
                break;
            }
            case AddressingMode::INDIRECT: {
                auto pointer = fetchWordOperand();
                // page wrapping
                uint16_t nextPointer = (pointer & 0xff) == 0xff ? pointer & 0xff00 : pointer + 1;
                address_ = fetchTwoBytes(pointer, nextPointer);
                break;
            }
            case AddressingMode::INDIRECT_X: {
                uint16_t pointer = (fetchByteOperand() + indexX_) & 0xff;
                uint16_t nextPointer = (pointer + 1) & 0xff;
                address_ = fetchTwoBytes(pointer, nextPointer);
                break;
            }
            case AddressingMode::INDIRECT_Y: {
                auto pointer = fetchByteOperand();
                uint16_t nextPointer = (pointer + 1) & 0xff;
                auto tempAddress = fetchTwoBytes(pointer, nextPointer);
                address_ = tempAddress + indexY_;
                pageCrossed_ = isPageCrossed(tempAddress, address_);
                break;
            }
            case AddressingMode::ACCUMULATOR:
            case AddressingMode::IMPLICIT:
            case AddressingMode::UNSPECIFIED:
                break;
        }
    }

    void Cpu::incrementProgramCounter() {
        switch(addressingMode_) {
            case AddressingMode::ACCUMULATOR:
            case AddressingMode::IMPLICIT:
            case AddressingMode::UNSPECIFIED:
                programCounter_ += 1;
                break;
            case AddressingMode::IMMEDIATE:
            case AddressingMode::INDIRECT_X:
            case AddressingMode::INDIRECT_Y:
            case AddressingMode::RELATIVE:
            case AddressingMode::ZERO_PAGE:
            case AddressingMode::ZERO_PAGE_X:
            case AddressingMode::ZERO_PAGE_Y:
                programCounter_ += 2;
                break;
            case AddressingMode::ABSOLUTE:
            case AddressingMode::ABSOLUTE_X:
            case AddressingMode::ABSOLUTE_Y:
            case AddressingMode::INDIRECT:
                programCounter_ += 3;
                break;
        }
    }

    uint8_t Cpu::readByte() const {
        if(addressingMode_ == AddressingMode::ACCUMULATOR) {
            return accumulator_;
        } else if(addressingMode_ != AddressingMode::UNSPECIFIED && addressingMode_ != AddressingMode::IMPLICIT) {
            return bus_.read(address_);
        }
        return 0x0; // throw AddressModeException
    }

    void Cpu::writeByte(uint8_t byte) {
        if(addressingMode_ == AddressingMode::ACCUMULATOR) {
            accumulator_ = byte;
        } else if(addressingMode_ != AddressingMode::UNSPECIFIED && addressingMode_ != AddressingMode::IMPLICIT) {
            bus_.write(address_, byte);
        }
    }

    /*
     * why does subtract with carry work with this implementation?
     * *****************************************
     * a - b = a + (-b)
     * -b <-> ~b+1 (U2 encoding)
     * borrow = ~c
     * if c is set, it means that there is no underflow, therefore no borrow
     * sum = x + y + c
     * diff = x - y - ~c <-> x + (~y + 1) - ~c
     * if ~c = 0: diff = x + ~y + 1 = x + ~y + c
     * if ~c = 1: diff = x + ~y = x + ~y + c
     * *****************************************
     * conclusion: to subtract all you need to do is invert the bits of an operand
     */

    int Cpu::addBase(uint8_t operand) {
        uint8_t carry = isFlagSet(Flag::CARRY);
        uint16_t result = accumulator_ + operand + carry;
        updateFlag(Flag::OVERFLOW, (accumulator_ ^ result) & (operand ^ result) & 0x80);
        updateFlag(Flag::CARRY, result & 0x100);
        accumulator_ = result & 0xff;
        updateZeroFlag();
        updateNegativeFlag();
        return pageCrossed_;
    }

    int Cpu::branchBase(bool takeBranch) {
        if(!takeBranch)
            return 0;
        auto oldProgramCounter = programCounter_;
        auto offset = readByte();
        programCounter_ += decodeU2SignedInt(offset);
        return 1 + isPageCrossed(oldProgramCounter, programCounter_);
    }

    int Cpu::compareBase(uint8_t minuend) {
        uint8_t invertedOperand = ~readByte();
        uint16_t negativeSubtrahend = invertedOperand + 1;
        uint16_t result = minuend + negativeSubtrahend;
        updateFlag(Flag::CARRY, result & 0x100);
        updateZeroFlag(result);
        updateNegativeFlag(result);
        return pageCrossed_;
    }

    int Cpu::incrementBase(uint8_t& variable, int direction) {
        variable += direction;
        updateZeroFlag(variable);
        updateNegativeFlag(variable);
        return 0;
    }

    int Cpu::incrementInMemoryBase(int direction) {
        auto operand = readByte();
        auto penalty = incrementBase(operand, direction);
        writeByte(operand);
        return penalty;
    }

    template<typename BinaryOperation>
    int Cpu::bitwiseOperationBase(BinaryOperation operation) {
        auto operand = readByte();
        accumulator_ = operation(accumulator_, operand);
        updateZeroFlag();
        updateNegativeFlag();
        return pageCrossed_;
    }

    int Cpu::loadBase(uint8_t& destination) {
        destination = readByte();
        updateZeroFlag(destination);
        updateNegativeFlag(destination);
        return pageCrossed_;
    }

    int Cpu::transferBase(uint8_t source, uint8_t& destination) {
        destination = source;
        updateZeroFlag(destination);
        updateNegativeFlag(destination);
        return 0;
    }

    void Cpu::pushStack(uint8_t value) {
        bus_.write(STACK_BASE_ADDRESS + stackPointer_, value);
        stackPointer_--;
    }

    uint8_t Cpu::pullStack() {
        stackPointer_++;
        return bus_.read(STACK_BASE_ADDRESS + stackPointer_);
    }

    int Cpu::addWithCarry() {
        return addBase(readByte());
    }

    int Cpu::andWithAccumulator() {
        return bitwiseOperationBase(std::bit_and());
    }

    int Cpu::arithmeticShiftLeft() {
        auto operand = readByte();
        updateFlag(Flag::CARRY, operand & 0x80);
        operand <<= 1;
        updateZeroFlag(operand);
        updateNegativeFlag(operand);
        writeByte(operand);
        return 0;
    }

    int Cpu::branchOnCarryClear() {
        return branchBase(isFlagClear(Flag::CARRY));
    }

    int Cpu::branchOnCarrySet() {
        return branchBase(isFlagSet(Flag::CARRY));
    }

    int Cpu::branchOnEqual() {
        return branchBase(isFlagSet(Flag::ZERO));
    }

    int Cpu::bitTest() {
        auto operand = readByte();
        uint8_t result = accumulator_ & operand;
        updateFlag(Flag::NEGATIVE, result & 0x80); // move 7th bit to the N flag
        updateFlag(Flag::OVERFLOW, result & 0x40); // move 6tb bit to the V flag
        updateZeroFlag(result);
        return 0;
    }

    int Cpu::branchOnMinus() {
        return branchBase(isFlagSet(Flag::NEGATIVE));
    }

    int Cpu::branchOnNotEqual() {
        return branchBase(isFlagClear(Flag::ZERO));
    }

    int Cpu::branchOnPlus() {
        return branchBase(isFlagClear(Flag::NEGATIVE));
    }

    int Cpu::forceBreak() {
        /*programCounter_++; // ignore signal number
        pushStack(getHigherByte(programCounter_));
        pushStack(getLowerByte(programCounter_));
        pushStack((status_ | 0x20) & ~0x10); // push status with 5th bit set and 4th bit clear
        programCounter_ = fetchTwoBytes(IRQ_VECTOR_ADDRESS, IRQ_VECTOR_ADDRESS + 1);
        setFlag(Flag::INTERRUPT_DISABLE);*/
        programCounter_++; // ignore signal number
        isInterruptPending_ = true; // set to true so that the POST_POLLING phase will execute correctly
        return interruptSequence(); // execute the PRE_POLLING phase
    }

    int Cpu::branchOnOverflowClear() {
        return branchBase(isFlagClear(Flag::OVERFLOW));
    }

    int Cpu::branchOnOverflowSet() {
        return branchBase(isFlagSet(Flag::OVERFLOW));
    }

    int Cpu::clearCarry() {
        clearFlag(Flag::CARRY);
        return 0;
    }

    int Cpu::clearDecimal() {
        clearFlag(Flag::DECIMAL);
        return 0;
    }

    int Cpu::clearInterruptDisable() {
        clearFlag(Flag::INTERRUPT_DISABLE);
        return 0;
    }

    int Cpu::clearOverflow() {
        clearFlag(Flag::OVERFLOW);
        return 0;
    }

    int Cpu::compareWithAccumulator() {
        return compareBase(accumulator_);
    }

    int Cpu::compareWithX() {
        return compareBase(indexX_);
    }

    int Cpu::compareWithY() {
        return compareBase(indexY_);
    }

    int Cpu::decrement() {
        return incrementInMemoryBase(-1);
    }

    int Cpu::decrementX() {
        return incrementBase(indexX_, -1);
    }

    int Cpu::decrementY() {
        return incrementBase(indexY_, -1);
    }

    int Cpu::exclusiveOrWithAccumulator() {
        return bitwiseOperationBase(std::bit_xor());
    }

    int Cpu::increment() {
        return incrementInMemoryBase(1);
    }

    int Cpu::incrementX() {
        return incrementBase(indexX_, 1);
    }

    int Cpu::incrementY() {
        return incrementBase(indexY_, 1);
    }

    int Cpu::jump() {
        programCounter_ = address_;
        return 0;
    }

    int Cpu::jumpSubroutine() {
        uint16_t previousCounter = programCounter_ - 1;
        pushStack(getHigherByte(previousCounter));
        pushStack(getLowerByte(previousCounter));
        return jump();
    }

    int Cpu::loadAccumulator() {
        return loadBase(accumulator_);
    }

    int Cpu::loadX() {
        return loadBase(indexX_);
    }

    int Cpu::loadY() {
        return loadBase(indexY_);
    }

    int Cpu::logicalShiftRight() {
        auto operand = readByte();
        updateFlag(Flag::CARRY, operand & 0x1);
        operand >>= 1;
        updateZeroFlag(operand);
        clearFlag(Flag::NEGATIVE);
        writeByte(operand);
        return 0;
    }

    int Cpu::noOperation() { // NOLINT(readability-convert-member-functions-to-static)
        return 0;
    }

    int Cpu::orWithAccumulator() {
        return bitwiseOperationBase(std::bit_or());
    }

    int Cpu::pushAccumulator() {
        pushStack(accumulator_);
        return 0;
    }

    int Cpu::pushProcessorStatus() {
        pushStack(status_ | 0x30); // status with 4th and 5th bit set
        return 0;
    }

    int Cpu::pullAccumulator() {
        accumulator_ = pullStack();
        updateZeroFlag();
        updateNegativeFlag();
        return 0;
    }

    int Cpu::pullProcessorStatus() {
        status_ = pullStack() & ~0x30; // pull status with 4th and 5th bit clear
        return 0;
    }

    int Cpu::rotateLeft() {
        auto operand = readByte();
        uint8_t firstBit = isFlagSet(Flag::CARRY);
        updateFlag(Flag::CARRY, operand & 0x80);
        operand = (operand << 1) | firstBit;
        updateZeroFlag(operand);
        updateNegativeFlag(operand);
        writeByte(operand);
        return 0;
    }

    int Cpu::rotateRight() {
        auto operand = readByte();
        uint8_t lastBit = isFlagSet(Flag::CARRY);
        updateFlag(Flag::CARRY, operand & 0x1);
        operand = (operand >> 1) | (lastBit << 7);
        updateZeroFlag(operand);
        updateNegativeFlag(operand);
        writeByte(operand);
        return 0;
    }

    int Cpu::returnFromInterrupt() {
        pullProcessorStatus();
        auto counterLsb = pullStack();
        auto counterMsb = pullStack();
        programCounter_ = makeWord(counterLsb, counterMsb);
        return 0;
    }

    int Cpu::returnFromSubroutine() {
        auto counterLsb = pullStack();
        auto counterMsb = pullStack();
        programCounter_ = makeWord(counterLsb, counterMsb) + 1;
        return 0;
    }

    int Cpu::subtractWithCarry() {
        return addBase(~readByte());
    }

    int Cpu::setCarry() {
        setFlag(Flag::CARRY);
        return 0;
    }

    int Cpu::setDecimal() {
        setFlag(Flag::DECIMAL);
        return 0;
    }

    int Cpu::setInterruptDisable() {
        setFlag(Flag::INTERRUPT_DISABLE);
        return 0;
    }

    int Cpu::storeAccumulator() {
        writeByte(accumulator_);
        return 0;
    }

    int Cpu::storeX() {
        writeByte(indexX_);
        return 0;
    }

    int Cpu::storeY() {
        writeByte(indexY_);
        return 0;
    }

    int Cpu::transferAccumulatorToX() {
        return transferBase(accumulator_, indexX_);
    }

    int Cpu::transferAccumulatorToY() {
        return transferBase(accumulator_, indexY_);
    }

    int Cpu::transferStackPointerToX() {
        return transferBase(stackPointer_, indexX_);
    }

    int Cpu::transferXToAccumulator() {
        return transferBase(indexX_, accumulator_);
    }

    int Cpu::transferXToStackPointer() {
        stackPointer_ = indexX_; // this special snowflake does not modify flags
        return 0;
    }

    int Cpu::transferYToAccumulator() {
        return transferBase(indexY_, accumulator_);
    }

    int Cpu::halt() { // NOLINT(readability-convert-member-functions-to-static)
        return 0;
    }

} // emu
#pragma clang diagnostic pop
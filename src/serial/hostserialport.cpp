//
// Created by Piotr on 06.09.2022.
//

#include "serial/hostserialport.h"

using emu::util::Milliseconds;

constexpr Milliseconds READ_INTERVAL_TIMEOUT(5);
constexpr std::size_t READ_BUFFER_SIZE = 32;
constexpr std::size_t WRITE_BUFFER_SIZE = 32;

namespace emu::serial {

    HostSerialPort::HostSerialPort()
        : portHandle_(),
          portName_("COM1"),
          baudRate_(9600),
          byteSize_(8),
          parity_(Parity::NO_PARITY),
          stopBits_(StopBits::ONE_STOP_BIT),
          cancelIoThreads_(false) {

    }

    HostSerialPort::~HostSerialPort() {
        close();
    }

    const std::string& HostSerialPort::getPortName() const {
        return portName_;
    }

    void HostSerialPort::setPortName(const std::string& portName) {
        portName_ = portName;
    }

    unsigned int HostSerialPort::getBaudRate() const {
        return baudRate_;
    }

    void HostSerialPort::setBaudRate(unsigned int baudRate) {
        baudRate_ = baudRate;
    }

    unsigned int HostSerialPort::getByteSize() const {
        return byteSize_;
    }

    void HostSerialPort::setByteSize(unsigned int byteSize) {
        byteSize_ = byteSize;
    }

    Parity HostSerialPort::getParity() const {
        return parity_;
    }

    void HostSerialPort::setParity(Parity parity) {
        parity_ = parity;
    }

    StopBits HostSerialPort::getStopBits() const {
        return stopBits_;
    }

    void HostSerialPort::setStopBits(StopBits stopBits) {
        stopBits_ = stopBits;
    }

    void HostSerialPort::open() {
        close();
        portHandle_ = os::serial::open(portName_);
        configure();
        os::serial::setReadIntervals(portHandle_,
                                     READ_INTERVAL_TIMEOUT,
                                     Milliseconds(0),
                                     Milliseconds(0));
        cancelIoThreads_ = false;
        readingThread_ = std::thread(&HostSerialPort::readingThreadRoutine, this);
        writingThread_ = std::thread(&HostSerialPort::writingThreadRoutine, this);
    }

    void HostSerialPort::close() {
        if(!portHandle_)
            return;
        cancelIoThreads_ = true;
        os::serial::close(portHandle_);
        readingThread_.join();
        writingThread_.join();
        portHandle_ = SerialPortHandle();
    }

    void HostSerialPort::clock() {
        std::scoped_lock lock(readMutex_);
        if(!readQueue_.empty()) {
            auto byte = readQueue_.front();
            readQueue_.pop();
            transmit(byte);
        }
    }

    void HostSerialPort::receive(uint8_t byte) {
        {
            std::scoped_lock lock(writeMutex_);
            writeQueue_.push(byte);
        }
        writeCondition_.notify_one();
    }

    void HostSerialPort::configure() {
        os::serial::configure(portHandle_, {
                .baudRate = baudRate_,
                .byteSize = byteSize_,
                .parity = parity_,
                .stopBits = stopBits_
        });
    }

    void HostSerialPort::readingThreadRoutine() {
        uint8_t buffer[READ_BUFFER_SIZE];
        while(!cancelIoThreads_) {
            try {
                auto bytesRead = os::serial::read(portHandle_, buffer, READ_BUFFER_SIZE);
                for (int i = 0; i < bytesRead; i++) {
                    auto byte = buffer[i];
                    {
                        std::scoped_lock lock(readMutex_);
                        readQueue_.push(byte);
                    }
                }
            } catch(std::system_error& error) {
                continue;
            }
        }
    }

    void HostSerialPort::writingThreadRoutine() {
        uint8_t buffer[WRITE_BUFFER_SIZE];
        while(!cancelIoThreads_) {
            // acquire at least one byte
            uint8_t byte;
            {
                std::unique_lock lock(writeMutex_);
                writeCondition_.wait(lock, [this]() -> bool { return !writeQueue_.empty(); });
                byte = writeQueue_.front();
                writeQueue_.pop();
            }
            buffer[0] = byte;
            // acquire remaining bytes (if any)
            int i = 1;
            while(i < WRITE_BUFFER_SIZE) {
                {
                    std::scoped_lock lock(writeMutex_);
                    if(writeQueue_.empty()) {
                        break;
                    }
                    byte = writeQueue_.front();
                    writeQueue_.pop();
                }
                buffer[i] = byte;
                i++;
            };
            try {
                os::serial::write(portHandle_, buffer, i);
            } catch(std::system_error& error) {
                continue;
            }
        }
    }

} // serial
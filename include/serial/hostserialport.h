//
// Created by Piotr on 06.09.2022.
//

#ifndef EMU_HOSTSERIALPORT_H
#define EMU_HOSTSERIALPORT_H

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "serialtransceiver.h"
#include "os/serial.h"

namespace emu::serial {

    using emu::os::serial::SerialPortHandle;

    class HostSerialPort : public SerialTransceiver {
        SerialPortHandle portHandle_;
        std::string portName_;
        unsigned int baudRate_;
        unsigned int byteSize_;
        Parity parity_;
        StopBits stopBits_;

        /* threading */
        std::queue<uint8_t> readQueue_;
        std::mutex readMutex_;
        std::thread readingThread_;

        std::queue<uint8_t> writeQueue_;
        std::mutex writeMutex_;
        std::condition_variable writeCondition_;
        std::thread writingThread_;

        std::atomic<bool> cancelIoThreads_;

    public:
        HostSerialPort();
        ~HostSerialPort() override;

        [[nodiscard]] const std::string& getPortName() const;
        void setPortName(const std::string& portName);

        [[nodiscard]] unsigned int getBaudRate() const;
        void setBaudRate(unsigned int baudRate);

        [[nodiscard]] unsigned int getByteSize() const;
        void setByteSize(unsigned int byteSize);

        [[nodiscard]] Parity getParity() const;
        void setParity(Parity parity);

        [[nodiscard]] StopBits getStopBits() const;
        void setStopBits(StopBits stopBits);

        void open();
        void close();

        void clock();

    protected:
        void receive(uint8_t byte) override;

    private:
        void configure();
        void readingThreadRoutine();
        void writingThreadRoutine();
    /* platform specific code */
    /* these methods should not mutate an object state */
    /*private:
        [[nodiscard]] SerialPortHandle openHandle() const;
        void closeHandle() const;
        void setupConfiguration() const;
        void setNonBlockingRead() const;
        [[nodiscard]] std::optional<uint8_t> readSerialPort() const;
        void writeSerialPort(uint8_t byte) const;*/
    };

} // serial

#endif //EMU_HOSTSERIALPORT_H

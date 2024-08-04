//
// Created by Piotr on 30.09.2022.
//

#ifndef EMU_CONTROLLERPORT_H
#define EMU_CONTROLLERPORT_H

#include <cstdint>

#include "bus/interfaceport.h"
#include "controller/controller.h"

namespace emu::cpu {

    class ControllerPort final : public bus::InterfacePort<uint8_t, uint16_t>{
    public:
        static constexpr uint16_t CONTROLLER_ADDRESS = 0x4016;

        enum class Player {
            FIRST,
            SECOND
        };

    private:
        using Controller = emu::controller::Controller;

        Controller& controller_;

        uint16_t readAddress_;

    public:
        explicit ControllerPort(Controller& controller, Player player = Player::FIRST);

        [[nodiscard]] bool isActive(uint16_t address) const override;
        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;

    };

} // cpu

#endif //EMU_CONTROLLERPORT_H

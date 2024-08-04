#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

#include <cstring>
//#include <cmath>

#include "SDL2/SDL.h"

#include "cartridge/loader.h"
#include "memory/randomaccessmemory.h"
#include "cpu/bus/cartridgeprgport.h"
#include "cpu/bus/internalramport.h"
#include "cpu/bus/ppuport.h"
#include "cpu/bus/cpubus.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "ppu/bus/cartridgechrport.h"
#include "ppu/bus/paletteramport.h"
#include "util/misc.h"
#include "bus/memoryport.h"
#include "util/bin.h"
#include "os/timer.h"
#include "controller/controller.h"
#include "ppu/bus/controllerport.h"

using emu::controller::Button;
using emu::util::Milliseconds;

/* todo: possible optimizations:
 * - no virtual R/W methods in CpuBus
 * - replace executor dispatching unordered_map with classic lookup table
 * - replace std::function with classic function pointers in executeNextOpcode
 * - ... something with Cpu::setAddressing/increasePC - seems like CPU has lots of branch mispredictions on switch-cases
 * - OpcodeAttributes fetching - cache miss problem?. then it calls setAddressing and has another cache miss?
 * - pageCrossed/penalty calculation in executors
 * - HostSerialPort - lock-free queue
 * - std::variant!
 */

namespace emu {

    int run(const char* romPath) {
        ////
        if(SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "Failed to initialize the SDL2 library\n";
            return -1;
        }

        SDL_Window *window = SDL_CreateWindow("SDL2 Window",
                                              SDL_WINDOWPOS_CENTERED,
                                              SDL_WINDOWPOS_CENTERED,
                                              ppu::Ppu::SCREEN_WIDTH, ppu::Ppu::SCREEN_HEIGHT,
                                              0);

        if(!window) {
            std::cout << "Failed to create window\n";
            return -1;
        }
        SDL_SetWindowResizable(window, SDL_TRUE);

        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
        SDL_Surface* surface = SDL_CreateRGBSurface(0, ppu::Ppu::SCREEN_WIDTH, ppu::Ppu::SCREEN_HEIGHT, 8, 0, 0, 0, 0);

        auto& colors = surface->format->palette->colors;
        auto palette = emu::util::misc::readBinaryFile("ntscpalette.pal");
        if(palette.size() % 3 != 0) {
            throw std::runtime_error("invalid palette format");
        }
        for(int i = 0; i < palette.size(); i++) {
            auto* bytes = &palette[i*3];
            colors[i] = SDL_Color{bytes[0], bytes[1], bytes[2], 0};
        }

        auto pixels = static_cast<uint8_t*>(surface->pixels);
        /////

        constexpr double frameFrequencyHz = 60;
        constexpr double frameTimeMs = 1000.0/frameFrequencyHz;
        constexpr Milliseconds timerPeriod(static_cast<int>(frameTimeMs));
        constexpr auto frequencyHz = 1789773;
        constexpr auto cyclesPerWindow = static_cast<int>(frequencyHz * std::chrono::duration<double>(timerPeriod).count());
        constexpr auto messageIntervalSeconds = 1.0;

        constexpr std::size_t RAM_SIZE = 0x10000;

        auto cartridge = cartridge::Loader::open(romPath);
        auto prgPort = cpu::CartridgePrgPort(cartridge);
        auto chrPort = ppu::CartridgeChrPort(cartridge);

        memory::RandomAccessMemory<uint8_t, uint16_t> ram(RAM_SIZE);
        cpu::InternalRamPort memoryPort(ram);

        std::unordered_map<SDL_Scancode, Button> buttonsMap = {
                {SDL_SCANCODE_X, Button::A},
                {SDL_SCANCODE_Z, Button::B},
                {SDL_SCANCODE_RETURN, Button::START},
                {SDL_SCANCODE_BACKSPACE, Button::SELECT},
                {SDL_SCANCODE_UP, Button::UP},
                {SDL_SCANCODE_DOWN, Button::DOWN},
                {SDL_SCANCODE_LEFT, Button::LEFT},
                {SDL_SCANCODE_RIGHT, Button::RIGHT}
        };

        controller::Controller controller;
        cpu::ControllerPort controllerPort(controller);

        cpu::CpuBus cpuBus;
        cpuBus.attach(memoryPort);
        cpuBus.attach(prgPort);
        cpuBus.attach(controllerPort);

        auto cpu = cpu::Cpu(cpuBus);

        ppu::PaletteRam paletteRam;
        ppu::PaletteRamPort paletteRamPort(paletteRam);

        ppu::PpuBus ppuBus;
        ppuBus.attach(chrPort);
        ppuBus.attach(paletteRamPort);

        ppu::Ppu ppu(ppuBus,
                          util::Wire(cpu, &cpu::Cpu::nonMaskableInterrupt),
                          std::span(pixels, surface->w * surface->h));
        cpu::PpuPort ppuPort(ppu);

        cpuBus.attach(ppuPort);

        auto now = std::chrono::high_resolution_clock::now;
        auto totalElapsed = 0.;
        auto showMessageTimer = 0.;
        auto lastCycles = 0;
        auto lastTime = now();

        os::timer::Timer timer(timerPeriod);
        SDL_Event event;
        bool quit = false;

        /*auto masterClock = [&] () {
            ppu.clock();
            ppu.clock();
            ppu.clock();
            cpu.clock();
            controller.clock();
        };*/

        while(!quit) {
            auto windowBeginCycles = cpu.getElapsedCycles();

            while(!ppu.isVerticalBlanking()) {
                ppu.clock();
                ppu.clock();
                ppu.clock();
                cpu.clock();
                controller.clock();
            }

            while(SDL_PollEvent(&event) != 0) {
                switch(event.type) {
                    case SDL_QUIT:
                        quit = true;
                        break;
                    case SDL_KEYDOWN: {
                        auto& keysym = event.key.keysym;
                        if (keysym.scancode == SDL_SCANCODE_R && keysym.mod & KMOD_CTRL) {
                            ppu.reset();
                            cpu.reset();
                        } else if (keysym.scancode == SDL_SCANCODE_ESCAPE) {
                            quit = true;
                        } else if(buttonsMap.contains(keysym.scancode)) {
                            controller.press(buttonsMap[keysym.scancode]);
                        }
                        break;
                    }
                    case SDL_KEYUP: {
                        auto& keysym = event.key.keysym;
                        if(buttonsMap.contains(keysym.scancode))
                            controller.release(buttonsMap[keysym.scancode]);
                        break;
                    }

                }
            }

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
            SDL_RenderClear(renderer);
            SDL_DestroyTexture(texture);

            while(ppu.isVerticalBlanking()) {
                ppu.clock();
                ppu.clock();
                ppu.clock();
                cpu.clock();
                controller.clock();
            }

#ifndef DEBUG
            timer.wait();
#endif

            auto currentTime = now();
            std::chrono::duration<double> elapsedDuration = (currentTime - lastTime);
            auto elapsed = elapsedDuration.count();
            lastTime = currentTime;
            totalElapsed += elapsed;
            showMessageTimer += elapsed;
            if(showMessageTimer >= messageIntervalSeconds) {
                auto currentCycles = cpu.getElapsedCycles();
                auto executedCycles = currentCycles - lastCycles;
                auto windowExecutedCycles = currentCycles - windowBeginCycles;
                lastCycles = currentCycles;

                std::cout << "avg: " << currentCycles / totalElapsed / 1000000 << " MHz, "
                          << "1s avg: " << executedCycles / messageIntervalSeconds / 1000000 << " MHz, "
                          << "wnd: " << windowExecutedCycles / elapsed / 1000000 << " MHz "
                          << std::endl;
                showMessageTimer = 0.;
            }
        }

        return 0;
    }

}

int main(int argc, char** argv) {
    if(argc <= 1) {
        std::cout << "no NES ROM was specified" << std::endl;
        return -1;
    }
    auto romPath = argv[1];
    return emu::run(romPath);
}

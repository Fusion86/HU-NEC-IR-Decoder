#pragma once

#include <rtos.hpp>

//
// Structs
//

struct ir_msg {
    int address;
    int command;
    bool repeat = false;
};

//
// States
//

enum pause_detector_state {
    idle,
    signal,
};

//
// Interfaces
//

class msg_listener {
  public:
    virtual void msg_received(ir_msg msg) = 0;
};

class pause_listener {
  public:
    virtual void pause_detected(int length) = 0;
};

//
// Tasks
//

class msg_logger : public rtos::task<>, public msg_listener {
  private:
  public:
    msg_logger()
        : task("msg_logger") {}

    void main() override {
        hwlib::cout << "hello from msg_logger";
        for (;;) {
            hwlib::wait_ms(1000);
        }
    }

    void msg_received(ir_msg msg) override {
        hwlib::cout << "sparkysparky\n";
    }
};

class msg_decoder : public rtos::task<>, public pause_listener {
  private:
    msg_listener& listener;

  public:
    msg_decoder(msg_listener& listener)
        : task("msg_decoder"), listener(listener) {}

    void main() override {
        hwlib::cout << "hello from msg_decoder";
        for (;;) {
            hwlib::wait_ms(1000);
        }
    }

    void pause_detected(int length) override {
        hwlib::cout << "pause: " << length << "us\n";
    }
};

class pause_detector : public rtos::task<> {
  private:
    hwlib::target::pin_in signal;
    pause_listener& listener;
    pause_detector_state state = pause_detector_state::idle;

  public:
    pause_detector(pause_listener& listener)
        : task("pause_detector"), signal(hwlib::target::pins::d8), listener(listener) {}

    void main() override {
        int length = 0;
        for (;;) {
            switch (state) {
                case pause_detector_state::idle:
                    hwlib::wait_us(100);
                    signal.refresh();
                    if (signal.read()) {
                        listener.pause_detected(length);
                        state = pause_detector_state::signal;
                    } else {
                        length += 100;
                    }
                    break;
                case pause_detector_state::signal:
                    hwlib::wait_us(100);
                    signal.refresh();
                    if (!signal.read()) {
                        length = 0;
                        state = pause_detector_state::idle;
                    }
                    break;
            }
        }
    }
};

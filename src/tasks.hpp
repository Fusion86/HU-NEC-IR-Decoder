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

enum class pause_detector_state {
    idle,
    signal,
};

enum class msg_decoder_state {
    idle,
    message_in_progress,
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
    msg_decoder_state state;
    rtos::channel<int, 100> pauses;

  public:
    msg_decoder(msg_listener& listener)
        : task("msg_decoder"), listener(listener), pauses(this, "pauses") {}

    void main() override {
        int p, m, n;
        for (;;) {
            switch (state) {
                case msg_decoder_state::idle:
                    p = pauses.read();
                    if (p > 4'000 && p < 5'000) {
                        n = m = 0;
                        state = msg_decoder_state::message_in_progress;
                    }
                    break;
                case msg_decoder_state::message_in_progress:
                    p = pauses.read();
                    if (p > 200 && p < 2'000) {
                        n++;
                        m = m << 1;
                        m |= (p > 1'000) ? 1 : 0;

                        if (n == 0) {
                            // check();
                            state = msg_decoder_state::idle;
                        }
                    } else {
                        state = msg_decoder_state::idle;
                    }
                    break;
            }
        }
    }

    void pause_detected(int length) override {
        pauses.write(length);
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
        // TODO: hwlib::wait_us() is inaccurate for creating a clock-based loop
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

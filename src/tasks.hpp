#pragma once

#include <rtos.hpp>

#include "util.hpp"

#define HIGH 1
#define LOW 0

//
// Structs
//

struct ir_msg {
    uint8_t address;
    uint8_t command;
};

//
// States
//

enum class pause_detector_state {
    // Waiting for signal (when input is high)
    idle,
    // Receiving signal (when input low)
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
    rtos::channel<ir_msg, 10> messages;

  public:
    msg_logger()
        : task("msg_logger"), messages(this, "messages") {}

    void main() override {
        for (;;) {
            auto msg = messages.read();
            hwlib::cout << "address: " << msg.address << "\ncommand: " << msg.command << "\n\n";
        }
    }

    void msg_received(ir_msg msg) override {
        messages.write(msg);
    }
};

class msg_decoder : public rtos::task<>, public pause_listener {
  private:
    msg_listener& listener;
    msg_decoder_state state = msg_decoder_state::idle;
    rtos::channel<int, 100> pauses;

  public:
    msg_decoder(msg_listener& listener)
        : task("msg_decoder"), listener(listener), pauses(this, "pauses") {}

    void main() override {
        int pause, num_bits, data = 0;
        for (;;) {
            pause = pauses.read();
            switch (state) {
                case msg_decoder_state::idle:
                    if (pause > 4'000 && pause < 5'000) {
                        // When pause after start signal
                        num_bits = data = 0;
                        state = msg_decoder_state::message_in_progress;
                    }
                    break;
                case msg_decoder_state::message_in_progress:
                    if (num_bits == 32) {
                        state = msg_decoder_state::idle;
                        if (check(data)) {
                            listener.msg_received(data_to_msg(data));
                        } else {
#ifdef DEBUG
                            hwlib::cout << "invalid msg\n";
                            print_bits(data);
                            hwlib::cout << "\n";
#endif
                        }
                    } else {
                        num_bits++;
                        data <<= 1;
                        data |= pause > 1'000 ? 1 : 0;
                    }
                    break;
            }
        }
    }

    void pause_detected(int length) override {
        pauses.write(length);
    }

    bool check(int data) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
        uint8_t* ptr = (uint8_t*)&data;
        // Check byte0 == ~byte1 && byte2 == ~byte3
        return ptr[0] == (~ptr[1] & 0xFF) && ptr[2] == (~ptr[3] & 0xFF);
#pragma GCC diagnostic pop
    }

    ir_msg data_to_msg(int data) {
        ir_msg msg;
        msg.address = data >> 24;
        msg.command = data >> 8 & 0xFF;
        return msg;
    }
};

class pause_detector : public rtos::task<> {
  private:
    hwlib::target::pin_in signal;
    pause_listener& listener;
    rtos::clock clock;
    pause_detector_state state = pause_detector_state::idle;

  public:
    pause_detector(pause_listener& listener, hwlib::target::pin_in pin)
        : task("pause_detector"), signal(pin), listener(listener), clock(this, 100, "clock") {}

    void main() override {
        int n = 0;
        for (;;) {
            wait(clock);
            switch (state) {
                case pause_detector_state::idle:
                    if (signal.read() == LOW) {
                        // When receiving an IR signal
                        listener.pause_detected(n);
                        state = pause_detector_state::signal;
                    } else {
                        // When NOT receiving an IR signal
                        n += 100;
                    }
                    break;
                case pause_detector_state::signal:
                    if (signal.read() == HIGH) {
                        // When NOT receiving an IR signal
                        n = 0;
                        state = pause_detector_state::idle;
                    }
                    break;
            }
        }
    }
};

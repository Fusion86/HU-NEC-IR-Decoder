#pragma once

#include <rtos.hpp>

//
// Interfaces
//

class msg_listener {
};

class pause_listener {
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
        for (;;) {
        }
    }
};

class msg_decoder : public rtos::task<>, public pause_listener {
  private:
  public:
    msg_decoder()
        : task("msg_decoder") {}

    void main() override {
        for (;;) {
        }
    }
};

class pause_detector : public rtos::task<> {
  private:
  public:
    pause_detector()
        : task("pause_detector") {}

    void main() override {
        for (;;) {
        }
    }
};

#include <rtos.hpp>

#include "tasks.hpp"

int main() {
    auto t1 = pause_detector();
    auto t2 = msg_decoder();
    auto t3 = msg_logger();
    rtos::run();
}

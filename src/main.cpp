#include <rtos.hpp>

#include "tasks.hpp"
#include "version.h"

int main() {
    // Wait for the PC console to start
    hwlib::wait_ms(2000);

    hwlib::cout << "SparkySparky v" << SPARKYSPARKY_VERSION << "\n";

    auto t1 = pause_detector();
    auto t2 = msg_decoder();
    auto t3 = msg_logger();
    rtos::run();
}

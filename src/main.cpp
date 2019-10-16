#include <rtos.hpp>

#include "tasks.hpp"
#include "version.h"

int main() {
    // Wait for the PC console to start
    hwlib::wait_ms(2000);

    hwlib::cout << "SparkySparky v" << SPARKYSPARKY_VERSION << "\n";

    auto logger = msg_logger();
    auto decoder = msg_decoder(logger);
    auto detector = pause_detector(decoder);
    rtos::run();
}

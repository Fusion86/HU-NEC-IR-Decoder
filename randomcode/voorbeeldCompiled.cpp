#include "hwlib.hpp"
#include <cstring>

unsigned long nec_remote_read(hwlib::target::pin_in IRpin);

int bytes[32] = {0};
int main(void) {

  namespace target = hwlib::target;

  auto tsop_signal = target::pin_in(target::pins::d8);

  // auto led = target::pin_out(target::pins::led);

  for (;;) {
    for (unsigned int i = 0; i < 32; i++) {
      bytes[i] = {0};
    };  
    nec_remote_read(tsop_signal);
    for (unsigned int i = 0; i < 32; i++) {
      hwlib::cout << bytes[i];
      if (i == 7 || i == 15 || i == 23) {
        hwlib::cout << ',';
      }
    }
    hwlib::cout << '\n';
  }
}

unsigned long nec_remote_read(hwlib::target::pin_in IRpin) {
  // unsigned long ir_code;
  unsigned int count = 0;

  while (IRpin.read() && (count < 300)) { /// 9MS ON
    count++;
    hwlib::wait_us(40);
  }
  if (count > 180 || count < 240) { // 4.5MS OFF
    count = 0;

    while (!IRpin.read() && (count < 200)) {
      count++;
      hwlib::wait_us(40);
    }
    if (count > 80 || count < 120) {
      for (unsigned int i = 0; i < 32; i++) {
        count = 0;
        while (IRpin.read() && (count < 800)) {
          count++;
          hwlib::wait_us(20);
        }

        count = 0;
        while (!IRpin.read() && (count < 110)) {
          count++;
          hwlib::wait_us(20);
        }
        // if (count > 95) {
        //   return 0;
        // }
        // if (count < 6) {
        //   return 0;
        // }

        if (count > 1) { // If space width > 1ms
          bytes[i] = 1;
          // hwlib::cout << "1" << hwlib::endl;

        } else { // If space width < 1ms
          bytes[i] = 0;
          // hwlib::cout << "0" << hwlib::endl;
        }
      };

      return 0;
    };
    return 0;
  };
  return 0;
};

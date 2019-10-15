// simple IR signal detector

#include "hwlib.hpp"

unsigned long nec_remote_read(hwlib::target::pin_in IRpin);

int main(void) {

  namespace target = hwlib::target;

  auto tsop_signal = target::pin_in(target::pins::d8);
  auto tsop_gnd = target::pin_out(target::pins::d9);
  auto tsop_vdd = target::pin_out(target::pins::d10);
  tsop_gnd.write(0);
  tsop_vdd.write(1);
  tsop_gnd.flush();
  tsop_vdd.flush();

  // auto led = target::pin_out(target::pins::led);


  for (;;) {
    hwlib::cout << nec_remote_read(tsop_signal) << hwlib::endl;
  }
}

unsigned long nec_remote_read(hwlib::target::pin_in IRpin) {
  unsigned long ir_code;
  unsigned long count = 0, i;
  // Check 9ms pulse (remote control sends logic high)
  while (IRpin.read() && (count < 180)) {   ///mischien  inverteren van pin.Read()? als niet werkt.
    count++;
    hwlib::wait_us(40);
  }

  count = 0;
  // Check 4.5ms space (remote control sends logic low)
  while (IRpin.read() && (count < 90)) {
    count++;
    hwlib::wait_us(40);
  }

  // Read code message (32-bit)
  for (i = 0; i < 32; i++) {
    count = 0;
    while ((!IRpin.read()) && (count < 15)) {
      count++;
      hwlib::wait_us(40);
    }
    if ((count > 14) || (count < 2)) // NEC protocol?
      return 0;

    count = 0;
    while ( IRpin.read() && (count < 30)) {
      count++;
      hwlib::wait_us(40);
    }
    if ((count > 29) || (count < 4)) // NEC protocol?
      return 0;

    if (count > 20)                  // If space width > 1ms
      ir_code |= 1ul << (31 - i);    // Write 1 to bit (31 - i)
    else                             // If space width < 1ms
      ir_code &= ~(1ul << (31 - i)); // Write 0 to bit (31 - i)
  }
  return ir_code;
}

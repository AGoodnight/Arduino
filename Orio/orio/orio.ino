/* -------------------------------------------------------------------------
WIFI HOTSPOT DETECTOR for Adafruit Pro Trinket and CC3000 WiFi adapter
(can also work with Arduino Uno, etc.).  Periodically scans for open
networks (those with no password required), LED on pin 9 shows status:
 
  Steady on    = One-time initialization
  Fast flicker = Scanning for WiFi networks
  Slow blink   = Open networks in range
  1 sec blips  = Closed networks only
  4 sec blips  = No WiFi networks (open OR closed) detected
 
CC3000 boards with integrated chip antenna are most compact but have
limited sensitivity.  The uFL connector versions improve range, but
require adding an adapter and antenna, making a larger package.
5dBi antenna is more sensitive than most laptops; 2dBi is ideal.
 
  https://www.adafruit.com/product/2000  Pro Trinket 5V 16 MHz
  https://www.adafruit.com/product/2010  Pro Trinket 3V 12 MHz
  https://www.adafruit.com/product/2124  Pro Trinket LiIon/LiPoly backpack
  https://www.adafruit.com/product/1570  Lithium Polymer 100 mAh battery
  https://www.adafruit.com/product/1578  Lithium Polymer 500 mAh battery
  https://www.adafruit.com/product/1469  CC3000 breakout w/ chip antenna
  https://www.adafruit.com/product/1510  CC3000 breakout w/ uFL connector
  https://www.adafruit.com/product/852   uFL->RP-SMA adapter
  https://www.adafruit.com/product/944   4" 2dBi gain RP-SMA antenna
  https://www.adafruit.com/product/945   8" 5dBi gain RP-SMA antenna
 
Requires Adafruit_CC3000 and TimerOne libraries:
 
  https://github.com/adafruit/Adafruit_CC3000_Library
  https://github.com/PaulStoffregen/TimerOne
 
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing products
from Adafruit!
 
Written by Phil Burgess and Tony DiCola for Adafruit Industries.
BSD license, all text above must be included in any redistribution.
----------------------------------------------------------------------------*/
 
#include <Adafruit_CC3000.h>
#include <TimerOne.h>
#include <SPI.h>
#include <avr/sleep.h>
#include <avr/power.h>
 
// Hardware-based PWM is used to drive the status LED.  This means it's tied
// to a specific pin and can't be moved, nor can this pin be used for CC3000
// interfacing or other functions.  The exact pin number may vary from board
// to board -- in low-level AVR pin maps it's identified as OC1A.  This is
// DIGITAL PIN 9 on the Adafruit Pro Trinket, Arduino Uno, Duemilanove,
// Diecimila and Leonardo.  Looks like this is pin 14 on a PJRC Teensy 2.0 or
// pin 11 on Arduino Mega (but this is a tiny sketch and a waste of a Mega).
// This goes to the anode (+) leg of the LED, cathode (-) goes to GND.
 
// The CC3000 interrupt and control pins are configurable...
#define ADAFRUIT_CC3000_IRQ  3 // MUST be an interrupt pin!
#define ADAFRUIT_CC3000_VBAT 5 // These can be
#define ADAFRUIT_CC3000_CS  10 // any two pins.
#define WLAN_SSID "rbn"
#define WLAN_PASS "SkyIsCl0udy"
#define WLAN_SECURITY WLAN_SEC_WPA2
// Hardware SPI for remaining pins.  On Pro Trinket, SCK=13, MISO=12, MOSI=11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS,
  ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER);
 
// AVR sleep mode is used to conserve power.  Wake is initiated by the
// watchdog timer (WDT), set later in this code for 8-second intervals.
// Because the CC3000 library is still a bit experimental in spots, the WDT
// is also used to detect code lockups...the interrupt handler counts down
// and if it reaches zero the WDT is switched to system reset mode; the
// sketch resets the counter periodically to indicate continued "OK" state.
const uint8_t         // These are the number of 8-second WDT interrupts...
  sleepIntervals = 7, // ...while sleeping, between SSID scans
  runIntervals   = 7; // ...during SSID scan, max # before reset issued.
volatile uint8_t      // 'volatile' because interrupt may change this
  wdtCount       = runIntervals+1; // Init counter to SSID scan max time
// Although the WDT interval is 8 seconds, the first interval may already
// be partly elapsed when the counter is reset...so although the sleep
// time is set for 7 intervals -- 56 sec -- in reality it might be up to
// one full interval shorter -- 48 sec.  The SSID scan typically just needs
// a few seconds to run, but 7 intervals are allotted for good measure.
// If it's finished before then, that's fine and normal sleep commences.
// If it's NOT finished, code is presumed locked up and the system resets.
 
// ---------------------------------------------------------------------------
 
void setup(void) {
 
  // Watchdog timer setup.  Abbreviated here; original from Tony DiCola's
  // excellent Low Power WiFi Datalogger guide:
  // https://learn.adafruit.com/low-power-wifi-datalogging
  noInterrupts();
  MCUSR  &= ~_BV(WDRF);
  WDTCSR  =  _BV(WDCE) | _BV(WDE);              // WDT change enable
  WDTCSR  =  _BV(WDIE) | _BV(WDP3) | _BV(WDP0); // Interrupt enable, 8 sec.
  interrupts();
 
  // Initial pin 9 LED state is always-on.  If the CC3000 init fails,
  // the LED remains in this state to indicate there's trouble.
  Timer1.initialize();
  Timer1.pwm(TIMER1_A_PIN, 1023); // 1023 = 100%
 
  // Serial console messages help with debugging, but aren't seen in
  // normal use.  Pro Trinket requires FTDI adapter for Serial I/O.
  Serial.begin(57600);
  Serial.print(F("Initializing CC3000..."));
  if(!cc3000.begin()) {
    Serial.println(F("failed. Check your wiring?"));
    for(;;);
  }
  Serial.println(F("OK."));
 
  // AVR peripherals that aren't used by this code are disabled to further
  // conserve power, and may take certain Arduino functionality with them.
  // If you adapt this code to other projects, may need to re-enable some.
  power_adc_disable();    // Disable ADC (no analogRead())
  power_twi_disable();    // Disable I2C (no Wire library)
  power_timer2_disable();
 
  set_sleep_mode(SLEEP_MODE_IDLE);
}
 
// ---------------------------------------------------------------------------
 
const uint8_t PROGMEM
   mode0[] = "Open",
   mode1[] = "WEP",
   mode2[] = "WPA",
   mode3[] = "WPA2";
  
 
void loop(void) {
  uint32_t count;
  uint8_t  rssi, sec;
  char     ssid[33];
 
  // PWM set to 10 Hz during scan w/ short 'blip' duty cycle.
  // This line can be commented out for a more discreet scan,
  // LED will retain prior blink until new state is known.
  Timer1.pwm(TIMER1_A_PIN, 10, 100000);
 
  Serial.print(F("Scanning..."));
  if(cc3000.startSSIDscan(&count)) { // Initiate SSID scan
    // Default LED state is 'no networks found' -- a short 'blip'
    // every 4 seconds provides a quiet heartbeat:
    Timer1.pwm(TIMER1_A_PIN, 1, 4000000); // 4 sec, tiny duty cycle
    if(count) { // Networks found!  How many?
      Serial.print(count); Serial.print(F(" network"));
      if(count > 1) Serial.print('s');
      Serial.println(F(" found:"));
      // Networks found, but open/closed state not yet known.
      // Presume closed only, shorten heartbeat to 1 sec.
      Timer1.pwm(TIMER1_A_PIN, 1, 1000000); // 1 sec, tiny duty cycle
 
      while(count--) { // For each network...
        if(cc3000.getNextSSID(&rssi, &sec, ssid)) { // Valid?
          if(sec > 3) sec = 3;
          // Dump basic info to Serial console for -all- networks
          Serial.print(F("SSID      : ")); Serial.println(ssid);
          Serial.print(F("  RSSI    : ")); Serial.println(rssi);
          Serial.print(F("  Security: "));
          //Serial.println((__FlashStringHelper *)pgm_read_word(&modes[sec]));
          if(sec == WLAN_SEC_UNSEC) { // If open network...
            // Switch LED to conspicuous 'open networks' flash immediately
            Timer1.pwm(TIMER1_A_PIN, 511, 1000000); // 1 sec, 50% duty cycle
            // "Open hotspot" is as good as the indicator gets and the scan
            // can stop now, get into power-saving sleep mode ASAP.
            // If you're using the Serial console and want to see all
            // networks displayed, comment out this line:
            break;
          }
        }
      }
    } else Serial.println(F("no networks found."));
  } else   Serial.println(F("failed!"));
 
  cc3000.stopSSIDscan();
 
  Serial.print(F("Sleeping..."));
  wlan_stop();
 
  // Additional AVR peripherals are now shut down, most everything except
  // Timer/Counter 1, used for the status LED.  'Idle' mode is the deepest
  // sleep level that still allows this timer to continue running.
  power_spi_disable();    // Disable SPI (CC3000 comms)
  delay(50);              // Stall for last serial output
  power_timer0_disable(); // Disable millis() etc.
  power_usart0_disable(); // Disable Serial
 
  // WDT interrupt counts down 8-second intervals from sleepIntervals+1
  // to 1 (NOT 0 -- that's when a lockup-busting system reset is performed).
  for(wdtCount=sleepIntervals+1; wdtCount>1; ) sleep_mode();
  // Done sleeping, set counter to max SSID scan time before sys reset
  wdtCount = runIntervals+1;
 
  // Restore AVR peripherals used elsewhere in the code
  power_usart0_enable(); // Enable Serial
  power_timer0_enable(); // Enable millis() etc.
  power_spi_enable();    // Enable SPI (for CC3000)
  wlan_start(0);         // Wake up CC3000
  Serial.println(F("done."));
}
 
ISR(WDT_vect) { // Watchdog interrupt @ 8 sec. interval
  if(!--wdtCount) { // Decrement sleep interval counter...
    // If it reaches zero, WDT switched from interrupt to system reset mode...
    noInterrupts();
    WDTCSR = _BV(WDCE) | _BV(WDE);             // WDT change enable
    WDTCSR = _BV(WDP2) | _BV(WDP1) | _BV(WDE); // System reset enable, 1 sec
    for(;;);                                   // Wait for reset
  }
}

#include <Arduino.h>

// reading liquid flow rate using Seeeduino and Water Flow Sensor from Seeedstudio.com
// Code adapted by Charles Gantt from PC Fan RPM code written by Crenn @thebestcasescenario.com
// http:/themakersworkbench.com http://thebestcasescenario.com http://seeedstudio.com

volatile int NbTopsFan; //measuring the rising edges of the signal
int Calc;
int hallsensor = 2;    //The pin location of the sensor

void rpm ()     //This is the function that the interupt calls
{
  NbTopsFan++;  //This function measures the rising and falling edge of the hall effect sensors signal
}



// Global variables
const unsigned int ZERODELAY =
    4000; // ms, zero RPM if no result for this time period
unsigned int windRpm = 0;
unsigned int windRpmMax = 0;
unsigned int windStopped = 0;
// volatiles are subject to modification by IRQs
volatile unsigned long tempWindRpm = 0UL;
volatile unsigned long windTime = 0UL;
volatile unsigned long windLast = 0UL;
volatile unsigned long windInterval = 0UL;
volatile unsigned char windIntCount;
volatile boolean gotWindSpeed;

void wind_speed_irq()
// if the Weather Meters are attached, measure anemometer RPM (2 ticks per
// rotation), set flag if RPM is updated
// activated by the magnet in the anemometer (2 ticks per rotation), attached to
// input D3

// this routine measures RPM by measuring the time between anemometer pulses
// windintcount is the number of pulses we've measured - we need two to measure
// one full rotation (eliminates any bias between the position of the two
// magnets)
// when windintcount is 2, we can calculate the RPM based on the total time from
// when we got the first pulse
// note that this routine still needs an outside mechanism to zero the RPM if
// the anemometer is stopped (no pulses occur within a given period of time)
{
  windTime = micros(); // grab current time
  if ((windIntCount == 0) || ((windTime - windLast) > 10000)) {
    // ignore switch-bounce glitches less than 10ms after the reed switch closes
    if (windIntCount == 0) {
      // if we're starting a new measurement, reset the interval
      windInterval = 0;
    } else {
      // otherwise, add current interval to the interval timer
      windInterval += (windTime - windLast);
    }
    if (windIntCount == 2) {
      // we have two measurements (one full rotation), so calculate result and
      // start a new measurement
      tempWindRpm = (60000000UL / windInterval); // calculate RPM (temporary
                                                 // since it may change
                                                 // unexpectedly)
      windIntCount = 0;
      windInterval = 0;
      gotWindSpeed = true; // set flag for main loop
    }

    windIntCount++;
    windLast = windTime; // save the current time so that we can calculate the
                         // interval between now and the next interrupt
  }
}

// void publish_wind_measurement()
// {
//   float windSpeedMeasurement = -1.0; // this value should never be published
// // publish instantaneous wind speed
// #if ENABLE_WIND_MEASUREMENT_AVERAGING
// windSpeedMeasurement = wind_spd_avg.getAverage();
// #else
// windSpeedMeasurement = float(windRpm) / WIND_RPM_TO_KNOTS;
// #endif
//
// sensor[F("speed")] = windSpeedMeasurement;
//
// // publish maximum wind speed since last report
// windSpeedMeasurement = float(windRpmMax) / WIND_RPM_TO_KNOTS;
// }


void setup()
{
  Serial.begin(9600);

  // init
  pinMode(hallsensor, INPUT);

  attachInterrupt(0, rpm, RISING);

  // // init wind speed interrupt global variables
  // gotWindSpeed = false;
  // windRpm = 0;
  // windIntCount = 0;
  // attachInterrupt(1, wind_speed_irq, FALLING);
}

void loop ()
{
  NbTopsFan = 0;   //Set NbTops to 0 ready for calculations
  // turn on interrupts
  interrupts();
  //  sei();      //Enables interrupts
  delay (1000);   //Wait 1 second
  cli();      //Disable interrupts
  Calc = (NbTopsFan * 60 / 5.5); //(Pulse frequency x 60) / 5.5Q, = flow rate in L/hour
  Serial.print (Calc, DEC); //Prints the number calculated above
  Serial.print (" L/hour\r\n"); //Prints "L/hour" and returns a  new line



  // // handle weather meter interrupts
  // static unsigned long windStopped = 0UL;
  //
  // // an interrupt occurred, handle it now
  // if (gotWindSpeed) {
  //   gotWindSpeed = false;
  //   windRpm = word(tempWindRpm);
  //   if (windRpm > windRpmMax) {
  //     windRpmMax = windRpm;
  //   }
  //   windStopped = millis() + ZERODELAY; // save this timestamp
  // }
  //
  // // zero wind speed RPM if a reading does not occur in ZERODELAY ms
  // if (millis() > windStopped) {
  //   windRpm = 0;
  //   windIntCount = 0;
  // }

}

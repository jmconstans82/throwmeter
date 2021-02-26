/*
   --------------------------------------------------
   Copyright Jmconstans82, France, 2021 

   based on J'm f5mmx mma8452 version
   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   This is a beerware; if you like it and if we meet some day, you can pay me a beer in return!
   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

*/
#include <Wire.h>
#include <MMA8452.h>                  // MMA8452 library            from https://github.com/akupila/Arduino-MMA8452
#include <U8g2lib.h>                  // Oled U8g2 library          from https://github.com/olikraus/U8g2_Arduino/archive/master.zip
#include <EasyButton.h>

int action = 0;
double corde,  ref_angle;
const float pi = M_PI;

long t1 = 0;                          // Timer for Display management
long t2 = 0;                          // Timer for measure
#define DELAY_MEASURE 10              // Measure each 10ms
#define DELAY_DISPLAY 250             // Display display each 250ms

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

//----------------------------------------------------------------------------------------------------------------------------------
// Push button definition
//----------------------------------------------------------------------------------------------------------------------------------
#define PinD3 3                   // Push button wired on D2
#define PinD4 4                   // Push button wired on D2
// boutonmoins
EasyButton boutonmoins(PinD3);
// boutonplus
EasyButton boutonplus(PinD4);

// Callback function to be called when boutonmoins is pressed
void onboutonmoinsPressed()
{
   //Serial.println("TAP - event detected");          
   if (corde > 11)  {
      corde--;
   }
}

// Callback function to be called when boutonplus is pressed
void onboutonplusPressed()
{
  //Serial.println("TAP + event detected");          
  if (corde < 149) {
     corde++;
  }
}
// Callback function to be called when the button is pressed.
void onPressedForDuration()
{
   //Serial.println("HOLD event detected");
   affiche_init();               // display initialisation page
   init_angle();                 // reference angle = current angle
}

//----------------------------------------------------------------------------------------------------------------------------------
MMA8452 mma;
//----------------------------------------------------------------------------------------------------------------------------------
double read_angle() {                             // Function returning the current rotation value along X axis - in degrees
  //----------------------------------------------------------------------------------------------------------------------------------
  float x, y, z;
  double l_angle = 0, ll_angle = 0;
  int mm = 150;
  for (int nn = 1;  nn <= mm; nn++) {
    mma.getAcceleration(&x, &y, &z);
    l_angle = atan2(y, z) / pi * 180 * 10;
    ll_angle = ll_angle + l_angle;
  }
  l_angle = round(ll_angle / mm);
  ll_angle = l_angle / 10;
  ////Serial.println(String(l_angle, 5) + "   -   " + String( ll_angle, 5));
  return ll_angle;
}
//----------------------------------------------------------------------------------------------------------------------------------
void init_angle() {
  //----------------------------------------------------------------------------------------------------------------------------------
  double ra = 0;
  delay(200);
  ra = read_angle();                              // Initialize the actual angle as the reference angle
  ref_angle = ra;
}
//----------------------------------------------------------------------------------------------------------------------------------
// Button initialization
//----------------------------------------------------------------------------------------------------------------------------------
void setup_Button() {
  // Initialize the boutonmoins
  boutonmoins.begin();
  // Initialize the boutonplus
  boutonplus.begin();
  // Add the callback function to be called when the boutonmoins is pressed.
  boutonmoins.onPressed(onboutonmoinsPressed);
  // Add the callback function to be called when the boutonplus is pressed.
  boutonplus.onPressed(onboutonplusPressed);
  // Add the callback function to be called when the boutonmoins is pressed more than 2s
  boutonmoins.onPressedFor(2000, onPressedForDuration);
  }
//----------------------------------------------------------------------------------------------------------------------------------
void setup() {
  //----------------------------------------------------------------------------------------------------------------------------------
//  delay(500);
  corde = 50;                                     // Chord width value
  Wire.begin();
  pinMode(PinD3, INPUT_PULLUP);              // define the Push Button input
  pinMode(PinD4, INPUT_PULLUP);              // define the Push Button input
    
  //Serial.begin(115200);
  //Serial.println("Init done");
  u8g2.begin();                                   // Start Oled
  affiche_init();
  delay(500);
  if (! mma.init()) {                            // Try to start MMA, if fails then display error message
    u8g2.firstPage();
    do {
      u8g2.setFontDirection(0);
      u8g2.setFont(u8g2_font_t0_14_tf);
      u8g2.setCursor(18, 15);
      u8g2.print("Erreur MMA8452");
      //Serial.println("Erreur MMA");
    } while ( u8g2.nextPage() );
    while (1);
  }

  mma.setDataRate(MMA_400hz);
  mma.setRange(MMA_RANGE_2G);
  setup_Button();                                // Initialize push button behavior

  init_angle();
}

//----------------------------------------------------------------------------------------------------------------------------------
String cnv_flt2str(float num, int car, int digit) { // Convert a float variable into a string with a specific number of digits
  //----------------------------------------------------------------------------------------------------------------------------------
  String str = "";
  if (digit > 0) {
    str = String(num, digit);
  } else {
    str = String(int(num));
  }
  while (str.length() < car) {
    str = " " + str;
  }
  return str;
}

//----------------------------------------------------------------------------------------------------------------------------------
void affiche_init() {
  //----------------------------------------------------------------------------------------------------------------------------------
  u8g2.firstPage();                                                 // Display values
  do {
    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_t0_14_tf);
    u8g2.setCursor(18, 24);
    u8g2.print("INIT EN COURS");
  } while ( u8g2.nextPage() );
}
//----------------------------------------------------------------------------------------------------------------------------------
void affiche(String l_angle, String l_corde, String l_debat) {
  //----------------------------------------------------------------------------------------------------------------------------------
  u8g2.firstPage();                                                 // Display values
  do {
    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_t0_11_tf);
    u8g2.setCursor(1, 10);
    u8g2.print("Angle deg -->");
    u8g2.setFont(u8g2_font_crox4tb_tn);
    u8g2.setCursor(78, 15);
    u8g2.print(l_angle);
    u8g2.setFont(u8g2_font_t0_11_tf);
    u8g2.setCursor(1, 21);
    u8g2.print("Corde " + l_corde + "mm");
    u8g2.setCursor(1, 31);
    u8g2.print("Debat mm --->");
    u8g2.setFont(u8g2_font_crox4tb_tn);
    u8g2.setCursor(78, 32);
    u8g2.print(l_debat);
  } while ( u8g2.nextPage() );
}

void manageButton() {
    boutonmoins.read();
    boutonplus.read();
}

//----------------------------------------------------------------------------------------------------------------------------------
void loop() {                                     // Main loop
  //----------------------------------------------------------------------------------------------------------------------------------
  float x_rot = 0, aff_angle = 0, angle = 0, debat = 0;
  int act = 0;
  manageButton();           // Read button action and react as needed
  if (millis() - t2 > DELAY_MEASURE)
  {
     t2 = millis();
     x_rot = read_angle();                                               // read current angle
     x_rot = ref_angle - x_rot;                                          // compute angle variation vs. reference angle
     angle = x_rot / 180 * pi;                                     // angle value converted from radian
     debat = sqrt(2 * sq(corde) - (2 * sq(corde) * cos(angle)));         // throw computation in same units as chord
  }
  if ((millis() - t1) > DELAY_DISPLAY) {                     // If update display authorized
     t1 = millis();
     affiche(cnv_flt2str(x_rot, 6, 1), cnv_flt2str(corde, 4, 1), cnv_flt2str(debat, 6, 1));
  }   
}

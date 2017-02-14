/*
  Cloud-SDR example to receive, process and display currently received frequency

 */
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
 
#define CT_NO_SDRNODE (0)
#define CT_SDRNODE_ATTACHED (1)
#define CT_SDRNODE_READY (2)

int current_state =  CT_NO_SDRNODE ;
int next_state = CT_NO_SDRNODE ;
int led = 13;


long last_call = 0 ;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

/*
 * Message is device:f:frequency
 * example : 
 *             rtlsdr:f:432.500
 */
void frequencyMessage( String message ) {
   ledshow() ;
   int p = message.indexOf(':');
   String sdr_name = message.substring(0,p);
   message = message.substring(p+3);
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("SDR:" + sdr_name );
   lcd.setCursor(0, 1);
   lcd.print("RX :" + message );

   // do something with the frequency
   float freq = message.toFloat();
   // example : hf/vhf/uhf antenna switch
   if( (freq>0.1) and (freq<30.0)) {
       // swith to HF
   }
   else 
   if( (freq >= 30) and (freq<300)) {
       // switch VHF
   }
   else
   if( freq>=300) {
      // switch UHF
   }
}

void setup() {
  current_state =  CT_NO_SDRNODE ;
  next_state = CT_NO_SDRNODE ;
  last_call =  millis() ; 
  
  pinMode(led, OUTPUT);     
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Booting");
  lcd.setCursor(0, 1);
  lcd.print("Cloud-SDR.com" );
}

void ledshow() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);                 
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(100);   
}

void loop() { 
  long now ;
  long diff ;
  
  switch( current_state ) {
      case CT_NO_SDRNODE:
        // no connection established, send request if long enough 
        diff = millis() - last_call ;
        if( diff > 2500 ) {
            lcd.clear();
            lcd.print("Waiting SDRNode");
            // last request was long second ago, retry
            Serial.println("!"); 
            ledshow();
            last_call = millis();  
        }
        if( stringComplete ) {
          if( inputString == "<SDRNODE>" ) {
              next_state = CT_SDRNODE_ATTACHED ;
          }
        }
      break ;  

      case CT_SDRNODE_ATTACHED:
        // we are connected to SDRNode
        lcd.clear();
        lcd.print("SDRNode ready !");
        next_state = CT_SDRNODE_READY ;
      break ;

      case CT_SDRNODE_READY:
        diff = millis() - last_call ;
        if( stringComplete ) {
            if( inputString.indexOf(":f:") > 0 ) {
                // this is a frequency message
                last_call = millis();
                frequencyMessage( inputString );
            }
            inputString = "";
            stringComplete = false;
        }
         
      break ;
  }
  
  if( next_state != current_state ) {
      current_state = next_state ;
  }
  if (stringComplete) {    
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    
    // if the incoming character is a newline, set a flag 
    if (inChar == '\n')  {
        stringComplete = true;
        ledshow(); 
    } else {
       inputString += inChar;
    }
  }
}



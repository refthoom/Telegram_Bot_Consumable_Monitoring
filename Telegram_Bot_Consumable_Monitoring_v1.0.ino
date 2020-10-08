
/*******************************************************************
    CONCEPT : Use a telegram bot to receive a message when ESP starts up and
    repeat at the set time, sleep in between messages to save battery
    NOTE : this works one way, only sending messages, any response is ignored
    ****************************************************************
    
    Telegram Bot library written by Brian Lough and ammended by Bolukan
    YouTube:  https://www.youtube.com/brianlough
    Tindie:   https://www.tindie.com/stores/brianlough/
    Twitter:  https://twitter.com/witnessmenow
    gtihub:   https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
    github:   https://github.com/bolukan

    ****************************************************************

    USE CASE : Generate an alert via Telegram when a consumable has run out
    by closing a hardware switch that powers up an ESP. For battery
    powered situations, the voltage level can be measured and also sent,
    together with the statically set minimum battery level.

    ****************************************************************

    USER VARIABLES : You need to fill out your WiFi SSID and password as well as
    the bot token and chat id below.

    You need to make a bot first to be able to use this. Search
    'botfather' and you will find plenty information on how to make your
    own telegram bot.
    - Botfather will give out your unique bot token when he creates the
    bot for you.
    - Use @myidbot (IDBot) or the HTTPS request method to find out
    the chat ID of your bot.
    - Also note that you need to "start" your bot before it will do
    anything:
    In Telegram type @botfather to open a chat. Type /mybots to show
    and pick your bot(s). Type /start to activate your bot. Type /stop
    to de-activate it.

    ****************************************************************

    HARDWARE REQUIREMENTS :
    - For the ESP to wake from deepsleep, it needs a connection
    between RST and A0 (or whatever is your ADC pin)
    - To be able to measure VCC you need to connect a resistor from VCC to A0.
    The value of this resistor depends on wether there already is a voltage divider
    in place like on the D1 mini and NodeMCU, or if it is a barebone ESP. For
    example; D1 mini measuring ~5V it should be 180k ohm. Alternatively
    you can use a multiturn potentiometer to exactly dial in the R value.

    ****************************************************************

    IMPORTANT : with the 'DeepSleep' pin open it is very hard to (re)flash
    as the ESP will go into deepsleep before you can upload the code.
    So, to re-flash connect this pin to GND. However, it cannoot be
    connected to GND during boot as this will prevent the ESP from
    booting up.

 *******************************************************************/

//=========== LIBRARIES ===============

#include <ESP8266WiFi.h>// WiFi library to connect to the home network
#include <WiFiClientSecure.h>// library to connect to the home network securely
#include <UniversalTelegramBot.h>// library to send and receive TelegramBot messages
#include <ArduinoJson.h>// library used by the TelegramBot library


//=========== VARIABLES ===============

// Wifi
#define WIFI_SSID "YOUR_SSID"// name of the home network
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"// password to connect to the WiFi network

// Telegram BOT
#define BOT_TOKEN "YOUR_BOT_TOKEN"// token received from botfather (see description above)
#define CHAT_ID "YOUR_CHAT_ID"// chat id belonging to your bot (or, if set, you can use a channel id (like @myBotChannel) belonging to your bot, to send messages to a group subscribed to this channel

// sleeptimer
double sleepTime = 3600;// number of seconds to go to sleep

// Voltage measurement (to get an alert when the batteries are almost depleted)
// Remember to add the correct resistor value between pin A0 and VCC to correct the 3.3V voltage divider on the D1 mini. For 4.5 - 5 Volt it is 180k
const float VCCval = 4.7;// the top value of the battery for the mapping function = the battery voltage when full/new = what is used as VCC
float BatteryVal = 0;// initial value for the measured battery voltage
int ADCvalue = 888;// set initial trivial value
const int VoltInPin = A0;// ESP8266 Analog Pin ADC0 = A0; the pin used to measure the voltage. Attach a resistor from VCC to this pin. The value
const float VCCminimalVal = 3.7;// fixed value, the minimum reference voltage only for visual reference in the message for when to change the batteries

// default msg value
String msg = "To bot or not to bot, that is the question.";// initial value for the message, contents not used any further

// (re)flashing, testing & debugging:
bool DeepSleep = true;// default value if SleepPin is open
int msgDelay = 15;// only for test purpose, the delay in seconds between messages
int SleepPin = D3;// connecting this pin to GND changes the deepsleep mode into
                  // a delayed sending of messages, it is for easier re-flashing and for testing
                  // however, D3 cannot be connected to GND during boot as that will prevent the ESP 
                  // from booting. So boot normalyy GND pin D3 and reset.
                  // Note: it is recommended to only use D3 on the ESP8266 for this, as it has 
                  // an internal pullup resistor and no special boot behaviour


//============== FUNCTIONS ==============

//initialisations
WiFiClientSecure secured_client;// set the connection secured
X509List cert(TELEGRAM_CERTIFICATE_ROOT);// use certificate to connect over a secure connection with Telegram API
UniversalTelegramBot bot(BOT_TOKEN, secured_client);// start the bot library

// function to see if running test or operational
void testMode(){
  DeepSleep = digitalRead(SleepPin);// sleep mode or not; check to see if the pin is connected to ground and store in the variable
  delay(20);// wait a bit for the digital read to be done
}

// mapping function for floats, used for the battery voltage measurement
    // x = read from ADC (A0)
    // in_min = the lowest value of the analog read
    // in_max = the highest value of the analog read
    // out_min = the lowest value of the mapping
    // out_max = the highest value of the mapping
float mapf(long x, long in_min, long in_max, float out_min, float out_max) {
  float result;
  result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return result;
}

// function to measure the battery voltage
float checkVoltage(){
  if (DeepSleep) {// operational situation
    ADCvalue = analogRead(VoltInPin); // A0 pin Anolog to Digital Conversion measurement
  } else {// testing. Uncomment either for your testing purpose
    ADCvalue = 888; // uncomment this if you want to use fixed value for testing just the message without actual measurements being taken
    //ADCvalue = analogRead(VoltInPin); // uncomment this if you want to take real measurements during testing
  }
  delay(100);// short delay to let the MCU process the measurement
  BatteryVal = mapf(ADCvalue, 0, 1023, 0, VCCval); // Note: out_max value should match the calculated value for the resistor voltage divider you applied
  Serial.println(BatteryVal);
  return BatteryVal;
}

// function to choose a random alert message and send it. 
// you can use just one message but this is more fun. Strip it if you don't like it :-)
void setRandomMsg() {
  int index = random(1, 7);
  switch (index) {
    case 1:
      msg = "Bro, you payin' attention? the salt!!";
      break;
    case 2:
      msg = "Guess what? No, seriously, guess what? Yeah, the salt's low.";
      break;
    case 3:
      msg = "Omg, don't you ever check the salt levels? It's low. Again.";
      break;
    case 4:
      msg = "The salt's low, fool.";
      break;
    case 5:
      msg = "You like calcium deposits? Because this is how you get calcium deposits.";
      break;
    case 6:
      msg = "Well, it had to come to this again with someone like you around. The salt is low again, dimwit!";
      break;
  }
}

// function to send the Telegram message, this is where the magic happens!
void sendMsg(){
  if (BatteryVal < VCCminimalVal) {// if the battery level is low, add a warning to the measurement
    bot.sendMessage(CHAT_ID, msg + " [BATTERY LOW!! " + String(BatteryVal) + "V (" + String(VCCminimalVal) + "V)]", "");
  } else {// add the voltage measurement to the message
    bot.sendMessage(CHAT_ID, msg + " [Battery " + String(BatteryVal) + "V (" + String(VCCminimalVal) + "V)]", "");
  }
  delay(1000);// wait a bit for the message to be sent
}

// function to put the ESP to sleep, or not when testing
void tryToSleep(){
  Serial.print("SleepPin is ");// print the measurement
  Serial.println(DeepSleep);
  if (DeepSleep) {// operational
    Serial.println("I'm feeling sleepy....");
    ESP.deepSleep(sleepTime * 1000000);// go to deepsleep for the specified time in MICROseconds (the clock of the ESP is not accurate), 1 hour is the max value for deep sleep
  } else {// testing
    Serial.println("I'm awake!");
    delay(msgDelay * 1000);// wait a bit before the loop restarts
  }
}


//=============== SETUP ================
void setup() {

  // serial interface
  Serial.begin(74880);// this speed setting prevents the garbled messages when you reset the ESP
  Serial.println();

  // hardware parameters
  pinMode(SleepPin, INPUT_PULLUP );// the internal pull up makes it high so if nothing is connected to it is high
  delay(50);// wait a bit
  digitalWrite(SleepPin, HIGH);// make sure it is high initially

  // set and get time for the certificate
  configTime(0, 0, "pool.ntp.org"); //  get UTC time via NTP. You need to get the current time and date otherwise the cert will prevent connection (because your visiting from 1970 if not set)
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);// this is a check to see that getting the time worked, it's the number of seconds past since EPOCH (1970)

  // (attempt to) connect to Wifi network:
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setTrustAnchors(&cert);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected @");
  Serial.println(WiFi.localIP());

  delay(3000); // wait a bit

}


//============= LOOP ==================

void loop() {

  testMode();// are we running test or operational

  checkVoltage();// measure battery voltage

  setRandomMsg();// choose a message

  sendMsg();// send the Telegram message

  tryToSleep();// go to sleep

}

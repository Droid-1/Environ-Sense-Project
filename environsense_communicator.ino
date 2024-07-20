/*   THIS IS A PROJECT TO GET BOTH THE INTERNAL AND EXTERNAL ENVIRONMENTAL CONDITIONS OF AN ENVIRONMENT THEN COMMUNICATE IT TO AN EXTERNAL API 
 *    
 *    AUTHOR: POLYCARP PETER PAULINUS
 */

// INCLUDE THE NECESSARY ARDUINO LIBRARIES
#include <TinyGPSPlus.h>
//#include <DHT.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>  
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <String.h>

// add my wifi connection details 
const char* ssid = "WAILINGDROID";
const char* password = "iamcryingtoo4";


/*
//define variables for DHT SENSOR
#define DHTPIN   D3
#define DHTTYPE DHT22*/
  

// Define global variables for project
float temperature_;
float humidity_;
double latitude;
double longitude;
String payload;
String payloadWheather;
String current_last_updatedG;
int current_temp_c;
float current_temp_f;
String current_condition_textG;
String display_nameG;
int current_wind_degree;
String current_wind_dirG;
int current_humidity;
int count = 0;
   

// a list of all the APIs I will connect to in the code


String serverNameOne = "https://us1.locationiq.com/v1/reverse?key=pk.f60f15c4e35533396e27b1c77b687ce7&lat=" + String(latitude, 6)+ "&lon=" + String(longitude, 6)+ "&format=json";
String serverNameTwo = "http://api.weatherapi.com/v1/current.json?key=772b8cfd9b4f4793ad171348242805&q=" + String(latitude, 6) + ","+ String(longitude, 6);
String serverNameThree = "api.thingspeak.com/update?api_key=24LCZQAW0M7D1JX8&field1=" + String(current_temp_c)
                         + "&field2=" + String(current_temp_f)
                         + "&field3=" + String(current_humidity) 
                         + "&field4=" + String(current_wind_dirG)
                         + "&field5=" + String(longitude, 6)
                         + "&field6=" + String(latitude, 6);




// create  instances  of library classes.
TinyGPSPlus gps ;
SoftwareSerial SerialPort(12, 14);
//DHT dht(DHTPIN, DHTTYPE);
WiFiClient client; // create instance of wifi client class for internet connection
HTTPClient http;
WiFiClientSecure httpsclient;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  



// DECLARE FUNCTIONS TO BE USED IN THE CODE
void get_gps_location();
float get_humidity();
float  get_temperature();
void connectToWifiNetwork();
void makeHttpGetRequest();
void makeSecureHttpGetRequest();
void makeSecureHttpGetRequest();
void ParseJsonLocation();
void ParseJsonWheather();
String readPayLoad();
void InitializeSIM800();
void ShowSimModuleResponse();
void SIM800SecureRequest();
void  SIM800Request();
void setConnectType();
void scrollText(String message, int row) ;



void setup() {

  lcd.init();                    
  lcd.backlight();
  lcd.print("Weather Station");
  delay(1000);
  
  Serial.begin(9600); 
  Serial.swap();
  Serial1.begin(9600);
 // dht.begin();
  SerialPort.begin(9600);

  get_gps_location();

 // setConnectType(); // i will use all this when I use esp32 microcontroller.
  
  SerialPort.println("AT+CFUN= 1"); 
  delay(1000); 
  ShowSimModuleResponse();

  InitializeSIM800();

  Serial1.println("first communication done");

}

void loop() {
 
 Serial1.println("In the loop");

 /*
  * The idea is to use these functionalities on esp32, esp8266 doesn't have enough pins for this program
  //makeHttpGetRequest();

 // Serial1.println(payload);
  
 // ParseJsonWheather();
  
 // makeSecureHttpGetRequest();
  //ParseJsonLocation();

  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Internal Conditions");
  delay(1000);
  
  humidity_ =  get_humidity();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.print( humidity_);
  

  temperature_ = get_temperature();
  lcd.setCursor(0,1);
  lcd.print("Temperature: ");
  lcd.print(temperature_);
  
  delay(500);

*/
  SIM800SecureRequest();
  
  SIM800Request();

  ParseJsonWheather();

  ParseJsonLocation();

  SIM800Send();

 
}


void get_gps_location(){

  //Serial1.println("In get GPS");
while  ( latitude == 0 && longitude == 0){
  while (SerialPort.available() > 0) {
    
      if (gps.encode(SerialPort.read())){
           lcd.setCursor(0,0);
           Serial1.println("In Serial Port ");
            lcd.clear();
           lcd.print("Initialising...");

          if (gps.location.isValid()){
            
                 Serial1.print("Latitude: ");
                 Serial1.println(gps.location.lat(), 6);
                 Serial1.print("Longitude: ");
                 latitude = gps.location.lat();
                 Serial1.println(gps.location.lng(), 6);
                 longitude = gps.location.lng();
                 Serial1.print("Altitude: ");
                 Serial1.println(gps.altitude.meters());       
           }

       }
          

  }
  
  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {   
    lcd.setCursor(0,0);
    Serial1.println("No GPS detected");
    lcd.print("No GPS detected");
    while(true);
  }  
  
  }
           lcd.clear();
           lcd.setCursor(0, 0);          
           lcd.print("Longitude: ");
           lcd.print(longitude);
           lcd.setCursor(0,1);
           lcd.print("Latitude: ");  
           lcd.print(latitude);
           delay(500);

}

/*float get_humidity(){
  
 humidity_ = dht.readHumidity();
 Serial1.println(humidity_);
 return humidity_;
}

float get_temperature(){

  temperature_ = dht.readTemperature();
  Serial1.println(temperature_);
  return temperature_;
  
}*/

/*
void connectToWifiNetwork(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); // begin wifi connection with my wifi connection details
  Serial1.println("Connecting"); 
  
  // this is a while loop to confirm internet connection via wifi
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print(".");
  }
  Serial1.println("");
  Serial1.print("Connected to WiFi network with IP Address: ");
  Serial1.println(WiFi.localIP());
}

void makeHttpGetRequest(){

  if (WiFi.status() == WL_CONNECTED) { // confirm wifi is still connected in the loop

     http.begin(client, serverNameTwo.c_str());
     int httpResponseCode = http.GET();
     
     if (httpResponseCode > 0) {
      Serial1.print("HTTP Response code: ");
      Serial1.println(httpResponseCode);
      payloadWheather = http.getString();
      Serial1.println("Response payload: ");
      //Serial1.println(payloadWheather);
      
    } else {
      Serial1.print("Error code: ");
      Serial1.println(httpResponseCode);
    }
    
    http.end();
    delay(2000);
    
  } else {
    Serial1.println("WiFi Disconnected");
  }
}

void makeSecureHttpGetRequest(){

   httpsclient.setInsecure();

  if (WiFi.status() == WL_CONNECTED) { // confirm wifi is still connected in the loop
    Serial1.println("There's a wifi connection");

    http.begin(httpsclient, serverNameOne.c_str());
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      Serial1.print("HTTP Response code: ");
      Serial1.println(httpResponseCode);
      payload = http.getString();
      Serial1.println("Response payload: ");
    //  Serial1.println(payload);
    } else {
      Serial1.print("Error code: ");
      Serial1.println(httpResponseCode);
    }
    http.end();
    delay(2000);
  } else {
    Serial1.println("WiFi Disconnected");
  }
}
*/

void ParseJsonWheather(){

  String time_ ;
  String  date_ ;

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, payloadWheather);

  if (error) {
  Serial1.print("deserializeJson() failed: ");
  Serial1.println(error.c_str());
  return;
  }


  JsonObject current = doc["current"];

 String   current_last_updated = current["last_updated"]; // "2024-05-31 04:15"
 current_last_updatedG = current_last_updated;

  int dateStart = current_last_updatedG.indexOf(" ");

  if (dateStart != -1) {
    // Extract the JSON part of the response
  date_  = current_last_updatedG.substring(0, dateStart);
  time_ = current_last_updatedG.substring(dateStart);
  }   


  
    current_temp_c = current["temp_c"]; // 23
   current_temp_f = current["temp_f"]; // 73.4
   

   JsonObject current_condition = current["condition"];
    String  current_condition_text = current_condition["text"]; 
    current_condition_textG = current_condition_text;

   current_wind_degree = current["wind_degree"]; // 232
  String  current_wind_dir = current["wind_dir"]; // "SW"
  current_wind_dirG = current_wind_dir;
   current_humidity = current["humidity"]; // 94


   Serial1.print("Time and date: ");
   Serial1.println(current_last_updatedG);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Date: ");
   lcd.print(date_);
   lcd.setCursor(0,1);
   lcd.print("Time: ");
   lcd.print(time_);
   delay(7000);

   Serial1.print("Temp in Celcius: ");
   Serial1.println(current_temp_c);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Temperature: ");
   lcd.print(current_temp_c);
   lcd.print("C");
   delay(7000);


   Serial1.print("Temperature in fahrenheit: ");
   Serial1.println(current_temp_f);
   lcd.setCursor(0,1);
   lcd.print("Temp: ");
   lcd.print(current_temp_f);
   lcd.print("F");
   delay(7000);

   Serial1.print("Humidity: ");
   Serial1.println(current_humidity);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Humidity: ");
   lcd.print(current_humidity);
   lcd.print("%");
   delay(7000);

  

   Serial1.print("Wind Degree: ");
   Serial1.println(current_wind_degree);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Wind Degree: ");
   lcd.print(current_wind_degree);
   delay(7000);


   Serial1.print("Wind Diretion: ");
   Serial1.println(current_wind_dirG);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Wind Dir: ");
   lcd.print(current_wind_dirG);
   delay(7000);

   Serial1.print("Weather Condition: ");
   Serial1.println(current_condition_text);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Weather Cond:");
   lcd.setCursor(0,1);
   lcd.print(current_condition_textG);
   delay(7000);

}



void ParseJsonLocation(){

  String LGA ;
  String  stateCountry;

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

   String display_name = doc["display_name"]; // 
   display_nameG = display_name;

  display_nameG.replace("State", "St.");
  display_nameG.replace("Nigeria", "NG");


 int stateStart = display_nameG.indexOf(",");

  if (stateStart != -1) {
    // Extract the state part of the response
  LGA = display_nameG.substring(0, stateStart);
  stateCountry = display_nameG.substring(stateStart+2);
  }   


  
   Serial1.print("Location: ");
   Serial1.println(display_nameG);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print(LGA);
   lcd.setCursor(0, 1);
   lcd.print(stateCountry);
   delay(10000);

 
}

void ShowSimModuleResponse(){
 Serial1.println("This function works");
  
  while(Serial.available()) {
  Serial1.write(Serial.read());
  }
  
  delay(3000);
}

String readPayLoad(){

 Serial.setTimeout(10000); // Set a timeout of 10 seconds (adjust as needed)
  String payload_internal = Serial.readString();

  int jsonStartIndex = payload_internal.indexOf("{");

  if (jsonStartIndex != -1) {
    // Extract the JSON part of the response
    payload_internal = payload_internal.substring(jsonStartIndex);
  }   
  
  // Optionally remove "OK" from the response
  int okIndex = payload_internal.indexOf("OK");
  if (okIndex != -1) {
    payload_internal.remove(okIndex);
  }

  Serial1.println(payload_internal);
  delay(2000);
  return payload_internal;
}


// AT+SAPBR=0,1 = THIS IS USED TO CLOSE GPRS CONTEXT



void SIM800Request(){

 Serial.println("AT+HTTPINIT");  // Iniialize HTTP connection, response should be ok
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+HTTPPARA=\"CID\",\"1\"");  // Set parameters for HTTP session. instruct SIM800  to set the bearer context ID to one. response should be ok.
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+HTTPPARA=\"URL\",\"http://api.weatherapi.com/v1/current.json?key=772b8cfd9b4f4793ad171348242805&q=" + String(latitude, 6) + ","+ String(longitude, 6) + "\"");  // Set the URL (Uniform Resource Locator) parameter for HTTP operations. Response should be ok
 delay(1000); 
 ShowSimModuleResponse();

 Serial.println("AT+HTTPPARA=\"REDIR\",0"); // disable redir if the api or webserve has a http redirect status code
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+HTTPSSL=0");  // enable communication with APIs or webservers with SSL encryption. in other words, websites that start with HTTPS Example: HTTPS://www.xxx.com
 delay(1000); 
 ShowSimModuleResponse(); 


 Serial.println("AT+HTTPACTION=0");  // tell SIM800 to initiate a HTTP POST session. response should be OK an
 // "+HTTPACTION: 0,200,0" which indicates that the HTTP POST request (action 1) was successful
 delay(30000); 
 ShowSimModuleResponse(); 


  Serial.println("AT+HTTPREAD");  // Read the data of HTTP server
  
  // ShowSimModuleResponse(); 
  payloadWheather = readPayLoad();

 

   Serial.println("AT+HTTPTERM");  // /Terminate HTTP service
  delay(1000); 
   ShowSimModuleResponse(); 


  
}

void SIM800SecureRequest(){

 if ( count == 0 ){
   count += 1;
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Processing...");

 }

 Serial.println("AT+HTTPINIT");  // Iniialize HTTP connection, response should be ok
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+HTTPPARA=\"CID\",\"1\"");  // Set parameters for HTTP session. instruct SIM800  to set the bearer context ID to one. response should be ok.
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+HTTPPARA=\"URL\",\"https://us1.locationiq.com/v1/reverse?key=pk.f60f15c4e35533396e27b1c77b687ce7&lat=" + String(latitude, 6)+ "&lon=" + String(longitude, 6)+ "&format=json\"");  // Set the URL (Uniform Resource Locator) parameter for HTTP operations. Response should be ok
 delay(1000); 
 ShowSimModuleResponse(); 


 Serial.println("AT+HTTPPARA=\"REDIR\",1"); // enable redir if the api or webserve has a http redirect status code
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+HTTPSSL=1");  // enable communication with APIs or webservers with SSL encryption. in other words, websites that start with HTTPS Example: HTTPS://www.xxx.com
 delay(1000); 
 ShowSimModuleResponse(); 


 Serial.println("AT+HTTPACTION=0");  // tell SIM800 to initiate a HTTP POST session. response should be OK an
 // "+HTTPACTION: 0,200,0" which indicates that the HTTP GET request (action 1) was successful
 delay(30000); 
 ShowSimModuleResponse(); 


  Serial.println("AT+HTTPREAD");  // Read the data of HTTP server
   //ShowSimModuleResponse(); 
  payload = readPayLoad();

   

   Serial.println("AT+HTTPTERM");  // /Terminate HTTP service
  delay(1000); 
   ShowSimModuleResponse(); 


  
}


void InitializeSIM800(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Initialising");
  lcd.setCursor(0,1);
  lcd.print("Connection...");
  delay(500);
  
 Serial.println("AT");   // Send AT test command
 delay(1000); 
 ShowSimModuleResponse(); // get response froms im800

 Serial.println("AT+CGMR");  // request for firmware version
 delay(1000); 
 ShowSimModuleResponse(); 
 
 Serial.println("AT+CCID");  // Request for sim card information; this is to confirm that simcard is plugged in. it will print simcard info
 delay(1000); 
 ShowSimModuleResponse(); 


 Serial.println("AT+CSQ");  // check for signal quality note: 31 is the highest value
 delay(1000); 
 ShowSimModuleResponse(); 

 
 Serial.println("AT+CREG?");  //  check id sim800 has registered the network. the response should be +creg: 0,1 ok
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+CPIN?");  //  check if there's any password or pin on the simcard. the response should be +Cpin ready
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");  //  tell SIM800  to set the connection type to GPRS. response should be ok
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+SAPBR=3,1,\"APN\",\"web.gprs.mtnnigeria.net\"");  // tell SIM800 to set APN. response should be ok
 delay(1000); 
 ShowSimModuleResponse(); 

 
 Serial.println("AT+SAPBR=1,1");  // tell SIM800 to initiate the establishment of a GPRS context using the specified profile ID. response should be ok
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+SAPBR=2,1");  //  tell SIM800 to query the status of the GPRS bearer. response should be +SAPBR:1,1,"IP address"
 delay(1000); 
 ShowSimModuleResponse(); 
 
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Connection Set");
 delay(1000);

}

/*void setConnectType(){
  static bool choosingConnection = true; // Flag to indicate if choosing unit (true) or setting time (false)
  static bool Selected = true; // Flag to indicate if seconds is selected
  
  if (choosingConnection){
    // Display unit prompt
    lcd.clear(); // Clear the LCD to remove any previous characters
    lcd.setCursor(0, 0);
    lcd.print("Set Connection");
    lcd.setCursor(0, 1);
    lcd.print(Selected ? "<Wifi>     Cellular " : " Wifi     <Cellular>");
    
    if (digitalRead(addButton) == LOW)
    {
      // Add button pressed, toggle unit selection
      secSelected = !secSelected;
      beep();
      delay(200);
    }

     else if (digitalRead(okButton) == LOW)
    {
      secSelected ?  connectToWifiNetwork(); : InitializeSIM800();
    }
  }
}*/

void Customdelay(int interval){
 unsigned long  start_time = 0;
 unsigned long current_time = millis();

  while (current_time - start_time <= interval){
    
  }
}

/*void WifiHttpPostRequest(){

    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
 
      http.begin(client, serverName);
  
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}

*/

void SIM800Send(){

 Serial.println("AT+HTTPINIT");  // Iniialize HTTP connection, response should be ok
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+HTTPPARA=\"CID\",\"1\"");  // Set parameters for HTTP session. instruct SIM800  to set the bearer context ID to one. response should be ok.
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+HTTPPARA=\"URL\",\"api.thingspeak.com/update?api_key=24LCZQAW0M7D1JX8&field1=" + String(current_temp_c)
                         + "&field2=" + String(current_temp_f)
                         + "&field3=" + String(current_humidity) 
                         + "&field4=" + String(current_wind_dirG)
                         + "&field5=" + String(longitude, 6)
                         + "&field6=" + String(latitude, 6)+ "\"");  // Set the URL (Uniform Resource Locator) parameter for HTTP operations. Response should be ok
 delay(1000); 
 ShowSimModuleResponse(); 

 Serial.println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");  // set the content type parameter for HTTP operations to
 // "application/x-www-form-urlencoded".
 // response should be ok
 delay(1000); 
 ShowSimModuleResponse(); 

 /* Serial.println("AT+HTTPDATA=" + String(serverNameThree.length()) + ",50000");  // instruct SIM800  to prepare for receiving data which will sent in an HTTP POST request
 // response should be download ok
 delay(1000); 
 ShowSimModuleResponse(); 
*/

 Serial.println("AT+HTTPACTION=1");  // tell SIM800 to initiate a HTTP POST session. response should be OK an
 // "+HTTPACTION: 1,200,0" which indicates that the HTTP POST request (action 1) was successful
 delay(50000); 
 ShowSimModuleResponse(); 


  Serial.println("AT+HTTPREAD");  // Read the data of HTTP server
  delay(1000); 
   ShowSimModuleResponse(); 

   Serial.println("AT+HTTPTERM");  // /Terminate HTTP service
  delay(1000); 
   ShowSimModuleResponse(); 


}

void scrollText(String message, int row) {
  
  int messageLength = message.length();
  if (messageLength <= 16) {
    // If the message is shorter than or equal to the LCD width, display it directly
    lcd.setCursor(0, row);
    lcd.print(message);
  } else {
    // Scroll the message if it's longer than the LCD width
    for (int start = 0; start <= messageLength - 16; start++) {
      lcd.setCursor(0, row);
      lcd.print(message.substring(start, start + 16));
      delay(300); // Adjust the delay to control the scrolling speed
    }
  }
}





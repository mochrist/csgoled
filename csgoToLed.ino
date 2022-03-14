// Library für WiFi-Verbindung
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WS2812FX.h>

#define LED_COUNT 30    //Mit LED-Anzahl ersetzen
#define LED_PIN 14      //Mit LED-Pin ersetzen
#define TIMER_MS 5000
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const char* ssid     = "YourSSID";        //SSID einsetzen
const char* password = "YourPassword";    //Passwort einsetzen
WiFiServer server(80);
String header;
char buffer[500];
int ind = 0;
int startind = 0;
int anfang = 0;
int startindex = 0;
int isDigitCounter = 0;
String StringConv;
int HP = 101;
int playerFrags = 0;

long lightafterkillpreviousMillis = 0;
long lightafterkillinterval = 2000;




enum states {
  off,
  freezetime,
  live,
  ct_win,
  t_win, 
  over,
  kill
};
states state=off;



void setup() {
  Serial.begin(115200);
  ws2812fx.init(); 
  ws2812fx.setBrightness(255);
  ws2812fx.setSpeed(2000);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_BREATH);
  ws2812fx.start();

  // Mit dem Netzwerk verbinden
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.begin();
  Serial.println("Enter this as uri in gamestate_integration_arduino.cfg:");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println(":80");
}

void loop() {
  
  ind = 0;
  anfang = 0;
  startindex = 0;
  isDigitCounter = 0;
  WiFiClient client = server.available();   // Auf Clients (Server-Aufrufe) warten

  if (client) {                             // Bei einem Aufruf des Servers
    ////Serial.println("Client available");
    String currentLine = "";                // String definieren für die Anfrage des Clients

    while (client.connected()) { // Loop, solange Client verbunden ist

      if (client.available()) {
          
        char c = client.read();
        if (c == ' ') {startindex++;}
        if (startindex >= 13)
        {  
          if (c == '{') {isDigitCounter = 1;} 
          if (isDigitCounter == 1)  {                        
            if(ind < 600)
                {
              buffer[ind] = c;    //In Puffer laden
              ind++;
              buffer[ind] = '\0';
                      }buffer[ind] = '}'; }
        }
        //Serial.write(c);                    // und es im Seriellen Monitor ausgeben
        

        
        header += c;  
        if (c == '\n') {                    // bis eine Neue Zeile ausgegeben wird
          // Wenn der Client eine Leerzeile sendet, ist das Ende des HTTP Request erreicht        
         
         if (currentLine.length() == 2) {   

            //Server sendet eine Antwort an den Client
            client.println("HTTP/1.1 200 OK");
            client.println("Connection: close");
            // Die Antwort mit einer Leerzeile beenden
            client.println();
            // Den Loop beenden
            String StringConv(buffer); //In String konvertieren
            DynamicJsonDocument doc(600); //Dynamisch für ESP8266


              DeserializationError error = deserializeJson(doc, StringConv);
      
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      
      const char* round_phase = doc["round"]["phase"]; // "over; freezetime; live"
      const char* round_win_team = doc["round"]["win_team"]; // "CT; T"
      
      int player_state_health = doc["player"]["state"]["health"]; // 100
      int player_state_round_kills = doc["player"]["state"]["round_kills"]; // 0




            //SPIELPLATZ ANFANG++++++++++++++++++++++++++++++++++++    
            float Health = player_state_health;
            float HPBright = Health / 100;

            Serial.print("Phase: ");
            Serial.write(round_phase);
            Serial.println(round_phase);

            Serial.print("Winteam: ");
            Serial.println(round_win_team);

            Serial.print("Health: ");
            Serial.println(player_state_health);

            Serial.print("Roundkills: ");
            Serial.println(player_state_round_kills);   



            
           if(std::string(round_phase) == "freezetime" && state !=freezetime) {
              state = freezetime;                           
            }  

            if(std::string(round_phase) == "live" && state !=live && state != kill) {
              state = live;
            }

            if(std::string(round_phase) == "over" && state !=over) {
              if(std::string(round_win_team) == "CT"){
                state = ct_win;
              }
            else if(std::string(round_win_team) == "T"){
                state = t_win;
              }
            }
            Serial.println(player_state_round_kills);
            if(player_state_round_kills == 1){
              state = kill;
              ws2812fx.setSegment(0,  0, LED_COUNT-1, 0, RED);
              delay(500);
              state = live;
            }





            





            if(state == freezetime){
                
                ws2812fx.setSegment(0,  0, LED_COUNT-1, 43, WHITE,  3000, SIZE_LARGE);
                
          
               }   
              
            if(state == live && state != kill){
              uint32_t livecolors[] = {WHITE, BLACK};
              ws2812fx.setSegment(0,  0, LED_COUNT-1, 15, livecolors,  8000, REVERSE);
                //ws2812fx.setMode(43);
            }
            
            if(state == ct_win){
              ws2812fx.setColor(0x0000FF);
                ws2812fx.setMode(FX_MODE_STATIC);
            }

            if(state == t_win){
              ws2812fx.setColor(0x916300);
                ws2812fx.setMode(FX_MODE_STATIC);
            }
          
                      
 
            

            break;
          } else { // Bei einer Neuen Zeile, die Variable leeren
           
            currentLine = "";
          } 
        } else if (c != '\r') {  // alles andere als eine Leerzeile wird
          currentLine += c;      // der Variable hinzugefüht 
                   
        }
      } 
    }
    
    
    // Variable für den Header leeren
    header = "";
    // Die Verbindung beenden
    client.stop();
  }
  ws2812fx.service();
}

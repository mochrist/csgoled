# csgoled
Read Events from CS:GO via HTTP POST and convert them into WS2812B-LED Effects | ESP8266

Reads Events (currently only roundstate) wireless and lights up WS2812B LED-Stripe according to Event

Enter your Network-SSID and Password in the csgoToLED.ino file and upload it to your ESP8266 Microcontroller
After connecting, you will see the uri you need to copy into gamestate_integration_arduino.cfg
Place gamestate_integration_arduino.cfg in \Steam\steamapps\common\Counter-Strike Global Offensive\csgo\cfg

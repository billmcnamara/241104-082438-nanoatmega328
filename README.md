# kids project for led temperature display on nano #

This is a repo for displaying the temperature (*C) on an 8x8 led matrix.

Basically, my kid is selling these for his school... and it helps me reduce my stockpile of MCUs.
I've built on stm blue pill, nanos. etc..
If you want to buy one, and help the school, let me know :)

# pinouts #

The data pin of the 8x8 led matrix (using WS2812FX lib is):
(using https://fr.aliexpress.com/item/1005006127587095.html)

    #define LED_PIN 13

For the temperature sensor I used a DS18B20 using the OneWire lib:
(using https://www.amazon.fr/dp/B01LXQF9B5)

    #define ONE_WIRE_BUS 4

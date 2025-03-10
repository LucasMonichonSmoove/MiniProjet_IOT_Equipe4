/*
The MIT License (MIT)


*/

#include "MicroBit.h"
#include "lib/bme280.h"
#include "lib/tsl256x.h"
#include "lib/veml6070.h"
#include "lib/ssd1306.h"

MicroBit    uBit;
MicroBitI2C i2c(I2C_SDA0, I2C_SCL0);
ssd1306 screen(&uBit, &uBit.i2c, &uBit.io.P0);
bme280 bme(&uBit, &uBit.i2c);
tsl256x tsl(&uBit, &uBit.i2c);
veml6070 veml(&uBit, &i2c);

// Fonction pour effacer l'écran OLED
void clear_screen() {
    for (int i = 1; i <= 5; i++) { 
        screen.display_line(i, 0, "                                                 "); // Efface chaque ligne
    }
}


void get_sensors(uint32_t* lux, uint16_t* humi, int32_t* temp, uint16_t* comb, uint16_t* ir, uint16_t* uv, uint32_t* pressure, ManagedString* luminosite, ManagedString* temperature, ManagedString* humidite){

    bme.sensor_read(*pressure, *temp, *humi);
    tsl.sensor_read(*comb, *ir, *lux);
    veml.sensor_read(*uv);

    int tmp = bme.compensate_temperature(temp);
    int hum = bme.compensate_humidity(humi);

    *luminosite = ManagedString((int)lux);
    *temperature = ManagedString(tmp / 100) + "." + (tmp > 0 ? ManagedString(tmp % 100) : ManagedString((-tmp) % 100));
    *humidite = ManagedString(hum / 100) + "." + ManagedString(hum % 100);
}


void afficher(ManagedString luminosite, ManagedString temperature, ManagedString humidite, ManagedString ordre){
    clear_screen();
    for (int i = 0; i < ordre.length(); i++)
    {
        char type = ordre.charAt(i);
        switch (type)
        {
            case 'L':
                screen.display_line(i+1, 0, ("Luminosite : " + luminosite + "lx" +"  ").toCharArray());
                break;
            case 'T':
                screen.display_line(i+1, 0, ("Temp : " + temperature + "°C" + "  ").toCharArray());
                break;
            case 'H':
                screen.display_line(i+1, 0, ("Hum : " + luminosite + "rH" + "  ").toCharArray());
                break;
        }
    }
    screen.update_screen();
    
    
}


int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    uBit.radio.enable();
    uBit.radio.setGroup(31);

    ManagedString ordre="LTH";

    int32_t temp = 0;
    uint16_t humi = 0;
    uint32_t lux = 0;
    uint16_t comb = 0;
    uint16_t ir = 0;
    uint16_t uv = 0;
    uint32_t pressure = 0;

    ManagedString luminosite;
    ManagedString temperature;
    ManagedString humidite;

    while (1)
    {
        // A faire : utiliser recv() en checkant si null. Si non null alors message reçu et donc appelle des fonctions plus envoi du message à l'autre carte
        PacketBuffer radio_recv = uBit.radio.datagram.recv();

        if (radio_recv != NULL)
        {
            get_sensors(lux, humi, temp, comb, ir, uv, pressure)
        }
        
    }
    
}


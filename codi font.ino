#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHTesp.h>

// Definim els GPIOs on tenim connectats diversos dispositius.
#define SDA 13
#define SCL 14
#define SENSOR 12
#define BOTO_AMUNT 4
#define BOTO_AVALL 15
#define CALEFACCIO 18
#define REFRIGERACIO 19

// Inicialitzem la variable lcd que ens permetrà controlar el display LCD1602.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Inicialitzem les variables que ens permetran controlar el sensor DHT11.
DHTesp dht;
TempAndHumidity valorsLlegits;
float histeresi = 0.5;

// Variable que ens representarà la temperatura "objectiu".
double tempObjectiu = 20.0;


void setup() {
    // Inicialitzem el display LCD1602.
    Wire.begin(SDA, SCL);
    lcd.init();
    lcd.backlight();

    // Inicialitzem el sensor de temperatura i humitat DHT11.
    dht.setup(SENSOR, DHTesp::DHT11);

    // Fem un bucle d'espera fins que el sensor comenci a enviar-nos dades, ja
    // que necessita un temps de posta en marxa.
    do {
        valorsLlegits = dht.getTempAndHumidity();
    } while (dht.getStatus() != 0);
    
    // Inicialització dels GPIOs que fem servir.
    pinMode(BOTO_AMUNT, INPUT_PULLUP);
    pinMode(BOTO_AVALL, INPUT_PULLUP);
    pinMode(CALEFACCIO, OUTPUT);
    pinMode(REFRIGERACIO, OUTPUT);

    // Assegurem que inicialment els LEDs indicadors estan apagats.
    digitalWrite(REFRIGERACIO, LOW);
    digitalWrite(CALEFACCIO, LOW);
}

void loop() {
    // Fem una lectura de les dades del sensor.
    valorsLlegits = dht.getTempAndHumidity();
 
    // Comprovem si la lectura ha estat correcta.
    if (dht.getStatus() == 0) {
        // Si la lectura ha anat bé, mostrarem la temperatura i la humitat.
        // Preparem la posició on es mostraran le dades: columna 0, fila 1.
        lcd.setCursor(0, 1);
        // Mostrem "T:" seguit de la temperatura amb 1 decimal.
        lcd.print("T: " + String(valorsLlegits.temperature, 1));
        // Mostrem "H:" seguit de la humitat sense decimals i un "%" al final.
        lcd.print(" H: " + String(valorsLlegits.humidity, 0) + "% ");

        // Activem o desactivem la calefacció i la refrigeració, segons
        // convingui, en funció de la temperatura observada i la desitjada.
        if (valorsLlegits.temperature > (tempObjectiu + histeresi)) {
            digitalWrite(REFRIGERACIO, HIGH);
            digitalWrite(CALEFACCIO, LOW);
        }
        else if (valorsLlegits.temperature < (tempObjectiu - histeresi)) {
            digitalWrite(REFRIGERACIO, LOW);
            digitalWrite(CALEFACCIO, HIGH);
        }
        else {
            digitalWrite(REFRIGERACIO, LOW);
            digitalWrite(CALEFACCIO, LOW);
        }
    }

 
    // Comprovem si s'ha premut algun dels dos botons i, en cas afirmatiu, fem
    // pujar o baixar la temperatura "objectiu" 0.5 graus, segons el cas.
    if (digitalRead(BOTO_AMUNT) == LOW)
    {
        while (digitalRead(BOTO_AMUNT) == LOW) 
        {
            delay(50);
        }

        if (tempObjectiu < 35)
        {
             tempObjectiu = tempObjectiu + 0.5;
            delay(200);
        }
    } 
    else if (digitalRead(BOTO_AVALL) == LOW)
    {
        while (digitalRead(BOTO_AVALL) == LOW) 
        {
            delay(50);
        }

        if (tempObjectiu > 5)
        {
            tempObjectiu = tempObjectiu - 0.5;
            delay(200);
        }
    }

    // Mostrem la temperatura "objectiu" amb 1 decimal a la fila 0 del display.
    lcd.setCursor(0, 0);
    lcd.print("Temp. Obj.: " + String(tempObjectiu, 1));
    delay(50);
}

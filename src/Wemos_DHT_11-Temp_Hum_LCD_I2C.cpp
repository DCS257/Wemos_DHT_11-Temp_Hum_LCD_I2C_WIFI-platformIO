// Incluimos librerías
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

//Creamos el objeto lcd 
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Definimos el pin digital donde se conecta el sensor
#define DHTPIN D7

// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

// Configuracion conexion WIFI ThingSpeak
const char* ssid = "XXXXXXXXXXXXXXXXX";
const char* password = "XXXXXXXXXXXXXXX";

// Configuracion ThinkSpeak
WiFiClient client;
unsigned long myChannelNumber = 9999999999999;
const char * myWriteAPIKey = "XXXXXXXXXXXXXXX";

void setup() {
  // Inicializamos el puerto serie
  Serial.begin(115200);
 
  // Inicializamos el sensor DHT
  dht.begin();  

  // Inicializamos el LCD de 16x2
  lcd.begin(16, 2);
  
  // Movemos el cursor a la primera posición de la pantalla (0, 0)
  lcd.home();
  lcd.print("Temperatura:");  

  lcd.setCursor(0, 1);
  lcd.print("Humedad:");  

  // Establecemos la conexion con ThingSpeak
  ThingSpeak.begin(client);

  //  Establecemos la configuracion WIFI y desconectamos cualquier conexion previa  
  WiFi.mode (WIFI_STA) ;
  WiFi.disconnect ();

  // Conectando a la WiFi 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectada. Direccion IP: ");
  Serial.println(WiFi.localIP());

  delay(100);
}

void loop() {
  // Retardo cada 5 segundos
  delay(5000);
 
  // Leemos la humedad relativa
  float h = dht.readHumidity();
  
  // Leemos la temperatura en grados centígrados (por defecto)
  float t = dht.readTemperature();
  
  // Leemos la temperatura en grados Fahrenheit
  float f = dht.readTemperature(true);
 
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
 
  // Calcular el índice de calor en Fahrenheit
  float hif = dht.computeHeatIndex(f, h);
  
  // Calcular el índice de calor en grados centígrados
  float hic = dht.computeHeatIndex(t, h, false);
 
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" ºC ");
  Serial.print(f);
  Serial.print(" ºF\t");
  Serial.print("Índice de calor: ");
  Serial.print(hic);
  Serial.print(" ºC ");
  Serial.print(hif);
  Serial.println(" ºF");

  // Mostramos la temperatura en Celsius en el LCD
  lcd.setCursor(12, 0);  
  lcd.print(t);

  // Mostramos la humedad en el LCD
  lcd.setCursor(12, 1);  
  lcd.print(h);

  // Carga los valores a enviar
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);

  // Escribe todos los campos a la vez.
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  Serial.println("¡Datos enviados a ThingSpeak!");
}

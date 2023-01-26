/* 
 * Parte de la programación esta basada: Optical SP02 Detection (SPK Algorithm) using the MAX30105 Breakout  
 * By: Nathan Seidle @ SparkFun Electronics  
 * Date: October 19th, 2016  
 * https://github.com/sparkfun/MAX30105_Breakout
 * 
 *  Parte de la programación esta basada en el programa ESP32CAM-DHT11 
 * por: Hugo Escalpelo
 * Fecha: 28 de julio de 2021
 * Actualizado por: Laura del Rocio Balandrán C.
 * Fecha: 22/agosto/022
 * 
 * Esta aplicación lleva a cabo la lectura del sensor DHT11 alojado dentro de un Dispesador médico inteligente usando la bliblioteca AdaFruit
 * envíando los valores de temperatura y humedad relativa por protocolo MQTT en un JSON.
 * Parte de la programación basada en el ejemplo de los programas MQTT Basic y JSON Basic del repositorio
 * del CodigoIoT.
 * Se enviará un JSON por MQTT con todas las variables. Usando el microcontrolador ESP32CAM
 * Para también conectar en paralelo el sensor MAX30102 y el MLX90614 que tomarán los indices de ritmo cardiaco, oxigenación 
 * y temperatura corporal sin conctacto de un adulto mayor, para registrar los datos en una BD usando MySQL y Node-Red.
 * 
 * Componente     PinESP32CAM     Estados lógicos
 * ledStatus------GPIO 33---------On=>LOW, Off=>HIGH
 * ledFlash-------GPIO 2----------On=>HIGH, Off=>LOW
 * 
 * DHT11      ESP32CAM
 * S (Data)-----GPIO 13
 * GND----------GND
 * Vdd----------5V
 * 
 * MAX30102     ESP32CAM
 * 5v-----------5V
 * GND----------GND
 * SDA----------14
 * SCL----------15
 * 
 * MLX90614     ESP32CAM
 * 5v-----------5V
 * GND----------GND
 * SDA----------14
 * SCL----------15
 * 
 * led de alerta
 * 5v-----------5V
 * GND----------GND
 * led----------GPIO 2----------On=>HIGH, Off=>LOW
 * 
 */

//Bibliotecas
#include <WiFi.h>  // Biblioteca para el control de WiFi
#include <PubSubClient.h> //Biblioteca para conexion MQTT
#include "DHT.h" // Bliblioteca para el sensor DHT11
#include <Wire.h> // Biblioteca para comunicación I2C  //bilbiotecas adicionadas para el MAX30102 y MLX90614
#include "MAX30105.h" // Biblioteca del sensor 
#include "spo2_algorithm.h" // Biblioteca para interpretación de señales
#include <Adafruit_MLX90614.h> // Biblioteca para el sensor de temperatura infrarrojo

#define DHTPIN 13     // Pin GPIO 13 de la ESP32CAm con el pin 2 del DHT11
#define DHTTYPE DHT11 //Configura el modelo de sensor, compatible también con DHT22

//Datos de WiFi
const char* ssid = "TROLLS";  // Aquí debes poner el nombre de tu red
const char* password = "m9EqQPk35H";  // Aquí debes poner la contraseña de tu red

//Datos del broker MQTT
const char* mqtt_server = "192.168.1.181"; // Si estas en una red local, coloca la IP asignada, en caso contrario, coloca la IP publica
IPAddress server(192,168,1,181); // 192,168,1,156

// Objetos
WiFiClient espClient; // Este objeto maneja los datos de conexion WiFi
PubSubClient client(espClient); // Este objeto maneja los datos de conexion al broker
DHT dht(DHTPIN, DHTTYPE); //Objeto que inicia el sensor
MAX30105 particleSensor; // Objeto para manejar el sensor MAX30102
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); // Objeto para manejar el MLX90614

#define MAX_BRIGHTNESS 255 // Constante de brillo para el MAX30105

// Variables
int flashLedPin = 2;  // Para ser controlado por MQTT //era =4 lo cambie =2
int statusLedPin = 33; // Para indicar el estatus de conexión
long timeNow, timeLast; // Variables de control de tiempo no bloqueante
int data = 0; // Contador
//int wait = 5000;  // Indica la espera cada 5 segundos para envío de mensajes MQTT

int ledPin = 33;  // Para indicar el estatus de conexión
int ledPin2 = 2; // Para mostrar mensajes recibidos //ledPin2 = 4;  lo considere para el led de alerta ahora ledPin2 = 2; 
long timeLastMQTT, timeLastMax30102, timeLastMLX; // Variables de control de tiempo no bloqueante
int wait = 5000;  // Indica la espera cada 5 segundos para envío de mensajes MQTT
int waitMax30102 = 4000; // Espera para lectura del sensor MAX30102
int waitMLX = 500; //Espera para lectura del sensor MLX90614

// Bloque de constantes necesarias para el calculo del BPM y SPO2   
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
//To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.
uint16_t irBuffer[100]; //infrared LED sensor data
uint16_t redBuffer[100];  //red LED sensor data
#else
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
#endif

int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

//byte pulseLED = 16; //Must be on PWM pin
//byte readLED = 2; //Blinks with each data 

int tir; 


// Inicialización del programa
void setup() {
  // Iniciar comunicación serial
  Serial.begin (115200);
  pinMode (flashLedPin, OUTPUT);
  pinMode (statusLedPin, OUTPUT);
  digitalWrite (flashLedPin, HIGH); //Iniciar encendido
  digitalWrite (statusLedPin, HIGH);

  Serial.println();
  Serial.println();
  Serial.print("Conectar a ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password); // Función que realiza la conexión a WiFi
 
  while (WiFi.status() != WL_CONNECTED) { // Este bucle espera a que se realice la conexión
    digitalWrite (statusLedPin, HIGH);
    delay(500); //dado que es de suma importancia esperar a la conexión, debe usarse espera bloqueante
    digitalWrite (statusLedPin, LOW);
    Serial.print(".");  // Indicador de progreso
    delay (5);
  }
  
  // Cuando se haya logrado la conexión, el programa avanzará, por lo tanto, puede informarse lo siguiente
  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());

  // Si se logro la conexión, encender led
  if (WiFi.status () > 0){
  digitalWrite (statusLedPin, LOW); //enciende en bajo
  }
  
  delay (1000); // Esta espera es solo una formalidad antes de iniciar la comunicación con el broker

  // Conexión con el broker MQTT
  client.setServer(server, 1883); // Conectarse a la IP del broker en el puerto indicado
  client.setCallback(callback); // Activar función de CallBack, permite recibir mensajes MQTT y ejecutar funciones a partir de ellos
  delay(1500);  // Esta espera es preventiva, espera a la conexión para no perder información

  dht.begin(); //Función que inicia el sensor

  timeLast = millis (); // Inicia el control de tiempo pregunta a microcontrolador cual es su hora, para que timelast comience a contar a partir de ahi

// Initialize sensor MAX30102
  Wire.begin (14,15); // checar si los pines no estan intercambiados
  if (!particleSensor.begin(Wire)) //Use default I2C port, 400kHz speed // se quito el parametro de velocidad  de esta linea if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) 
  {
    Serial.println(F("MAX30102 was not found. Please check wiring/power."));
    while (1);
  }

  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

  max30102First (); // Esta funcion realiza las primeras 100 lecturas

  Serial.println("Adafruit MLX90614 test");

  
  if (!mlx.begin(0x5A, &Wire)) { //, I2C_SPEED_FAST
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };

  Serial.print("Emissivity = "); Serial.println(mlx.readEmissivity());
  Serial.println("================================================");
  delay (500);
  timeLastMQTT = millis (); // Inicia el control de tiempo de envio mqtt
  timeLastMax30102 = millis (); // Inicia el control de tiempo del sensor
  timeLastMLX = millis (); // Inicia el control de tiempo del sensor MLX90614

}// fin del void setup ()

// Cuerpo del programa, bucle principal
void loop() {
  //Verificar siempre que haya conexión al broker
  if (!client.connected()) {
    reconnect();  // En caso de que no haya conexión, ejecutar la función de reconexión, definida despues del void setup ()
  }// fin del if (!client.connected())
  client.loop(); // Esta función es muy importante, ejecuta de manera no bloqueante las funciones necesarias para la comunicación con el broker
  
  timeNow = millis(); // Control de tiempo para esperas no bloqueantes
  if (timeNow - timeLast > wait) { // Manda un mensaje por MQTT cada cinco segundos
    timeLast = timeNow; // Actualización de seguimiento de tiempo

 // ¡Leer la temperatura o la humedad toma alrededor de 250 milisegundos!
 // Las lecturas del sensor también pueden tener hasta 2 segundos de antigüedad (ya que es un sensor que actua muy lento)
   //OBTENER TEMPERATURA Y HUMEDAD
    float t = dht.readTemperature();
    float h = dht.readHumidity();

  // Comprobar si alguna lectura falló y salir antes (para volver a intentarlo).
  //Secuencia que se asegura de que la conexión con el sensor exista
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    
    

     //Se construye el string correspondiente al JSON 
    String json = "{\"id\":\"SmartDispenser\",\"temp\":"+String(t)+",\"hum\":"+String(h)+"}";
    Serial.println(json); // Se imprime en monitor solo para poder visualizar que el string esta correctamente creado
    int str_len = json.length() + 1;//Se calcula la longitud del string
    char char_array[str_len];//Se crea un arreglo de caracteres de dicha longitud
    json.toCharArray(char_array, str_len);//Se convierte el string a char array    
    client.publish("SmartDispenser/envio/mqtt", char_array); // Esta es la función que envía los datos por MQTT, especifica el tema y el valor
  }// fin del if (timeNow - timeLast > wait)

if (timeNow - timeLastMax30102 > waitMax30102) {
    timeLastMax30102 = timeNow;

    //Bloque de programacion de lectura del sensor
    //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    //take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
        particleSensor.check(); //Check the sensor for new data

      //digitalWrite(readLED, !digitalRead(readLED)); //Blink onboard LED with every data read

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample

      //send samples and calculation result to terminal program through UART

      //Serial.print(F("HR="));
      //Serial.print(heartRate, DEC);

      //Serial.print(F(", HRvalid="));
      //Serial.print(validHeartRate, DEC);

      //Serial.print(F(", SPO2="));
      //Serial.print(spo2, DEC);

      //Serial.print(F(", SPO2Valid="));
      //Serial.println(validSPO2, DEC);
    }

    //After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    
  }

  if (timeNow - timeLastMLX > waitMLX) {
    timeLastMLX = timeNow; // Actualización de seguimiento de tiempo

    tir = mlx.readObjectTempC();
    Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());    
    Serial.print("*C\tObject = "); Serial.print(tir); Serial.println("*C");
  }
  
  if (timeNow - timeLastMQTT > wait) { // Manda un mensaje por MQTT cada cinco segundos
    timeLastMQTT = timeNow; // Actualización de seguimiento de tiempo

    //Se construye el string correspondiente al JSON que contiene 3 variables
    String json = "{\"hr\":" + String (heartRate) + ",\"hrv\":" + String (validHeartRate) + ",\"spo2\":" + String (spo2) + ",\"spo2v\":"+ String (validSPO2) + ",\"tir\":"+ String(tir) + "}";
    Serial.println(json); // Se imprime en monitor solo para poder visualizar que el string esta correctamente creado
    int str_len = json.length() + 1;//Se calcula la longitud del string
    char char_array[str_len];//Se crea un arreglo de caracteres de dicha longitud
    json.toCharArray(char_array, str_len);//Se convierte el string a char array    
    client.publish("SmartDispenser/indices/mqtt", char_array); // Esta es la función que envía los datos por MQTT, especifica el tema y el valor
  
//Variable para el led
    data++; // Incremento a la variable para ser enviado por MQTT
    char dataString[8]; // Define una arreglo de caracteres para enviarlos por MQTT, especifica la longitud del mensaje en 8 caracteres
    dtostrf(data, 1, 2, dataString);  // Esta es una función nativa de leguaje AVR que convierte un arreglo de caracteres en una variable String
                                    //dtostrf(valor punto flotante a convertir,cantidad minima de caracteres a generar, precisión en decimales, varible donde se guardará)
    Serial.print("Contador: "); // Se imprime en monitor solo para poder visualizar que el evento sucede
    Serial.println(dataString);
    client.publish("codigoIoT/led/mqtt", dataString); // Esta es la función que envía los datos por MQTT, especifica el tema y el valor

 }// fin del if (timeNow - timeLastMQTT > wait)
}// fin del void loop ()

// Funciones de usuario

// Esta función permite tomar acciones en caso de que se reciba un mensaje correspondiente a un tema al cual se hará una suscripción
void callback(char* topic, byte* message, unsigned int length) {

  // Indicar por serial que llegó un mensaje
  Serial.print("Llegó un mensaje en el tema: ");
  Serial.print(topic);

  // Concatenar los mensajes recibidos para conformarlos como una varialbe String
  String messageTemp; // Se declara la variable en la cual se generará el mensaje completo  
  for (int i = 0; i < length; i++) {  // Se imprime y concatena el mensaje
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  // Se comprueba que el mensaje se haya concatenado correctamente
  Serial.println();
  Serial.print ("Mensaje concatenado en una sola variable: ");
  Serial.println (messageTemp);

  // En esta parte puedes agregar las funciones que requieras para actuar segun lo necesites al recibir un mensaje MQTT

  // Ejemplo, en caso de recibir el mensaje true - false, se cambiará el estado del led soldado en la placa.
  // El ESP323CAM está suscrito al tema SmartDispenser/envio/mqtt
  if (String(topic) == "SmartDispenser/estadoled/mqttin") {  // En caso de recibirse mensaje en el tema SmartDispenser/estadoled/mqttin
    if(messageTemp == "true"){
      Serial.println("Led encendido");
      digitalWrite(flashLedPin, HIGH);
    }// fin del if (String(topic) == "SmartDispenser/estadoled/mqttin")
    else if(messageTemp == "false"){
      Serial.println("Led apagado");
      digitalWrite(flashLedPin, LOW);
    }// fin del else if(messageTemp == "false")
  }// fin del if (String(topic) == "SmartDispenser/estadoled/mqttin")
}// fin del void callback

// Función para reconectarse
void reconnect() {
  // Bucle hasta lograr conexión
  while (!client.connected()) { // Pregunta si hay conexión
    Serial.print("Tratando de contectarse...");
    // Intentar reconexión
    if (client.connect("ESP32CAMClient")) { //Pregunta por el resultado del intento de conexión
      Serial.println("Conectado");
      client.subscribe("SmartDispenser/estadoled/mqttin"); // Esta función realiza la suscripción al tema
    }// fin del  if (client.connect("ESP32CAMClient"))
    else {  //en caso de que la conexión no se logre
      Serial.print("Conexion fallida, Error rc=");
      Serial.print(client.state()); // Muestra el codigo de error
      Serial.println(" Volviendo a intentar en 5 segundos");
      // Espera de 5 segundos bloqueante
      delay(5000);
      Serial.println (client.connected ()); // Muestra estatus de conexión
    }// fin del else
  }// fin del bucle while (!client.connected())
}// fin de void reconnect()

// Funcion de primeras 100 lecturas
void max30102First () {
  bufferLength = 100; //buffer length of 100 stores 4 seconds of samples running at 25sps

  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample

    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.println(irBuffer[i], DEC);
  }

  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
}

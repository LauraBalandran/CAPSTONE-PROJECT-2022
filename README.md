# CAPSTONE-PROJECT- SMARTDISPENSER -2022
Este repositorio contiene la descripción del proyecto Capstone del Diplomado CodigoIoT Samsung Innovation Campus
para llevar a cabo el monitoreo de la toma de medicamentos de una paciente adulto mayor, ademas el monitoreo del ambiente o clima dentro del dispensador médico.

## **Introducción**

EL dispensador fue desarrollado como primer prototipo con material reusable, usando cajoncillos de material de plástico para los compartimentos y carton cascarón para las divisiones, pastilleros de dulce para adicionar a los compartimentos, dando así una vida útil máxima y cuidar el medio ambiente.
La finalidad de este dispositivo es, apoyar al adulto mayor para que organice de una forma práctica sus medicamentos y lo promueva a esta monitoreando los indices vitales para su salud.
Además del monitoreo de indices vitales también cuenta con un monitoreo del clima dentro del dispensador, para mantener el medicamento conservando mejor su producto activo.

La aplicación del Dispensador médico se visualiza en un flow de Node-red que recibe por mqtt los valores del circuito que tiene el sensor de temperatura MLX90614 y pulsioxímetro MAX30102, tomando la temperatura, oxigenación y ritmo cardíaco para algún paciente, guardando estos indices en una base de datos de MySQL y así tener también las valoraciones de salud que genera la revisión de dichos indices. Además el monitoreo de los indices de temperatura y humedad dentro del dispensador hacen posible la revisión de los rango adecuados para el almacenamiento de los medicamentos.



## **Requerimientos**


 - Ubuntu 20.04
 -  Instalar MySQL en Ubuntu 
 - Node-Red con mysql y tablas
 -  IDE de Arduino
 - Mosquito MQTT Broker instalado
 


## **Para el circuito**

 - Módulo ESP32CAM
 - conversor serial-USB, FTDI232RL
 - pulsioxímetro MAX30102
 - sensor de temperatura corporal a distancia MLX90614
 - sensor de temperatura y humedad ambiental DHT11
 - miniventilador de enfriamiento


## **Descripción de la Aplicación**


![Diagrama del CI](https://github.com/LauraBalandran/CAPSTONE-PROJECT-2022/blob/main/Diagrama%20de%20Circuito/Circuito%20digital%20Smart%20Dispenser.png)

![Acomodamiento del Circuito ](https://github.com/LauraBalandran/CAPSTONE-PROJECT-2022/blob/main/imgs%20readme/inserci%C3%B3n%20del%20circuito.jpeg)

![Circuito de los sensores MLX90614, MAX30102, DHT11 ](https://github.com/LauraBalandran/CAPSTONE-PROJECT-2022/blob/main/imgs%20readme/Primera%20forma%20del%20Dispensador.jpeg)

![Circuito insertado dentro del Dispensador](https://github.com/LauraBalandran/CAPSTONE-PROJECT-2022/blob/main/imgs%20readme/circuito%20insertado%20dentro%20del%20Dispensador.jpg)


Interfaz principal del SmartDispenser en Dashboard de Node Red 

Mostrando un reloj para recordar la hora, botones principales para dar de alta su perfil o guardar los indices vitales de pulso, oxigenación y temperatura, tambíen se muestra la
tabla de tratamiento, y puede dar de alta uno nuevo o revisar las estadisticas con las valoraciones acerca de los indices vitales tomados.

![Menu Principal](https://github.com/LauraBalandran/CAPSTONE-PROJECT-2022/blob/main/Imagenes%20Proyecto/NodeRed/1_Interfaz%20gr%C3%A1fica%20principal.png)

Conjunto de nodo en Node Red que hacen posible la lectura de los datos del ambiente dentro del dispensador

![MQTT](https://github.com/LauraBalandran/CAPSTONE-PROJECT-2022/blob/main/imgs%20readme/mqtt%20estado%20smartdispenser.png)


Tabla que muestra el registro de los indices guardados y las valoraciones sobre su estado de salud (las valoraciones pueden ser supuestas solo para que este en observación de 
su salud).

![Dashboard de Valoraciones](https://github.com/LauraBalandran/CAPSTONE-PROJECT-2022/blob/main/Imagenes%20Proyecto/NodeRed/5_Estadisticas%20y%20Valoraciones%20de%20los%20indices%20vitales.png)

Base de datos llamada SmartDispenserData para guardar los datos e indices vitales del propietario del SmartDispenser

![Base de Datos DataSmartDispenser](https://github.com/LauraBalandran/CAPSTONE-PROJECT-2022/blob/main/imgs%20readme/tablas%20de%20la%20base%20de%20datos%20del%20SD.png)


Si los indices climaticos dentro del Dispensador son inadecuados, emitirá una alerta para ventilar la habitación y informará acerca de los rangos adecuados.

![Alerta de indices ambientales dentro del dispensador](https://github.com/LauraBalandran/CAPSTONE-PROJECT-2022/blob/main/Imagenes%20Proyecto/NodeRed/6_Reloj%20Indices%20Altos%20del%20SmartDispenser.png)

Al guardar los indices vitales para revisión y valoración, emite un saludo (con la finalidad de revisar estado de animo del paciente).

![Saludo](https://github.com/LauraBalandran/CAPSTONE-PROJECT-2022/blob/main/Imagenes%20Proyecto/NodeRed/Notificaci%C3%B3n%20para%20consulta%20de%20estadisticas.png)


En el programa de la IDE de arduino  realiza una conexión a internet por wifi, para enviar un json por mqtt al tema SmartDispenser/envio/mqtt
[Programa que realiza la lectura de los valores de los sensores mencionados](https://github.com/LauraBalandran/CAPSTONE-PROJECT-2022/blob/main/imgs%20readme/ino%20arduino.png)



## **Vistas previas y vídeos de los resultados**

[Vista video Tiktok](https://vm.tiktok.com/ZMY8SANQy/) Funcionamiento de la aplicación SmartDispenser.


**Hashtags**
  
  #CodigoIoT
  #SamsungInnovationCampus
  #InternetDeLasCosas
  #IoT
  #NodeRed
  #ESP32CAM
  #SmartDispenser
  #MAX30102
  #MLX90614
  #DHT11

 

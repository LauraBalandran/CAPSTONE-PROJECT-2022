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


![Diagrama del CI](https://github.com/LauraBalandran/DetectorDeSintomasCOVID/blob/main/Imagenes/Diagrama%20del%20circuto%20pulsoximetro-temperatura.png)


![Circuito de los sensores MLX90614 y MAX30102](https://github.com/LauraBalandran/DetectorDeSintomasCOVID/blob/main/Imagenes/circuito%20sensor%20MLX90614%20y%20MAX30102.jpg)

Valores recibidos en terminal Ubuntu con la subscripción al tema codigoIoT/detectorSintomas/flow

![valores por mqtt en la terminal de ubuntu](https://github.com/LauraBalandran/DetectorDeSintomasCOVID/blob/main/Imagenes/valores%20por%20mqtt.png)

![nodo mqtt de Node-red](https://github.com/LauraBalandran/DetectorDeSintomasCOVID/blob/main/Imagenes/nodo%20mqtt.png)


Posteriormente toma los valores para mostrarlos en las gráficas mostradas en el Dashboard, donde también se solicita los datos del paciente, los guarda en variables globales para luego enviarlos y almacenarlos en una BD de Mysql llamada “detectorsintomas” en una tabla llamada “registro”.
![Dashboard de la aplicación](https://github.com/LauraBalandran/DetectorDeSintomasCOVID/blob/main/Imagenes/DashboardDetectorSintomasCOVID.jpeg)


![Datos almacenados en la BD](https://github.com/LauraBalandran/DetectorDeSintomasCOVID/blob/main/Imagenes/datos%20en%20la%20BD.png)


Al oprimir un botón llamado “Realizar diagnostico”, una función compara los valores para emitir un proto diagnóstico emitido en audio y enviar este último valor por correo electrónico.

![correo recibido](https://github.com/LauraBalandran/DetectorDeSintomasCOVID/blob/main/Imagenes/nodo%20boton.png)

![enter image description here](https://github.com/LauraBalandran/DetectorDeSintomasCOVID/blob/main/Imagenes/correo%20recibido.png)

![Abrir correo con el proto diagnóstico](https://github.com/LauraBalandran/DetectorDeSintomasCOVID/blob/main/Imagenes/ver%20correo.png)


En el programa de la IDE de arduino  realiza una conexión a internet por wifi, para enviar un json por mqtt al tema codigoIoT/detectorSintomas/flow
[Programa que realiza la lectura de los valores de los sensores mencionados](https://github.com/LauraBalandran/DetectorDeSintomasCOVID/tree/main/ESP32CAM/ESP32CAM-JSON-MQTT-MLX90614-MAX30102)



## **Vistas previas y vídeos de los resultados**

[Vista video Tiktok](https://vm.tiktok.com/ZMNEqkYQk/) Funcionamiento de la aplicación Detector de Sintomas Covid.

[Vista video Youtube](https://youtu.be/E8k-flcJAIE)
![](https://youtu.be/E8k-flcJAIE) Funcionamiento de la aplicación en el dashboard Node-red.

[Vista video2 Youtube](https://youtu.be/UWlHNyCOzPU)
![](https://youtu.be/UWlHNyCOzPU) Funcionamiento de la aplicación dashboard Node-red, vista datos en la BD.

**Hashtags**
  
  #CodigoIoT
  #SamsungInnovationCampus
  #InternetDeLasCosas
  #IoT
  #NodeRed
  #ESP32CAM
  #DetectorSintomasCovid
  #MAX30102
  #MLX90614
 

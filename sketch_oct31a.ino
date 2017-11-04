/*
  Ethernet Monitor Analogo
 By: http://www.elprofegarcia.com/
 Este programa usa el Modulo Ethernet de Arduino para monitorear las entradas analogicas A0-A5
 
 Conexiones:
 * Ethernet shield usa los pins 10, 11, 12, 13
 * Monitorea los pines Analogos de A0 a A5
 
Se debe conectar el modulo a la red local y se debe asignar una IP fija que no coincida
con los equipos de la red que ya estan funcionando pero dede estar dentro de la SubRed.
puede monitorear la IP de su PC dentro de la ventana de comandos CMD con el comando ipconfig
*/

#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"

#define DHTTYPE DHT11

const int DHTPin = 5;
DHT dht(DHTPin, DHTTYPE);

                               // Introduzca una dirección MAC y la dirección IP para el controlador
byte mac[] = { 
0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress ip(192,168,0,70);

EthernetServer server(80);    // Puerto 80 por defecto para HTTP

float t = 0;    //TEMPERATURA
float h = 0;    //HUMEDAD
String data;

void setup() {
  Ethernet.begin(mac,ip);    //inicializa la conexión Ethernet y el servidor
  //Serial.begin(9600);
  server.begin();
  //Serial.begin("Empieza sensado");
  dht.begin();
  delay(10000);       //le damos tiempo al DHT para que empieze

  h = dht.readHumidity();
  t = dht.readTemperature();
  data = "";
}

void loop() {
  EthernetClient cliente = server.available(); // Inicializa cliente como servidor ethernet
  if (cliente) {
    boolean currentLineIsBlank = true;
    while (cliente.connected()) {
      if (cliente.available()) {
        char c = cliente.read();
        if (c == '\n' && currentLineIsBlank) { 
          cliente.println("HTTP/1.1 200 OK");
          cliente.println("Content-Type: text/html");   // Envia el encabezado en codigo HTML estandar
          cliente.println("Connection: close"); 
          cliente.println("Refresh: 3");  // refresca la pagina automaticamente cada 3 segundos
          cliente.println();
          cliente.println("<!DOCTYPE HTML>"); 
          cliente.println("<html>");
          cliente.println("<HEAD>");
          cliente.println("<TITLE>Lectura de temperatura y humedad</TITLE>");
          cliente.println("</HEAD>");
          cliente.println("<BODY>");
          cliente.println("<hr />");
          cliente.println("<H1>Lectura de temperatura y humedad</H1>");
          cliente.println("<br />");    
          cliente.println("<br />");

          float h = dht.readHumidity();
          float t = dht.readTemperature();

          data = "temp=" + String(t) + "&hum=" + String(h);
          
          if(isnan(h) || isnan(t)){
                cliente.print("Fallo del sensor al medir");
                cliente.println("<br />");         
                return;
          }
          
          /*
          if(client.connect("www.*******.heroku.com",80)){      //DIRECCION DEL SERVER
              client.println("POST /*****.html HTTP/1.1");
              client.println("Host: *******.heroku.com");       //DIRECCION DEL SERVER
              client.println("Content-Type: application/x-www-form-urlencoded");
              client.print("Content-Length: ");
              client.println(data.length());
              client.println();
              client.print(data);
         }
          */

          cliente.print("Humedad: ");
          cliente.print(h);
          cliente.println("<br />");       
        
          cliente.print("Temperatura: ");
          cliente.print(t);
          cliente.println("<br />"); 

                
          cliente.print("Datos: ");
          cliente.print(data);
          cliente.println("<br />");       
        
        
          cliente.println("<br />"); 
          cliente.println("</html>");
          break;
        }
        if (c == '\n') {
           currentLineIsBlank = true;
        } 
        else if (c != '\r') {
           currentLineIsBlank = false;
        }
      }
    }
   //delay(300000);
   delay(15);           // Da tiempo al Servidor para que reciba los datos 15ms
   cliente.stop();     // cierra la conexion
  }
}

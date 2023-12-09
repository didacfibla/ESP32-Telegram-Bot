template<class T> inline Print &operator <<(Print &obj, T arg) {
  obj.print(arg);
  return obj;
}

#include "CTBot.h"
#include <ezButton.h>

#define RELAY_PIN  23
#define BUTTON_PIN 22 // GPIO22 pin connected to button
#define DEBOUNCE_TIME 50 // the debounce time in millisecond, increase this time if it still chatters


ezButton button(BUTTON_PIN); // create ezButton object that attach to pin GPIO21
volatile bool pulsador_activo = false;
String estadoPulsador = "desactivado";  // Valor inicial

CTBot miBot;

const char* ssid = "CHANGE_THIS";
const char* password = "CHANGE_THIS";
const String token = "CHANGE_THIS"; 

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Bot de Telegram");

  // Relay config
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Button config
  button.setDebounceTime(DEBOUNCE_TIME); // set debounce time to 50 milliseconds

  bool wifi_conection = miBot.wifiConnect(ssid, password);

  while (!wifi_conection){
    wifi_conection = miBot.wifiConnect(ssid, password);
    delay(1000);
    Serial.print(".");
  }
  
  miBot.setTelegramToken(token);

  if (miBot.testConnection()) {
    Serial.println("Conectado");
    miBot.sendMessage(CHANGE_THIS, "El bot se ha iniciado.\nEstos son los comandos disponibles:\n/activar : para activar el relé durante 500ms \n/pulsador : para habilitar/deshabilitar la apertura manual\n/estado : muestra si el pulsador está habilitado o no");
  }
  else {
    Serial.println("Problemas Auxilio");
  }
}

void loop() {
  TBMessage msg;
  button.loop(); // MUST call the loop() function first

  // Si el boton esta activo y lo pulsamos activará el relé
  if (pulsador_activo) {
    if (button.isReleased()){
      Serial.println("Activamos relay por el pulsador");
      digitalWrite(RELAY_PIN, HIGH);
      delay(500);
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Desactivamos relay");

      miBot.sendMessage(CHANGE_THIS, "Relay pulsado durante 500 ms");
    }
  }

  // Si alguien pulsa el boton mientras no estaba activado nos avisará!
  if (!pulsador_activo){
    if (button.isPressed()){
      miBot.sendMessage(CHANGE_THIS, "Alguien ha pulsado el botón mientras estaba desactivado");
      delay(500);
    }
  }


  if (CTBotMessageText == miBot.getNewMessage(msg)) {
    Serial << "Mensaje: " << msg.sender.firstName << " - " <<  msg.text << "\n";

    if (msg.sender.id == CHANGE_THIS) {

      if (msg.text.equalsIgnoreCase("/start")) {
       miBot.sendMessage(msg.sender.id, "Bienvenido " + msg.sender.firstName +"\nEstos son los comandos disponibles:\n/activar : para activar el relé durante 500ms \n/pulsador : para habilitar/deshabilitar la apertura manual\n/estado : muestra si el pulsador está habilitado o no");
      }

      else if (msg.text.equalsIgnoreCase("/activar")) {
        Serial.println("Activamos relay");
        digitalWrite(RELAY_PIN, HIGH);
        delay(500);
        digitalWrite(RELAY_PIN, LOW);
        Serial.println("Desactivamos relay");

        miBot.sendMessage(msg.sender.id, "Relay pulsado durante 500 ms");
      }

      else if (msg.text.equalsIgnoreCase("/pulsador")) {
        Serial.println("Activar/Desactivar el pulsador de apertura manual");
        pulsador_activo = !pulsador_activo;
        estadoPulsador = pulsador_activo ? "activado" : "desactivado";

        miBot.sendMessage(msg.sender.id, "Pulsador: "+ estadoPulsador);
      }

      else if (msg.text.equalsIgnoreCase("/estado")) {
        Serial.println("Ver el estado del pulsador");
        estadoPulsador = pulsador_activo ? "activado" : "desactivado";
        miBot.sendMessage(msg.sender.id, "Pulsador: "+ estadoPulsador);
      }
    
      else {
         miBot.sendMessage(msg.sender.id, "Comando no reconocido\nEstos son los comandos disponibles:\n/activar : para activar el relé durante 500ms \n/pulsador : para habilitar/deshabilitar la apertura manual\n/estado : muestra si el pulsador está habilitado o no");
      
      }

    }else{
      miBot.sendMessage(msg.sender.id, "Usuario no autorizado");
    }
  }

  delay(250); 
}

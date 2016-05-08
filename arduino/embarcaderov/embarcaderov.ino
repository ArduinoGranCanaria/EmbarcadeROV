/* Motor. Programacion con estructura. */
#include <SoftwareSerial.h>

#define RUTA_MAX_EVENTOS 100

const int PAUSA = 2000;
const int PAUSA_PARAR = 500;

const int MOTOR_DERECHO_PIN_A = 10;
const int MOTOR_DERECHO_PIN_B = 9;
const int MOTOR_DERECHO_PWM = 5;

const int MOTOR_IZQUIERDO_PIN_A = 12;
const int MOTOR_IZQUIERDO_PIN_B = 11;
const int MOTOR_IZQUIERDO_PWM = 6;

const int VELOCIDAD_MEDIA = 127;
const int VELOCIDAD_ALTA = 255;

struct Motor {
  int pinA;
  int pinB;
  int pinPWM;

  void setup(int pin_a, int pin_b, int pin_pwm) {
    pinA = pin_a;
    pinB = pin_b;
    pinPWM = pin_pwm;
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinPWM, OUTPUT);
    parar();
  }

  void parar() {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    digitalWrite(pinPWM, HIGH);
  }

  void avanzar(int velocidad = VELOCIDAD_ALTA) {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
    analogWrite(pinPWM, velocidad);
  }

  void retroceder(int velocidad = VELOCIDAD_ALTA) {
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
    analogWrite(pinPWM, velocidad);
  }

};

struct Coche {
  Motor motorIzquierdo;
  Motor motorDerecho;

  void avanzar() {
    motorIzquierdo.avanzar();
    motorDerecho.avanzar();
  }

  void parar() {
    motorIzquierdo.parar();
    motorDerecho.parar();
  }

  void retroceder() {
    motorIzquierdo.retroceder();
    motorDerecho.retroceder();
  }

  void rotarIzquierda() {
    motorIzquierdo.retroceder();
    motorDerecho.avanzar();
  }

  void rotarDerecha() {
    motorIzquierdo.avanzar();
    motorDerecho.retroceder();
  }

  void girarIzquierda() {
    motorIzquierdo.avanzar();
    motorDerecho.parar();
  }

  void girarDerecha() {
    motorIzquierdo.parar();
    motorDerecho.avanzar();
  }

  void izquierda() {
    motorIzquierdo.avanzar();
    motorDerecho.avanzar(VELOCIDAD_MEDIA);
  }

  void derecha() {
    motorIzquierdo.avanzar(VELOCIDAD_MEDIA);
    motorDerecho.avanzar();
  }

  void avanzar_metros(int metros) {
    avanzar();
    delay(5000 * metros);
    parar();
  }

  void retroceder_metros(int metros) {
    retroceder();
    delay(5000 * metros);
    parar();
  }

  void izquierda_grados(int grados) {
    girarIzquierda();
    delay(int(grados / 90.0 * 650));
    parar();
  }

  void derecha_grados(int grados) {
    girarDerecha();
    delay(int(grados / 90.0 * 650));
    parar();
  }

};

struct Evento {
  char orden;
  int cantidad;
};

SoftwareSerial btSerial(4, 3); // RX, TX

struct Coche coche;
struct Evento cola_eventos[RUTA_MAX_EVENTOS];

int num_eventos = 0;
String linea = "";

void setup() {
  // Establecer conexion Bluetooth
  Serial.begin(9600);
  btSerial.begin(9600);
  // btSerial.println("Hello, world?");

  // Inicializar motores
  coche.motorIzquierdo.setup(MOTOR_IZQUIERDO_PIN_A,
                             MOTOR_IZQUIERDO_PIN_B,
                             MOTOR_IZQUIERDO_PWM);
  coche.motorDerecho.setup(MOTOR_DERECHO_PIN_A,
                           MOTOR_DERECHO_PIN_B,
                           MOTOR_DERECHO_PWM);

}

void loop() {
  // Leer comandos
  while (btSerial.available()) {
    char c = btSerial.read();
    Serial.print(c);
    // Final de línea, interpretar
    if (c == '\n') {
      String comando = "";
      for (int i = 0; i <= linea.length(); i++) {
        char s = linea[i];
        if (s == ',' || i == linea.length()) {
          // Nuevo comando
          char orden = comando[0];
          int cantidad = (comando.substring(1, comando.length())).toInt();

          Serial.println("Recibiendo ruta " + String(num_eventos) + ": " + String(orden) + " " + String(cantidad));

          if (num_eventos < RUTA_MAX_EVENTOS) {
            cola_eventos[num_eventos].orden = orden;
            cola_eventos[num_eventos].cantidad = cantidad;
            num_eventos++;
          }

          comando = "";
        } else {
          comando = comando + String(s);
        }
      }
      // Reiniciar
      linea = "";
      break; // Procesar solo una ruta
    } else if (c >= 32 && c <= 255) {
      // Componer orden
      linea = linea + String(c);
    }
  }
  // Realizar eventos
  if (num_eventos > 0) {
    Serial.println("Realizando ruta...");
    for (int e = 0; e < num_eventos; e++) {
      char orden = cola_eventos[e].orden;
      int cantidad = cola_eventos[e].cantidad;
      Serial.print("Evento " + String(e + 1) + ": ");
      if (orden == 'A') {
        Serial.println("Avanzo " + String(cantidad) + " metros");
        coche.avanzar_metros(cantidad);
      } else if (orden == 'R') {
        Serial.println("Retrocedo " + String(cantidad) + " metros");
        coche.retroceder_metros(cantidad);
      } else if (orden == 'I') {
        Serial.println("Izquierda " + String(cantidad) + " grados");
        coche.izquierda_grados(cantidad);
      } else if (orden == 'D') {
        Serial.println("Derecha " + String(cantidad) + " grados");
        coche.derecha_grados(cantidad);
      } else {
        Serial.println("Orden no comprendida");
        break;
      }
    }
    num_eventos = 0;
  }
}


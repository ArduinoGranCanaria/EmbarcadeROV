import processing.serial.*;

Serial serial;
PImage mapa;
Punto[] puntos;

int puntos_num = 0; 

void setup() {
  size(1188, 971);
  puntos = new Punto[100];
  // Cargar mapa del puerto
  mapa = loadImage("mapa-puerto.png");
  // Inicializar comunicacion Bluetooth
  String dispositivo = "/dev/rfcomm2";
  serial = new Serial(this, dispositivo, 9600); 
}

void draw() {
  image(mapa, 0, 0);
  // Dibujar puntos
  for (int i = 0; i < puntos_num; i++) {
    if (i > 0) {
      // Dibujar linea entre puntos
      stroke(color(0, 0, 255));
      line(puntos[i-1].x, puntos[i-1].y, puntos[i].x, puntos[i].y);
    }
    fill(color(255, 0, 0));
    noStroke();
    ellipse(puntos[i].x, puntos[i].y, 5, 5);
  }
}

void mouseClicked() {
  // Añadir punto
  if (puntos_num < 100) {
    println(puntos_num);
    puntos[puntos_num] = new Punto();
    // Guardar las coordenadas del raton
    puntos[puntos_num].x = mouseX;
    puntos[puntos_num].y = mouseY;
    puntos_num++;
  }
}

float angulo(Punto p1, Punto p2) {
  // Calcular angulo de la recta
  float a;
  if (p2.x > p1.x) {
    // Entre 0 y 180
    a = (atan2((p2.x - p1.x), (p1.y - p2.y)) * 180 / PI);
  } else if ((p2.x < p1.x)) {
    // Entre 180 y 360/0
    a = 360 - (atan2((p1.x - p2.x), (p1.y - p2.y)) * 180 / PI);
  } else {
    // Finalizar si((p2.x > p1.x) && (p2.y <= p1.y))
    a = degrees(atan2(0, 0));
  }
  return a;
}

void keyPressed() {
  if (keyCode == '-' && puntos_num > 0) {
    // Eliminar ultimo punto
    puntos[puntos_num] = null;
    puntos_num--;
  } else if (keyCode == BACKSPACE || keyCode == DELETE) {
    // Eliminar todos los puntos
    while (puntos_num > 0) {
      puntos[puntos_num] = null;
      puntos_num--;
    }
  } else if (keyCode == ENTER) {
    float angulo_anterior = 0.0;
    String comando = "";
    
    // Componer ruta
    for (int i = 1; i < puntos_num; i++) {
      float distancia = sqrt(pow(puntos[i].x - puntos[i-1].x, 2) + pow(puntos[i].y - puntos[i-1].y, 2));
      float angulo = angulo(puntos[i-1], puntos[i]);
      println(i + ": " + distancia + " puntos - angulo: " + angulo + " (" + (angulo - angulo_anterior) + ")");
      distancia = distancia / 50.0;
      if (i > 1) {
        comando = comando + ",";
      }
      if (angulo < 180.0) {
        comando = comando + "D" + str(int(angulo)) + ",A" + str(int(distancia));
      } else {
        comando = comando + "I" + str(int(360 - angulo)) + ",A" + str(int(distancia));
      }
      angulo_anterior = angulo;
    }
    // Enviar orden
    if (comando.length() > 0) {
      comando = comando + "\n";
      serial.write(comando);
      print(comando);
    }
  }
}


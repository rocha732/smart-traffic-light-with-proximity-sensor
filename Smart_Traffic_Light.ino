
// Constantes de los pines
const float CM = 0.016949;          // Factor de conversión de tiempo a distancia en centímetros
const float DISTANCIA_CALLE = 8.5;  // Distancia mínima para considerar que hay un vehículo en la calle

// Los pines de trigger y echo de los sensores ultrasónicos
const int TRIGGER_PIN[] = { 9, 8, 5, 4 };  // Pines de trigger para cada sensor
const int ECHO_PIN[] = { 11, 10, 3, 2 };   // Pines de echo para cada sensor

// Los pines de los LEDs para el semáforo
const int LED_RED[] = { 7, 13 };    // Pines de los LEDs rojos
const int LED_GREEN[] = { 6, 12 };  // Pines de los LEDs verdes

// Variables
float cm[4] = { 0, 0, 0, 0 };             // Almacenar las distancias medidas por los sensores
int contador[3] = { 0, 0, 0 };            // Contadores para controlar los tiempos y cambios de semáforo
int contadorA[3] = { 5, 10, 20 };         // Valores de tiempo para los cambios de semáforo
bool semaforoCalle[2] = { true, false };  // Estado del semáforo para cada calle (true = verde, false = rojo)

// Función para obtener la distancia en centímetros mediante el sensor ultrasónico
long readUltrasonicDistance(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);  // Enviamos un pulso de 10us
  digitalWrite(triggerPin, LOW);
  return pulseIn(echoPin, HIGH);
}

// Función para cambiar las luces del semáforo según el estado de la calle
void cambiarLucesSemaforo() {
  digitalWrite(LED_RED[0], semaforoCalle[0] ? LOW : HIGH);
  digitalWrite(LED_GREEN[0], semaforoCalle[0] ? HIGH : LOW);
  digitalWrite(LED_RED[1], semaforoCalle[1] ? LOW : HIGH);
  digitalWrite(LED_GREEN[1], semaforoCalle[1] ? HIGH : LOW);
}

// Función para realizar la lectura del sensor ultrasónico en una posición específica
void lectura(int index) {
  //CM = 0.016949
  cm[index] = CM * readUltrasonicDistance(TRIGGER_PIN[index], ECHO_PIN[index]);
  Serial.println("Sensor (" + String(index) + ") : " + String(cm[index]) + "cm");
}

// Función para detectar si hay vehículos en la calle
void deteccionVehiculo(int index) {

  if (cm[index] <= DISTANCIA_CALLE) {
    if (index == 0) {
      // En caso de que la otra calle esté vacía, se realizará un cambio
      if (cm[2] >= DISTANCIA_CALLE) {
        contador[0]++;
        if (contador[0] == contadorA[0]) {
          semaforoCalle[0] = false;
          semaforoCalle[1] = true;
          cambiarLucesSemaforo();
          contador[0] = 0;
        }
      } else {
        contador[0] = 0;
        contador[2]++;
        if (contador[2] == contadorA[2]) {
          semaforoCalle[0] = false;
          semaforoCalle[1] = true;
          cambiarLucesSemaforo();
          contador[0] = 0;
          contador[1] = 0;
          contador[2] = 0;
        }
        if (cm[1] <= DISTANCIA_CALLE) {
          contador[1]++;
          if (contador[1] == contadorA[1]) {
            semaforoCalle[0] = false;
            semaforoCalle[1] = true;
            cambiarLucesSemaforo();
            contador[0] = 0;
            contador[1] = 0;
            contador[2] = 0;
          }
        } else {
          contador[1] = 0;
        }
      }
    } else {
      if (cm[0] >= DISTANCIA_CALLE) {
        contador[0]++;
        if (contador[0] == contadorA[0]) {
          semaforoCalle[0] = true;
          semaforoCalle[1] = false;
          cambiarLucesSemaforo();
          contador[0] = 0;
        }
      } else {
        contador[0] = 0;
        contador[2]++;
        if (contador[2] == contadorA[2]) {
          semaforoCalle[0] = true;
          semaforoCalle[1] = false;
          cambiarLucesSemaforo();
          contador[1] = 0;
          contador[2] = 0;
        }
        if (cm[3] <= DISTANCIA_CALLE) {
          contador[1]++;
          if (contador[1] == contadorA[1]) {
            semaforoCalle[0] = true;
            semaforoCalle[1] = false;
            cambiarLucesSemaforo();
            contador[1] = 0;
            contador[2] = 0;
          }
        } else {
          contador[1] = 0;
        }
      }
    }
  } else {
    contador[0] = 0;
    contador[1] = 0;
  }
}

void setup() {
  // Configurar pines de los sensores ultrasónicos HC-SR04
  for (int i = 0; i < 4; i++) {
    pinMode(ECHO_PIN[i], INPUT);
    pinMode(TRIGGER_PIN[i], OUTPUT);
  }

  // Configurar pines de los LEDs
  for (int i = 0; i < 2; i++) {
    pinMode(LED_RED[i], OUTPUT);
    pinMode(LED_GREEN[i], OUTPUT);
  }

  Serial.begin(9600);  // Iniciar la comunicación serial
}

void loop() {
  // Realizar lecturas de los sensores ultrasónicos
  for (int i = 0; i < 4; i++) {
    lectura(i);
  }

  cambiarLucesSemaforo();  // Actualizar las luces del semáforo

  // Detectar vehículos en las calles y realizar cambios en el semáforo
  if (!semaforoCalle[1]) {
    deteccionVehiculo(0);
  } else {
    deteccionVehiculo(2);
  }

  Serial.println(String(contador[0]));  // Mostrar el valor del contador 0 en el monitor serial
  Serial.println(String(contador[1]));  // Mostrar el valor del contador 1 en el monitor serial
  Serial.println(String(contador[2]));  // Mostrar el valor del contador 2 en el monitor serial

  delay(1000);  // Esperar 1 segundo antes de repetir el bucle
}

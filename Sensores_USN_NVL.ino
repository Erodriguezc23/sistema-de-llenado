// Incluímos la libreria externa para poder utilizarla
#include <LiquidCrystal.h> // Entre los símbolos <> buscará en la carpeta de librerías configurada

// Lo primero is inicializar la librería indicando los pins de la interfaz
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Definimos las constantes
#define COLS 16 // Columnas del LCD
#define ROWS 2 // Filas del LCD

// Configuramos los pines del sensor Trigger, Echo y La bomba de agua
const int PinTrig = 7;
const int PinEcho = 6;
const int PUMP_PIN = 9;


// Constante velocidad sonido en cm/s
const float VelSon = 34000.0;

// Número de muestras
const int numLecturas = 10;

// Distancia a los 100 ml y vacío
const float distancia100 = 6;
const float distanciaVacio = 14.9499999;

float lecturas[numLecturas]; // Array para almacenar lecturas
int lecturaActual = 0; // Lectura por la que vamos
float total = 0; // Total de las que llevamos
float media = 0; // Media de las medidas
bool primeraMedia = false; // Para saber que ya hemos calculado por lo menos una

void setup()
{
  // Iniciamos el monitor serie para mostrar el resultado
  Serial.begin(9600);
  // Ponemos el pin Trig en modo salida
  pinMode(PinTrig, OUTPUT);
  // Ponemos el pin Echo en modo entrada
  pinMode(PinEcho, INPUT);
  // Ponemos el pin de la bomba de agua en modo salida
  pinMode(PUMP_PIN, OUTPUT);

  // Inicializamos el array
  for (int i = 0; i < numLecturas; i++)
  {
    lecturas[i] = 0;
  }

  // Configuramos las filas y las columnas del LCD en este caso 16 columnas y 2 filas
  lcd.begin(COLS, ROWS);
}
void loop()
{
  // Eliminamos la última medida
  total = total - lecturas[lecturaActual];

  iniciarTrigger();

  // La función pulseIn obtiene el tiempo que tarda en cambiar entre estados, en este caso a HIGH
  unsigned long tiempo = pulseIn(PinEcho, HIGH);

  // Obtenemos la distancia en cm, hay que convertir el tiempo en segudos ya que está en microsegundos
  // por eso se multiplica por 0.000001
  float distancia = tiempo * 0.000001 * VelSon / 2.0;

  // Almacenamos la distancia en el array
  lecturas[lecturaActual] = distancia;

  // Añadimos la lectura al total
  total = total + lecturas[lecturaActual];

  // Avanzamos a la siguiente posición del array
  lecturaActual = lecturaActual + 1;

  // Comprobamos si hemos llegado al final del array
  if (lecturaActual >= numLecturas)
  {
    primeraMedia = true;
    lecturaActual = 0;
  }

  // Calculamos la media
  media = total / numLecturas;

  // Solo mostramos si hemos calculado por lo menos una media
  if (primeraMedia)
  {
    //                       14.9499      - 14.95 = 0
    float distanciaLleno = distanciaVacio - media;
    //                        1             * 100 / 5 = 20
    float cantidadLiquido = distanciaLleno * 100 / distancia100;
    //                        1            * 100 / 14.94999 ===
    int porcentaje = (int) (distanciaLleno * 100 / distanciaVacio);

    // Mostramos en la pantalla LCD
    lcd.clear();
    // Cantidada de líquido
    lcd.setCursor(0, 0);
    lcd.print(String(cantidadLiquido) + " ml");

    // Porcentaje
    lcd.setCursor(0, 1);
    lcd.print(String(porcentaje) + " %");

    // La media es el valor en centimetros
    // que se debe ocupar para iniciar la bomba
    Serial.print(media);
    Serial.print(" cm ");

    Serial.print(cantidadLiquido);
    Serial.print(" ml ");

    Serial.print(" Distancia de llenado: ");
    Serial.print(distanciaLleno);

    Serial.print(" Cantidad de liquido: ");
    Serial.print(cantidadLiquido);

/*
    Serial.print("Distancia de llenado");
    Serial.println(distanciaLleno);
*/

  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("Calculando: " + String(lecturaActual));
  }

  //  Se inicia al finalizar la carga de lectura
  //  6,7,8,9,10,11,12,13,14,15 -> BOMBA ENCENDIDA
  if ((media >= 6 && media <= 15)) {
    digitalWrite(PUMP_PIN, LOW);
    Serial.println(" Bomba ENCENDIDA");
  // 1,2,3,4,5,16,17,18,19,20          -> BOMBA APAGADA
  }else {
    digitalWrite(PUMP_PIN, HIGH);
    Serial.println(" Bomba APAGADA");
  }

  delay(1000);
}

// Método que inicia la secuencia del Trigger para comenzar a medir
void iniciarTrigger()
{
  // Ponemos el Triiger en estado bajo y esperamos 2 ms
  digitalWrite(PinTrig, LOW);
  delayMicroseconds(4);

  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(PinTrig, HIGH);
  delayMicroseconds(10);

  // Comenzamos poniendo el pin Trigger en estado bajo
  digitalWrite(PinTrig, LOW);
}

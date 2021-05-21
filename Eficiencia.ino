/*    Comentarios
* @file Eficiencia.ino                      Nombre del archivo
* @author Nestor Martinez, Juan J. Rojas    Autores
* @date 21 May 2021                         Fecha de ultima modificación
* @datalogger for LIENE turbine             Descripción breve de objetivo del código
* @par Institution:                         Involucrados
* DeltaLab
* LIENE
* Tecnologico de Costa Rica
* @par mail:                                Correo de los autores
* nesmar@estudiantec.cr
* juan.rojas@tec.ac.cr                                             
* @par Github repository:                   Repositorio donde se encuentra el programa
* https://github.com/DeltaLabo/anemos
*/

// NM: VERY Important ADC2 pins cannot be used when the WiFi is being used, and some of the ADC1 pins are also blocked (34 and 35)

// 1 - Librerias utilizadas
#include <WiFi.h>                   /* Incluir Biblioteca Wifi.h de Arduino                            */
#include "Adafruit_MQTT.h"          /* Incluir Biblioteca Adafruit_MQTT.h de Adafruit [Externa]        */
#include "Adafruit_MQTT_Client.h"   /* Incluir Biblioteca Adafruit_MQTT_Client.h de Adafruit [Externa] */

// 2 - Datos para la conexión wifi y transmisión mqtt
#define WLAN_SSID       "Fmartinez"                          /* Ingresar Red a la cual va a registrarse */
#define WLAN_PASS       "santia7go7"                         /* Ingresar Clave                          */
#define AIO_SERVER      "io.adafruit.com"                    /* Ingresar Servidor para registrar datos  */
#define AIO_SERVERPORT  1883                                 /* Ingresar puerto de acceso               */          
#define AIO_USERNAME    "arnold7martinez"                    /* Ingresar nombre del Usuario             */
#define AIO_KEY         "aio_NBsv87VLWolabDx4K6V8WcLtcFpH"   /* Ingresar clave/permiso del servidor     */
#define PUB_INTERVAL    2                                    /* Definir que se tomara uno de cada dos datos al publicar */ 

// 3 - Voltaje Fase 1
unsigned long VcurrentmicrosRMS1;                       /* Tiempo de la medición i         */
unsigned long VpreviousmicrosRMS1 = micros();           /* Tiempo de la medición i-1       */
unsigned long Vtbajada1;                                /* Instante en que se da un cruce por cero de positivo a negativo */
unsigned long Vtsubida1;                                /* Instante en que se da un cruce por cero de negativo a positivo */
unsigned long Vcumulativemicros1;                       /* Suma de intervalos entre mediciones, el resultado es el periodo*/
unsigned long VintervalRMS1;                            /* Intervalo entre mediciones voltaje                             */
int Vsignals1 = 0;                                      /* Numero de ondas muestreadas para el promedio de 1s             */
double VsamRMS1;                                        /* Suma de valores RMS para el valor promedio de 1s               */
double VavgRMS1;                                        /* Valor RMS promedio de 1s        */
float Vfrecuencia1;                                     /* Frecuencia de la onda muestreada                               */
float Vvaluecuadrado1;                                  /* Sumatoria del voltaje al cuadrado por el intervalo de medicion */
float Vcurrentvalue1;                                   /* Valor de la medición i          */
float Voffset1 = 1.64;                                  /* Valor del Offset en Corriente directa                          */
float Vpreviousvalue1 = analogRead(36) * 3.3/4095;      /* Valor de la medición i-1        */
bool Vsubida1;                                          /* Indicador de un cruce por cero de negativo a positivo          */
bool Vbajada1;                                          /* Indicador de un cruce por cero de positivo a negativo          */

// 4 - Corriente Fase 1
unsigned long IcurrentmicrosRMS1;
unsigned long IpreviousmicrosRMS1 = micros();
unsigned long Itbajada1;
unsigned long Itsubida1;
unsigned long Icumulativemicros1;
unsigned long IintervalRMS1;
int Isignals1 = 0;
double IsamRMS1;
double IavgRMS1;
float Ifrecuencia1;
float Ivaluecuadrado1;
float Icurrentvalue1;
float Ioffset1 = 1.31;
float Ipreviousvalue1 = analogRead(39) * 3.3/4095;
bool Isubida1;
bool Ibajada1;

// 5 - Voltaje Fase 2
unsigned long VcurrentmicrosRMS2;
unsigned long VpreviousmicrosRMS2 = micros();
unsigned long Vtbajada2;
unsigned long Vtsubida2;
unsigned long Vcumulativemicros2;
unsigned long VintervalRMS2;
int Vsignals2 = 0;
double VsamRMS2;
double VavgRMS2;
float Vfrecuencia2;
float Vvaluecuadrado2;
float Vcurrentvalue2;
float Voffset2 = 1.642;
float Vpreviousvalue2 = analogRead(37) * 3.3/4095;
bool Vsubida2;
bool Vbajada2;

// 6 - Corriente Fase 2
unsigned long IcurrentmicrosRMS2;
unsigned long IpreviousmicrosRMS2 = micros();
unsigned long Itbajada2;
unsigned long Itsubida2;
unsigned long Icumulativemicros2;
unsigned long IintervalRMS2;
int Isignals2 = 0;
double IsamRMS2;
double IavgRMS2;
float Ifrecuencia2;
float Ivaluecuadrado2;
float Icurrentvalue2;
float Ioffset2 = 1.31;
float Ipreviousvalue2 = analogRead(32) * 3.3/4095;
bool Isubida2;
bool Ibajada2;

// 7 - Voltaje Fase 3
unsigned long VcurrentmicrosRMS3;
unsigned long VpreviousmicrosRMS3 = micros();
unsigned long Vtbajada3;
unsigned long Vtsubida3;
unsigned long Vcumulativemicros3;
unsigned long VintervalRMS3;
int Vsignals3 = 0;
double VsamRMS3;
double VavgRMS3;
float Vfrecuencia3;
float Vvaluecuadrado3;
float Vcurrentvalue3;
float Voffset3 = 1.648;
float Vpreviousvalue3 = analogRead(38) * 3.3/4095;
bool Vsubida3;
bool Vbajada3;

// 8 - Corriente Fase 3
unsigned long IcurrentmicrosRMS3;
unsigned long IpreviousmicrosRMS3 = micros();
unsigned long Itbajada3;
unsigned long Itsubida3;
unsigned long Icumulativemicros3;
unsigned long IintervalRMS3;
int Isignals3 = 0;
double IsamRMS3;
double IavgRMS3;
float Ifrecuencia3;
float Ivaluecuadrado3;
float Icurrentvalue3;
float Ioffset3 = 1.31;
float Ipreviousvalue3 = analogRead(33) * 3.3/4095;
bool Isubida3;
bool Ibajada3;

// 9 - Potencia y Frecuencia Fase 1
double acumuladafrecuencia1;            /* Suma de frecuencias para el promedio de 1s */
double promediofrecuencia1;             /* Promedio de frecuencia de 1s               */
float pacumulado1;                      /* Suma del producto de voltaje y corriente i */
int psample1;                           /* Numero de muestras para potencia real      */
float psignal1;                         /* Suma de potencia real para promedio de 1s  */
float pcalculada1;                      /* Potencia real promedio de 1s               */
float PRMS1;                            /* Potencia real promedio de 1s en W          */
float SRMS1;                            /* Potencial aparente promedio de 1s en VAR   */
double FP1;                             /* Factor de potencia: PRMS1/SRMS1            */

// 10 - Potencia y Frecuencia Fase 2
double acumuladafrecuencia2;
double promediofrecuencia2;
float pacumulado2;
int psample2;
float psignal2;
float pcalculada2;
float PRMS2;
float SRMS2;
double FP2;

// 11 - Potencia y Frecuencia Fase 3
double acumuladafrecuencia3;
double promediofrecuencia3;
float pacumulado3;
int psample3;
float psignal3;
float pcalculada3;
float PRMS3;
float SRMS3;
double FP3;

// 12 - Potencia entrada, Potencia de Salida y Eficiencia
float Potencia_salida;    /* Potencia de salida, electrica */
float Potencia_entrada;   /* Potencia de entrada, eolica   */
float Eficiencia;         /* Eficiencia de la turbina      */

//float desfase;
//float FPS;
//float FP;
//const float pi = 3.1416;

// 13 - Variables generales
unsigned long ZerointRMS = VpreviousmicrosRMS1; /* Instante de referencia                       */
long PeriodRMS = 1000000;                       /* Define tiempo para promediar, 1000000us = 1s */

// 14 - Anemometro
unsigned long currentmicros;                    /* Tiempo en que se dio un pulso i   */
unsigned long previousmicros = micros();        /* Tiempo en que se dio un pulso i-1 */
unsigned long zerointerval = previousmicros;    /* Sustituida por ZerointRMS         */
unsigned long interval;                         /* Intervalo entre pulsos            */
unsigned long debounce = 35000;                 /* Ignorar pulsos con intervalos menores a 35000 microsegundos [Rebote] */
double avg_period;                              /* Intervalo promedio entre pulsos   */
long period = 1000000;                          /* Sustituida por PeriodRMS          */
uint32_t reed_rotations = 0;                    /* Inactiva                           */
bool reed_flag;                                 /* Indicador de presencia de un pulso */
// uint64_t current;
// uint64_t previous = rtc_time_get();
// uint64_t cycles_passed;
// double curr_interval = 0;
// double prev_interval = 0;
double avg_time_ms = 0;
double anemo_speed = 0;
double pub_speed = 0;
double prev_speed = 0;
double pub_salida = 0;
double prev_salida = 0;
double pub_eficiencia = 0;
double prev_eficiencia = 0;
bool timer_flag;
int counter = 0;                                /* Contador de pulsos durante 1s  */
bool pub_flag = 1;
bool print_flag = 1;
int pub_secs = 0;                               /* in seconds                     */
int result = 0;
char the_buffer[20] = {'\0'};
char the_buffersalida[20] = {'\0'};
char the_buffereficiencia[20] = {'\0'};
char received_char = '\0';                      /* Define la variable para guardar letras/numeros de comunicación serial */

// 15 - Cliente wifi y servidor
WiFiClient client;                                                                                            /* Crea un cliente con capacidad de conectarse a una IP dada */
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);                        /* Librería de Adafruit, definir un servidor donde se registran los datos, utiliza el client del punto anterior */
Adafruit_MQTT_Publish anemo_data = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/anemo_test");            /* Indicar el feed  donde se van a registrar los datos, usar MQTT del punto anterior                            */
Adafruit_MQTT_Publish eficiencia_data = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/datos_eficiencia"); /* Indicar el feed  donde se van a registrar los datos, usar MQTT del punto anterior                            */
Adafruit_MQTT_Publish salida_data = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/datos_salida");         /* Indicar el feed  donde se van a registrar los datos, usar MQTT del punto anterior                            */

// 16 - Rutinas de interrupcion
void IRAM_ATTR reed_isr() {     /* Definir la rutina de interrupción reed_isr() para asegurarse de capturar los pulsos */
  reed_flag = 1;                /* Indicador afirmativo de la presencia de un pulso                                    */
}

void IRAM_ATTR timer_isr(){
  timer_flag = 1;
}

// 16 - Void setup()
void setup() {
  Serial.begin(230400);                     /* Comunicación serial a 230400 baudrate */
  pinMode(2, INPUT);                        /* Velocidad del viento */
  pinMode(36, INPUT);                       /* Voltaje Fase 1       */
  pinMode(37, INPUT);                       /* Voltaje Fase 2       */
  pinMode(38, INPUT);                       /* Voltaje Fase 3       */
  pinMode(39, INPUT);                       /* Corriente Fase 1     */
  pinMode(32, INPUT);                       /* Corriente Fase 2     */
  pinMode(33, INPUT);                       /* Corriente Fase 3     */
  attachInterrupt(2, &reed_isr, RISING);    /* Define el Pin 2 como un interruptor que se activa ante un flanco positivo y llama a reed_isr() */
  menu();
}

char telemetry[15] = {'\0'};
char telemetrysalida[19] = {'\0'};
char telemetryeficiencia[15] = {'\0'};

void loop() {
// 17 - Comunicacion Serial
  if (Serial.available()) received_char = Serial.read();  /* Si la comuniación serial esta disponible, guardar el caracter recibido */
  if (received_char == 0x1B) menu();                      /* Si se recibe el comando 0x1B [Hexadecimal] llamar a menu()             */

// 18 - Anemometro
  if (reed_flag){                                         /* Si hubo un pulso, continuar con la función */
    reed_flag = 0;                                        /* Reiniciar indicador de pulsos              */
    if (micros() - previousmicros > debounce){            /* Ignorar pulsos producidos por el rebote    */
      counter++;                                          /* Contador de pulsos     */
      currentmicros = micros();                           /* Instante del pulso i   */
      interval += currentmicros - previousmicros;         /* Intervalo entre pulsos */
      previousmicros = currentmicros;                     /* Instante del pulso i-1 */
    }
  }

// 19 - Voltaje instantaneo Fase 1
  Vcurrentvalue1 = analogRead(36) * 3.3/4095;                         /* Valor actual de voltaje */
  Vcurrentvalue1 = Vcurrentvalue1 * 1.0212 + 0.1455 - Voffset1;       /* Valor linealizado       */
  VcurrentmicrosRMS1 = micros();                                      /* Instante de medición i  */
  VintervalRMS1 = VcurrentmicrosRMS1 - VpreviousmicrosRMS1;           /* Intervalo de medicion   */
  VpreviousmicrosRMS1 = VcurrentmicrosRMS1;                           /* Instante de medicion i-1*/
  Vcumulativemicros1 += VintervalRMS1;                                /* Suma de intervalos      */
  Vvaluecuadrado1 += Vcurrentvalue1 * Vcurrentvalue1 * VintervalRMS1; /* Suma de voltaje al cuadrado por intervalo */

// 20 - Corriente instantanea Fase 1
  Icurrentvalue1 = analogRead(39) * 3.3/4095;
  Icurrentvalue1 = Icurrentvalue1 * 1.0212 + 0.1455 - Ioffset1;
  IcurrentmicrosRMS1 = micros();
  IintervalRMS1 = IcurrentmicrosRMS1 - IpreviousmicrosRMS1;
  IpreviousmicrosRMS1 = IcurrentmicrosRMS1;
  Icumulativemicros1 += IintervalRMS1;
  Ivaluecuadrado1 += Icurrentvalue1 * Icurrentvalue1 * IintervalRMS1;

// 21 - Potencia instantanea Fase 1
  pacumulado1 += Vcurrentvalue1 * Icurrentvalue1 * VintervalRMS1;     /* Potencia instantanea por intervalo */
  psample1++;                                                         /* Cantidad de muestras               */

// 22 - Voltaje instantaneo Fase 2
  Vcurrentvalue2 = analogRead(37) * 3.3/4095;
  Vcurrentvalue2 = Vcurrentvalue2 * 1.0212 + 0.1455 - Voffset2;
  VcurrentmicrosRMS2 = micros();
  VintervalRMS2 = VcurrentmicrosRMS2 - VpreviousmicrosRMS2;
  VpreviousmicrosRMS2 = VcurrentmicrosRMS2;
  Vcumulativemicros2 += VintervalRMS2;
  Vvaluecuadrado2 += Vcurrentvalue2 * Vcurrentvalue2 * VintervalRMS2;

// 23 - Corriente instantanea Fase 2
  Icurrentvalue2 = analogRead(32) * 3.3/4095;
  Icurrentvalue2 = Icurrentvalue2 * 1.0212 + 0.1455 - Ioffset2;
  IcurrentmicrosRMS2 = micros();
  IintervalRMS2 = IcurrentmicrosRMS2 - IpreviousmicrosRMS2;
  IpreviousmicrosRMS2 = IcurrentmicrosRMS2;
  Icumulativemicros2 += IintervalRMS2;
  Ivaluecuadrado2 += Icurrentvalue2 * Icurrentvalue2 * IintervalRMS2;

// 24 - Potencia instantanea Fase 2
  pacumulado2 += Vcurrentvalue2 * Icurrentvalue2 * VintervalRMS2;
  psample2++;

// 25 - Voltaje instantaneo Fase 3
  Vcurrentvalue3 = analogRead(38) * 3.3/4095;
  Vcurrentvalue3 = Vcurrentvalue3 * 1.0212 + 0.1455 - Voffset3;
  VcurrentmicrosRMS3 = micros();
  VintervalRMS3 = VcurrentmicrosRMS3 - VpreviousmicrosRMS3;
  VpreviousmicrosRMS3 = VcurrentmicrosRMS3;
  Vcumulativemicros3 += VintervalRMS3;
  Vvaluecuadrado3 += Vcurrentvalue3 * Vcurrentvalue3 * VintervalRMS3;

// 26 - Corriente instantanea Fase 3
  Icurrentvalue3 = analogRead(33) * 3.3/4095;
  Icurrentvalue3 = Icurrentvalue3 * 1.0212 + 0.1455 - Ioffset3;
  IcurrentmicrosRMS3 = micros();
  IintervalRMS3 = IcurrentmicrosRMS3 - IpreviousmicrosRMS3;
  IpreviousmicrosRMS3 = IcurrentmicrosRMS3;
  Icumulativemicros3 += IintervalRMS3;
  Ivaluecuadrado3 += Icurrentvalue3 * Icurrentvalue3 * IintervalRMS3;

// 27 - Potencia instantanea Fase 3
  pacumulado3 += Vcurrentvalue3 * Icurrentvalue3 * VintervalRMS3;
  psample3++;

// 28 - Voltaje RMS Fase 1
  if (Vcurrentvalue1 >= 0 & Vpreviousvalue1 < 0){ /* Cruce por cero de negativo a positivo */
    Vtsubida1 = micros();                         /* Instante del cruce                    */
    if (Vtsubida1 - Vtbajada1 > 2000){            /* Ignorar cruces aparentes por oscilaciones de una medicion */
      Vsubida1 = 1;                               /* Indicador del cruce                   */
    }
  }
  if (Vcurrentvalue1 < 0 & Vpreviousvalue1 >= 0){ /* Cruce por cero de positivo a negativo */
    Vtbajada1 = micros();                         /* Instante del cruce                    */
    if (Vtbajada1 - Vtsubida1 > 2000){            /* Ignorar cruces aparentes por oscilaciones de una medicion */
      Vbajada1 = 1;                               /* Indicador del cruce                   */
    }
  }
  if (Vsubida1 == 1 & Vbajada1 == 1){                             /* Calcular RMS una vez pasado por dos cruces */
    Vfrecuencia1 = (1000000.00 / Vcumulativemicros1);             /* Calcular frecuencia de la onda muestreada  */
    acumuladafrecuencia1 += Vfrecuencia1;                         /* Suma de frecuencias para el promedio       */
    VsamRMS1 += sqrt(Vfrecuencia1 * Vvaluecuadrado1 * 0.000001);  /* Valor RMS                                  */
    Vsignals1++;                                                  /* Cantidad de ondas muestreadas              */
    Vcumulativemicros1 = 0;                                       /* Reiniciar intervalo                        */
    Vvaluecuadrado1 = 0;
    Vsubida1 = 0;                                                 /* Reiniciar indicador de cruce               */
    Vbajada1 = 0;                                                 /* Reiniciar indicador de cruce               */
    psignal1 += pacumulado1 * 0.000001 * Vfrecuencia1;            /* Potencia real de la onda                   */
    pacumulado1 = 0;
    psample1 = 0;
 //   desfase = (Vtsubida1 - Itsubida1) * 0.000001 * 2 * pi * Vfrecuencia1;
 //   FPS += abs(cos (desfase));
  }

// 29 - Corriente RMS Fase 1
  if (Icurrentvalue1 >= 0 & Ipreviousvalue1 < 0){
    Itsubida1 = micros();
    if (Itsubida1 - Itbajada1 > 2000){
      Isubida1 = 1;
    }
  }
  if (Icurrentvalue1 < 0 & Ipreviousvalue1 >= 0){
    Itbajada1 = micros();
    if (Itbajada1 - Itsubida1 > 2000){
      Ibajada1 = 1;
    }
  }
  if (Isubida1 == 1 & Ibajada1 == 1){
    Ifrecuencia1 = (1000000.00 / Icumulativemicros1);
    IsamRMS1 += sqrt(Ifrecuencia1 * Ivaluecuadrado1 * 0.000001);
    Isignals1++;
    Icumulativemicros1 = 0;
    Ivaluecuadrado1 = 0;
    Isubida1 = 0;
    Ibajada1 = 0;
  }

// 30 - Voltaje RMS Fase 2
  if (Vcurrentvalue2 >= 0 & Vpreviousvalue2 < 0){
    Vtsubida2 = micros();
    if (Vtsubida2 - Vtbajada2 > 2000){
      Vsubida2 = 1;
    }
  }
  if (Vcurrentvalue2 < 0 & Vpreviousvalue2 >= 0){
    Vtbajada2 = micros();
    if (Vtbajada2 - Vtsubida2 > 2000){
      Vbajada2 = 1;
    }
  }
  if (Vsubida2 == 1 & Vbajada2 == 1){
    Vfrecuencia2 = (1000000.00 / Vcumulativemicros2);
    acumuladafrecuencia2 += Vfrecuencia2;
    VsamRMS2 += sqrt(Vfrecuencia2 * Vvaluecuadrado2 * 0.000001);
    Vsignals2++;
    Vcumulativemicros2 = 0;
    Vvaluecuadrado2 = 0;
    Vsubida2 = 0;
    Vbajada2 = 0;
    psignal2 += pacumulado2 * 0.000001 * Vfrecuencia2;
    pacumulado2 = 0;
    psample2 = 0;
    //desfase = (Vtsubida - Itsubida) * 0.000001 * 2 * pi * Vfrecuencia;
    //FPS += abs(cos (desfase));
  }

// 31 - Corriente RMS Fase 2
  if (Icurrentvalue2 >= 0 & Ipreviousvalue2 < 0){
    Itsubida2 = micros();
    if (Itsubida2 - Itbajada2 > 2000){
      Isubida2 = 1;
    }
  }
  if (Icurrentvalue2 < 0 & Ipreviousvalue2 >= 0){
    Itbajada2 = micros();
    if (Itbajada2 - Itsubida2 > 2000){
      Ibajada2 = 1;
    }
  }
  if (Isubida2 == 1 & Ibajada2 == 1){
    Ifrecuencia2 = (1000000.00 / Icumulativemicros2);
    IsamRMS2 += sqrt(Ifrecuencia2 * Ivaluecuadrado2 * 0.000001);
    Isignals2++;
    Icumulativemicros2 = 0;
    Ivaluecuadrado2 = 0;
    Isubida2 = 0;
    Ibajada2 = 0;
  }

// 32 - Voltaje RMS Fase 3
  if (Vcurrentvalue3 >= 0 & Vpreviousvalue3 < 0){
    Vtsubida3 = micros();
    if (Vtsubida3 - Vtbajada3 > 2000){
      Vsubida3 = 1;
    }
  }
  if (Vcurrentvalue3 < 0 & Vpreviousvalue3 >= 0){
    Vtbajada3 = micros();
    if (Vtbajada3 - Vtsubida3 > 2000){
      Vbajada3 = 1;
    }
  }
  if (Vsubida3 == 1 & Vbajada3 == 1){
    Vfrecuencia3 = (1000000.00 / Vcumulativemicros3);
    acumuladafrecuencia3 += Vfrecuencia3;
    VsamRMS3 += sqrt(Vfrecuencia3 * Vvaluecuadrado3 * 0.000001);
    Vsignals3++;
    Vcumulativemicros3 = 0;
    Vvaluecuadrado3 = 0;
    Vsubida3 = 0;
    Vbajada3 = 0;
    psignal3 += pacumulado3 * 0.000001 * Vfrecuencia3;
    pacumulado3 = 0;
    psample3 = 0;
    //desfase = (Vtsubida - Itsubida) * 0.000001 * 2 * pi * Vfrecuencia;
    //FPS += abs(cos (desfase));
  }

// 33 - Corriente RMS Fase 3
  if (Icurrentvalue3 >= 0 & Ipreviousvalue3 < 0){
    Itsubida3 = micros();
    if (Itsubida3 - Itbajada3 > 2000){
      Isubida3 = 1;
    }
  }
  if (Icurrentvalue3 < 0 & Ipreviousvalue3 >= 0){
    Itbajada3 = micros();
    if (Itbajada3 - Itsubida3 > 2000){
      Ibajada3 = 1;
    }
  }
  if (Isubida3 == 1 & Ibajada3 == 1){
    Ifrecuencia3 = (1000000.00 / Icumulativemicros3);
    IsamRMS3 += sqrt(Ifrecuencia3 * Ivaluecuadrado3 * 0.000001);
    Isignals3++;
    Icumulativemicros3 = 0;
    Ivaluecuadrado3 = 0;
    Isubida3 = 0;
    Ibajada3 = 0;
  }

// 34 - Actualización de valores i -1
  Vpreviousvalue1 = Vcurrentvalue1;
  Ipreviousvalue1 = Icurrentvalue1;

  Vpreviousvalue2 = Vcurrentvalue2;
  Ipreviousvalue2 = Icurrentvalue2;

  Vpreviousvalue3 = Vcurrentvalue3;
  Ipreviousvalue3 = Icurrentvalue3;

// 35 - Definicion de 1 segundo  
  if (micros()- ZerointRMS >= PeriodRMS){  /* Calculo de Eficiencia con 1 segundo de mediciones */
    ZerointRMS = micros();                 /* Actualizar punto de referencia para 1s            */
    
// 36 - Promedio Voltaje RMS Fase 1
    if (Vsignals1 == 0){
      VavgRMS1 = 0;
    }
    else{
      VavgRMS1 = VsamRMS1 / Vsignals1;
    }

// 37 - Promedio Corriente RMS Fase 1
    if (Isignals1 == 0){
      IavgRMS1 = 0;
    }
    else{
      IavgRMS1 = IsamRMS1 / Isignals1;
    }

// 38 - Promedio Voltaje RMS Fase 2
    if (Vsignals2 == 0){
      VavgRMS2 = 0;
    }
    else{
      VavgRMS2 = VsamRMS2 / Vsignals2;
    }

// 39 - Promedio Corriente RMS Fase 2
    if (Isignals2 == 0){
      IavgRMS2 = 0;
    }
    else{
      IavgRMS2 = IsamRMS2 / Isignals2;
    }

// 40 - Promedio Voltaje RMS Fase 3
    if (Vsignals3 == 0){
      VavgRMS3 = 0;
    }
    else{
      VavgRMS3 = VsamRMS3 / Vsignals3;
    }

    //Promedio Corriente RMS Fase 3
    if (Isignals3 == 0){
      IavgRMS3 = 0;
    }
    else{
      IavgRMS3 = IsamRMS3 / Isignals3;
    }
    
    //FP = FPS/Vsignals1;
    //FPS = 0;

// 41 y 42 - Frecuencia promedio y Potencia Real promedio de cada fase
    if (Vsignals1 == 0){
      promediofrecuencia1 = 0;
      pcalculada1 = 0;
    }
    else{
      promediofrecuencia1 = acumuladafrecuencia1 / Vsignals1;
      pcalculada1 = psignal1 / Vsignals1;
    }
    
    if (Vsignals2 == 0){
      promediofrecuencia2 = 0;
      pcalculada2 = 0;
    }
    else{
      promediofrecuencia2 = acumuladafrecuencia2 / Vsignals2;
      pcalculada2 = psignal2 / Vsignals2;
    }
    
    if (Vsignals3 == 0){
      promediofrecuencia3 = 0;
      pcalculada3 = 0;
    }
    else{
      promediofrecuencia3 = acumuladafrecuencia3 / Vsignals1;
      pcalculada3 = psignal3 / Vsignals3;
    }
    
    acumuladafrecuencia1 = 0;
    acumuladafrecuencia2 = 0;
    acumuladafrecuencia3 = 0;
    psignal1 = 0;
    psignal2 = 0;
    psignal3 = 0;

// 43 - Reinicio de las variables acumuladas
    Vsignals1 = 0;
    VsamRMS1 = 0;
    Vsignals2 = 0;
    VsamRMS2 = 0;
    Vsignals3 = 0;
    VsamRMS3 = 0;

    Isignals1 = 0;
    IsamRMS1 = 0;
    Isignals2 = 0;
    IsamRMS2 = 0;
    Isignals3 = 0;
    IsamRMS3 = 0;

// 44 - Voltaje y Corriente RMS calibrados
    VavgRMS1 = VavgRMS1 * 122.0/0.241;
    IavgRMS1 = IavgRMS1 * 50/1;
    VavgRMS2 = VavgRMS2 * 122.3/0.242;
    IavgRMS2 = IavgRMS2 * 50/1;
    VavgRMS3 = VavgRMS3 * 122.5/0.243;
    IavgRMS3 = IavgRMS3 * 50/1;

// 45 - Potencia Real [W], Potencia Aparente [VAR] y Factor de Potencia de cada fase
    PRMS1 = pcalculada1 * 122.0/0.241 * 50;
    SRMS1 = VavgRMS1 * IavgRMS1;   
    FP1 = PRMS1/SRMS1;

    PRMS2 = pcalculada2 * 122.3/0.242 * 50;
    SRMS2 = VavgRMS2 * IavgRMS2;   
    FP2 = PRMS2/SRMS2;

    PRMS3 = pcalculada3 * 122.5/0.243 * 50;
    SRMS3 = VavgRMS3 * IavgRMS3;   
    FP3 = PRMS3/SRMS3;

/*
    //Imprimir Voltaje y Corriente RMS
    Serial.print("Voltaje RMS:");
    Serial.println(VavgRMS1,3);
    Serial.print("Voltaje RMS:");
    Serial.println(VavgRMS2,3);
    Serial.print("Voltaje RMS:");
    Serial.println(VavgRMS3,3);
    Serial.print("Corriente RMS:");
    Serial.println(IavgRMS1,3);
    Serial.print("Corriente RMS:");
    Serial.println(IavgRMS2,3);
    Serial.print("Corriente RMS:");
    Serial.println(IavgRMS3,3);
*/
/*
    //Imprimir Frecuencia de cada Fase
    Serial.print("Frecuencia 1:");
    Serial.println(promediofrecuencia1);
    Serial.print("Frecuencia 2:");
    Serial.println(promediofrecuencia2);
    Serial.print("Frecuencia 3:");
    Serial.println(promediofrecuencia3);
*/
/*
    //Imprimir Potencia Real y Aparente de cada fase
    Serial.print("Potencial Real 1:");
    Serial.println(PRMS1,4);
    Serial.print("Potencial Aparente 1:");
    Serial.println(SRMS1,4);
    Serial.print("Potencial Real 2:");
    Serial.println(PRMS2,4);
    Serial.print("Potencial Aparente 2:");
    Serial.println(SRMS2,4);  
    Serial.print("Potencial Real 3:");
    Serial.println(PRMS3,4);
    Serial.print("Potencial Aparente 3:");
    Serial.println(SRMS3,4);    
 */

// 46 - Velocidad del viento
      if (counter == 0){                              /* Si no existen pulsos, indica cero como velocidad */
        anemo_speed = 0;
      }else{
        if (counter == 1 & interval > 2500000){       /* below 2500cycles, ie below 0.45m/s report zero   */
          interval = 0; 
          anemo_speed = 0;       
        }else{                                        /* Calculo de velocidad                             */
          avg_period = (interval / counter)*0.001;    /* Periodo promedio de los pulsos                   */
          anemo_speed = (1000/avg_period)*2.5*0.44704;/* Velocidad calculada según fabricante             */
          interval = 0;     
        }        
        counter = 0;                                  /* Reiniciar el contador de pulsos                  */
      }

// 47 - Eficiencia
    Potencia_salida = PRMS1 + PRMS2 + PRMS3;                                           /* Potencia electrica trifasica */
    Potencia_entrada = (1.1 * 1.767 * anemo_speed * anemo_speed * anemo_speed) / 2;    /* Recurso eolico disponible    */
    if (Potencia_entrada == 0){
      Eficiencia = 0;
    }
    else{
      Eficiencia = 100 * (Potencia_salida / Potencia_entrada);                         /* Definicion de eficiencia      */
    }

// 48 - Publicacion de datos
    sprintf(telemetry, "S%05.2fN", anemo_speed);      /* Create telemetry string before publising in MTTQ, % es sustituido por anemo_speed o el valor correspondiente        */
    /* The string has the S as firsts character, then 5 spaces for the velocity including two before the dot and two after the dot, then the status of the MTTQ transmission */
    sprintf(telemetrysalida, "T%07.2fN", Potencia_salida);  
    sprintf(telemetryeficiencia, "E%05.2fN", Eficiencia);     
    strcpy(the_buffer, "00.00");                      /* Copia el string de la posición 2 en la posición 1                                                                   */
    strcpy(the_buffersalida, "0000.00"); 
    strcpy(the_buffereficiencia, "00.00"); 
    if(pub_flag){                                     /* Por defecto va a publicar, en menu() puede escogerse que publique o que no                                          */
      pub_secs++;                                     /* Contador para definir si se publica o no                                                                            */
      pub_speed += anemo_speed;
      pub_salida += Potencia_salida;
      pub_eficiencia += Eficiencia;
      if (pub_secs == PUB_INTERVAL){                  /* Solo publica en el intervalo predefinido                                                                            */
        pub_speed /= pub_secs;
        pub_salida /= pub_secs;
        pub_eficiencia /= pub_secs;
        pub_secs = 0;       
        MQTT_connect();
        
        if(pub_speed != prev_speed){ //only publish if the value is different from the previous
          if (! anemo_data.publish(pub_speed)) {
            telemetry[6] = 'E';
            strcpy(the_buffer, "00.00");
          }else{
            telemetry[6] = 'S';
            sprintf(the_buffer, "%05.2f", pub_speed);
          }
          prev_speed = pub_speed;
          pub_speed = 0;
        }

        if(pub_salida != prev_salida){ //only publish if the value is different from the previous
          if (! salida_data.publish(pub_salida)) {
            telemetrysalida[8] = 'E';
            strcpy(the_buffersalida, "0000.00");
          }else{
            telemetrysalida[8] = 'S';
            sprintf(the_buffersalida, "%07.2f", pub_salida);
          }
          prev_salida = pub_salida;
          pub_salida = 0;
        }

        if(pub_eficiencia != prev_eficiencia){ //only publish if the value is different from the previous
          if (! eficiencia_data.publish(pub_eficiencia)) {
            telemetryeficiencia[6] = 'E';
            strcpy(the_buffereficiencia, "00.00");
          }else{
            telemetryeficiencia[6] = 'S';
            sprintf(the_buffereficiencia, "%05.2f", pub_eficiencia);
          }
          prev_eficiencia = pub_eficiencia;
          pub_eficiencia = 0;
        }
      }else{
        strcpy(the_buffer, "00.00");  
        strcpy(the_buffersalida, "0000.00");  
        strcpy(the_buffereficiencia, "00.00");                  /* Copia el string de la posición 2 en la posición 1       */
      }
      
    }    
    strcat(telemetry, the_buffer);                    /* Concatenar telemetry y the_buffer: Combinar en uno solo */
    strcat(telemetrysalida, the_buffersalida); 
    strcat(telemetryeficiencia, the_buffereficiencia); 
    telemetry[12] = '\r';                             /* New line */
    telemetry[13] = '\n';                             /* New line */
    telemetrysalida[16] = '\r';                       /* New line */
    telemetrysalida[17] = '\n';                       /* New line */
    telemetryeficiencia[12] = '\r';                   /* New line */
    telemetryeficiencia[13] = '\n';                   /* New line */
    if (print_flag){
      Serial.print(telemetry);          /* Imprimir telemetry ya concatenado en el puerto Serial   */
      Serial.print(telemetrysalida);
      Serial.print(telemetryeficiencia);    
    }
  }
}

// 49 - Conexion MQTT
void MQTT_connect(){
  int8_t ret;
  if (mqtt.connected()){
    return;
  }
  Serial.print("Connecting to MQTT...");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0){
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0){
      while (1);
    }    
  }
  Serial.println("MQTT Connected!");
}

// 50 - Menu
void menu(){
  Serial.setTimeout(10000);
  Serial.println("Do you want to publish on Adafruit.io (y/n)?");
  while (!(received_char == 0x79 | received_char == 0x6E)){
    if (Serial.available()) received_char = Serial.read();
  }
  if (received_char == 0x79){ //if a "y" is received
    pub_flag = 1;
    Serial.print("Publish interval: ");
    Serial.print(PUB_INTERVAL);
    Serial.println(" seconds");
    Serial.print("Connecting to... ");
    Serial.println(WLAN_SSID);
    WiFi.begin(WLAN_SSID, WLAN_PASS);  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: "); 
    Serial.println(WiFi.localIP());  
  }else{
    pub_flag = 0;
  }
}

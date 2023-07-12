#include <Arduino.h>
#include <CytronMotorDriver.h>
#include <SharpIR.h>

// Conexiones
#define infra_diagonal_der 10 // Sensor infrarrojo delantero derecho
#define infra_diagonal_izq 11 // Sensor infrarrojo delantero izquierdo
//#define infra_frontal 12 // Sensor infrarrojo frontal
#define infra_lateral_der 12 // Sensor lateral derecho
#define infra_lateral_izq 13 // Sensor lateral izquierdo

//#define infra A2 // Sensor analogico frontal
//#define model 1080
SharpIR sensor( SharpIR::GP2Y0A21YK0F, A2 );
//int distance = sensor.getDistance();
int distance;

#define pulsador A3  // Botón de arranque
int sw; // Valor de voltaje de cada boton
/*
sw1 = Encendido y estrategia 1  ; Voltaje = 0
sw2 = Encendido y estrategia 2  ; Voltaje = 142
sw3 = Encendido y estrategia 3  ; Voltaje = 302
sw4 = Encendido y estrategia 4  ; Voltaje = 490
sw5 = Apagado                   ; Voltaje = 737
*/
bool encendido = false;

#define linea_frontal_izq A0 // Sensor de linea delantero izquierdo
#define linea_frontal_der A1 // Sensor de linea delantero derecho
//#define linea_trasero A2 // Sensor de linea trasero central

#define scl_com A5 // pin para establecer comunicacion giroscopio-placa
#define sda_com A4 // pin para establecer comunicacion giroscopio-placa

// Direcciones.
#define LEFT  0 // Constante para la direccion Izquierda
#define RIGHT 1 // Constante para la direccion Derecha

// Variables Globales.
uint8_t searchDir = LEFT; //inicialmente buscará en la dirección izquierda

// Configuracion del driver del motor.
/************************************************************************************************ 
 * CytronMD10C(uint8_t pwmPin, uint8_t dirPin) : CytronMD(PWM_DIR, pwmPin, dirPin)
Modo: "de magnitud de signo" -> Necesita 2 señales separadas para controlar el motor, una es para la 
dirección (en sentido contrario a las agujas del reloj o en el sentido de las agujas del reloj)
y otra es para la velocidad.
*************************************************************************************************/
CytronMD motorL(PWM_DIR, 5, 4);  // Motor IZQUIERDO
CytronMD motorR(PWM_DIR, 6, 7);  // Motor DERECHO 

/*******************************************************************************
 * Funcion de Retardo de tiempo (para no usar el comando Delay)
 *******************************************************************************/
void Retardo(unsigned long tiempo) {
  unsigned long TiempoActual = millis();
  while (millis() - TiempoActual < tiempo) {
    // no hacemos nada, solo esperamos
  }
}

/*******************************************************************************
 * Funcion de Inicio (para elegir alguna de las 4 estrategias)
 *******************************************************************************/
bool inicio()
{
  if (sw < 10)
  {
    Retardo(5000);
    Serial.println("Encendido y estrategia 1");
    Serial.println();
    return encendido = true;
  }


    else if (sw > 132 and sw < 152)
  {
    Retardo(5000);
    Serial.println("Encendido y estrategia 2");
    Serial.println();
    return encendido = true;
  }


    else if (sw > 292 and sw < 312)
  {
    Retardo(5000);
    Serial.println("Encendido y estrategia 3");
    Serial.println();
    return encendido = true;
  }


    else if (sw > 480 and sw < 500)
  {
    Retardo(5000);
    Serial.println("Encendido y estrategia 4");
    Serial.println();
    return encendido = true;
  }

}


/*******************************************************************************
 * Funcion de Parada (para apagar los motores y detener las lecturas de los sensores)
 *******************************************************************************/
bool parada()
{
    // Valor tipico sw = 737
    if (sw > 727 and sw < 747)
  {
    Serial.println("Apagado");
    Serial.println();
    motorL.setSpeed(0);
    motorR.setSpeed(0);
    return encendido = false;
  }
}

/*******************************************************************************
 * Funcion de Estrategias de acuerdo a la posicion inicial del sumo
 *******************************************************************************/
void estrategias()
{
  if (encendido == true)
  {
    // Valor tipico sw = 0
    if (sw < 10) // Estrategia 1 (Frente a frente)
    {
      // Ir recto
      motorL.setSpeed(255);
      motorR.setSpeed(255);
      Serial.println("Avance Recto");
      Retardo(450);
      // Gire a la izquierda hasta que se detecte al oponente.
      motorL.setSpeed(0);
      motorR.setSpeed(255);
      Serial.println("Giro a la izquierda");
      uint32_t startTimestamp = millis();
      while (distance > 50) {
        // Salir si el oponente no se encuentra después del tiempo de espera.
        if (millis() - startTimestamp > 400) {
          break;
            }
          }
      }
    
    // Valor tipico sw = 142
    if (sw > 132 and sw < 152) // Estrategia 2 (de frente, pero con ataque curvilineo)
    {
    // Gire a la derecha alrededor de 45 grados.
    Serial.println("Giro a la derecha 45 grados");
    motorL.setSpeed(255);
    motorR.setSpeed(0);
    Retardo(180);
    // Ir recto
    Serial.println("Avance Recto");
    motorL.setSpeed(255);
    motorR.setSpeed(255);
    //delay(450);
    Retardo(450);

    // Gire a la izquierda hasta que se detecte al oponente.
    Serial.println("Gire a la izquierda hasta que se detecte al oponente");
    motorL.setSpeed(0);
    motorR.setSpeed(255);
    uint32_t startTimestamp = millis();
    while (distance > 50) {
    // Salir si el oponente no se encuentra después del tiempo de espera.
    if (millis() - startTimestamp > 400) {
      break;
        }
      }
    }

    // Valor tipico sw = 302
    if (sw > 292 and sw < 312) // Estrategia 3 (Costado derecho del sumo hacia el centro del ring)
    {
      // Girar 
      motorL.setSpeed(-150);
      motorR.setSpeed(150);
      Retardo(100);
      // Ir recto 
      motorL.setSpeed(255);
      motorR.setSpeed(255);
      uint32_t startTimestamp = millis();
      while (distance > 50) {
        // Salir si el oponente no se encuentra después del tiempo de espera.
        if (millis() - startTimestamp > 400) {
          break;
            }
          }
    }

    // Valor tipico sw = 490
    if (sw > 480 and sw < 500) // Estrategia 4 (Costado izquierdo del sumo hacia el centro del ring)
    {
      // Girar 
      motorL.setSpeed(150);
      motorR.setSpeed(-150);
      Retardo(100);
      // Ir recto 
      motorL.setSpeed(255);
      motorR.setSpeed(255);
      uint32_t startTimestamp = millis();
      while (distance > 50) {
        // Salir si el oponente no se encuentra después del tiempo de espera.
        if (millis() - startTimestamp > 400) {
          break;
            }
          }
    }


}
}


/*******************************************************************************
 * Rutina de inicio
 * Esta función debe llamarse una vez solo cuando comience el juego.
 *******************************************************************************/
void RutinaInicial() {
 // Retardo al inicio
 if (encendido == true)
 {
  Retardo(5000);

  // Gire a la derecha alrededor de 45 grados.
  motorL.setSpeed(255);
  motorR.setSpeed(0);
  //delay(180);
  Retardo(180);

  // Ir recto
  motorL.setSpeed(255);
  motorR.setSpeed(255);
  //delay(450);
  Retardo(450);

  // Gire a la izquierda hasta que se detecte al oponente.
  motorL.setSpeed(0);
  motorR.setSpeed(255);
  uint32_t startTimestamp = millis();
  while (distance > 50) {
    // Salir si el oponente no se encuentra después del tiempo de espera.
    if (millis() - startTimestamp > 400) {
      break;
    }
  }
  
 }
}

/*******************************************************************************
 * Retroceder
 * Esta función debe llamarse cuando se detecta el borde del anillo.
 *******************************************************************************/
void Retroceder(uint8_t dir) {

  if (encendido == true)
  {
    Serial.println("Retrocede");
  // Parar los motores.
  motorL.setSpeed(0);
  motorR.setSpeed(0);
  //delay(100);
  Retardo(100);

  // Reversa.
  motorL.setSpeed(-255);
  motorR.setSpeed(-255);
  //delay(200);
  Retardo(200);

  // Parar los motores.
  motorL.setSpeed(0);
  motorR.setSpeed(0);
  //delay(100);
  Retardo(100);

  // Girar.
  if (dir == LEFT) {
    Serial.println("Giro a la izquierda");
    motorL.setSpeed(-150);
    motorR.setSpeed(150);
  } else {
    Serial.println("Giro a la derecha");
    motorL.setSpeed(150);
    motorR.setSpeed(-150);
  }
  //delay(100);
  Retardo(100);

  // Empieza a buscar oponente.
  // Tiempo de espera después de un período corto.
  uint32_t uTurnTimestamp = millis();
  while (millis() - uTurnTimestamp < 300) {
    // El oponente se detecta si se activa cualquiera de los sensores del oponente.
    if ( (distance < 50) ||
         !digitalRead(infra_diagonal_der) ||
         !digitalRead(infra_diagonal_izq) ||
         !digitalRead(infra_lateral_der) ||
         !digitalRead(infra_lateral_izq ) ){
          
      // Para los motores.
      motorL.setSpeed(0);
      motorR.setSpeed(0);
      //delay(100);
      Retardo(100);

      // Regrese al bucle principal(main loop) y ejecute el programa de conexión.
      return;
    }
  }

  // Si no encuentra al oponente, avance y continúe buscando en el bucle principal.
  motorL.setSpeed(255);
  motorR.setSpeed(255);
  //delay(200);
  Retardo(200);
}
}

/*******************************************************************************
 * Buscar
 *******************************************************************************/
void buscar() {
  if (encendido == true)
  {
    Serial.println("busca");
  // Mover en movimiento circular.
  if (searchDir == LEFT) {
    motorL.setSpeed(100);
    motorR.setSpeed(255);
  } else {
    motorL.setSpeed(255);
    motorR.setSpeed(100);
  }
}
}

/*******************************************************************************
 * Atacar
 * Rastrea y ataca al oponente a toda velocidad.
 * No hacer nada si el oponente no se encuentra.
 *******************************************************************************/
void atacar() {
  if (encendido == true)
  {
    Serial.println("ataca");
  uint32_t attackTimestamp = millis();

  // Oponente en el centro delantero.
  // Ir recto a toda velocidad.
  /*if (digitalRead(infra_frontal)) {
    motorL.setSpeed(255);
    motorR.setSpeed(255);
  } */
  if (distance < 50)
  {
    motorL.setSpeed(255);
    motorR.setSpeed(255);
  }
  // Oponente en frente a la izquierda.
  // Gire a la izquierda.
  else if (digitalRead(infra_diagonal_izq)) {
    motorL.setSpeed(0);
    motorR.setSpeed(255);
  }

  // Oponente en frente a la derecha.
  // Gire a la derecha.
  else if (digitalRead(infra_diagonal_der)) {
    motorL.setSpeed(255);
    motorR.setSpeed(0);
  }

  // Oponente en el lado izquierdo.
  // Gira a la izquierda hasta que el oponente esté delante.
  else if (digitalRead(infra_lateral_izq)) {
    motorL.setSpeed(-150);
    motorR.setSpeed(150);
    while (distance > 50) {
      // Abandonar si no se encuentra al oponente después de un tiempo de espera.
      if (millis() - attackTimestamp > 400) {
        break;
      }
    }
  }

  // Oponente en el lado derecho.
  // Gira a la derecha hasta que el oponente esté delante.
  else if (digitalRead(infra_lateral_der) ) {
    motorL.setSpeed(150);
    motorR.setSpeed(-150);
    while (distance > 50) {
      // Abandonar si no se encuentra al oponente después de un tiempo de espera.
      if (millis() - attackTimestamp > 400) {
        break;
      }
    }
  }
  
}
}

void sensores() {
  if (encendido == true)
  {
if (analogRead(linea_frontal_izq) < 200) {
    // Retrocede y gira en U a la derecha..
    Serial.println("Retrocede hacia la derecha");
    Retroceder(RIGHT);

    // Cambiar la dirección de búsqueda.
    searchDir ^= 1;
  }
  
  // Se detecta el borde a la derecha.
  else if (analogRead(linea_frontal_der) < 200) {
    // Retrocede y gira en U a la izquierda.
    Serial.println("Retrocede hacia la izquierda");
    Retroceder(LEFT);
    
    // Cambiar la dirección de búsqueda.
    searchDir ^= 1;
  }

  // Se detecta el borde trasero
  /*else if (analogRead(linea_trasero) < 200) {
    motorL.setSpeed(122);
    motorR.setSpeed(122);
    
    // Cambiar la dirección de búsqueda.
    searchDir ^= 1;
  }*/

  
  // No se detecta el borde.
  else {
    // Seguir buscando si no se detecta al oponente.
    if ( (distance > 50) &&
         !digitalRead(infra_diagonal_der) &&
         !digitalRead(infra_diagonal_izq) &&
         !digitalRead(infra_lateral_der) &&
         !digitalRead(infra_lateral_izq) ) {
      buscar();
    }
    
    // Atacar si el oponente está a la vista.
    else {
      atacar();
    }
  }
}
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //pinMode(linea_frontal_izq, INPUT_PULLUP);
  //pinMode(linea_frontal_der, INPUT_PULLUP);
  //pinMode(linea_trasero, INPUT_PULLUP);

  pinMode(infra_diagonal_izq, INPUT_PULLUP);
  pinMode(infra_diagonal_der, INPUT_PULLUP);
  //pinMode(infra, INPUT_PULLUP);
  pinMode(infra_lateral_izq, INPUT_PULLUP);
  pinMode(infra_lateral_der, INPUT_PULLUP);

  // Para los motores.
  //motorL.setSpeed(0);
  //motorR.setSpeed(0);

 // Al pulsar cualquiera de los 4 botones de estrategia, comienza la puesta en marcha despues que transcurra 5 segundos
  /*
  sw = analogRead(pulsador);

  inicio();

  if (inicio)
  {
    estrategias();
  }
  // Al pulsar el boton de parada, paran los motores y las lecturas de los sensores
  parada();
  */
}


void loop() {
  // put your main code here, to run repeatedly:
  sw = analogRead(pulsador);
  distance = sensor.getDistance();
  inicio();
  if (inicio)
  {
    estrategias();
    sensores();
  }
  // Al pulsar el boton de parada, se detienen los motores
  parada();

  
    // Bucle para siempre aquí.
    //while (1);
  }


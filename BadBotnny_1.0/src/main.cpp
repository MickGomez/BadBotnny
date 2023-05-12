#include <Arduino.h>
#include <CytronMotorDriver.h>

// Conexiones
#define infra_diagonal_der 10 // Sensor infrarrojo delantero derecho
#define infra_diagonal_izq 11 // Sensor infrarrojo delantero izquierdo
#define infra_frontal 12 // Sensor infrarrojo frontal
#define infra_lateral_der 13 // Sensor lateral derecho
#define infra_lateral_izq 14 // Sensor lateral izquierdo

#define Boton 13  // Botón de arranque

#define linea_frontal_izq A0 // Sensor de linea delantero izquierdo
#define linea_frontal_der A1 // Sensor de linea delantero derecho
#define linea_trasero_izq A2 // Sensor de linea trasero izquierdo
#define linea_trasero_der A3 // Sensor de linea trasero derecho

#define scl_com A5 // pn para establacer comunicacion giroscopio-placa
#define sda_com A4 // pin para establacer comunicacion giroscopio-placa

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
 * Rutina de inicio
 * Esta función debe llamarse una vez solo cuando comience el juego.
 *******************************************************************************/
void inicio() {
 // Retardo al inicio
  delay(5000);

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
  while (digitalRead(infra_frontal)) {
    // Salir si el oponente no se encuentra después del tiempo de espera.
    if (millis() - startTimestamp > 400) {
      break;
    }
  }
  
 }


/*******************************************************************************
 * Retroceder
 * Esta función debe llamarse cuando se detecta el borde del anillo.
 *******************************************************************************/
void Retroceder(uint8_t dir) {
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
    motorL.setSpeed(-150);
    motorR.setSpeed(150);
  } else {
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
    if ( !digitalRead(infra_frontal) ||
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


/*******************************************************************************
 * Buscar
 *******************************************************************************/
void buscar() {
  // Mover en movimiento circular.
  if (searchDir == LEFT) {
    motorL.setSpeed(100);
    motorR.setSpeed(255);
  } else {
    motorL.setSpeed(255);
    motorR.setSpeed(100);
  }
}


/*******************************************************************************
 * Atacar
 * Rastrea y ataca al oponente a toda velocidad.
 * No hacer nada si el oponente no se encuentra.
 *******************************************************************************/
void atacar() {
  uint32_t attackTimestamp = millis();

  // Oponente en el centro delantero.
  // Ir recto a toda velocidad.
  if (digitalRead(infra_frontal)) {
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
    while (!digitalRead(infra_frontal)) {
      // Abandonar si no se encuentra al oponente después de un tiempo de espera.
      if (millis() - attackTimestamp > 400) {
        break;
      }
    }
  }

  // Oponente en el lado derecho.
  // Gira a la derecha hasta que el oponente esté delante.
  else if (digitalRead(infra_lateral_der) == 0) {
    motorL.setSpeed(150);
    motorR.setSpeed(-150);
    while (!digitalRead(infra_frontal)) {
      // Abandonar si no se encuentra al oponente después de un tiempo de espera.
      if (millis() - attackTimestamp > 400) {
        break;
      }
    }
  }
  
}




void setup() {
  // put your setup code here, to run once:

  pinMode(linea_frontal_izq, INPUT_PULLUP);
  pinMode(linea_frontal_der, INPUT_PULLUP);
  pinMode(linea_trasero_izq, INPUT_PULLUP);
  pinMode(linea_trasero_der, INPUT_PULLUP);

  pinMode(infra_diagonal_izq, INPUT_PULLUP);
  pinMode(infra_diagonal_der, INPUT_PULLUP);
  pinMode(infra_frontal, INPUT_PULLUP);
  pinMode(infra_lateral_izq, INPUT_PULLUP);
  pinMode(infra_lateral_der, INPUT_PULLUP);

  // Para los motores.
  motorL.setSpeed(0);
  motorR.setSpeed(0);

 // Espere hasta que se suelte el botón.
  while (!digitalRead(Boton));

    // Empieza la rutina de inicio.
  inicio();
}


void loop() {
  // put your main code here, to run repeatedly:

  // Se detecta el borde izquierdo.
  if (analogRead(linea_frontal_izq) < 500) {
    // Retrocede y gira en U a la derecha..
    Retroceder(RIGHT);

    // Cambiar la dirección de búsqueda.
    searchDir ^= 1;
  }
  
  // Se detecta el borde a la derecha.
  else if (analogRead(linea_frontal_der) < 500) {
    // Retrocede y gira en U a la izquierda.
    Retroceder(LEFT);
    
    // Cambiar la dirección de búsqueda.
    searchDir ^= 1;
  }

  // Se detecta el borde trasero a la derecha.
  else if (analogRead(linea_trasero_der) < 500) {
    motorL.setSpeed(122);
    motorR.setSpeed(122);
    
    // Cambiar la dirección de búsqueda.
    searchDir ^= 1;
  }

  // Se detecta el borde trasero a la izquierda.
  else if (analogRead(linea_trasero_der) < 500) {
    motorL.setSpeed(122);
    motorR.setSpeed(122);
    
    // Cambiar la dirección de búsqueda.
    searchDir ^= 1;
  }

  
  // No se detecta el borde.
  else {
    // Seguir buscando si no se detecta al oponente.
    if ( !digitalRead(infra_frontal) &&
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

  // Detener el robot si se pulsa el botón.
  if (!digitalRead(Boton)) {
    // Para los motores.
    motorL.setSpeed(0);
    motorR.setSpeed(0);

    // Bucle para siempre aquí.
    while (1);
  }
}

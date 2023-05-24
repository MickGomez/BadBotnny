const int pulsador = A2;
const int sensor = 3; //Conecte el pin de señal(Negro) al pin digital 3
bool encendido = false;
int sw;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  pinMode(sensor,INPUT);
  
  sw = analogRead(pulsador);
  inicio();

  if(inicio)
  {
    oponente();
  }
  parada();
}

bool inicio()
{
  if (sw == 0)
  {
    delay(5000);
    Serial.println("Encendido y estrategia 1");
    return encendido = true;
  }

    if (sw == 142)
  {
    delay(5000);
    Serial.println("Encendido y estrategia 2");
    return encendido = true;
  }

    if (sw == 302)
  {
    delay(5000);
    Serial.println("Encendido y estrategia 3");
    return encendido = true;
  }

    if (sw == 490)
  {
    delay(5000);
    Serial.println("Encendido y estrategia 4");
    return encendido = true;
  }
}

void oponente()
{
  int valor = digitalRead(sensor);
  if (encendido == true) 
    {
    if(!digitalRead(sensor))  {
      Serial.println("Objeto Detectado");
    }
    else  {
      Serial.println("No hay Nada");
    }
    delay(500);
  }
}

bool parada()
{
    if (sw == 737)
  {
    Serial.println("Apagado");
    return encendido = false;
  }
}

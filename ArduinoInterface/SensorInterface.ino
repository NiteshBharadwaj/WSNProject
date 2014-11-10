int vPin = A1;
int iPin = A2;

boolean start = true;
int startT = 0;

int noO = 0;
float vsqr = 0;
float isqr = 0;
float vi = 0;

void setup() {
  pinMode(vPin,INPUT);
  pinMode(iPin, INPUT);
  Serial.begin(9600);
}


void loop() {
  if (start) {
    start = false;
    startT = millis();
  }
  else {
    if ((millis()-startT)<1000) {
      float v = ((analogRead(vPin)/1023.0)-3.3)*100;
      float i = ((analogRead(iPin)/1023.0)-3.3)*100;
      vsqr = vsqr + v*v;
      isqr = isqr + i*i;
      vi = vi + v*i;
      noO = noO + 1;
    }   
    else {
      start = true;
      float vRms = vsqr/noO;
      float iRms = isqr/noO;
      float power = vi/noO; 
      Serial.print(vRms);
      Serial.print(' ');
      Serial.print(iRms);
      Serial.print(' ');
      Serial.println(power);
      noO = 0;
      isqr = 0;
      vsqr = 0;
      vi = 0;     
    }
  }
}

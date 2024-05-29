const int dirPin = 5;
const int stepPin = 2;
const int dirPin2 = 6;
const int stepPin2 = 3;
int pos_kar = 0;

const int en = 8;

void setup()
{
pinMode(stepPin, OUTPUT);
pinMode(dirPin, OUTPUT);
pinMode(stepPin2, OUTPUT);
pinMode(dirPin2, OUTPUT);
pinMode(en, OUTPUT);
Serial.begin(9600);
}


void step_mot1(int step1, int dir ){
    digitalWrite(en, LOW);
    digitalWrite(dirPin, dir); // Установка вращения по часовой стрелки
    for(int x = 0; x < step1; x++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(1000);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(1000);
    } 
    digitalWrite(en, HIGH);
}

void step_mot2(int step1, int dir ){
  digitalWrite(en, LOW);
    //1032 поный оборот 8 лепистков
  digitalWrite(dirPin2, dir); // Установка вращения по часовой стрелки
  for(int x = 0; x < step1; x++)
    {
      digitalWrite(stepPin2, HIGH);
      delayMicroseconds(5000);
      digitalWrite(stepPin2, LOW);
      delayMicroseconds(5000);
    }
  digitalWrite(en, HIGH);
}


String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop()
{

  if(Serial.available()>0){
    String myString = Serial.readString();
    String xval = getValue(myString, ' ', 0);
    String yval = getValue(myString, ' ', 1);
    String zval = getValue(myString, ' ', 2);
    if (xval=="K"){
      Serial.print(yval);
      Serial.println(" Karuselka");
      step_mot2(yval.toInt(),1);    
    }
  
    if (xval=="P"){
      Serial.print(yval);
      Serial.println(" Podiemnik");
      step_mot1(yval.toInt(),zval.toInt());    
    }
        if (xval=="C"){
      
      Serial.println(" Start pogr");
      step_mot1(20000,1);    
      delay(10000);      
      step_mot1(20000,0);    
      delay(1000);      
      step_mot2(516,1);    
      delay(1000);      
      step_mot1(20000,1);    
      delay(10000);      
      step_mot1(20000,0);    

    }

 }

}

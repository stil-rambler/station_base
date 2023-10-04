

String val;
String stat;
String  err1;


void setup() {
  Serial.begin(9600);
  Serial.println("Hello!");
err1 = "0";
stat = "ok";
  
}
void loop() {
    if (Serial.available() > 0) {

       val = Serial.readString(); // переменная val равна полученной команде

       if (val == "ST") {
          Serial.println("Version 0.1");
          Serial.println("Status " + stat);
          Serial.println("Error " + err1);
       }
       
          Serial.println(val);
          Serial.println("ok");
      
       

   }
  delay(1);
}

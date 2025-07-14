
#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 3);  // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(LED_BUILTIN, OUTPUT);


  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.println("BRCMD+SEND=Hello, world!>48:E7:29:6E:8B:16\n");
}

void loop() {  // run over and over
  if (mySerial.available()) {
    String buff = mySerial.readString();
    buff.trim();
    Serial.println(buff);
    String cmd = buff.substring(buff.indexOf(" ") + 1, buff.indexOf("\n"));
    cmd.trim();
    //Serial.println(cmd);
    if (cmd.startsWith("LEDON")) {

      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("LED On!");
      mySerial.println("BRCMD+SEND=LED On!>48:E7:29:6E:8B:16\n");
    }
    if (cmd.startsWith("LEDOFF")) {

      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("LED Off!");
      mySerial.println("BRCMD+SEND=LED Off!>48:E7:29:6E:8B:16\n");
    }
  }
}

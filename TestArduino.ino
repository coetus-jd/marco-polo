void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
}

void loop() {
  if (Serial.available()) {
    String inputString = Serial.readString();
    Serial.println(inputString);
  }
}
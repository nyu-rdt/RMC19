#define start_measure 2.75
#define end_measure 53.5
#define start_value 7
#define end_value 986
#define sample_size 50

float slope = (end_measure - start_measure) / (end_value - start_value);
float intercept = end_measure - (end_value*slope);

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(9600);
}

void loop() {
  int sample;
  float average = 0;
  for (int i = 0; i < sample_size; i++) {
    sample = analogRead(A0);
    average += sample;
    delay(5);
  }
  Serial.print(analogRead(A0));
  Serial.print("   ");
  average = average / sample_size;
  Serial.println((average * slope) + intercept);
}

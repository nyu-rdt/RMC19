#define READ_IN 0
#define PARSE_IN 1
#define EXEC_IN 2
#define WRITE_IN 3

int oPhase = 0;

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (oPhase == 0) {
    readPhase();
    oPhase = PARSE_IN;
  }
  else if (oPhase == 1) {
    parsePhase();
    oPhase = EXEC_IN;
  }
  else if (oPhase == 2) {
    execPhase();
    oPhase = WRITE_IN;
  }
  else if (oPhase == 3) {
    writePhase();
    oPhase = READ_IN;
  }
}

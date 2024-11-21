#define LED         2
#define BUTTON      0
#define ANALOGPIN   0

int adcValue = 0;

void setup()
{
    // put your setup code here, to run once:
    delay(1000);
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    //pinMode(BUTTON, INPUT_PULLUP);
}

void loop()
{
    // put your main code here, to run repeatedly:
    adcValue = analogRead(ANALOGPIN);
    Serial.println(adcValue);

    digitalWrite(LED, HIGH);
    delay(250);
    digitalWrite(LED, LOW);
    delay(250);
}
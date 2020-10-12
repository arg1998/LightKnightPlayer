#define LEFT_GREEN 9
#define LEFT_RED 10
#define LEFT_BLUE 11

#define RIGHT_GREEN 3
#define RIGHT_RED 5
#define RIGHT_BLUE 6

//brightness of the LEDs

//[red, green, blue]
char rgb_buffer[6];
//we update rgb led only if encoded serail data is available
bool rgb_update = false;

void setup() {
  Serial.begin(9600);
  pinMode(LEFT_GREEN, OUTPUT);
  pinMode(LEFT_RED, OUTPUT);
  pinMode(LEFT_BLUE, OUTPUT);
  
  pinMode(RIGHT_GREEN, OUTPUT);
  pinMode(RIGHT_RED, OUTPUT);
  pinMode(RIGHT_BLUE, OUTPUT);
}

void loop() {
  if(Serial.available()){
    if(Serial.readBytes(rgb_buffer, 6) > 0){
      rgb_update = true;
    }else{
      rgb_update = false;
    }
    Serial.flush();
  }


  if(rgb_update){
    analogWrite(LEFT_RED, rgb_buffer[0]);
    analogWrite(LEFT_GREEN, rgb_buffer[1]);
    analogWrite(LEFT_BLUE, rgb_buffer[2]);

    analogWrite(RIGHT_RED, rgb_buffer[3]);
    analogWrite(RIGHT_GREEN, rgb_buffer[4]);
    analogWrite(RIGHT_BLUE, rgb_buffer[5]);
    rgb_update = false;
  }
}

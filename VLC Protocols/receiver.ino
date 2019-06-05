int sensor = A1;  // define analog pin A1. (input/sensor)
int ptr = 0;
boolean result[4001];
int index;
int count;
int total_errors = 0;
int preamble_bit;
int acc;
String byte_;
String byte_1;
String byte_2;
String checksum;
int main_counter;
uint8_t VLC_BUFFER[50]; // buffer that can be used with ArduinoISP
int v = 0;
int THRESHOLD = 700; // sets the divider of the signals received for 1 and 0 
int interval = 40; // for the 5x oversampling

boolean preamble_detected;


float avg_res;
float sum_res;
int OS_val = 5; // 5x oversampling
// OS_val * interval should equal interval on the TX side
int total_count = 0;

int result_(int index){
  sum_res = 0;
  for(int i = 0; i < OS_val; i++){
    sum_res = sum_res + float(result[index+i]);
  }
  avg_res = sum_res/OS_val;
  
  if(avg_res > 0.5){return 1;}
  else{return 0;}
}

float shift_sync(int index){
  sum_res = 0;
  for(int i = 0; i < OS_val; i++){
    sum_res = sum_res + float(result[index+i]);
  }
  avg_res = sum_res/OS_val;
  return avg_res;
}

int synchronize(int index){
  int i = 0;
  int binary_val = result_(index);
  int maximum = index;
  int start = index - 2;
  for( int j = 0; j < OS_val; j++){
    if (binary_val == 1 && result_(start+j) == 1){
      float temp = shift_sync(start+j) - shift_sync(maximum);
      if (temp > 0){
        maximum = start+j;
      }
    }
    if (binary_val == 0 && result_(start+j) == 0){
      float temp = shift_sync(start+j) - shift_sync(maximum);
      if (temp < 0){
        maximum = start+j;
      }
    }
    Serial.println(maximum);
  }
  return maximum;
}


void setup(){
  Serial.begin(9600); //sets serial communication between arduino and computer
  pinMode(sensor,INPUT); // sets analog pin A1 as input.
}

void loop(){
  float val1 = analogRead(sensor);
  
  Serial.println(val1);
  if (val1 <THRESHOLD){
      result[ptr] = 0;
  }else { result[ptr]= 1;}
  ptr = ptr+1;
  if(ptr%100==0){Serial.println(ptr);}
  delay(interval); //half a second delay is 500
  
  if (ptr >= 2000){
    delay(100);
    Serial.println("Ending of the experiment");
    index = 0;
    count = 0;
    total_errors = 0;
    
    preamble_bit = 1;
   
    while (index < 2000) {
      preamble_detected = false;
      index = synchronize(index);
      if(result_(index) == preamble_bit){
        count = count + 1;
        index = index + OS_val;
        if(preamble_bit==0){
          preamble_bit = 1;
        }else{preamble_bit=0;}
      }else{
        index = index + OS_val;
        count = 0;
      }
      
      if(count==8){
        preamble_detected = true;
        Serial.println("=======");
        Serial.println("Synchronizing");
        index = synchronize(index);
        Serial.println("=======");
        count = 0;
        preamble_bit = 1;
      }
      
      if(preamble_detected == true){
        Serial.println(result_(index));    
        acc = 0;
        byte_ = "";
        byte_1 = "";
        byte_2 = "";
        checksum = "";
        main_counter = main_counter + 1;
        
        for (int i=0; i<8; i++){
          index = synchronize(index);
          acc = acc << 1;
          int temp = i * OS_val;
          if(result_(index+temp) == 1 ){
            acc |= 1;
          }
          byte_ = byte_ + String(result_(index+temp));
          if(i<4){ byte_1 = byte_1 + String(result_(index+temp)); }
          if(i>=4){ byte_2 = byte_2 + String(result_(index+temp)); }
        }
        
        int temp = 8 * OS_val;
        index = index + temp;
 
        // at this point acc will have a value in it consisting of 8 bits
        Serial.println("byte rx: " + byte_);
        Serial.println("byte rx 1: " + byte_1);
        Serial.println("byte rx 2: " + byte_2);
          
        for(int i=0; i<4; i++){
          if(byte_1.substring(i,i + 1) == "0" && byte_2.substring(i,i + 1) == "0"){
            checksum = checksum + "0";
          }
          else if(byte_1.substring(i,i + 1) == "1" && byte_2.substring(i,i + 1) == "1"){
            checksum = checksum + "0";
          }         
          else{
            checksum = checksum + "1";
          }
        }
        
        Serial.println("byte rx checksum: " + checksum);
        for(int i=0; i<4; i++){
          index = synchronize(index);
          int temp = i*OS_val;
          if(checksum.substring(i, i + 1) != String(result_(index+temp))){
            total_errors = total_errors + 1;
            Serial.println("Error in checksum!");
            Serial.println("total errors :" + total_errors);
            i = i + 4;
          }
        }
        
        temp = 4 * OS_val;
        index = index + temp;
        Serial.println("Checksum test finished");
        Serial.println("hexadecimal type: ");
        Serial.println(acc,HEX); 
        String t1 = String(acc,HEX);
        t1.toUpperCase();
        if(t1.length() != 2){
          t1 = "0" + t1;
        }
        t1 = "0x" + t1;
        Serial.println("hexadecimal to string: " + t1);
   
        Serial.println("");    
        
        
        uint8_t t2 = acc;
        
        if(v<20){
          VLC_BUFFER[v++] = acc;
        }
        
        Serial.println("int to string:");
        Serial.println(t2,HEX);
      }
    }
    Serial.println("errors: ");
    Serial.println(total_errors);
    Serial.println(total_errors);
    Serial.println("n: ");
    Serial.println(main_counter);
    Serial.println(main_counter);
    Serial.println("================");
    Serial.println("================");
    for(int i = 0; i<20;i++){
      Serial.println(VLC_BUFFER[i], HEX);
    }
    Serial.println("================");
    Serial.println("================");
    delay(1000);
    exit(0);
  }
}

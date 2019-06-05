int pin_number = 10; // LED pin for sending VLC signal
int interval = 200; // for 5x oversampling the receiver has 40 = 200/5
String data_seq;
String hex[16] = {"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};
String bin[16] = {"0000","0001","0010","0011","0100","0101","0110","0111","1000","1001","1010","1011","1100","1101","1110","1111"};
String data_binary;
String byte_chksm;

// Array of the program code to be sent can be appended to the array

String data_seq_[10] = {"64","00","80","46","0c"}; 



void setup()
{
   // initialize digital pin 0 as an output.
  Serial.begin(9600);
  pinMode(pin_number, OUTPUT);

}


void loop(){
  
  for(int b = 0; b<4; b++){
    
    data_seq = data_seq_[b];
    data_binary = "";
    
    
    for(int i = 0; i < 2; i++){
      for(int j = 0; j < 16; j++){
        if(data_seq.substring(i,i+1) == hex[j]){
          data_binary = data_binary + bin[j];
          break;
        }
      }
    }
    
    Serial.println("data sequence: " + data_binary);
    
    String preamble = "10101010";  
    for(int i = 0; i<8; i++){
  
      if (preamble.substring(i,i+1)=="0"){
        digitalWrite(pin_number,HIGH);
      }
      else{
        digitalWrite(pin_number,LOW);
      }
      delay(interval);
    }
    
    for(int i = 0; i<8; i++){
      
      if (data_binary.substring(i,i+1)=="0"){
        digitalWrite(pin_number,HIGH);
      }
      else{
        digitalWrite(pin_number,LOW);
      }
      delay(interval);
    }
    
    byte_chksm = "";
    
    for(int i = 0; i<4; i++){
      if(data_binary.substring(i,i + 1) == "0" && data_binary.substring(i + 4,i + 5) == "0"){
        byte_chksm = byte_chksm + "0";
      }
      else if(data_binary.substring(i,i + 1) == "1" && data_binary.substring(i + 4,i + 5) == "1"){
        byte_chksm = byte_chksm + "0";
      }
      else{
        byte_chksm = byte_chksm + "1";
      }
    }
    
    Serial.println("checksum: " + byte_chksm);
    
    for(int i = 0; i<4; i++){
      
      if (byte_chksm.substring(i,i + 1) == "0"){
        digitalWrite(pin_number,HIGH);
      }
      else{
        digitalWrite(pin_number,LOW);
      }
      delay(interval);
    }   
  }
}

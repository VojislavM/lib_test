
#include "message.hpp"
#include "frame.hpp"
//#include "crc32.hpp"
#include "inet.hpp"

//String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

volatile uint8_t rx_buffer[100];
uint8_t rx_data[2];
int rx_indx;
volatile int transfer_cplt;
uint8_t rx_last[2] = {0x00, 0x00};
int message_len = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("begin");

  // reserve 200 bytes for the inputString:
  //inputString.reserve(200);
}

void loop() {
  uint8_t test_frame[1024];
  ssize_t test_frame_size;
  // put your main code here, to run repeatedly:
  /* Generate message - test message */
  message_t msg;
  message_init(&msg);
  //Serial.print("msg sizeof: ");
  //Serial.println(sizeof(msg));
  //message_tlv_add_command(&msg, COMMAND_GET_STATUS);
  //Serial.print("msg sizeof: ");
  //Serial.println(sizeof(msg));
  message_tlv_add_command(&msg, COMMAND_MOVE_MOTOR);
  //message_tlv_add_reply(&msg, REPLY_ERROR_REPORT);
  tlv_motor_position_t position = {1000, 0, 0};
  message_tlv_add_motor_position(&msg, &position);
  //message_tlv_add_power_reading(&msg, 0x0444);
  //tlv_error_report_t error_report;
  //error_report.code = 35;
  //message_tlv_add_error_report(&msg, &error_report);
  //Serial.print("msg.length ");
  //Serial.println(msg.length);
  message_tlv_add_checksum(&msg);
  //Serial.print("msg sizeof: ");
  //Serial.println(sizeof(msg));
  
//  Serial.println("Generated protocol message: ");
//  message_print(&msg);
  
  //Serial.println();
  
  test_frame_size = frame_message(test_frame, sizeof(test_frame), &msg);
//  Serial.print("Serialized protocol message with frame:");
//  Serial.println();
//  for (size_t i = 0; i < test_frame_size; i++) {
//    Serial.print(test_frame[i], HEX);
//    Serial.print(" ");
//  }
//  Serial.println();

  message_free(&msg);

  //Serial.println("end");
  //delay(1000);
  //while(1){
    if (stringComplete) {
      //Serial.println(inputString);
      Serial.println("Received serialized protocol message:");
      for (size_t i = 0; i < message_len; i++) {
        Serial.print(rx_buffer[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      // clear the string:
      //inputString = "";
      stringComplete = false;
    }
  //}
}


/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    rx_data[0] = (uint8_t)Serial.read();
    /* Clear Rx_Buffer before receiving new data */
    if (rx_indx==0){
      for (int i=0; i<100; i++) rx_buffer[i]=0;
    }

    /* Start byte received */
    if(rx_data[0] == FRAME_MARKER_START){
      /* Start byte received in the frame */
//      Serial.println("rx_last, rx_buff[0]:");
//      Serial.println(rx_last[0], HEX);
//      Serial.println(rx_buffer[0], HEX);
      if((rx_last[0] == FRAME_MARKER_ESCAPE) && (rx_buffer[0] == FRAME_MARKER_START)){
        rx_buffer[rx_indx++]=rx_data[0];
      }
      /* Real start byte received */
      else if(rx_last[0] != FRAME_MARKER_ESCAPE){
        rx_indx = 0;
        rx_buffer[rx_indx++]=rx_data[0];

      }
    }
    /* End byte received */
    else if(rx_data[0] == FRAME_MARKER_END){
      /* End byte received in the frame */
      if(rx_last[0] == FRAME_MARKER_ESCAPE && rx_buffer[0] == FRAME_MARKER_START){
        rx_buffer[rx_indx++]=rx_data[0];
      }
      /* Real end byte received */
      else if(rx_last[0] != FRAME_MARKER_ESCAPE && rx_buffer[0] == FRAME_MARKER_START){
        rx_buffer[rx_indx++]=rx_data[0];
        message_len = rx_indx;
        rx_indx=0;
        /* Transfer complete, data is ready to read */
        stringComplete = true; //Transfer_cplt=1;
        /* Disable USART1 interrupt */
        //HAL_NVIC_DisableIRQ(USART1_IRQn);
      }
    }
    else{
      if(rx_buffer[0] == FRAME_MARKER_START){
        rx_buffer[rx_indx++]=rx_data[0];
      }
    }
    /* Store last received byte for ESC check */
    rx_last[0] = rx_data[0];
    
  }
}
/*
GET_STATUS:
F1 01 00 01 01 03 00 04 A5 05 DF 1B F2
    1  0  1  1  3  0  4  8  0  0  0 

*/

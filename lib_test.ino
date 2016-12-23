
#include "message.hpp"
#include "frame.hpp"
#include "inet.hpp"

/*** RECEIVING DATA VARIABLES ***/
/* True when receiving string is completed */
boolean stringComplete = false;
boolean send_frame_bool = false;
/* Receiving buffer for incomming serial data */
volatile uint8_t rx_buffer[100];
/* Temperary receiving buffer for incomming serial data*/
uint8_t rx_data[2];
/* Number of bytes received on serial */
int rx_indx;
/* Check buffer for receiving serual data */
uint8_t rx_last[2] = {0x00, 0x00};
/* Final length of serial received data */
int message_len = 0;


void setup() {
  /*Serial for communication */
  Serial.begin(115200);
  /* Serial test */
  Serial.println("begin");
}

void loop() {
    /* Parsed message */
    message_t msg_parsed;
    
    /*** TRANSMITTING DATA VARIABLES ***/
    /* Sending frame buffer */
    uint8_t send_frame[100];
    /* Sending frame size */
    ssize_t send_frame_size;
    /* Sending message */
    message_t msg_send; 

    send_frame_bool = true;

  while(true){
    if(send_frame_bool){
      /* Init for sending message */
      message_init(&msg_send);
      /* Uncomment to add values to the message */
       message_tlv_add_command(&msg_send, COMMAND_GET_STATUS);
    // message_tlv_add_command(&msg_send, COMMAND_MOVE_MOTOR);
    // message_tlv_add_reply(&msg_send, REPLY_ERROR_REPORT);
    // tlv_motor_position_t position = {1000, 0, 0};
    // message_tlv_add_motor_position(&msg_send, &position);
    //  message_tlv_add_power_reading(&msg_send, 0x0444);
    //  tlv_error_report_t error_report;
    //  error_report.code = 35;
    //  message_tlv_add_error_report(&msg_send, &error_report);
      message_tlv_add_checksum(&msg_send);
      /* Print generated message */
      Serial.println("Generated protocol message: ");
      message_print(&msg_send);  
      Serial.println();
  
      send_frame_size = frame_message(send_frame, sizeof(send_frame), &msg_send);
      Serial.print("Serialized protocol message with frame:");
      Serial.println();
      for (size_t i = 0; i < send_frame_size; i++) {
        Serial.print(send_frame[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      /* Free generated message */
      message_free(&msg_send);

      send_frame_bool = false;
    }
    /* When new frame is received on serial, previously generated message will be sent to serial*/
    /* and received message will be decoded and value sent to serial.*/
    if (stringComplete){
      /* Received serail message */
      Serial.println("Received serialized protocol message:");
      for (size_t i = 0; i < message_len; i++) {
        Serial.print(rx_buffer[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
  
      Serial.print("Received message_len: ");
      Serial.println(message_len);
      
  
      /* Parse received message */
      int num = frame_parser((uint8_t *)&rx_buffer, message_len, &msg_parsed);
      Serial.println("Parsed protocol message: ");
      message_print(&msg_parsed);
      Serial.println();
      /* Free received message */
      message_free(&msg_parsed); 
      stringComplete = false;
      
    }//end if
    serialEvent();
  }//while
}//end loop()


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
        stringComplete = true;
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


#include "message.hpp"
#include "frame.hpp"
#include "crc32.hpp"
#include "inet.hpp"

void setup() {
  Serial.begin(115200);
  Serial.println("begin");
  Serial.println("begin");


}

void loop() {
  // put your main code here, to run repeatedly:
  /* Generate message - test message */
  message_t msg;
  message_init(&msg);
  //Serial.print("msg sizeof: ");
  //Serial.println(sizeof(msg));
  message_tlv_add_command(&msg, COMMAND_GET_STATUS);
  //Serial.print("msg sizeof: ");
  //Serial.println(sizeof(msg));
  //message_tlv_add_command(&msg, COMMAND_MOVE_MOTOR);
  //message_tlv_add_reply(&msg, REPLY_ERROR_REPORT);
  //tlv_motor_position_t position = {0, 0, 0};
  //message_tlv_add_motor_position(&msg, &position);

  uint8_t buffer[1024];
  size_t length = message_serialize(buffer, 1024, &msg);
  Serial.print("Message length: ");
  Serial.println(length);
  Serial.println("Serialized protocol message:");
  for (size_t i = 0; i < length; i++) {
    Serial.print(buffer[i], HEX);
    //Serial.write(buffer[i]);
    Serial.print(" ");
  }
  //printf("\n");
  Serial.println();
  
  
  //message_tlv_add_power_reading(&msg, 0x0444);
  //tlv_error_report_t error_report;
  //error_report.code = 35;
  //message_tlv_add_error_report(&msg, &error_report);
  //Serial.print("msg.length ");
  //Serial.println(msg.length);
  message_tlv_add_checksum(&msg);
  //Serial.print("msg sizeof: ");
  //Serial.println(sizeof(msg));
  
  uint8_t test_frame[1024];
  ssize_t test_frame_size;
  
  Serial.println("Generated protocol message: ");
  message_print(&msg);
 
  Serial.println("\n");
  
  //uint8_t buffer[1024];
 /* size_t*/ length = message_serialize(buffer, 1024, &msg);
  Serial.print("Message length: ");
  Serial.println(length);
  Serial.println("Serialized protocol message:");
  for (size_t i = 0; i < length; i++) {
    Serial.print(buffer[i], HEX);
    //Serial.write(buffer[i]);
    Serial.print(" ");
  }
  //printf("\n");
  Serial.println();
  
  //test_frame_size = frame_message(test_frame, sizeof(test_frame), &msg);
  Serial.print("Serialized protocol message with frame:");
  Serial.println();
/*  for (size_t i = 0; i < test_frame_size; i++) {
    Serial.print(test_frame[i], HEX);
  }
*/  //printf("\n");
  Serial.println();

  message_free(&msg);

  Serial.println("end");
  while(1);
}
/*
GET_STATUS:
F1 01 00 01 01 03 00 04 A5 05 DF 1B F2
    1  0  1  1  3  0  4  8  0  0  0 

*/

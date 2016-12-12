
#include "message.hpp"

void setup() {
  Serial.begin(115200);
  Serial.println("begin");


}

void loop() {
  // put your main code here, to run repeatedly:
  /* Generate message - test message */
  message_t msg;
  message_init(&msg);
  Serial.print("msg sizeof: ");
  Serial.println(sizeof(msg));
  //message_tlv_add_command(&msg, COMMAND_GET_STATUS);
  //Serial.print("msg sizeof: ");
  //Serial.println(sizeof(msg));
  Serial.println(message_tlv_add_command(&msg, COMMAND_MOVE_MOTOR));
  //message_tlv_add_reply(&msg, REPLY_ERROR_REPORT);
  tlv_motor_position_t position = {-100000, -100000, -100000};
  message_tlv_add_motor_position(&msg, &position);
  //message_tlv_add_power_reading(&msg, 0x0444);
  //tlv_error_report_t error_report;
  //error_report.code = 35;
  //message_tlv_add_error_report(&msg, &error_report);
  Serial.print("msg.length ");
  Serial.println(msg.length);
  message_tlv_add_checksum(&msg);
  Serial.print("msg sizeof: ");
  Serial.println(sizeof(msg));
  
  uint8_t test_frame[1024];
  ssize_t test_frame_size;
  
  Serial.println("Generated protocol message: ");
  message_print(&msg);
 
  Serial.println("\n");
  
  uint8_t buffer[1024];
  size_t length = message_serialize(buffer, 1024, &msg);
  Serial.print("Message length: ");
  Serial.println(length);
  printf("Serialized protocol message:\n");
  for (size_t i = 0; i < length; i++) {
    Serial.print(buffer[i], HEX);
  }
  printf("\n");
  /*
  test_frame_size = frame_message(test_frame, sizeof(test_frame), &msg);
  printf("Serialized protocol message with frame:\n");
  for (size_t i = 0; i < test_frame_size; i++) {
    Serial.print(test_frame[i], HEX);
  }
  printf("\n");
    */
  message_free(&msg);

  Serial.println("end");
  while(1);
}

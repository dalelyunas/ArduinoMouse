//placeholder

import processing.serial.*;
import java.io.*;

Serial port;
String input = "";
PrintWriter output;

void setup() {
  output = createWriter("DataLog.txt");

  println(Serial.list());

  port = new Serial(this, Serial.list()[0], 115200);
  port.bufferUntil('\n');
}

void draw() {
  if (port.available() > 0) {
    input = (port.readString());
    output.println(input);
  }
}

void keyPressed() {
  output.flush(); // Writes the remaining data to the file
  output.close(); // Finishes the file
  exit(); // Stops the program
}


#include <WioLTEforArduino.h>
#include <SHT31.h>
#include <stdio.h>

#define APN                  "soracom.io"
#define USERNAME             "sora"
#define PASSWORD             "sora"
#define SORACOM_BEAM_EP_URL  "http://beam.soracom.io:8888/"

#define INTERVAL             (60000)
#define DATA_SIZE            (128)
#define NUMBER_SIZE          (32)

WioLTE Wio;
SHT31 sht31 = SHT31();
  
void setup() {
  delay(200);

  SerialUSB.println("");
  SerialUSB.println("--- START ---------------------------------------------------");
  
  SerialUSB.println("### I/O Initialize.");
  Wio.Init();
  SerialUSB.println("### Grove Power On...");
  Wio.PowerSupplyGrove(true);
  SerialUSB.println("### SHT31 begin sensing...");
  sht31.begin();

  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyLTE(true);
  delay(5000);

  SerialUSB.println("### Turn on or reset.");
  if (!Wio.TurnOnOrReset()) {
    SerialUSB.println("### ERROR! ###");
    return;
  }

  SerialUSB.println("### Connecting to \""APN"\".");
  delay(10000);
  if (!Wio.Activate(APN, USERNAME, PASSWORD)) {
    SerialUSB.println("### ERROR! ###");
    return;
  }
}

void loop() {
  char data[DATA_SIZE];
  int status;
  int connectId;

  float temp = sht31.getTemperature();
  float humi = sht31.getHumidity();

  if (temp == NAN || humi == NAN) {
    SerialUSB.println("ERROR!");
    goto err;
  }

  char number[NUMBER_SIZE];
  int result;
  result = Wio.GetPhoneNumber(number, NUMBER_SIZE);

  SerialUSB.println("### Post.");
  sprintf(data,"{\"value1\":\"%s\",\"value2\":\"%.2f\",\"value3\":\"%.2f\"}", number, temp, humi);
  SerialUSB.print("Post:");
  SerialUSB.print(data);
  SerialUSB.println("");
  if (!Wio.HttpPost(SORACOM_BEAM_EP_URL, data, &status)) {
    SerialUSB.println("### ERROR! ###");
    goto err;
  }
  SerialUSB.print("Status:");
  SerialUSB.println(status);
  
err:
  SerialUSB.println("### Wait.");
  delay(INTERVAL);
}


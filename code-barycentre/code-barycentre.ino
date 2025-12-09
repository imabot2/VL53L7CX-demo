#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX

SparkFun_VL53L5CX myImager;
VL53L5CX_ResultsData measurementData;

int imageResolution = 0;
int imageWidth = 0;

void setup()  //Init capteur
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("SparkFun VL53L5CX Imager Example");

  Wire.begin();
  Wire.setClock(400000);

  Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");
  if (myImager.begin() == false)
  {
    Serial.println(F("Sensor not found - check your wiring. Freezing"));
    while (1);
  }

  myImager.setRangingFrequency(10); // en Hz (max 15 pour 8x8)
  myImager.setResolution(8 * 8); // 64 zones
  imageResolution = myImager.getResolution();
  imageWidth = sqrt(imageResolution);

  myImager.startRanging();
}

void loop()
{
  if (myImager.isDataReady() == true)
  {
    if (myImager.getRangingData(&measurementData))
    {
      int activeCount = 0;
      float sumX = 0;
      float sumY = 0;
      float sumZ = 0;

      //Serial.println("Distance Matrix:");
      
      for (int y = 0; y <= imageWidth * (imageWidth - 1); y += imageWidth)
      {
        for (int x = imageWidth - 1; x >= 0; x--)
        {
          int index = x + y;
          uint16_t distance = measurementData.distance_mm[index];

          if (distance <= 1000) // Detected object
          {
            //Serial.print("X"); // Mark detected pixel
            sumX += x;
            sumY += y / imageWidth; // convert linear index y to real y
            sumZ += distance;
            activeCount++;
          }
        }
      }

      Serial.print("DATA:");
      for (int i = 0; i < imageResolution; i++) 
      {
        if (measurementData.distance_mm[i] < 1000)
        Serial.print(measurementData.distance_mm[i]);
        else
        Serial.print("0");
        if (i < imageResolution - 1) 
        Serial.print(",");
      }
        Serial.println();

      if (activeCount > 0)
      {
        float baryX = sumX / activeCount;
        float baryY = sumY / activeCount;
        float baryZ = sumZ / activeCount;

        // Print Barycentre coordinates

        Serial.print("BARY:");
        Serial.print(baryX); Serial.print(",");
        Serial.print(baryY); Serial.print(",");
        Serial.println(baryZ);

      }
      else
      {
        Serial.println("No valid measurements (all distances > 1m)");
      }
    }
  }
}

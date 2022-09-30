#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h> // for sqrt()

#include "shapes.hpp"

#if defined(_R_DEBUG)
static int _debug = 1;
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
static int _debug = 0;
#define DEBUG_PRINTF(...) 
#endif

void printSquare(uint8_t *data, size_t dataLength)
{
  const int dimension = sqrt(dataLength); // Or as close as..?
  const unsigned int horizontalSpacing = 2;
  char lineBuf[horizontalSpacing * dimension];
  // Print a square buffer
  for (int i = 0; i < dimension; ++i)
  {
    memset(lineBuf, 0, sizeof(lineBuf));
    for (int j = 0; j < dimension; ++j)
    {
      // Put the hexademical characters in a loop
      int jIndex = (data[i * dimension + j] % 16);
      char jChar = (jIndex < 10 ? '0' + jIndex : 'A' + (jIndex - 10));
      // We also want horizontal spacing so it is more square-like
      lineBuf[horizontalSpacing * j] = jChar;
      for (int spaceIndex = 1; spaceIndex < horizontalSpacing; ++spaceIndex)
        lineBuf[horizontalSpacing * j + spaceIndex] = ' ';
    }
    char outFormatBuf[16];
    memset(outFormatBuf, 0, sizeof(outFormatBuf));
    snprintf(outFormatBuf, sizeof(outFormatBuf), "%%.%ds \n", dimension * horizontalSpacing);
    printf(outFormatBuf, lineBuf);
  }
}

void populateDataCircle(uint8_t* dataBuffer, const unsigned int dataLength)
{
  // Constants for maths
  const int sideLength = (int)sqrt(dataLength);

  // Construct the encyption geometry
  const float centreValue = (sideLength - 1) / 2.0;
  const float radius = sideLength / 2.0 - 3.0;
  Point centre = { centreValue, centreValue };
  Circle circle = { &centre, radius };
  Pentacle pentacle;
  constructPentacle(&circle, &pentacle);
  DEBUG_PRINTF("End construct pentacle \n");

  // Iterate over data buffer input like pixels
  for (int i = 0; i < dataLength; ++i)
  {
    // Determine data point coordinates
    const int y = (i / sideLength);
    const int x = i - ((sideLength) * y);
    Point dataPoint = { (float)x, (float)y };
    DEBUG_PRINTF("Data point: %f, %f \n", dataPoint.x, dataPoint.y);

    // Calculate point geometric property - inside a triangle?
    const int insidePentacleTriangle = pointInsidePentacleTriangles(&dataPoint, &pentacle);

    // Render the point + condition appropriately
    dataBuffer[i] = insidePentacleTriangle; /* Call encyption hook here */
  }
}

int main(int argc, char **argv)
{
  printf("Hello, world! \n");

  // Set up the data buffer for drawing/encrypting
  const unsigned int dataLength = 32 * 32;
  uint8_t dataBuffer[dataLength];
  memset(dataBuffer, 0, sizeof(dataBuffer));

  // Put data in the shape of a pentacle
  populateDataCircle(dataBuffer, dataLength);

  // Print the buffer
  printSquare(dataBuffer, sizeof(dataBuffer));
  
  printf("Goodbye, world! \n");
  return 0;
}

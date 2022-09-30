#include "shapes.hpp"

#include <string.h>
#include <stdint.h>
#include <math.h>

//#define _R_DEBUG 1

#if defined(_R_DEBUG)
static int _debug = 1;
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
static int _debug = 0;
#define DEBUG_PRINTF(...) 
#endif



/**
 * @param[in] line - the line to return the equation constants for
 * @param[out] m - the gradient of line
 * @param[out] c - the y-value value of line at x = 0
 *
 * return - 0 for success, 1 for error
 */
int getLineEquation(Line *line, float *m, float *c)
{
  // Calculations
  const float rise = ( line->b->y - line->a->y );
  const float run = ( line->b->x - line->a->x );
  const float gradient = ( rise / run );
  const float offset  = line->a->y - ( gradient * line->a->x );
  // Return values
  DEBUG_PRINTF("Equation for line: %f, %f -> %f, %f is y = %.2fx + %.2f \n",
               line->a->x, line->a->y,
               line->b->x, line->b->y,
               gradient, offset);
  *m = gradient;
  *c = offset;
  return 0; // Success
}

/**
 * @param[in] angle1 The first angle (in degrees)
 * @param[in] angle2 The second angle (in degrees)
 *
 * return A positive number between 0 and 360; a value of >= 180 indicates that angle2 is closest
 * to angle1 counter-clockwise
 */
float getAngularDistance(float angle1, float angle2)
{
  float angularDistance = (angle2 - angle1);
  while (angularDistance > 360) angularDistance -= 360;
  while (angularDistance < 0) angularDistance += 360;
  return angularDistance;
}

float angleBetweenPoints(Point *point1, Point *point2)
{
  // Handle special cases to avoid divide-by-zero
  if (point1->x == point2->x)
  {
    const float theta = (point1->y > point2->y ? 90 : 270);
    return theta;
  }

  // Otherwise just use inverse tan
  const float rise = ( point2->y - point1->y );
  const float run = ( point2->x - point1-> x );
  const float theta = RAD2DEG(atan2(rise, run));
  return theta;
}

float angularDistanceBetweenLines(Point *origin, Point *line1, Point *line2)
{
  DEBUG_PRINTF("origin: %f, %f\tline a:%f, %f\tline b: %f, %f \n",
         origin->x, origin->y,
         line1->x, line1->y,
         line2->x, line2->y);
  const float angle1 = angleBetweenPoints(origin, line1);
  const float angle2 = angleBetweenPoints(origin, line2);
  DEBUG_PRINTF("angle1: %f\t angle2: %f \n", angle1, angle2);
  const float theta = getAngularDistance(angle2, angle1);
  DEBUG_PRINTF("theta: %f \n", theta);
  return theta;
}

// Determine if the point is inside of the triangle
// TODO: Handle line- and point- triangle cases
bool pointInsideTriangle(Point *point, Triangle *triangle)
{
  DEBUG_PRINTF("pointInsideTriangle start \n");
  Triangle tempTri;
  tempTri.a = triangle->a;
  tempTri.b = triangle->b;
  tempTri.c = triangle->c;
  // Make sure that the points are clock-wise
  if (180 < angularDistanceBetweenLines(tempTri.a, tempTri.b, tempTri.c))
  {
    Point *temp = tempTri.b;
    tempTri.b = tempTri.c;
    tempTri.c = temp;
    DEBUG_PRINTF("pointInsideTriangle: Swapped for clockwise \n");
  }
  // The point must be clockwise of all three points to be 'in the triangle'
  DEBUG_PRINTF("pointInsideTriangle: Checking angle1.. \n");
  const float angle1 = angularDistanceBetweenLines(tempTri.a, tempTri.b, point);
  if (180 < angle1) return false;
  DEBUG_PRINTF("pointInsideTriangle: Checking angle2.. \n");
  const float angle2 = angularDistanceBetweenLines(tempTri.b, tempTri.c, point);
  if (180 < angle2) return false;
  DEBUG_PRINTF("pointInsideTriangle: Checking angle3.. \n");
  const float angle3 = angularDistanceBetweenLines(tempTri.c, tempTri.a, point);
  if (180 < angle3) return false;
  DEBUG_PRINTF("pointInsideTriangle: Point is inside triangle \n");
  return true;
}

bool pointInsidePentagon(Point *point, Point *pentagon[5])
{
  // This function assumes clockwise points as input
  //for (int index = 0; index < 5; ++index)
  for (int index = 0; index < 1; ++index)
  {
    Point *pentagonPoint1 = pentagon[index];
    Point *pentagonPoint2 = pentagon[( (index + 1) % 5)];
    const float theta = angularDistanceBetweenLines(pentagon[(index + 1) % 5],
                                                    pentagon[index],
                                                    pentagon[(index + 2) % 5]);
      //angularDistanceBetweenLines(pentagonPoint1, pentagonPoint2, point);
    DEBUG_PRINTF("pointInsidePentagon theta=%f \n", theta);
    if (180 < theta) return false;
  }
  return true;
}

bool pointInsideCircle(Point *point, Circle *circle)
{
  const float h_diff = abs(point->x - circle->centre->x);
  const float v_diff = abs(point->y - circle->centre->y);
  const float distance_from_centre = sqrt((h_diff * h_diff) + (v_diff * v_diff));
  return ( distance_from_centre <= circle->radius );
}

/**
 * constructs an equilateral trangle based using the centre and radius of the base circle
 */
int constructTriangle(Circle *triangleBase, Triangle *triangle)
{
  if ((0 == triangleBase) || (0 == triangle)) return 1; // Failure

  for (int pointIndex = 0; pointIndex < 3; ++pointIndex)
  {
    Point *point = ((0 == pointIndex) ? triangle->a :
                    (1 == pointIndex) ? triangle->b :
                    triangle->c);
    float theta = 90.0 - ( (360.0 / 3.0) * pointIndex);
    while (theta >= 360.0) theta -= 360.0;
    while (theta < 0.0) theta += 360.0;
    point->x = triangleBase->centre->x + cos(DEG2RAD(theta)) * triangleBase->radius;
    point->y = triangleBase->centre->y + sin(DEG2RAD(theta)) * triangleBase->radius;
  }
  return 0; // Success
}

/**
 * Does the math to construct a pentacle given a centre and a radius
 *
 * @param[in] pentacleBase This is the centre and radius of the pentacle
 * @param[in/out] pentacle Not allocated by function, just filled out
 *
 * return 0 success, 1 fail
 */
int constructPentacle(Circle *pentacleBase, Pentacle *pentacle)
{
  if ((0 == pentacleBase) || (0 == pentacle)) return 1; // failure

  DEBUG_PRINTF("constructPentacle from base %f, %f (%f) \n",
               pentacleBase->centre->x,
               pentacleBase->centre->y,
               pentacleBase->radius);
  
  // Add points around edge of circle
  for (int outerPointIndex= 0; outerPointIndex< 5; ++outerPointIndex)
  {
    Point *point = &pentacle->outerPoints[outerPointIndex];
    float theta = ( (360.0 / 5.0) * outerPointIndex) - 90.0;
    while (theta >= 360.0) theta -= 360.0;
    while (theta < 0.0) theta += 360.0;
    point->x = pentacleBase->centre->x + cos(DEG2RAD(theta)) * pentacleBase->radius;
    point->y = pentacleBase->centre->y + sin(DEG2RAD(theta)) * pentacleBase->radius;
    DEBUG_PRINTF("Pentacle, outer %d: %f degrees, %f, %f \n",
                 outerPointIndex,
                 theta,
                 point->x, point->y);
  }

  // Add lines between the outer points to make a star
  for (int line_index = 0; line_index < 5; ++line_index)
  {
    Line *line = &pentacle->lines[line_index];
    line->a = &pentacle->outerPoints[line_index];
    line->b = &pentacle->outerPoints[ ( line_index + 2) % 5 ];
    DEBUG_PRINTF("Pentacle, line %d: %f, %f -> %f, %f \n",
                 line_index,
                 line->a->x, line->a->y,
                 line->b->x, line->b->y);
  }

  // Add the intersections of the lines
  for (int innerPointIndex = 0; innerPointIndex < 5; innerPointIndex++)
  {
    Point *innerPoint = &pentacle->innerPoints[innerPointIndex];
    Line *l1 = &pentacle->lines[innerPointIndex];
    Line *l2 = &pentacle->lines[( (innerPointIndex + 4) % 5)];
    // Get the equations for these two lines
    float m1, c1, m2, c2;
    getLineEquation(l1, &m1, &c1);
    getLineEquation(l2, &m2, &c2);
    // and solve for the intersection
    innerPoint->x = (c1 - c2) / (m2 - m1);
    //innerPoint->y = (m2 / c2) - (m1 / c1);
    //innerPoint->y = ( ( (m1 / m2) + c1 - c2 ) / ( 1 - (m1 / m2) ) );
    //innerPoint->y = ((m2 * c1) - (m1 * c2)) / (m1 + m2);
    innerPoint->y = ((m1 * c2) - (m2 * c1)) / (m1 - m2);

    // Dummy code to check later calcs
    // float theta = 90.0 + (360.0 / 5.0) - ( (360.0 / 5.0) * innerPointIndex);
    // while (theta >= 360.0) theta -= 360.0;
    // while (theta < 0.0) theta += 360.0;
    // innerPoint->x = pentacleBase->centre->x + cos(DEG2RAD(theta)) * pentacleBase->radius * 0.5;
    // innerPoint->y = pentacleBase->centre->y + sin(DEG2RAD(theta)) * pentacleBase->radius * 0.5;
    
    DEBUG_PRINTF("Pentacle, inner: %d: %f, %f \n",
                 innerPointIndex,
                 innerPoint->x, innerPoint->y);
  }
  
  return 0; // success
}

int pointInsidePentacleTriangles(Point *point, Pentacle *pentacle)
{
  for (int j = 0; j < 5; ++j)
  {
    Triangle pentacleTriangle;
    pentacleTriangle.a = &pentacle->outerPoints[j];
    pentacleTriangle.b = &pentacle->innerPoints[j];
    pentacleTriangle.c = &pentacle->innerPoints[( (j + 4) % 5 )];
    DEBUG_PRINTF("Pentacle triangle %d: %f, %f -> %f, %f -> %f, %f \n",
                 j,
                 pentacleTriangle.a->x, pentacleTriangle.a->y,
                 pentacleTriangle.b->x, pentacleTriangle.b->y,
                 pentacleTriangle.c->x, pentacleTriangle.c->y);
    if (pointInsideTriangle(point, &pentacleTriangle)) return (j + 1);
  }
  return 0; // Not in any triangles
}


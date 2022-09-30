#ifndef __SHAPES_H__
#define __SHAPES_H__

/* Shape defintiions */
#define PI 3.14159265359
#define RAD2DEG(x) ((x) * (180 / PI))
#define DEG2RAD(x) ((x) / (180 / PI))

typedef struct _point
{
  float x;
  float y;
} Point;

typedef struct _line
{
  Point *a;
  Point *b;
} Line;

typedef struct _triangle
{
  Point *a, *b, *c;
} Triangle;

typedef struct _circle
{
  Point *centre;
  float radius;
} Circle;

typedef struct _pentacle
{
  Circle *pentacleBase; // The centre and radius of the pentacle
  Point outerPoints[5]; // These lie on the base circle
  Line lines[5]; // These form the star-shape
  Point innerPoints[5]; // These are intersections of the lines of the star
} Pentacle;

/* Function definitions */
/* Angle functions */
int getLineEquation(Line *line, float *m, float *c);
float getAngularDistance(float angle1, float angle2);
float angleBetweenPoints(Point *point1, Point *point2);
float angularDistanceBetweenLines(Point *origin, Point *line1, Point *line2);
/* Point-inside functions*/
bool pointInsideCircle(Point *point, Circle *circle);
bool pointInsideTriangle(Point *point, Triangle *triangle);
bool pointInsidePentagon(Point *point, Point *pentagon[5]);
int pointInsidePentacleTriangles(Point *point, Pentacle *pentacle);
/* Shape constructors */
int constructTriangle(Circle *triangleBase, Triangle *triangle);
int constructPentacle(Circle *pentacleBase, Pentacle *pentacle);

#endif // __SHAPES_H__

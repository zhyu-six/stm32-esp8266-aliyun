#include "stm32f10x.h"                  // Device header
#include "math.h"
#include "GPS.h"

double  hypot(double  x,double  y)  
{
      return sqrt(x * x + y * y);
}

double  distance(double  wd1,double  jd1,double  wd2,double  jd2)  // 根据经纬度坐标计算实际距离,单位m
{
      double x, y, out;
      double PI = 3.1415926535898;
      double R = 6.371229 * 1e6;
       x = (jd2 - jd1) * PI * R * cos( ( (wd1 + wd2) / 2) * PI / 180) / 180;
       y = (wd2 - wd1) * PI * R / 180;
       out = hypot(x, y);
       return out;
}
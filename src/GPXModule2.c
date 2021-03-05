#include "GPXParser.h"
#include "LinkedListAPI.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>


float round10(float len) {

    int num1;
    int lastDigit;

    
    num1 = (int) len;

    lastDigit = num1 % 10;
    if (lastDigit < 5) {
        num1 = num1 - lastDigit;
    } else {
        num1 = num1 + (10 - lastDigit);
    }
    
    len = (float) num1;

    return(len);
}
  /*  
float getRouteLen(Route *rt) {
    if (rt == NULL) {
        return 0;
    }
    float distanceSum;
    int r = 6371000;
    Waypoint* tempWaypoint;
    ListIterator waypointIterator;
    
    

    distanceSum = 2 * r * asin(sqrt(pow(sin((lat1 - lat2)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon1 - lon2)/2),2))); 

    return(distanceSum);

}
*/
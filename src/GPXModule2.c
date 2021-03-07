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
    
float getRouteLen(const Route *rt) {
    if (rt == NULL) {
        return 0;
    }
    float distanceSum;
    float lat1,lon1,lat2,lon2;
    int r = 6371000;
    Waypoint* tempWaypoint;
    ListIterator waypointIterator;
    
    waypointIterator = createIterator(rt->waypoints);
    tempWaypoint = nextElement(&waypointIterator);
    lat1 = tempWaypoint->latitude;
    lon1 = tempWaypoint->longitude;
    for(tempWaypoint = nextElement(&waypointIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&waypointIterator)) {
        lat2 = tempWaypoint->latitude;
        lon2 = tempWaypoint->longitude;
        distanceSum += 2 * r * asin(sqrt(pow(sin((lat2 - lat1)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon2 - lon1)/2),2))); 
        //printf("The current sum is %f\n", distanceSum);
        lat1 = lat2;
        lon1 = lon2;
    }
    
    distanceSum = distanceSum * M_PI / 180; 

    return(distanceSum);

}


float getTrackLen(const Track *tr) {
    if (tr == NULL) {
        return 0;
    }
    float distanceSum;
    ListIterator trackSegmentIterator;
    ListIterator waypointIterator;
    TrackSegment* tempTrackSegment;
    Waypoint* tempWaypoint;
    float lat1,lon1,lat2,lon2;
    int r = 6371000;
    int i =0;

    trackSegmentIterator = createIterator(tr->segments);
    for(tempTrackSegment = nextElement(&trackSegmentIterator); tempTrackSegment != NULL; tempTrackSegment = nextElement(&trackSegmentIterator)) {
        waypointIterator = createIterator(tempTrackSegment->waypoints);
        tempWaypoint = nextElement(&waypointIterator);

        if (i > 0) {
            lat2 = tempWaypoint->latitude;
            lon2 = tempWaypoint->longitude;
            distanceSum += 2 * r * asin(sqrt(pow(sin((lat2 - lat1)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon2 - lon1)/2),2)));
        }

        lat1 = tempWaypoint->latitude;
        lon1 = tempWaypoint->longitude;
        for(tempWaypoint = nextElement(&waypointIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&waypointIterator)) {
            lat2 = tempWaypoint->latitude;
            lon2 = tempWaypoint->longitude;
            distanceSum += 2 * r * asin(sqrt(pow(sin((lat2 - lat1)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon2 - lon1)/2),2))); 
            //printf("The current sum is %f\n", distanceSum);
            lat1 = lat2;
            lon1 = lon2;
        }

        
        i++;
    }

    distanceSum = distanceSum * M_PI / 180; 

    return distanceSum;
}

int numRoutesWithLength(const GPXdoc* doc, float len, float delta) {
    if (doc == NULL || len < 0 || delta <0) {
        return 0;
    }
    ListIterator routeIterator;
    Route* tempRoute;
    int numRoutes = 0;

    routeIterator = createIterator(doc->routes);
    for(tempRoute = nextElement(&routeIterator); tempRoute != NULL; tempRoute = nextElement(&routeIterator)) {
        if (fabs((getRouteLen(tempRoute) - len)) <= delta) {
            numRoutes++;
        }
    }

    

    return(numRoutes);
}
 
int numTracksWithLength(const GPXdoc* doc, float len, float delta) {
    if (doc == NULL||  len < 0 || delta < 0) {
        return 0;
    }        

    
}

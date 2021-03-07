#include "GPXParser.h"
#include "LinkedListAPI.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

void dummyDeleteRoute(void* data);
void dummyDeleteTrack(void* data);

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

    ListIterator trackIterator;
    Track* tempTrack;
    int numTracks = 0;

    trackIterator = createIterator(doc->tracks);
    for (tempTrack = nextElement(&trackIterator); tempTrack != NULL; tempTrack = nextElement(&trackIterator)) {
         if (fabs((getTrackLen(tempTrack) - len)) <= delta) {
            numTracks++;
        }
    }
    
    return(numTracks);

}

bool isLoopRoute(const Route* route, float delta) {
    if (route == NULL || delta < 0) {
        return false;
    }
    int lat1,lat2,lon1,lon2;
    Waypoint* tempWaypoint;
    float distance = 0;
    int r = 6371000;

    if (getLength(route->waypoints) < 4) {
        return false;
    }

    tempWaypoint = getFromFront(route->waypoints);

    lat1 = tempWaypoint->latitude;
    lon1 = tempWaypoint->longitude;

    tempWaypoint = getFromBack(route->waypoints);

    lat2 = tempWaypoint->latitude;
    lon2 = tempWaypoint->longitude;

    distance = 2 * r * asin(sqrt(pow(sin((lat2 - lat1)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon2 - lon1)/2),2))); 

    if (distance <= delta) {
        return(true);
    }

    return (false);
}

bool isLoopTrack(const Track *tr, float delta) {
    if (tr == NULL || delta < 0) {
        return false;
    }
    ListIterator trackSegmentIterator;
    TrackSegment* tempTrackSegment;
    int lat1,lat2,lon1,lon2;
    Waypoint* tempWaypoint;
    float distance = 0;
    int r = 6371000;
    int sum = 0;

    trackSegmentIterator = createIterator(tr->segments);
    for(tempTrackSegment = nextElement(&trackSegmentIterator); tempTrackSegment != NULL; tempTrackSegment = nextElement(&trackSegmentIterator)) {
        sum+= getLength(tempTrackSegment->waypoints);  
    }

    if (sum < 4) {
        return false;
    }

    tempTrackSegment = getFromFront(tr->segments);
    tempWaypoint = getFromFront(tempTrackSegment->waypoints);

    lat1 = tempWaypoint->latitude;
    lon1 = tempWaypoint->longitude;

    tempTrackSegment = getFromBack(tr->segments);
    tempWaypoint = getFromBack(tempTrackSegment->waypoints);

    lat2 = tempWaypoint->latitude;
    lon2 = tempWaypoint->longitude;

    
    distance = 2 * r * asin(sqrt(pow(sin((lat2 - lat1)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon2 - lon1)/2),2))); 

    if (distance <= delta) {
        return(true);
    }


    return(false);
}

List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    if (doc == NULL || delta < 0) {
        return false;
    }
    Waypoint* tempWaypoint;
    List* routeList;
    ListIterator routeIterator;
    Route* tempRoute;
    int lat1,lat2,lon1,lon2;
    float distance = 0;
    int r = 6371000;

    routeList = initializeList(&routeToString, &dummyDeleteRoute, &compareRoutes);

    routeIterator = createIterator(doc->routes);
    for(tempRoute = nextElement(&routeIterator); tempRoute != NULL; tempRoute = nextElement(&routeIterator)) {
        tempWaypoint = getFromFront(tempRoute->waypoints);
        lat1 = tempWaypoint->latitude;
        lon1 = tempWaypoint->longitude;

        tempWaypoint = getFromFront(tempRoute->waypoints);
        lat2 = tempWaypoint->latitude;
        lon2 = tempWaypoint->longitude;

        distance = 2 * r * asin(sqrt(pow(sin((lat2 - lat1)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon2 - lon1)/2),2))); 

        if (distance < delta) {
            insertBack(routeList, tempRoute);
        }

    }

    return(routeList);

}

void dummyDeleteRoute(void* data) {
    Route *tmpRoute;

    if (data == NULL){
		return;
    }

    tmpRoute = (Route*)data;

    free(tmpRoute);
}


List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    if (doc == NULL || delta < 0) {
        return false;
    }
    List* trackList;
    ListIterator trackIterator;
    Track* tempTrack;
    Waypoint* tempWaypoint;
    TrackSegment* tempTrackSegment;
    int lat1,lat2,lon1,lon2;
    float distance = 0;
    int r = 6371000;
    
    trackList = initializeList(&trackToString, &dummyDeleteTrack, &compareTracks);

    trackIterator = createIterator(doc->tracks);
    for(tempTrack = nextElement(&trackIterator); tempTrack != NULL; tempTrack = nextElement(&trackIterator)) {
        tempTrackSegment = getFromFront(tempTrack->segments);
        tempWaypoint = getFromFront(tempTrackSegment->waypoints);

        lat1 = tempWaypoint->latitude;
        lon1 = tempWaypoint->longitude;

        tempTrackSegment = getFromBack(tempTrack->segments);
        tempWaypoint = getFromBack(tempTrackSegment->waypoints);

        lat2 = tempWaypoint->latitude;
        lon2 = tempWaypoint->longitude;

        distance = 2 * r * asin(sqrt(pow(sin((lat2 - lat1)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon2 - lon1)/2),2))); 

        if (distance < delta) {
            insertBack(trackList, tempTrack);
        }

    }

    return(trackList);
}

void dummyDeleteTrack(void* data) {
    Track *tmpTrack;

    if (data == NULL){
		return;
    }

    tmpTrack = (Track*)data;

    free(tmpTrack);
}

char* trackToJSON(const Track *tr) {
    return(NULL);
}

char* routeToJSON(const Route *rt) {
    if (rt == NULL) {
        return "{}";
    }

    char* strn;
    strn = (char*) malloc(256);

    char* tempName = (char*) malloc(256);
    if (strcmp(rt->name, "")==0 ) {
        strcpy(tempName, "None");
    } else {
        strcpy(tempName, rt->name);
    }

    char* tempBool = (char*) malloc(6);
    if (isLoopRoute(rt, 10.0)) {
        strcpy(tempBool, "true");
    } else {
        strcpy(tempBool, "false");
    }

    sprintf(strn, "{name\":\"%s\",\"numPoints\":%d,\"len\":%f,\"loop\":%s}", tempName, getLength(rt->waypoints), round10(getRouteLen(rt)), tempBool);

    free(tempName);
    free(tempBool);

    return(strn);
}

char* routeListToJSON(const List *list) {
    if (list == NULL ) {
        return "[]";
    }

    if (getLength((List*) list) == 0) {
        return "[]";
    }
    int i = 0;
    ListIterator routeIterator;
    Route* tempRoute;
    char* strn;
    strn = (char*) malloc(2048);

    strcpy(strn, "[");

    char* rtetJSONchar;
    routeIterator = createIterator((List*) list);
    for(tempRoute = nextElement(&routeIterator); tempRoute != NULL; tempRoute = nextElement(&routeIterator)) {
        if (i > 0) {
            strcat(strn, ",");
        }
        rtetJSONchar = routeToJSON(tempRoute);
        strcat(strn, rtetJSONchar);
        free(rtetJSONchar);
        i++;
    }

    strcat(strn, "]");

    return(strn);
}

char* trackListToJSON(const List *list) {
    return NULL;
}

char* GPXtoJSON(const GPXdoc* gpx) {
    return NULL;
}








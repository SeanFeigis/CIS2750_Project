#include "GPXParser.h"
#include "LinkedListAPI.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

void dummyDeleteRoute(void* data);
void dummyDeleteTrack(void* data);

float round10(float len) {

    int num1 = 0;
    int lastDigit = 0;

    
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
    float distanceSum = 0;
    float lat1,lon1,lat2,lon2;
    int r = 6371000;
    Waypoint* tempWaypoint;
    ListIterator waypointIterator;
    
    waypointIterator = createIterator(rt->waypoints);
    tempWaypoint = nextElement(&waypointIterator);
    lat1 = tempWaypoint->latitude * M_PI / 180;
    lon1 = tempWaypoint->longitude * M_PI / 180;
    for(tempWaypoint = nextElement(&waypointIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&waypointIterator)) {
        lat2 = tempWaypoint->latitude * M_PI / 180;
        lon2 = tempWaypoint->longitude * M_PI / 180;
        distanceSum += 2 * r * asin(sqrt(pow(sin((lat2 - lat1)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon2 - lon1)/2),2))); 
        //printf("The current sum is %f\n", distanceSum);
        lat1 = lat2;
        lon1 = lon2;
    }
    
    return(distanceSum);

}


float getTrackLen(const Track *tr) {
    if (tr == NULL) {
        return 0;
    }
    float distanceSum = 0;
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
            lat2 = tempWaypoint->latitude  * M_PI / 180;
            lon2 = tempWaypoint->longitude  * M_PI / 180;
            distanceSum += 2 * r * asin(sqrt(pow(sin((lat2 - lat1)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon2 - lon1)/2),2)));
        }

        lat1 = tempWaypoint->latitude  * M_PI / 180;
        lon1 = tempWaypoint->longitude  * M_PI / 180;
        for(tempWaypoint = nextElement(&waypointIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&waypointIterator)) {
            lat2 = tempWaypoint->latitude  * M_PI / 180;
            lon2 = tempWaypoint->longitude  * M_PI / 180;
            distanceSum += 2 * r * asin(sqrt(pow(sin((lat2 - lat1)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon2 - lon1)/2),2))); 
            //printf("The current sum is %f\n", distanceSum);
            lat1 = lat2;
            lon1 = lon2;
        }

        
        i++;
    }

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

    lat1 = tempWaypoint->latitude  * M_PI / 180;
    lon1 = tempWaypoint->longitude  * M_PI / 180;

    tempWaypoint = getFromBack(route->waypoints);

    lat2 = tempWaypoint->latitude  * M_PI / 180;
    lon2 = tempWaypoint->longitude  * M_PI / 180;

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

    lat1 = tempWaypoint->latitude  * M_PI / 180;
    lon1 = tempWaypoint->longitude  * M_PI / 180;

    tempTrackSegment = getFromBack(tr->segments);
    tempWaypoint = getFromBack(tempTrackSegment->waypoints);

    lat2 = tempWaypoint->latitude  * M_PI / 180;
    lon2 = tempWaypoint->longitude  * M_PI / 180;

    
    distance = 2 * r * asin(sqrt(pow(sin((lat2 - lat1)/2),2)+ cos(lat1)*cos(lat2)*pow(sin((lon2 - lon1)/2),2))); 

    if (distance <= delta) {
        return(true);
    }


    return(false);
}

List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    if (doc == NULL || delta < 0) {
        return NULL;
    }
    Waypoint* tempWaypoint;
    List* routeList;
    ListIterator routeIterator;
    Route* tempRoute;
    int lat1,lon1;
    float distance = 0;
    float distance2 = 0;
    int r = 6371000;

    /*
    sourceLat = sourceLat * M_PI /180;
    sourceLong = sourceLong * M_PI /180;
    destLat = destLat * M_PI /180;
    destLong = destLong * M_PI /180;
    */
    routeList = initializeList(&routeToString, &dummyDeleteRoute, &compareRoutes);

    routeIterator = createIterator(doc->routes);
    for(tempRoute = nextElement(&routeIterator); tempRoute != NULL; tempRoute = nextElement(&routeIterator)) {
        tempWaypoint = getFromFront(tempRoute->waypoints);
        lat1 = tempWaypoint->latitude  * M_PI / 180;
        lon1 = tempWaypoint->longitude  * M_PI / 180;

        distance = 2 * r * asin(sqrt(pow(sin((sourceLat - lat1)/2),2)+ cos(lat1)*cos(sourceLat)*pow(sin((sourceLong - lon1)/2),2)));

        tempWaypoint = getFromBack(tempRoute->waypoints);
        lat1 = tempWaypoint->latitude  * M_PI / 180;
        lon1 = tempWaypoint->longitude  * M_PI / 180;

        distance2 = 2 * r * asin(sqrt(pow(sin((destLat - lat1)/2),2)+ cos(lat1)*cos(destLat)*pow(sin((destLong - lon1)/2),2))); 
        printf("Delta: %f,  Distance1: %f, Distance2: %f\n", delta, distance, distance2);
        if (distance < delta && distance2 < delta) {
            insertBack(routeList, tempRoute);
        }

    }

    if (getLength(routeList) == 0) {
        return NULL;
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
        return NULL;
    }
    List* trackList;
    ListIterator trackIterator;
    Track* tempTrack;
    Waypoint* tempWaypoint;
    TrackSegment* tempTrackSegment;
    int lat1,lon1;
    float distance = 0;
    float distance2 = 0;
    int r = 6371000;

    sourceLat = sourceLat * M_PI /180;
    sourceLong = sourceLong * M_PI /180;
    destLat = destLat * M_PI /180;
    destLong = destLong * M_PI /180;
    
    trackList = initializeList(&trackToString, &dummyDeleteTrack, &compareTracks);

    trackIterator = createIterator(doc->tracks);
    for(tempTrack = nextElement(&trackIterator); tempTrack != NULL; tempTrack = nextElement(&trackIterator)) {
        tempTrackSegment = getFromFront(tempTrack->segments);
        tempWaypoint = getFromFront(tempTrackSegment->waypoints);

        lat1 = tempWaypoint->latitude  * M_PI / 180;
        lon1 = tempWaypoint->longitude  * M_PI / 180;

        distance = 2 * r * asin(sqrt(pow(sin((sourceLat- lat1)/2),2)+ cos(lat1)*cos(sourceLat)*pow(sin((sourceLong - lon1)/2),2))); 

        tempTrackSegment = getFromBack(tempTrack->segments);
        tempWaypoint = getFromBack(tempTrackSegment->waypoints);

        lat1 = tempWaypoint->latitude  * M_PI / 180;
        lat1 = tempWaypoint->longitude  * M_PI / 180;

        distance2 = 2 * r * asin(sqrt(pow(sin((destLat - lat1)/2),2)+ cos(lat1)*cos(destLat)*pow(sin((destLong - lon1)/2),2))); 

        if (distance < delta && distance2 < delta) {
            insertBack(trackList, tempTrack);
        }

    }

    if (getLength(trackList) == 0) {
        return NULL;
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
    if (tr == NULL) {
        return "{}";
    }

    char* strn;
    strn = (char*) malloc(256);

    char* tempName = (char*) malloc(256);
    if (strcmp(tr->name, "")==0 ) {
        strcpy(tempName, "None");
    } else {
        strcpy(tempName, tr->name);
    }

    char* tempBool = (char*) malloc(6);
    if (isLoopTrack(tr, 10.0)) {
        strcpy(tempBool, "true");
    } else {
        strcpy(tempBool, "false");
    }

    sprintf(strn, "{\"name\":\"%s\",\"len\":%.1f,\"loop\":%s}", tempName, round10(getTrackLen(tr)), tempBool);

    free(tempName);
    free(tempBool);

    return(strn);
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

    sprintf(strn, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s}", tempName, getLength(rt->waypoints), round10(getRouteLen(rt)), tempBool);

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
    if (list == NULL ) {
        return "[]";
    }

    if (getLength((List*) list) == 0) {
        return "[]";
    }
    int i = 0;
    ListIterator trackIterator;
    Track* tempTrack;
    char* strn;
    strn = (char*) malloc(2048);

    strcpy(strn, "[");

    char* trktJSONchar;
    trackIterator = createIterator((List*) list);
    for(tempTrack = nextElement(&trackIterator); tempTrack != NULL; tempTrack = nextElement(&trackIterator)) {
        if (i > 0) {
            strcat(strn, ",");
        }
        trktJSONchar = trackToJSON(tempTrack);
        strcat(strn, trktJSONchar);
        free(trktJSONchar);
        i++;
    }

    strcat(strn, "]");

    return(strn);
}

char* GPXtoJSON(const GPXdoc* gpx) {
    if (gpx == NULL) {
        return "{}";
    }

    char* strn;
    strn = (char*) malloc(2048);

    sprintf(strn, "{\"version\":%.1f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}",
        gpx->version, gpx->creator, getLength(gpx->waypoints), getLength(gpx->routes) , getLength(gpx->tracks));


    return strn;
}


void addWaypoint(Route *rt, Waypoint *pt) {

}

void addRoute(GPXdoc* doc, Route* rt) {

}

GPXdoc* JSONtoGPX(const char* gpxString) {
    return NULL;
}

Waypoint* JSONtoWaypoint(const char* gpxString) {
    return NULL;
}

Route* JSONtoRoute(const char* gpxString) {
    return NULL;
}



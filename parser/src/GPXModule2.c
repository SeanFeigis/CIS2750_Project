#include "GPXParser.h"
#include "LinkedListAPI.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void dummyDeleteRoute(void* data);
void dummyDeleteTrack(void* data);
char* waypointToJSON(Waypoint* wpt);

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
    float lat1,lat2,lon1,lon2;
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
    float lat1,lat2,lon1,lon2;
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
    float lat1,lon1;
    float distance = 0;
    float distance2 = 0;
    int r = 6371000;


    sourceLat = sourceLat * M_PI /180;
    sourceLong = sourceLong * M_PI /180;
    destLat = destLat * M_PI /180;
    destLong = destLong * M_PI /180;

    routeList = initializeList(&routeToString, &dummyDeleteRoute, &compareRoutes);

    routeIterator = createIterator(doc->routes);
    for(tempRoute = nextElement(&routeIterator); tempRoute != NULL; tempRoute = nextElement(&routeIterator)) {
        tempWaypoint = getFromFront(tempRoute->waypoints);
        lat1 = tempWaypoint->latitude  * M_PI / 180;
        lon1 = tempWaypoint->longitude  * M_PI / 180;
       // printf("Lat1: %f, Lon1: %f\n", lat1, lon1);
        distance = 2 * r * asin(sqrt(pow(sin((sourceLat - lat1)/2),2)+ cos(lat1)*cos(sourceLat)*pow(sin((sourceLong - lon1)/2),2)));

        tempWaypoint = getFromBack(tempRoute->waypoints);
        lat1 = tempWaypoint->latitude  * M_PI / 180;
        lon1 = tempWaypoint->longitude  * M_PI / 180;

        distance2 = 2 * r * asin(sqrt(pow(sin((destLat - lat1)/2),2)+ cos(lat1)*cos(destLat)*pow(sin((destLong - lon1)/2),2)));
        //printf("Delta: %f,  Distance1: %f, Distance2: %f\n", delta, distance, distance2);
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
    float lat1,lon1;
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

int getLengthofTrack(const Track *tr) {
    int i = 0;
    TrackSegment* tempTrack;
    ListIterator segiterator;
    segiterator = createIterator(tr->segments);
    for(tempTrack = nextElement(&segiterator); tempTrack != NULL; tempTrack = nextElement(&segiterator)) {
       i+=  getLength(tempTrack->waypoints);
    }
    return i;
}


char* trackToJSON(const Track *tr) {
    char* strn;

    if (tr == NULL) {
        strn = malloc(3);
        strcpy(strn, "{}");
        return strn;
    }

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

    sprintf(strn, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s}", tempName, getLengthofTrack(tr), round10(getTrackLen(tr)), tempBool);

    free(tempName);
    free(tempBool);

    return(strn);
}

char* routeToJSON(const Route *rt) {
    char* strn;

    if (rt == NULL) {
        strn = malloc(3);
        strcpy(strn, "{}");
        return strn;
    }

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
    char* strn;

    if (list == NULL ) {
        strn = malloc(3);
        strcpy(strn, "[]");
        return strn;
    }

    if (getLength((List*) list) == 0) {
        strn = malloc(3);
        strcpy(strn, "[]");
        return strn;
    }
    int i = 0;
    ListIterator routeIterator;
    Route* tempRoute;
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
    char* strn;

    if (list == NULL ) {
        strn = malloc(3);
        strcpy(strn, "[]");
        return strn;
    }

    if (getLength((List*) list) == 0) {
        strn = malloc(3);
        strcpy(strn, "[]");
        return strn;
    }
    int i = 0;
    ListIterator trackIterator;
    Track* tempTrack;
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
    char* strn;

    if (gpx == NULL) {
        strn = malloc(3);
        strcpy(strn, "{}");
        return strn;
    }

    strn = (char*) malloc(2048);

    sprintf(strn, "{\"version\":%.1f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}",
        gpx->version, gpx->creator, getLength(gpx->waypoints), getLength(gpx->routes) , getLength(gpx->tracks));


    return strn;
}


void addWaypoint(Route *rt, Waypoint *pt) {
    if (rt == NULL || pt == NULL) {
        return;
    }
    insertBack(rt->waypoints, pt);
}

void addRoute(GPXdoc* doc, Route* rt) {
    if (doc == NULL || rt == NULL) {
        return;
    }
    insertBack(doc->routes, rt);
}

GPXdoc* JSONtoGPX(const char* gpxString) {
    if  (gpxString == NULL) {
        return NULL;
    }

    GPXdoc* doc = malloc(sizeof(GPXdoc));
    strcpy(doc->namespace, "http://www.topografix.com/GPX/1/1");

    char* commaChar1;
    char* commaChar2;
    char* colonChar;


    commaChar1 = strtok((char*)gpxString, ",");
    commaChar2 = strtok(NULL, ",");

    colonChar = strtok(commaChar1, ":");
    colonChar = strtok(NULL, ":");

    doc->version = atof(colonChar);

    colonChar = strtok(commaChar2, ":");
    colonChar = strtok(NULL, ":");
    colonChar = strtok(colonChar, "\"");

    doc->creator = malloc(strlen(colonChar)+1);
    strcpy(doc->creator, colonChar);

    doc->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    doc->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    doc->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);

    return doc;

}

void JSONListToWaypointList(char* list, char* name, char* filename, char* numWpts) {
    if (list == NULL|| name == NULL || filename == NULL || numWpts == NULL) {
        return ;
    }
    GPXdoc* doc = malloc(sizeof(GPXdoc));
    doc = createValidGPXdoc(filename, "gpx.xsd");
    List* wptList = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    char* commaChar1;
    char* someChar = malloc(1024);
    strcpy(someChar, list);
    Waypoint* wpt;
    int num = atoi(numWpts);
    char** strArray = malloc(num * 8);
    //printf("%s\n", list);
    
    commaChar1 = strtok(someChar, "[");
    commaChar1[strlen(commaChar1)-1] = '\0';
    //printf("%s\n", commaChar1);

    commaChar1 = strtok(commaChar1, "{");
    for (int i = 0; i < num-1; i++) {
        commaChar1[strlen(commaChar1)-1] = '\0';
        char* tempChar = malloc(256);
        strcpy(tempChar, "{");
        strcat(tempChar, commaChar1);
        strArray[i] = malloc(strlen(tempChar)+1);
        strcpy(strArray[i], tempChar);
        //printf("%s\n", tempChar);
        commaChar1 = strtok(NULL, "{"); 
        //wpt = JSONtoWaypoint(tempChar);
        //insertBack(wptList, wpt);
        free(tempChar);
    }
    char* tempChar = malloc(256);
    strncpy(tempChar, "{", 2);
    strncat(tempChar, commaChar1, strlen(commaChar1)+1);
    //printf("%s\n", tempChar);
    strArray[num-1] = malloc(strlen(tempChar)+1);
    strcpy(strArray[num-1], tempChar);
    //printf("Cringe\n");
    //wpt = JSONtoWaypoint(tempChar);
    //insertBack(wptList, wpt);
    free(tempChar);

    for (int i = 0; i < num; i++) {
        //printf("%s\n", strArray[i]);
        wpt = JSONtoWaypoint(strArray[i]);
        insertBack(wptList, wpt);
        free(strArray[i]);
    }

    free(strArray);

    Route* rte = malloc(sizeof(Route));

    rte->name = malloc(strlen(name)+1);
    strcpy(rte->name, name);
    rte->waypoints = wptList;
    rte->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    addRoute(doc, rte);
    writeGPXdoc(doc, filename);
    deleteGPXdoc(doc);

    free(someChar);

    return;
}

Waypoint* JSONtoWaypoint(const char* gpxString) {
    if (gpxString == NULL) {
        return NULL;
    }

    Waypoint* tempWaypoint = malloc(sizeof(Waypoint));
    double lat,lon;
    char* commaChar1;
    char* commaChar2;
    char* colonChar;

    commaChar1 = strtok((char*)gpxString, ",");
    commaChar2 = strtok(NULL, ",");

    colonChar = strtok(commaChar1, ":");
    colonChar = strtok(NULL, ":");
    colonChar = strtok(colonChar, "\"");

    //printf("%s\n", colonChar);
    lat = atof(colonChar);

    colonChar = strtok(commaChar2, ":");
    colonChar = strtok(NULL, ":");
    colonChar = strtok(colonChar, "\"");
    
    lon = atof(colonChar);
    //printf("%s\n", colonChar);

    tempWaypoint->latitude = lat;
    tempWaypoint->longitude = lon;
    tempWaypoint->name = malloc(2);
    tempWaypoint->name[0] = '\0';
    tempWaypoint->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    return tempWaypoint;
}


Route* JSONtoRoute(const char* gpxString) {
    if (gpxString == NULL) {
        return NULL;
    }

    Route* tempRoute = malloc(sizeof(Route));

    char* delimChar;

    delimChar = strtok((char*)gpxString, ":");
    delimChar = strtok(NULL, ":");

    delimChar = strtok(delimChar, "\"");

    tempRoute->name = malloc(strlen(delimChar)+1);
    strcpy(tempRoute->name, delimChar);

    tempRoute->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    tempRoute->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    return tempRoute;
}

char* GPXFiletoJSONRouteList(char* gpxFile, char* xsdFile) {
  GPXdoc* doc = malloc(sizeof(GPXdoc));
  doc = createValidGPXdoc(gpxFile, xsdFile);
  char* string;
  string = routeListToJSON(doc->routes);
  deleteGPXdoc(doc);
  return(string);
}

char* GPXFiletoJSONTrackList(char* gpxFile, char* xsdFile) {
  GPXdoc* doc = malloc(sizeof(GPXdoc));
  doc = createValidGPXdoc(gpxFile, xsdFile);
  char* string;
  string = trackListToJSON(doc->tracks);
  deleteGPXdoc(doc);
  return(string);
}

void JSONtoGPXtoFile(char* JSON, char* fileName) {

    GPXdoc* doc = JSONtoGPX(JSON);
    writeGPXdoc(doc, fileName);
    deleteGPXdoc(doc);

}

char* gpxDataToJSON(GPXData* data) {
    char* strn;

    if (data == NULL) {
        strn = malloc(3);
        strcpy(strn, "{}");
        return strn;
    }

    strn = (char*) malloc(256);

    char* tempName = (char*) malloc(256);
    if (strcmp(data->name, "")==0 ) {
        strcpy(tempName, "None");
    } else {
        strcpy(tempName, data->name);
    }


    sprintf(strn, "{\"name\":\"%s\",\"value\":\"%s\"}", tempName, data->value);
    free(tempName);

    return(strn);
}

char* gpxDataListToJSON(const List *list) {
    char* strn;

    if (list == NULL ) {
        strn = malloc(3);
        strcpy(strn, "[]");
        return strn;
    }

    if (getLength((List*) list) == 0) {
        strn = malloc(3);
        strcpy(strn, "[]");
        return strn;
    }
    int i = 0;
    ListIterator trackIterator;
    GPXData* tempTrack;
    strn = (char*) malloc(2048);

    strcpy(strn, "[");

    char* trktJSONchar;
    trackIterator = createIterator((List*) list);
    for(tempTrack = nextElement(&trackIterator); tempTrack != NULL; tempTrack = nextElement(&trackIterator)) {
        if (i > 0) {
            strcat(strn, ",");
        }
        trktJSONchar = gpxDataToJSON(tempTrack);
        strcat(strn, trktJSONchar);
        free(trktJSONchar);
        i++;
    }

    strcat(strn, "]");

    return(strn);
}

char* RouteNameToJson(char* filename, char* routeName) {
    
    GPXdoc* doc = malloc(sizeof(GPXdoc));
    doc = createValidGPXdoc(filename, "gpx.xsd");
    if (doc == NULL) {
        return NULL;
    }

    Track* tempTrack;
    Route* tempRoute;
    char* strn = NULL;

    if ((tempTrack = getTrack(doc, routeName)) != NULL) {
        strn = gpxDataListToJSON(tempTrack->otherData);
    }

    if ((tempRoute = getRoute(doc, routeName)) != NULL) {
        strn = gpxDataListToJSON(tempTrack->otherData);
    }

    return(strn);

}

char* renameRoute(char* filename, char* newName, char* oldName) {

    GPXdoc* doc = malloc(sizeof(GPXdoc));
    doc = createValidGPXdoc(filename, "gpx.xsd");
    if (doc == NULL) {
        return NULL;
    }

    Route* rte;
    Track* trk;

    rte = getRoute(doc, oldName);
    trk = getTrack(doc, oldName);

    if (rte != NULL) {
        char* name = malloc(strlen(newName)+1);
        strcpy(name, newName);
        free(rte->name);
        rte->name = name;

        writeGPXdoc(doc, filename);
        deleteGPXdoc(doc);

        return("1");
    }
    
    if (trk != NULL) {
        char* name = malloc(strlen(newName)+1);
        strcpy(name, newName);
        free(trk->name);
        trk->name = name;

        writeGPXdoc(doc, filename);
        deleteGPXdoc(doc);

        return("1");
    }


    return(NULL);
}

char* waypointToJSON(Waypoint* wpt) {
    if  (wpt == NULL) {
        return NULL;
    }

    char* temp;
    temp = malloc(256);

    sprintf(temp, "{\"name\":\"%s\",\"lon\":%f,\"lat\":%f}", wpt->name, wpt->latitude, wpt->longitude);
    return(temp);
}

char* routeToWaypointList(char* filename, char* routeNum) {

    if (filename == NULL || routeNum == NULL) {
        return NULL;
    }

    GPXdoc* doc = createValidGPXdoc(filename, "gpx.xsd");
    Route* rte;
    ListIterator routeIterator;
    int max = atoi(routeNum);
    routeIterator = createIterator(doc->routes);

    for (int i = 0; i <= max; i++) {
        rte = nextElement(&routeIterator);
    }

    char* strn;

    if (rte == NULL) {
        return NULL;
    }
    
    if (rte->waypoints == NULL ) {
        strn = malloc(3);
        strcpy(strn, "[]");
        return strn;
    }

    if (getLength((List*) rte->waypoints) == 0) {
        strn = malloc(3);
        strcpy(strn, "[]");
        return strn;
    }

    int i = 0;
    ListIterator waypointIterator;
    Waypoint* tempWaypoint;
    strn = (char*) malloc(2048);

    strcpy(strn, "[");

    char* trktJSONchar;
    waypointIterator = createIterator((List*) rte->waypoints);
    for(tempWaypoint = nextElement(&waypointIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&waypointIterator)) {
        if (i > 0) {
            strcat(strn, ",");
        }
        trktJSONchar = waypointToJSON(tempWaypoint);
        strcat(strn, trktJSONchar);
        free(trktJSONchar);
        i++;
    }

    strcat(strn, "]");

    return(strn);
}
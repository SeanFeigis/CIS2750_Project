#include "GPXParser.h"
#include "LinkedListAPI.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Waypoint* createWaypoint(xmlNode* node);
Route* createRoute(xmlNode* node);
Track* createTrack(xmlNode* node);
bool validateXML(xmlDoc* doc, char* xsd);
xmlDoc* GPXdocToXmlDoc(GPXdoc* doc);


GPXdoc* createGPXdoc(char* fileName) {

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    LIBXML_TEST_VERSION

    doc = xmlReadFile(fileName, NULL, 0);

    if (fileName == NULL) {
        return NULL;
    }

    if (doc == NULL) {
        free(doc);
        return(NULL);
    }

    GPXdoc* gdoc = malloc(sizeof(GPXdoc));

    root_element = xmlDocGetRootElement(doc);

    gdoc->creator = NULL;
    gdoc->version = 0;
    gdoc->namespace[0] = '\0';

    xmlNs* tempNs;
    tempNs = (xmlNs*) root_element->ns;
    strcpy(gdoc->namespace, (char*) tempNs->href);

    xmlNode *cur_node = root_element;
    xmlAttr *attr;

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {

        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);

        if (strcmp(attrName, "version") == 0) {
            gdoc->version = strtod(cont, &cont);
        }

        if (strcmp(attrName, "creator") == 0) {
            gdoc->creator = malloc(strlen(cont)+1);
            strcpy(gdoc->creator, cont);
        }
    }

    if (gdoc->creator == NULL) {
        gdoc->creator = malloc(1);
        strcpy(gdoc->creator, "\0");
    }

    List* waypointList = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    List* routeList = initializeList(&routeToString, &deleteRoute, &compareWaypoints);
    List* trackList = initializeList(&trackToString, &deleteTrack, &compareTracks);

    cur_node = NULL;
    for (cur_node = root_element->children; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (strcmp((char*)cur_node->name, "wpt") == 0) {
                insertBack(waypointList, createWaypoint(cur_node));
            } else if (strcmp((char*)cur_node->name, "rte") == 0) {
                insertBack(routeList, createRoute(cur_node));
            } else if (strcmp((char*)cur_node->name, "trk") == 0) {
                insertBack(trackList, createTrack(cur_node));
            }
        }
    }

    gdoc->waypoints = waypointList;
    gdoc->routes = routeList;
    gdoc->tracks = trackList;

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return(gdoc);
}


GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile) {

    if (fileName == NULL || gpxSchemaFile == NULL) {
        return(NULL);
    }

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    LIBXML_TEST_VERSION

    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        return NULL;
    }

    if (validateXML(doc, gpxSchemaFile) == false) {
        free(doc);
        return NULL;
    }

    GPXdoc* gdoc = malloc(sizeof(GPXdoc));

    root_element = xmlDocGetRootElement(doc);

    gdoc->creator = NULL;
    gdoc->version = 0;
    gdoc->namespace[0] = '\0';

    xmlNs* tempNs;
    tempNs = (xmlNs*) root_element->ns;
    strcpy(gdoc->namespace, (char*) tempNs->href);

    xmlNode *cur_node = root_element;
    xmlAttr *attr;

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {

        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);

        if (strcmp(attrName, "version") == 0) {
            gdoc->version = strtod(cont, &cont);
        }

        if (strcmp(attrName, "creator") == 0) {
            gdoc->creator = malloc(strlen(cont)+1);
            strcpy(gdoc->creator, cont);
        }
    }

    if (gdoc->creator == NULL) {
        gdoc->creator = malloc(1);
        strcpy(gdoc->creator, "\0");
    }

    List* waypointList = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    List* routeList = initializeList(&routeToString, &deleteRoute, &compareWaypoints);
    List* trackList = initializeList(&trackToString, &deleteTrack, &compareTracks);

    cur_node = NULL;
    for (cur_node = root_element->children; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (strcmp((char*)cur_node->name, "wpt") == 0) {
                insertBack(waypointList, createWaypoint(cur_node));
            } else if (strcmp((char*)cur_node->name, "rte") == 0) {
                insertBack(routeList, createRoute(cur_node));
            } else if (strcmp((char*)cur_node->name, "trk") == 0) {
                insertBack(trackList, createTrack(cur_node));
            }
        }
    }

    gdoc->waypoints = waypointList;
    gdoc->routes = routeList;
    gdoc->tracks = trackList;

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return(gdoc);

}

bool writeGPXdoc(GPXdoc* doc, char* fileName) {
    if (doc == NULL || fileName == NULL) {
        return false;
    }


    xmlDocPtr xDoc;
    int i;
    xDoc = GPXdocToXmlDoc(doc);
    i = xmlSaveFormatFileEnc(fileName, xDoc, "UTF-8", 1);

    xmlFreeDoc(xDoc);

    if (i == -1) {
        return false;
    }

    return true;

}

bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile) {

    if (doc == NULL || gpxSchemaFile == NULL) {
        return false;
    }

    ListIterator gpxDataIterator;
    ListIterator waypointIterator;
    ListIterator routeIterator;
    ListIterator trackSegmentIterator;
    ListIterator trackIterator;

    GPXData* tempGPXData;
    Waypoint* tempWaypoint;
    Route* tempRoute;
    TrackSegment* tempTrackSegment;
    Track* tempTrack;

    bool validFile = true;

    xmlDocPtr xDoc;
    xDoc = GPXdocToXmlDoc(doc);

    if (doc->creator == NULL) {
        validFile = false;
        //printf("A\n");
    }

    if (strcmp(doc->namespace, "") == 0) {
        validFile = false;
        //printf("B\n");
    }

    if (doc->waypoints == NULL ||doc->routes == NULL ||doc->tracks == NULL) {
        validFile = false;
        //printf("C\n");
    }

    waypointIterator = createIterator(doc->waypoints);
    for(tempWaypoint = nextElement(&waypointIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&waypointIterator)) {
        if (tempWaypoint->name == NULL) {
            validFile = false;
            //printf("D\n");
        }

        gpxDataIterator = createIterator(tempWaypoint->otherData);
        for (tempGPXData = nextElement(&gpxDataIterator); tempGPXData != NULL; tempGPXData = nextElement(&gpxDataIterator)) {
            if (strcmp(tempGPXData->name, "") == 0 || strcmp(tempGPXData->value, "") == 0) {
                validFile = false;
                //printf("E\n");
            }
        }

    }

    routeIterator = createIterator(doc->routes);
    for(tempRoute = nextElement(&routeIterator); tempRoute != NULL; tempRoute = nextElement(&routeIterator)) {

        if (tempRoute->name == NULL) {
            validFile = false;
            //printf("F\n");
        }

        waypointIterator = createIterator(tempRoute->waypoints);
        for(tempWaypoint = nextElement(&waypointIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&waypointIterator)) {
            if (tempWaypoint->name == NULL) {
                validFile = false;
                //printf("G\n");
            }

            gpxDataIterator = createIterator(tempWaypoint->otherData);
            for (tempGPXData = nextElement(&gpxDataIterator); tempGPXData != NULL; tempGPXData = nextElement(&gpxDataIterator)) {
                if (strcmp(tempGPXData->name, "") == 0 || strcmp(tempGPXData->value, "") == 0) {
                    validFile = false;
                   // printf("H\n");
                }
            }

        }

        gpxDataIterator = createIterator(tempRoute->otherData);
        for (tempGPXData = nextElement(&gpxDataIterator); tempGPXData != NULL; tempGPXData = nextElement(&gpxDataIterator)) {
            if (strcmp(tempGPXData->name, "") == 0 || strcmp(tempGPXData->value, "") == 0) {
                validFile = false;
                //printf("The name and value: %s, %s",tempGPXData->name, tempGPXData->value );
                //printf("I\n");
            }
        }


    }


    trackIterator = createIterator(doc->tracks);
    for (tempTrack = nextElement(&trackIterator); tempTrack != NULL; tempTrack = nextElement(&trackIterator)) {
        if (tempTrack->name == NULL) {
            validFile = false;
        }


        trackSegmentIterator = createIterator(tempTrack->segments);
        for(tempTrackSegment = nextElement(&trackSegmentIterator); tempTrackSegment != NULL; tempTrackSegment = nextElement(&trackSegmentIterator)) {
            waypointIterator = createIterator(tempTrackSegment->waypoints);
            for(tempWaypoint = nextElement(&waypointIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&waypointIterator)) {
                if (tempWaypoint->name == NULL) {
                    validFile = false;
                }

                gpxDataIterator = createIterator(tempWaypoint->otherData);
                for (tempGPXData = nextElement(&gpxDataIterator); tempGPXData != NULL; tempGPXData = nextElement(&gpxDataIterator)) {
                    if (strcmp(tempGPXData->name, "") == 0 || strcmp(tempGPXData->value, "") == 0) {
                        validFile = false;
                    }
                }
            }
        }

        gpxDataIterator = createIterator(tempTrack->otherData);
        for (tempGPXData = nextElement(&gpxDataIterator); tempGPXData != NULL; tempGPXData = nextElement(&gpxDataIterator)) {
            if (strcmp(tempGPXData->name, "") == 0 || strcmp(tempGPXData->value, "") == 0) {
                validFile = false;
                //printf("The name and value: %s, %s",tempGPXData->name, tempGPXData->value );
            }
        }

    }

    if (validateXML(xDoc, gpxSchemaFile) == false) {
        validFile = false;
        //printf("Validation failed inside validateGPX\n");
    }

    xmlFreeDoc(xDoc);

    if (validFile == false) {

        return false;
    }

    return true;

}




void deleteGPXdoc (GPXdoc* doc) {

    if (doc == NULL) {
        return;
    }

    free(doc->creator);
    freeList(doc->waypoints);
    freeList(doc->routes);
    freeList(doc->tracks);
    free(doc);

}

char* GPXdocToString(GPXdoc* doc) {

    if (doc == NULL) {
        return NULL;
    }

    char* text;
    char* creatorTemp;
    char* versionTemp;
    char* namespaceTemp;
    char* waypointTemp;
    char* routeTemp;
    char* trackTemp;
    int len;

    creatorTemp = malloc(strlen(doc->creator) + 19);
    sprintf(creatorTemp, "The creator is %s\n", doc->creator);
    versionTemp = malloc(25);
    sprintf(versionTemp, "The version is %.1f\n", doc->version);
    namespaceTemp = malloc(256);
    sprintf(namespaceTemp, "The namespace is %s\n", doc->namespace);
    waypointTemp = toString(doc->waypoints);
    routeTemp = toString(doc->routes);
    trackTemp = toString(doc->tracks);


    len = strlen(waypointTemp) + strlen(routeTemp) + strlen(creatorTemp) + strlen(versionTemp) + strlen(trackTemp) + strlen(namespaceTemp);
    text = malloc(len + 100);

    text[0] = '\0';

    strcat(text, creatorTemp);
    strcat(text, versionTemp);
    strcat(text, namespaceTemp);
    strcat(text, waypointTemp);
    strcat(text, routeTemp);
    strcat(text, trackTemp);
    strcat(text, "\n");

    free(creatorTemp);
    free(versionTemp);
    free(namespaceTemp);
    free(waypointTemp);
    free(routeTemp);
    free(trackTemp);

    return(text);
}

int getNumWaypoints(const GPXdoc* doc) {
    int numWaypoints = 0;

    if (doc == NULL) {
        return 0;
    }

    numWaypoints = getLength(doc->waypoints);

    return(numWaypoints);
}

//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc) {
    int numRoutes = 0;

    if (doc == NULL) {
        return 0;
    }

    numRoutes = getLength(doc->routes);

    return(numRoutes);
}

//Total number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc) {
    int numTracks = 0;

    if (doc == NULL) {
        return 0;
    }

    numTracks = getLength(doc->tracks);

    return(numTracks);
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc) {

    if (doc == NULL) {
        return 0;
    }

    int numSegments = 0;
    Track* tempTrack;

    ListIterator tempIterator = createIterator(doc->tracks);

    for (tempTrack = nextElement(&tempIterator); tempTrack != NULL; tempTrack = nextElement(&tempIterator)) {
        numSegments+= getLength(tempTrack->segments);
    }

    return(numSegments);

}

//Total number of GPXData elements in the document
int getNumGPXData(const GPXdoc* doc) {

    if (doc == NULL) {
        return 0;
    }

    int numGPXData = 0;
    Waypoint* tempWaypoint;
    Track* tempTrack;
    Route* tempRoute;
    TrackSegment* tempTrackSegment;
    ListIterator tempIterator;
    ListIterator tempIterator2;
    ListIterator tempIterator3;

    tempIterator = createIterator(doc->waypoints);

    for(tempWaypoint = nextElement(&tempIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&tempIterator)) {
        numGPXData+= getLength(tempWaypoint->otherData);

        if (strcmp(tempWaypoint->name, "\0") != 0) {
            numGPXData++;
        }

    }

    tempIterator = createIterator(doc->routes);

    for(tempRoute = nextElement(&tempIterator); tempRoute != NULL; tempRoute = nextElement(&tempIterator)) {
        tempIterator2 = createIterator(tempRoute->waypoints);
        for(tempWaypoint = nextElement(&tempIterator2); tempWaypoint != NULL; tempWaypoint = nextElement(&tempIterator2)) {
            numGPXData+= getLength(tempWaypoint->otherData);

            if (strcmp(tempWaypoint->name, "\0") != 0) {
                numGPXData++;
            }
        }
        numGPXData+= getLength(tempRoute->otherData);

        if (strcmp(tempRoute->name, "\0") != 0) {
            numGPXData++;
        }

    }

    tempIterator = createIterator(doc->tracks);

    for (tempTrack = nextElement(&tempIterator); tempTrack != NULL; tempTrack = nextElement(&tempIterator)) {
        tempIterator2 = createIterator(tempTrack->segments);
        for(tempTrackSegment = nextElement(&tempIterator2); tempTrackSegment != NULL; tempTrackSegment = nextElement(&tempIterator2)) {
            tempIterator3 = createIterator(tempTrackSegment->waypoints);
            for(tempWaypoint = nextElement(&tempIterator3); tempWaypoint != NULL; tempWaypoint = nextElement(&tempIterator3)) {
                numGPXData += getLength(tempWaypoint->otherData);
                if (strcmp(tempWaypoint->name, "\0") != 0) {
                    numGPXData++;
                }
            }
        }
        numGPXData+= getLength(tempTrack->otherData);

        if (strcmp(tempTrack->name, "\0") != 0) {
            numGPXData++;
        }
    }

    return(numGPXData);
}

// Function that returns a waypoint with the given name.  If more than one exists, return the first one.
// Return NULL if the waypoint does not exist
Waypoint* getWaypoint(const GPXdoc* doc, char* name) {

    if (doc == NULL || name == NULL) {
        return 0;
    }

    Waypoint* tempWaypoint = NULL;
    ListIterator tempIterator;

    tempIterator = createIterator(doc->waypoints);

    for(tempWaypoint = nextElement(&tempIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&tempIterator)) {
        if (strcmp(tempWaypoint->name, name) == 0) {
            return(tempWaypoint);
        }
    }

    return(tempWaypoint);
}
// Function that returns a track with the given name.  If more than one exists, return the first one.
// Return NULL if the track does not exist
Track* getTrack(const GPXdoc* doc, char* name) {

    if (doc == NULL || name == NULL) {
        return NULL;
    }

    Track* tempTrack;
    ListIterator tempIterator;

    tempIterator = createIterator(doc->tracks);

    for(tempTrack = nextElement(&tempIterator); tempTrack != NULL; tempTrack = nextElement(&tempIterator)) {
        if (strcmp(tempTrack->name, name) == 0) {
            return(tempTrack);
        }
    }

    return(tempTrack);
}
// Function that returns a route with the given name.  If more than one exists, return the first one.
// Return NULL if the route does not exist
Route* getRoute(const GPXdoc* doc, char* name) {

    if (doc == NULL || name == NULL) {
        return NULL;
    }

    Route* tempRoute;
    ListIterator tempIterator;

    tempIterator = createIterator(doc->routes);

    for(tempRoute= nextElement(&tempIterator); tempRoute != NULL; tempRoute = nextElement(&tempIterator)) {
        if (strcmp(tempRoute->name, name) == 0) {
            return(tempRoute);
        }
    }
    return(tempRoute);
}


char* GPXFiletoJSON(char* gpxFile, char* xsdFile) {
  GPXdoc* doc = malloc(sizeof(GPXdoc));
  doc = createValidGPXdoc(gpxFile, xsdFile);
  char* string;
  string = GPXtoJSON(doc);
  deleteGPXdoc(doc);
  return(string);
}

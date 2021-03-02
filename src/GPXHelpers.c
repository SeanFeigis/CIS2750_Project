#include "GPXParser.h"
#include "LinkedListAPI.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

TrackSegment* createTrackSegment(xmlNode* node);
void createxmlWaypoint(Waypoint* tempWaypoint, xmlNodePtr root_node);

void deleteGpxData( void* data) {

    if (data == NULL){
		return;
	}
    free(data);
}

char* gpxDataToString( void* data) {
    char* tempStr;
    GPXData* tempData;  

    if (data == NULL){
		return NULL;
	}

    tempData = (GPXData*)data;
    tempStr = malloc(256+15+strlen(tempData->name));
    sprintf(tempStr, "Name: %s, Data: %s", tempData->name, tempData->value);

    return(tempStr);
}

int compareGpxData(const void *first, const void *second) {
    return 0;
}


void deleteWaypoint(void* data) {
    Waypoint* tmpWaypoint;
	
	if (data == NULL){
		return;
	}
	
	tmpWaypoint = (Waypoint*)data;
	
	free(tmpWaypoint->name);
	freeList(tmpWaypoint->otherData);
	free(tmpWaypoint);
}
    
char* waypointToString(void* data) {
    Waypoint* tmpWaypoint;
    char* tmpStr;
    char* temp;
    int len;

	if (data == NULL){
		return NULL;
	}
	
	tmpWaypoint = (Waypoint*)data;
    temp = toString(tmpWaypoint->otherData);
    len = 60 + strlen(tmpWaypoint->name) + strlen(temp);
	tmpStr = (char*)malloc(len);
	
	sprintf(tmpStr, "Name: %s, Lon: %.6f, Lat: %.6f\n Other Data: %s", tmpWaypoint->name, tmpWaypoint->longitude, tmpWaypoint->latitude, temp);
	free(temp);

	return tmpStr;
}

int compareWaypoints(const void* first, const void *second) {
    return 0;
}

void deleteRoute(void* data) {
    Route *tmpRoute;

    if (data == NULL){
		return;
    }

    tmpRoute = (Route*)data;

    free(tmpRoute->name);
    freeList(tmpRoute->waypoints);
    freeList(tmpRoute->otherData);
    free(tmpRoute);
}

char* routeToString(void* data) {
    char* tmpString;
    Route *tmpRoute;
    char* waypointsTemp;
    char* otherDataTemp;
    int len;

    if (data == NULL){
		return NULL;
    }

    tmpRoute = (Route*)data;
    waypointsTemp = toString(tmpRoute->waypoints);
    otherDataTemp = toString(tmpRoute->otherData);
    len = 48 + strlen(tmpRoute->name) + strlen(waypointsTemp) + strlen(otherDataTemp);
    tmpString = (char*)malloc(len);
    sprintf(tmpString, "\n\nRoute Name: %s\n\n  Otherdata: %s\n\n  WaypointData: %s", tmpRoute->name, otherDataTemp, waypointsTemp);
    free(waypointsTemp);
    free(otherDataTemp);

    return(tmpString);
}

int compareRoutes(const void* first, const void* second) {
    return 0;
}

void deleteTrackSegment(void * data) {
    TrackSegment* tmpSegment; 

    if (data == NULL){
		return;
    }

    tmpSegment = (TrackSegment*)data;

    freeList(tmpSegment->waypoints);
    free(tmpSegment);
}

char* trackSegmentToString(void* data) {
    char* tmpString;
    char* waypointsTemp; 
    TrackSegment* tmpSegment;
    int len;

    if (data == NULL){
		return NULL;
    }

    tmpSegment = (TrackSegment*)data;
    waypointsTemp = toString(tmpSegment->waypoints);
    len = 15 + strlen(waypointsTemp);
    tmpString = malloc(len);
    sprintf(tmpString, "\n Waypoints: %s\n", waypointsTemp);
    free(waypointsTemp);

    return(tmpString);
}

int compareTrackSegments(const void *first, const void *second) {
    return 0;
}

void deleteTrack(void* data) {

    Track* tmpTrack; 

    if (data == NULL){
		return;
    }

    tmpTrack = (Track*)data;

    free(tmpTrack->name);
    freeList(tmpTrack->segments);
    freeList(tmpTrack->otherData);
    free(tmpTrack);
}

char* trackToString(void* data) {

    Track* tmpTrack;
    char* tmpString;
    char* tmpSegment;
    char* otherDataTemp;

    if (data == NULL){
		return NULL;
    }

    tmpTrack = (Track*)data;

    tmpSegment = toString(tmpTrack->segments);
    otherDataTemp = toString(tmpTrack->otherData);
    int len = 66 + strlen(tmpSegment) + strlen(otherDataTemp) + strlen(tmpTrack->name); 
    tmpString = (char*)malloc(len);
    sprintf(tmpString, "\n\nTrack Name: %s\n Track Segments: %s\n  Other Data of Track: %s\n", tmpTrack->name, tmpSegment, otherDataTemp);
    free(tmpSegment);
    free(otherDataTemp);

    return(tmpString);
}

int compareTracks(const void *first, const void *second) {
    return 0;
}

Waypoint* createWaypoint(xmlNode* node) {

    Waypoint* tmpWaypoint = malloc(sizeof(Waypoint));
    tmpWaypoint->name = NULL;
    int len = 0;

    xmlAttr *attr;
    for (attr = node->properties; attr != NULL; attr = attr->next)
    {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        if (strcmp(attrName, "lon") == 0) {
            tmpWaypoint->longitude = strtod(cont, &cont);
        } else if (strcmp(attrName, "lat") == 0) {
            tmpWaypoint->latitude = strtod(cont, &cont);
        }
    }

    tmpWaypoint->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    if (node->children!=NULL) {
        node = node->children;

        xmlNode *cur_node = NULL;
        for (cur_node = node; cur_node != NULL; cur_node = cur_node->next) {
            if (cur_node->type == XML_ELEMENT_NODE) {
                if (strcmp((char*) cur_node->name, "name") == 0) { 
                    char* tempContent = (char*)xmlNodeGetContent(cur_node);
                    len = strlen(tempContent) + 1;
                    tmpWaypoint->name = (char*)malloc(len);
                    strcpy(tmpWaypoint->name, tempContent); 
                    free(tempContent);
                } else { 
                    char* tempContent = (char*)xmlNodeGetContent(cur_node);
                    if (tempContent == NULL) {
                        break;
                    }
                    len = sizeof(GPXData) + ((strlen(tempContent) + 1) * sizeof(char));
                    GPXData* tmpData = (GPXData*) malloc(len);
                    strcpy(tmpData->name, (char*)cur_node->name);
                    strcpy(tmpData->value, tempContent);
                    free(tempContent);
                    insertBack(tmpWaypoint->otherData, tmpData);
                }
            }
        }
    }

    if (tmpWaypoint->name == NULL) {
        tmpWaypoint->name = malloc(1);
        strcpy(tmpWaypoint->name, "\0");
    }
    return(tmpWaypoint);
}

Route* createRoute(xmlNode* node) {

    Route* tmpRoute = malloc(sizeof(Route));
    tmpRoute->name = NULL;
    tmpRoute->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    tmpRoute->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    xmlNode *a_node = node->children;
    xmlNode *cur_node;
    int len = 0;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            //printf("node type: Element, name: %s\n", cur_node->name);
            if (strcmp((char*) cur_node->name, "name") == 0) { 
                char* tempContent = (char*)xmlNodeGetContent(cur_node);
                len = strlen(tempContent) + 1;
                tmpRoute->name = (char*)malloc(len);
                strcpy(tmpRoute->name, tempContent); 
                free(tempContent);
            } else if (strcmp((char*) cur_node->name, "rtept") == 0) {
                insertBack(tmpRoute->waypoints, createWaypoint(cur_node));
            } else {
                char* tempContent = (char*)xmlNodeGetContent(cur_node);
                if (tempContent == NULL) {
                    break;
                }
                len = sizeof(GPXData) + ((strlen(tempContent) + 1) * sizeof(char));
                GPXData* tmpData = (GPXData*) malloc(len);
                strcpy(tmpData->name, (char*)cur_node->name);
                strcpy(tmpData->value, tempContent);
                free(tempContent);
                insertBack(tmpRoute->otherData, tmpData);
            }
        }
    }

    if (tmpRoute->name == NULL) {
        tmpRoute->name = malloc(1);
        strcpy(tmpRoute->name, "\0");
    }
    return(tmpRoute);
}

Track* createTrack(xmlNode* node) {

    Track* tmpTrack = malloc(sizeof(Track));
    tmpTrack->name = NULL;
    tmpTrack->segments = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
    tmpTrack->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    xmlNode *a_node = node->children;
    xmlNode *cur_node;
    int len = 0;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            //printf("node type: Element, name: %s\n", cur_node->name);
            if (strcmp((char*) cur_node->name, "name") == 0) { 
                char* tempContent = (char*)xmlNodeGetContent(cur_node);
                len = strlen(tempContent) + 1;
                tmpTrack->name = (char*)malloc(len);
                strcpy(tmpTrack->name, tempContent); 
                free(tempContent);
            } else if (strcmp((char*) cur_node->name, "trkseg") == 0) {
                insertBack(tmpTrack->segments, createTrackSegment(cur_node));
            } else {
                char* tempContent = (char*)xmlNodeGetContent(cur_node);
                if (tempContent == NULL) {
                    break;
                }
                len = sizeof(GPXData) + ((strlen(tempContent) + 1) * sizeof(char));
                GPXData* tmpData = (GPXData*) malloc(len);
                strcpy(tmpData->name, (char*)cur_node->name);
                strcpy(tmpData->value, tempContent);
                free(tempContent);
                insertBack(tmpTrack->otherData, tmpData);
            }
        }
    }

    if (tmpTrack->name == NULL) {
        tmpTrack->name = malloc(1);
        strcpy(tmpTrack->name, "\0");
    }

    return tmpTrack;

}

TrackSegment* createTrackSegment(xmlNode* node) {

    TrackSegment* tmpTrackSegment = malloc(sizeof(TrackSegment));
    tmpTrackSegment->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);

    xmlNode *a_node = node->children;
    xmlNode *cur_node;

     for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            //printf("node type: Element, name: %s\n", cur_node->name);
            if (strcmp((char*) cur_node->name, "trkpt") == 0) {
                insertBack(tmpTrackSegment->waypoints, createWaypoint(cur_node));
            }
        }
    }

    return(tmpTrackSegment);
}


xmlDoc* GPXdocToXmlDoc(GPXdoc* doc) {
    //Partial Code used from http://www.xmlsoft.org/examples/tree2.c

    xmlDocPtr docPtr = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL;/* node pointers */
    Waypoint* tempWaypoint;
    GPXData* tempGPXData;
    Route* tempRoute;
    Track* tempTrack;
    TrackSegment* tempTrackSegment;
    ListIterator tempIterator;
    ListIterator tempIterator2;
    ListIterator tempIterator3;
    LIBXML_TEST_VERSION;

    docPtr = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "root");

    xmlDocSetRootElement(docPtr, root_node);

    if (doc->creator != NULL) {
        xmlNewProp(root_node, BAD_CAST "creator", BAD_CAST doc->creator);
    } else {
        return (NULL);
    }

    char* versionTemp = malloc(256);
    sprintf(versionTemp, "%f.2", doc->version);
    xmlNewProp(root_node, BAD_CAST "creator", BAD_CAST versionTemp);
    free(versionTemp);

    xmlNsPtr nsPtr;
    if (doc->namespace != NULL) {
        nsPtr = xmlNewNs(root_node, (xmlChar*) (doc->namespace), NULL);
        xmlSetNs(root_node, nsPtr);
    }

    
    tempIterator = createIterator(doc->waypoints);
    for(tempWaypoint = nextElement(&tempIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&tempIterator)) {
        /*
        xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "wpt");
        xmlAddChild(root_node, node);

        char* positionTemp = malloc(256);
        sprintf(positionTemp, "%.6f", tempWaypoint->latitude);
        xmlNewProp(node, BAD_CAST "lat", BAD_CAST positionTemp);
        sprintf(positionTemp, "%.6f", tempWaypoint->longitude);
        xmlNewProp(node, BAD_CAST "lon", BAD_CAST positionTemp);
        free(positionTemp);
        xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST tempWaypoint->name);

        tempIterator2 = createIterator(tempWaypoint->otherData);
        for (tempGPXData = nextElement(&tempIterator2); tempGPXData != NULL; tempGPXData = nextElement(&tempIterator2)) {
            xmlNewChild(node, NULL, BAD_CAST tempGPXData->name, BAD_CAST tempGPXData->value);
        } */

        createxmlWaypoint(tempWaypoint, root_node);

    }

    tempIterator = createIterator(doc->routes);
    for(tempRoute = nextElement(&tempIterator); tempRoute != NULL; tempRoute = nextElement(&tempIterator)) {

        xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "rte");
        xmlAddChild(root_node, node);

        xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST tempRoute->name);

        tempIterator2 = createIterator(tempRoute->otherData);
        for (tempGPXData = nextElement(&tempIterator2); tempGPXData != NULL; tempGPXData = nextElement(&tempIterator2)) {
            xmlNewChild(node, NULL, BAD_CAST tempGPXData->name, BAD_CAST tempGPXData->value);
        }


        tempIterator2 = createIterator(tempRoute->waypoints);
        for(tempWaypoint = nextElement(&tempIterator); tempWaypoint != NULL; tempWaypoint = nextElement(&tempIterator)) {
            
            xmlNodePtr node2 = xmlNewNode(NULL, BAD_CAST "rtept");
            xmlAddChild(node, node2);

            char* positionTemp = malloc(256);
            sprintf(positionTemp, "%.6f", tempWaypoint->latitude);
            xmlNewProp(node2, BAD_CAST "lat", BAD_CAST positionTemp);
            sprintf(positionTemp, "%.6f", tempWaypoint->longitude);
            xmlNewProp(node2, BAD_CAST "lon", BAD_CAST positionTemp);

            xmlNewChild(node2, NULL, BAD_CAST "name", BAD_CAST tempWaypoint->name);

            tempIterator3 = createIterator(tempWaypoint->otherData);
            for (tempGPXData = nextElement(&tempIterator2); tempGPXData != NULL; tempGPXData = nextElement(&tempIterator2)) {
                xmlNewChild(node2, NULL, BAD_CAST tempGPXData->name, BAD_CAST tempGPXData->value);
            }

        }




       

    }


    xmlCleanupParser();
    xmlMemoryDump();
    return(docPtr);
}


void createxmlWaypoint(Waypoint* tempWaypoint, xmlNodePtr root_node) {
    ListIterator tempIterator;
    GPXData* tempGPXData;


    xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "wpt");
    xmlAddChild(root_node, node);

    char* positionTemp = malloc(256);
    sprintf(positionTemp, "%.6f", tempWaypoint->latitude);
    xmlNewProp(node, BAD_CAST "lat", BAD_CAST positionTemp);
    sprintf(positionTemp, "%.6f", tempWaypoint->longitude);
    xmlNewProp(node, BAD_CAST "lon", BAD_CAST positionTemp);
    free(positionTemp);
    xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST tempWaypoint->name);

    tempIterator = createIterator(tempWaypoint->otherData);
    for (tempGPXData = nextElement(&tempIterator); tempGPXData != NULL; tempGPXData = nextElement(&tempIterator)) {
        xmlNewChild(node, NULL, BAD_CAST tempGPXData->name, BAD_CAST tempGPXData->value);
    }


}



#include "GPXParser.h"
#include "LinkedListAPI.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

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

    tempStr = malloc(256+10);

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
    
    char* tmpStr;
	Waypoint* tmpWaypoint;
	
	if (data == NULL){
		return NULL;
	}
	
	tmpWaypoint = (Waypoint*)data;
    char* temp = toString(tmpWaypoint->otherData);
    int len = 65 + strlen(tmpWaypoint->name) + strlen(temp);
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

    if (data == NULL){
		return NULL;
    }

    tmpRoute = (Route*)data;
    char* waypointsTemp = toString(tmpRoute->waypoints);
    char* otherDataTemp = toString(tmpRoute->otherData);
    int len = 50 + strlen(tmpRoute->name) + strlen(waypointsTemp) + strlen(otherDataTemp);
    tmpString = (char*)malloc(len);
    sprintf(tmpString, "Route Name: %s\n\nWaypointData: %s\n\n, Otherdata: %s\n", tmpRoute->name, waypointsTemp, otherDataTemp);
    free(waypointsTemp);
    free(otherDataTemp);

    return(tmpString);
}

int compareRoutes(const void* first, const void* second) {
    return 0;
}

Waypoint* createWaypoint(xmlNode* node) {

    Waypoint* tmpWaypoint = malloc(sizeof(Waypoint));
    int len;

    xmlAttr *attr;
    for (attr = node->properties; attr != NULL; attr = attr->next)
    {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        //printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);

        if (strcmp(attrName, "lon") == 0) {
            tmpWaypoint->longitude = strtod(cont, &cont);
            //printf("Longitude saved as %s\n", cont);
        } else if (strcmp(attrName, "lat") == 0) {
            tmpWaypoint->latitude = strtod(cont, &cont);
            //printf("Latitude saved as %s\n", cont);
        }

    }

    tmpWaypoint->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    if (node->children!=NULL) {
        node = node->children;

        xmlNode *cur_node = NULL;
        //printf("Does it have children?\n");
        for (cur_node = node; cur_node != NULL; cur_node = cur_node->next) {
            if (cur_node->type == XML_ELEMENT_NODE) {
               // printf("Name of node: %s\n", (char*) cur_node->name);
                if (strcmp((char*) cur_node->name, "name") == 0) { 
                    char* tempContent = (char*)xmlNodeGetContent(cur_node);
                    len = strlen(tempContent) + 1;
                    tmpWaypoint->name = (char*)malloc(len);
                    strcpy(tmpWaypoint->name, tempContent); 
                    free(tempContent);
                } else {
                    GPXData* tmpData = malloc(sizeof(GPXData));
                    char* tempContent = (char*)xmlNodeGetContent(cur_node);
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

    tmpRoute* = malloc(sizeof(Route));
    tmpRoute->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    xmlNode *cur_node = node->children;

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
                
            } else {
                GPXData* tmpData = malloc(sizeof(GPXData));
                char* tempContent = (char*)xmlNodeGetContent(cur_node);
                strcpy(tmpData->name, (char*)cur_node->name);
                strcpy(tmpData->value, tempContent);
                free(tempContent);
                insertBack(tmpRoute->otherData, tmpData);
            }

        }
    }
}








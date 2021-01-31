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

    tempStr = malloc(50);

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
		
	tmpStr = (char*)malloc(100);
	
	sprintf(tmpStr, "Name: %s, Lon: %f, Lat: %f, %s", tmpWaypoint->name, tmpWaypoint->longitude, tmpWaypoint->latitude, toString(tmpWaypoint->otherData));
	
	return tmpStr;

}

int compareWaypoints(const void* first, const void *second) {
    return 0;
}

Waypoint* createWaypoint(xmlNode* node) {

    tmpWaypoint* = malloc(sizeof(Waypoint));
    int len;

    if (node->content != NULL ){
        len = strlen((char*)node->content));
        tmpWaypoint->name = (char*)malloc(len);
        strcpy(tmpWaypoint->name, (char*)node->content);
    } else {
        tmpWaypoint->name = malloc(1);
        strcpy(tmpWaypoint->name, "\0");
    }

    xmlAttr *attr;
    for (attr = node->properties; attr != NULL; attr = attr->next)
    {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);

        if (strcmp(attrName, "lon") == 0) {
            Waypoint->lon = strtod(cont, &cont)
        } else if (strcmp(attrName, "lon") == 0) {
            Waypoint->lat = strtod(cont, &cont)
        }

    }

    Waypoint->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    if (node->children!=NULL) {
        
    }

    return(Waypoint);
}







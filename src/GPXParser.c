#include "GPXParser.h"
#include "LinkedListAPI.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

void parse_doc(GPXdoc *gdoc, xmlNode * a_node, List* waypointList, List* routeList, List* trackList);
Waypoint* createWaypoint(xmlNode* node);
Route* createRoute(xmlNode* node);
Track* createTrack(xmlNode* node);


GPXdoc* createGPXdoc(char* fileName) {

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    GPXdoc* gdoc = malloc(sizeof(GPXdoc));
    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        return(NULL);
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

   /* xmlNs* tempNs;
    tempNs = (xmlNs*) doc->oldNs;

    printf("%s\n", (char*) tempNs->href);
    */
    gdoc->creator = malloc(256);
    
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
            strcpy(gdoc->creator, cont);
        }
    }

    List* waypointList = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    List* routeList = initializeList(&routeToString, &deleteRoute, &compareWaypoints);
    List* trackList = initializeList(&trackToString, &deleteTrack, &compareTracks);

    parse_doc(gdoc, root_element->children, waypointList, routeList, trackList);
    
    gdoc->waypoints = waypointList;
    gdoc->routes = routeList;
    gdoc->tracks = trackList;

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    return(gdoc);

}

void parse_doc(GPXdoc *gdoc, xmlNode * a_node, List* waypointList, List* routeList, List* trackList)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (strcmp((char*)cur_node->name, "wpt") == 0) {
                insertBack(waypointList, createWaypoint(cur_node));
                //printf("Encountered Waypoint\n");
            } else if (strcmp((char*)cur_node->name, "rte") == 0) {
                insertBack(routeList, createRoute(cur_node));
            } else if (strcmp((char*)cur_node->name, "trk") == 0) {
                insertBack(trackList, createTrack(cur_node));
                printf("Encountered Track\n");
            }
        }

    }
}


void deleteGPXdoc (GPXdoc* doc) {

    free(doc->creator);

    freeList(doc->waypoints);

    freeList(doc->routes);

    freeList(doc->tracks);

    free(doc);

}

char* GPXdocToString(GPXdoc* doc) {

    char* text; 
    char* creatorTemp;
    char* versionTemp;
    char* waypointTemp;
    char* routeTemp;
    char* trackTemp;
    int len;

    creatorTemp = malloc(strlen(doc->creator) + 19);
    sprintf(creatorTemp, "The creator is %s\n", doc->creator);
    versionTemp = malloc(25);
    sprintf(versionTemp, "The version is %.1f\n", doc->version);
    waypointTemp = toString(doc->waypoints);
    routeTemp = toString(doc->routes); 
    trackTemp = toString(doc->tracks);
    

    len = strlen(waypointTemp) + strlen(routeTemp) + strlen(creatorTemp) + strlen(versionTemp) + strlen(trackTemp);
    text = malloc(len + 100);

    text[0] = '\0';

    strcat(text, creatorTemp);
    strcat(text, versionTemp);
    strcat(text, waypointTemp);
    strcat(text, routeTemp);
    strcat(text, trackTemp);
    strcat(text, "\n");

    free(creatorTemp);
    free(versionTemp);
    free(waypointTemp);
    free(routeTemp);
    free(trackTemp);

    return(text);
}


#include "GPXParser.h"
#include "LinkedListAPI.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

void parse_doc(GPXdoc *gdoc, xmlNode * a_node, List* waypointList);
Waypoint* createWaypoint(xmlNode* node);


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
    //List* trackList = initializeList();
    //List* routeList = initializeList();

    parse_doc(gdoc, root_element->children, waypointList);
    
    gdoc->waypoints = waypointList;

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    return(gdoc);

}

void parse_doc(GPXdoc *gdoc, xmlNode * a_node, List* waypointList, List* routeList)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (strcmp((char*)cur_node->name, "wpt") == 0) {
                insertBack(waypointList, createWaypoint(cur_node));
                //printf("Encountered Waypoint\n");
            } else if (strcmp((char*)cur_node->name, "trk") == 0) {
                insertBack(routeList, createRoute(cur_node));
            } else if (strcmp((char*)cur_node->name, "rte") == 0) {

            }
        }

    }
}







void deleteGPXdoc (GPXdoc* doc) {

    free(doc->creator);

    freeList(doc->waypoints);

    free(doc);

}

char* GPXdocToString(GPXdoc* doc) {

    char* text = malloc(2000);
    char* temp = malloc(100);
    text[0] = '\0';

    sprintf(temp, "The creator is %s\n", doc->creator);
    strcat(text, temp);
    
    sprintf(temp, "The version is %.1f\n", doc->version);
    strcat(text, temp);
    
    char* temp2 = toString(doc->waypoints);
    strcat(text, temp2);
    strcat(text, "\n");

    free(temp);
    free(temp2);
    return(text);
}


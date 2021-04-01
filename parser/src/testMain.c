/**
 * section: Tree
 * synopsis: Navigates a tree to print element names
 * purpose: Parse a file to a tree, use xmlDocGetRootElement() to
 *          get the root element, then walk the document and print
 *          all the element name in document order.
 * usage: tree1 filename_or_URL
 * test: tree1 test2.xml > tree1.tmp && diff tree1.tmp $(srcdir)/tree1.res
 * author: Dodji Seketeli
 * copy: see Copyright for the status of this software.
 */
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "GPXParser.h"

/*
 *To compile this file using gcc you can type.  Alternatively, see the Makefile for include path settings
 *gcc `xml2-config --cflags --libs` -o xmlexample libxml2-example.c
 */

/**
 * Simple example to parse a file called "file.xml", 
 * walk down the DOM, and print the name of the 
 * xml elements nodes.
 */

xmlDoc* GPXdocToXmlDoc(GPXdoc* doc);
//static void print_element_names(xmlNode * a_node);

int
main(int argc, char **argv)
{

    if (argc != 3){
        printf("usage: testMain <someXMLfile> <someXSDfile\n");

        return(1);
    }

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION
    //xmlNode *root_element = NULL;
    GPXdoc *Gdoc;
    float len;
    Gdoc = createValidGPXdoc(argv[1], argv[2]);

    if (Gdoc == NULL) {
        return 0;
    }

    char* toString;
    toString = GPXdocToString(Gdoc);

    //printf(toString);

    free(toString);
    
    //printf("Rounding Testing: Unrounded %f: Rounded: %f\n", 105.0 ,round10(105.0));


    //printf("#Waypoints: %d\n#Routes: %d\n#Tracks: %d\n#TrackSegments: %d\n#GPXData: %d\n", 
    //getNumWaypoints(Gdoc), getNumRoutes(Gdoc), getNumTracks(Gdoc), getNumSegments(Gdoc), getNumGPXData(Gdoc));
    xmlDoc* xmlD;
    xmlD = GPXdocToXmlDoc(Gdoc);
    //root_element = xmlDocGetRootElement(xmlD);
    //print_element_names(root_element);


    if (writeGPXdoc(Gdoc, "testOutput.gpx") == true) {
        printf("GPX written to file succesfully\n");
    } else {
        printf("Write failed\n");
    }

    if (validateGPXDoc(Gdoc, argv[2]) == false ) {
        printf("Validation Failed\n");
    }


   

    len = 0.0;
    char* stringTemp;
    ListIterator routeIterator;
    Route* tempRoute;
    routeIterator = createIterator(Gdoc->routes);
    for(tempRoute = nextElement(&routeIterator); tempRoute != NULL; tempRoute = nextElement(&routeIterator)) {
        len+= getRouteLen(tempRoute);  
        stringTemp = routeToJSON(tempRoute);
        printf("%s\n", stringTemp);  
        if (isLoopRoute(tempRoute, 170.0)) {
            printf("%s is a Route!\n", tempRoute->name);
        } else {
            printf("%s is not a route!\n", tempRoute->name);
        }
        free(stringTemp);
    }

    printf("The len of the routes are %f \n", len);

    len = 0;
    ListIterator trackIterator;
    Track* tempTrack;
    trackIterator = createIterator(Gdoc->tracks);
    for(tempTrack = nextElement(&trackIterator); tempTrack != NULL; tempTrack = nextElement(&trackIterator)) {
        len+= getTrackLen(tempTrack);
        if (isLoopTrack(tempTrack, 340.0)) {
            printf("%s is a track!\n", tempTrack->name);
        } else {
            printf("%s is not a track!\n", tempTrack->name);
        }
    }

    printf("The len of the Tracks are %f \n", len);

    len = 160.0;
    float delta = 6.0;
    printf("The number of routes within %.1fm of %.1fm are %d \n",delta, len, numRoutesWithLength(Gdoc, len, delta));

    len = 330.0;
    delta = 1.0;
    printf("The number of tracks within %.1fm of %.1fm are %d \n",delta, len, numTracksWithLength(Gdoc, len, delta));

    char *tempChar3 = GPXtoJSON(Gdoc);
    printf("%s\n", tempChar3);
    free(tempChar3);

    tempChar3 = routeListToJSON(Gdoc->routes);
    printf("%s\n", tempChar3);
    free(tempChar3);

    tempChar3 = trackListToJSON(Gdoc->tracks);
    printf("%s\n", tempChar3);
    free(tempChar3);

    //getRoutesBetween(Gdoc, 44.595850, -123.620950, 44.597150, -123.620220, 15.0);

    GPXdoc* tempGPXdoc;
    char* gpxdocTemp = malloc(256);
    strcpy(gpxdocTemp, "{\"version\":1.1,\"creator\":\"WebTool\"}");
    tempGPXdoc = JSONtoGPX(gpxdocTemp);
    free(gpxdocTemp);
    printf("Version: %.1f, Creator: %s, namespace: %s\n", tempGPXdoc->version, tempGPXdoc->creator, tempGPXdoc->namespace);
    deleteGPXdoc(tempGPXdoc);

    Route* tempRte;
    char* charTEMPASD = malloc(256);
    strcpy(charTEMPASD, "{\"name\":\"Reynolds walk\"}");
    tempRte = JSONtoRoute(charTEMPASD);
    free(charTEMPASD);
    printf("The name of the route is: %s\n", tempRte->name);
    deleteRoute(tempRte);
    
    Waypoint* tempWpt;
    char* blasjd = malloc(256);
    strcpy(blasjd, "{\"lat\":43.537299,\"lon\":-80.218267}");
    tempWpt = JSONtoWaypoint(blasjd);
    free(blasjd);
    printf("The values of the wpt are %f, %f\n", tempWpt->latitude, tempWpt->longitude);
    deleteWaypoint(tempWpt);
    
    //Route* blah = malloc(sizeof(Route));
    //addRoute(Gdoc, blah);


    xmlFreeDoc(xmlD);
    deleteGPXdoc(Gdoc);

    return 0;
}

/*
static void print_element_names(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        // Uncomment the code below if you want to see the content of every node.

         if (cur_node->content != NULL ){
             printf("  content: %s\n", cur_node->content);
         }

        // Iterate through every attribute of the current node
        xmlAttr *attr;
        for (attr = cur_node->properties; attr != NULL; attr = attr->next)
        {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);
            printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
        }

        print_element_names(cur_node->children);
    }
}
*/



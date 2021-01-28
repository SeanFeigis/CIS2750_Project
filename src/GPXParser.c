#include "GPXParser.h"
#include <LinkedListAPI.h>
#include <stdlib.h>
#include <stdio.h>

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

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    return(gdoc);

}


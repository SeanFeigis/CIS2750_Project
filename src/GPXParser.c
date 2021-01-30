#include "GPXParser.h"
#include <LinkedListAPI.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

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


    gdoc->creator = malloc(256);
    

    xmlNode *cur_node = root_element;
    xmlAttr *attr;

    for (attr = cur_node->properties; attr != NULL; attr = attr->next)
        {
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
    
    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    return(gdoc);

}

void deleteGPXdoc (GPXdoc* doc) {

    free(doc->creator);

    free(doc);

}


/*
GPXdoc mainIterator(xmlNode * a_node) {

    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        // Uncomment the code below if you want to see the content of every node.

        // if (cur_node->content != NULL ){
        //     printf("  content: %s\n", cur_node->content);
        // }

        // Iterate through every attribute of the current node
        xmlAttr *attr;
        for (attr = cur_node->properties; attr != NULL; attr = attr->next)
        {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);
            printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
        }      

    }
    return(a_node);
}
*/

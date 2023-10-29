#define __USE_XOPEN
#define _GNU_SOURCE

//#define DEBUG

#include <stdio.h>
#include <libxml/parser.h>
#include <string.h>
#include <time.h>
#include <argp.h>

#include "pm2click.h"
#include "cmdline.h"



/* 
* Return struct slocalDn {elementName, mc, mnc} extracted from localDn:
*   "SubNetwork=MME-1,ManagedElement=cmm-1" for CMM
*   or
*   "MCC=xxx,MNC=xxx,ManagedElement=yyy"
*/
void
fillLocalDnStruct(char * localDn, struct structLocalDn * sLocalDn, char * elementType) {
    char *token = NULL;
    char *localBuf = malloc(sizeof(char) * (strlen(localDn)+1));

    // Init structure 
    strcpy(sLocalDn->mcc, "notset");
    strcpy(sLocalDn->mnc, "notset");

    token = strtok(localDn, "=,");
    while (token != NULL) {
        // Sgsn, MMe
        if (strstr(elementType, "Mme") != NULL || strstr(elementType, "Sgsn") != NULL) {
            if (strcmp(token, "SubNetwork") == 0) {
                strcpy(sLocalDn->elementName, strtok(NULL, ",="));
            }
        } else {
        // Pgw
            if (strcmp(token, "ManagedElement") == 0) {
                strcpy(sLocalDn->elementName, strtok(NULL, ",="));
            }
        }
        if (strcmp(token, "MCC") == 0) {
            char mcc[8] = "MCC=";
            strcpy(sLocalDn->mcc, strcat(mcc, strtok(NULL, ",=")));
        }
        if (strcmp(token, "MNC") == 0) {
            char mnc[8] = "MNC=";
            strcpy(sLocalDn->mnc, strcat(mnc, strtok(NULL, ",=")));
        }
        token = strtok(NULL, ",=");
    }
}


/*
* 2022-03-30T00:15:00+03:00 -> int 3
*/
char *getTzOffsetString(char *isoDateTime) {    struct
        tm *gmcal,
        tmIsoTime;

    time_t gmt_ts;
    int 
        hrOffset = 0,
        minOffset = 0;

    //char dateTimeUTC[25] = {0};
    char *dateTimeUTC = (char *) malloc(25);
    if (dateTimeUTC == NULL) return NULL;  
    //memset(dateTimeUTC, 0 , sizeof(dateTimeUTC));

    memset (&tmIsoTime,0,sizeof(struct tm));

    if (strlen(isoDateTime) != 25) {
        printf("Incorrect iso datetime format\n");

        return NULL;
    }

    if (strptime (isoDateTime, "%Y-%m-%dT%H:%M:%S%z", &tmIsoTime) == NULL) {
        printf("\n84: strptime failed: iso datetime: %s\n", isoDateTime);

        return NULL;
    }

    strftime(dateTimeUTC, 20, "%z", &tmIsoTime);

    return dateTimeUTC;
}


int getHrTzOffset(char *isoDateTime) {
    char *tzOffsetStr = getTzOffsetString(isoDateTime);
    if (tzOffsetStr == NULL) return -1;

    // Copy 3 bytes from "+0300" like string
    char arrHrOffsetBuf[4] = {0};
    strncpy(arrHrOffsetBuf, tzOffsetStr, 3);
    free(tzOffsetStr);

    int hrOffset = atoi(arrHrOffsetBuf);
    //printf("OFFSET (CHAR)---> %s\n", arrHrOffsetBuf);
    //printf("OFFSET (INT)---> %d\n", hrOffset);

    return hrOffset;
}


int getMinTzOffset(char *isoDateTime) {
    char *tzOffsetStr = getTzOffsetString(isoDateTime);
    if (tzOffsetStr == NULL) return -1;

    // Copy 3 bytes from "+0300" like string
    char arrMinOffsetBuf[4] = {0};
    strncpy(arrMinOffsetBuf, tzOffsetStr, 3);
    free(tzOffsetStr);

    int minOffset = atoi(arrMinOffsetBuf);
    printf("OFFSET (CHAR)---> %s\n", arrMinOffsetBuf);
    printf("OFFSET (INT)---> %d\n", minOffset);

    return minOffset;
}


/*
* Converts ISO8601 dateTime string to UTC: 
* e.g. 2022-03-30T00:15:00+03:00 -> 2022-03-29 22:15:00
*/
static int getDateTimeUTC(char *dateTimeUTC, char *isoDateTime) {
    struct tm *gmcal, tmIsoTime;
    time_t gmt_ts;

    memset(&tmIsoTime, 0, sizeof(struct tm));

    //2019-08-20T00:15:00+02:00
    if (strptime (isoDateTime, "%Y-%m-%dT%H:%M:%S%z", &tmIsoTime) == NULL) {
        printf("\nstrptime failed\n");

        return -1;
    }

    int tzOffset = getHrTzOffset(isoDateTime);

    tmIsoTime.tm_hour -= tzOffset;
    gmt_ts = mktime(&tmIsoTime);
    gmcal = localtime(&gmt_ts);
    strftime(dateTimeUTC, 20, "%Y-%m-%d %H:%M:%S", gmcal);

    return 0;
}


static void print_xml(xmlNode * node, char *customerName, FILE *ofile) {
    while (node) {
        if (node->type == XML_ELEMENT_NODE)  {
            if (strcmp (node->name, "fileSender") == 0)  {
                char 
                    *ptrLocalDn = xmlGetProp(node, "localDn"),
                    *ptrElementType = xmlGetProp(node, "elementType");

                if (ptrLocalDn != NULL) {
                    strcpy(localDn, ptrLocalDn);
                    free(ptrLocalDn);
                }
                if (ptrElementType != NULL) {
                    strcpy(elementType, ptrElementType);
                    free(ptrElementType);
                }

                fillLocalDnStruct(localDn, &sLocalDn, elementType);
            }

            if (strcmp (node->name, "managedElement") == 0)  {
                char 
                    *ptrSwVersion = xmlGetProp(node, "swVersion");

                if (ptrSwVersion != NULL) {
                    strcpy(swVersion, ptrSwVersion);
                    free(ptrSwVersion);
                }
            }

            if (strcmp (node->name, "measInfo") == 0)  {
                char * ptrMeasInfoId = xmlGetProp(node, "measInfoId");

                if (ptrMeasInfoId != NULL) {
                    strcpy(measInfoId, ptrMeasInfoId);
                    free(ptrMeasInfoId);
                } else {
                    strcpy(measInfoId, "notset");
                }
            }

            //granPeriod duration="PT900S" endTime="2022-03-30T00:15:00+03:00
            if (strcmp (node->name, "granPeriod") == 0)  {
                char 
                    * ptrEndTime = xmlGetProp(node, "endTime"),
                    * ptrDuration = xmlGetProp(node, "duration");

                if (ptrEndTime != NULL ) {
                    strcpy(endTime, ptrEndTime);
                    free(ptrEndTime);
                }

                if (ptrDuration != NULL) {
                    strcpy(duration, ptrDuration);
                    free(ptrDuration);
                }

                if (getDateTimeUTC(buf, endTime) != 0) {
                    printf("\ngetDateTimeUTC failed\n");
                }
            }

            //Getting counterName
            if (strcmp (node->name, "measType") == 0)  {
                char *ptrCounterName = xmlNodeGetContent(node);
                char *ptrP = xmlGetProp(node, "p");

                int index = 0;
                if (ptrP != NULL) {
                    index = atoi(ptrP);
                    free(ptrP);
                } else {
                    //Do exit(-1) to not avoid parser provide an incorect value 
                    fprintf (stderr, "Pointer=NULL to counter index");
                    exit(EXIT_FAILURE);
                }

                if (ptrCounterName != NULL) {
                    if (index >= MAX_COUNTERS_NUMBER) {
                        fprintf(stderr, "Counter index exeeds max counters buf[] capacity");
                        exit(EXIT_FAILURE);
                    }
                    if (sizeof(ptrCounterName) >= MAX_COUNTER_LEN) {
                        fprintf(stderr, "Counter char length  exeeds MAX_COUNTER_LEN");
                        exit(EXIT_FAILURE);
                    }
                    strcpy(tmpCountersArray[index], ptrCounterName);
                    free(ptrCounterName);
                }
            }

            // measobjLdn
            if (strcmp (node->name, "measValue") == 0)  {
                char * ptrMeasObjLdn = xmlGetProp(node, "measObjLdn");

                if (ptrMeasObjLdn != NULL) {
                    strcpy(measObjLdn, ptrMeasObjLdn);
                    free(ptrMeasObjLdn);
                }
            }

            //counterValue
            if (strcmp (node->name, "r") == 0)  {
                char *ptrP = xmlGetProp(node, "p");

                int indexValue = 0;
                if (ptrP != NULL) {
                    indexValue = atoi(ptrP);
                    free(ptrP);
                }

                char *tmpVal = xmlNodeGetContent(node);

                char date[12] = {0};
                strncpy(date, buf, 10);

                fprintf (ofile,"%s\t%s\t%s\t%s\t%s\t%s\t%i\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", 
                        buf, date, measInfoId, measObjLdn, tmpCountersArray[indexValue], tmpVal, 0, sLocalDn.elementName, elementType, swVersion, sLocalDn.mcc, sLocalDn.mnc, duration, customerName);

                free(tmpVal);
            }
        }

        print_xml(node->children, customerName, ofile);
        node = node->next;
    }
}


/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'c':
      arguments->customer_name = arg;
      break;

    case 'o':
      arguments->output_file = arg;
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num >= 2)
        /* Too many arguments. */
        argp_usage (state);

      arguments->args[state->arg_num] = arg;
      break;

    case ARGP_KEY_END:
      if (state->arg_num < 1)
        /* Not enough arguments. */
        argp_usage (state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };


int main (int argc, char *argv[]) {
  struct arguments arguments;

  /* Default values. */
  arguments.customer_name = "default";
  arguments.output_file = "-";

  /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
  argp_parse (&argp, argc, argv, 0, 0, &arguments);
#ifdef DEBUG
  fprintf (stderr,"DEBUG:\tCUSTOMER = %s\nFILE = %s\nOUTPUT = %s\n",
          arguments.customer_name, arguments.args[0], arguments.output_file);
#endif

    xmlDocPtr doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadFile(arguments.args[0], NULL, 0);

    if (doc == NULL) {
        fprintf(stderr, "Could not parse the XML file");
    }

    root_element = xmlDocGetRootElement(doc);
    if (arguments.output_file == "-") {
        print_xml(root_element, arguments.customer_name, stdout);
    }
    else {
        FILE *f = fopen(arguments.output_file, "w");
        if (f == NULL) {
            fprintf(stderr, "Error opening file!\n");
            exit(1);
        }
        else {
            fprintf(stderr,"We should write output to a file: %s!\n", arguments.output_file);
                print_xml(root_element, arguments.customer_name, f);
            fclose(f);
        }
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
}

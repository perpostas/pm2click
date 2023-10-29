/*
  cmd arguments related consts 
*/
const char *argp_program_version =
  "pm2click 1.0.1";

const char *argp_program_bug_address =
  "<alexey.panov@some-organization.org>";

static char args_doc[] = "PM-FILENAME";

static char doc[] =
  "pm2click -- a program that takes PM (Performance Measurement) XML as input, and converts it to text suitable for storing in Clickhouse DB";

static struct argp_option options[] = {
  {"customer", 'c', 0,    OPTION_ARG_OPTIONAL,  "Customer name - optional.\n\"default\" if not specified" },
  {"output",   'o', "FILE", 0,
   "Output to FILE instead of standard output" },
  { 0 }
};

struct arguments
{
  char *args[1];                /* args*/
  char *customer_name;          /* customer name */
  char *output_file;            /* file to put result*/
};


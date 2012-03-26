#include <stdio.h>

FILE *g_log = stderr;
const char *g_name;
int g_run = 1;
int g_verbose = 1;

void usage ()
{
  fprintf(stderr,
	  "Usage: %s [-dqv] [-c CONFIG] [IFACE ...]\n"
	  " -c CONFIG	Read config from file CONFIG\n"
	  " -d		"
	  "Debug : log to stderr and do not daemonize.\n",
	  " -q		Quiet : only log errors.\n",
	  " -v		"
	  "Verbose : log more information, use twice for even more.\n"
	  "You can specify one or more network interfaces to monitor.",
	  g_name);
  exit(1);
}

void not_implemented (const char *thing)
{
  fprintf(g_log, "%s%s%s: not implemented.\n", g_name,
	  thing ? ": " : "",
	  thing ? thing : "");
}

void read_config(const char *path)
{
  FILE *fp = fopen(path, "r");
  if (!fp)
    err(1, "%s", path);
  fclose(fp);
}

int main (int argc, char *argv[])
{
  g_name = argv[0];
  int ch;
  
  int daemonize = 1;
  while ((ch = getopt(argc, argv, "c:dqv"))) {
    switch (ch) {
    case 'c':
      read_config(optarg);
      break;
    case 'd':
      if (g_verbose)
	g_verbose = 3;
      daemonize = 0;
      break;
    case 'v':
      g_verbose++;
      break;
    case 'q':
      g_verbose = 0;
      break;
    default:
      fprintf(stderr, "%s: unknown option: -%c\n", g_name, ch);
      usage();
    }
  }

  if (!g_debug) {
    not_implemented();
    usage();
  }

  while (g_run) {
    int i = iface_count;
    
    while (i--) {
      
    }
    sleep(1);
  }
  return 0;
}

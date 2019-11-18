#if defined(WIN32) || defined(WIN64)

/*
 * Implementation of getopt for windows.
 * https://gist.github.com/superwills/5815344
 */
#ifndef GETOPT_H
#define GETOPT_H
extern int opterr;		/* if error message should be printed */
extern int optind;		/* index into parent argv vector */
extern int optopt;		/* character checked for validity */
extern int optreset;  	/* reset getopt  */
extern char* optarg;	/* argument associated with option */
int getopt(int nargc, char* const nargv[], const char* ostr);
#endif

#endif // _MS_VER

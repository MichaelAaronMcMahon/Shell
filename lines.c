// Reading through a file by line, non-copying enumerator.
//
// read_lines iterates through the contents of a file, passing
// each line it encounters to the specified use_lines function.
// The strings passed to the use_lines function are stored in
// the buffer itself, meaning they do not persist between calls.
// This approach makes fewer memory allocations, and can be
// advantageous when the data segments do not need to be stored.
//
// Note that this code divides the enumeration algorithm itself
// (the double loop that calls read and scans through the buffer)
// from the code that uses the lines. For simple programs, you
// may not need this degree of generality.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFLENGTH
#define BUFLENGTH 16
#endif

void read_lines(int fd, void (*use_line)(void *, char *), void *arg)
{
    int buflength = BUFLENGTH;
    char *buf = malloc(BUFLENGTH);

    int pos = 0;
    int bytes;
    int line_start;

    while ((bytes = read(fd, buf + pos, buflength - pos)) > 0) {
	if (DEBUG) printf("read %d bytes; pos=%d\n", bytes, pos);

	line_start = 0;
	int bufend = pos + bytes;

	while (pos < bufend) {
	    if (DEBUG) printf("start %d, pos %d, char '%c'\n", line_start, pos, buf[pos]);

	    if (buf[pos] == '\n') {
		// we found a line, starting at line_start ending before pos
		buf[pos] = '\0';
		use_line(arg, buf + line_start);

		line_start = pos + 1;
	    }
	    pos++;
	}

	// no partial line
	if (line_start == pos) {
	    pos = 0;
	
	// partial line
	// move segment to start of buffer and refill remaining buffer
	} else if (line_start > 0) {
	    int segment_length = pos - line_start;
	    memmove(buf, buf + line_start, segment_length);
	    pos = segment_length;

	    if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
	
	// partial line filling entire buffer
	} else if (bufend == buflength) {
	    buflength *= 2;
	    buf = realloc(buf, buflength);

	    if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
	}
    }
    
    // partial line in buffer after EOF
    if (pos > 0) {
	if (pos == buflength) {
	    buf = realloc(buf, buflength + 1);
	}
	buf[pos] = '\0';
	use_line(arg, buf + line_start);
    }

    free(buf);
	
    
}

void print_line(void *st, char *line)
{
    int *p = st;
    printf("%d: |%s|\n", *p, line);
    (*p)++;
}

void rev_line(void *st, char *line)
{
    int len = strlen(line);

    for (int i = len-1; i >= 0; i--) {
	putchar(line[i]);
    }
    putchar('\n');
}


/*int main(int argc, char **argv)
{
    char *fname = argc > 1 ? argv[1] : "test.txt";

    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
	perror(fname);
	exit(EXIT_FAILURE);
    }

    int n = 0;

    read_lines(fd, print_line, &n);
    //read_lines(fd, rev_line, NULL);
    


    return EXIT_SUCCESS;
}
*/
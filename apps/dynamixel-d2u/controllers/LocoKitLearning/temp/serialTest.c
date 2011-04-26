#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */
int open_port(void)
{
  int fd; /* File descriptor for the port */
  fd = open("/dev/com5", O_RDWR | O_NOCTTY | O_NDELAY);

  if (fd == -1)   {
	  printf("Open_port: Unable to open /dev/com5");
  }
  else {
	  struct termios options;

	  /*
	   * Get the current options for the port...
	   */
	  tcgetattr(fd, &options);
	  /*
	   * Set the baud rates to 9600...
	   */
	  cfsetispeed(&options, B9600);
	  cfsetospeed(&options, B9600);

	  /*
	   * Enable the receiver and set local mode...
	   */
	  options.c_cflag |= (CLOCAL | CREAD);

	  /*
	   * Set the new options for the port...
	   */
	  tcsetattr(fd, TCSANOW, &options);

	  //fcntl(fd, F_SETFL, 0);
	  char buffer[255];  /* Input buffer */
	  char *bufptr;      /* Current char in buffer */
	  int  nbytes;       /* Number of bytes read */
	  while (1) {
		  nbytes = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1);
		  if(nbytes!=-1) printf("nBytes=%i\n",nbytes);
		  //bufptr += nbytes;
		  //if (bufptr[-1] == '\n' || bufptr[-1] == '\r') {
		  //  break;
		  // }
	  }


  }
  return (fd);
}

int main() {
	open_port();
	return 0;
}

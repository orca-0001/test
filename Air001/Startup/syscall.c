#include <stdlib.h>
#include <reent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

extern char end[];              /*  end is set in the linker command 	*/
				/* file and is the end of statically 	*/
				/* allocated data (thus start of heap).	*/

static char *heap_ptr=0;		/* Points to current end of the heap.	*/

/************************** _sbrk_r *************************************
 * Support function. Adjusts end of heap to provide more memory to
 * memory allocator. Simple and dumb with no sanity checks.

 *  struct _reent *r -- re-entrancy structure, used by newlib to
 *                      support multiple threads of operation.
 *  ptrdiff_t nbytes -- number of bytes to add.
 *                      Returns pointer to start of new heap area.
 *
 *  Note:  This implementation is not thread safe (despite taking a
 *         _reent structure as a parameter).
 *         Since _s_r is not used in the current implementation, 
 *         the following messages must be suppressed.
 */

void * _sbrk_r(
    struct _reent *_s_r, 
    int nbytes)
{
	char  *base;		/*  errno should be set to  ENOMEM on error  */

	if (!heap_ptr) {	/*  Initialize if first time through.  */
		heap_ptr = end;
	}
	base = heap_ptr;	/*  Point to end of heap.  */
	heap_ptr += nbytes;	/*  Increase heap.  */
	
	return base;		/*  Return pointer to start of new heap area.  */
}
int link( char *cOld, char *cNew )
{
    return -1 ;
}

int _close( int file )
{
    return -1 ;
}

int _fstat( int file, struct stat *st )
{
    st->st_mode = S_IFCHR ;

    return 0 ;
}

int _isatty( int file )
{
    return 1 ;
}

int _lseek( int file, int ptr, int dir )
{
    return 0 ;
}

int _read(int file, char *ptr, int len)
{
    return 0 ;
}

 int _write( int file, char *ptr, int len )
{
  int iIndex=0 ;

  switch ( file )
  {
    case 1 /*STDOUT_FILENO*/:
      for ( ; len >= 0 ; len--, ptr++, iIndex++ )
      {
        // Check if the transmitter is ready
        // while ((UART->UART_SR & UART_SR_TXRDY) != UART_SR_TXRDY)
        {
        }

        // Send character
        // UART->UART_THR = *ptr;
      }
    break;

    default:
      errno = EBADF ;
      iIndex = -1 ;
    break;
  }

  return iIndex ;
}



void _kill( int pid, int sig )
{
    return ;
}

int _getpid ( void )
{
    return -1 ;
}
void abort(void)
{
	while(1);
}

void    _exit(int n){
         while(1){
n = n;
 }   
} 



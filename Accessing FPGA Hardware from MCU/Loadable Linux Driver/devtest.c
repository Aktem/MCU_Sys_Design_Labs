#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
          
int main() {
  unsigned int result;
  int fd;                       /* file descriptor */
  int i,j;                      /* loop variables */
  
  int w_temp[2]; /* variable going to be written*/
  int r_temp[3]; /* variable being read */
  
  char input = 0;
  
  /*open device file for reading and writing*/
  /*use 'open' to open `/dev/multiplier'*/
  fd = open("/dev/multiplier", O_RDWR);
  /*Error Handling*/
  if(fd ==-1)
  {
	  printf("Failed to open device file.\n:");
	  return -1;
  }

  /*handle error opening file*/
  if(fd == -1){
    printf("Failed to open device file!\n");
    return -1;
  }


  while(input != 'q'){ /*continue unless user entered 'q'*/
    
    for(i=0; i<=16; i++){
      for(j=0; j<=16; j++){
        
        /*write values to registers using char dev*/
        /* use write to write i and j to peripheral */
        /*read i, j, and result using char dev*/
        /*use read to read from peripheral*/
        /*print unsigned ints to screen*/
		w_temp[0] = i;
		w_temp[1] = j;
		
		write(fd, w_temp, 8);
		read(fd, r_temp, 12);
        printf("%u * %u = %u ",r_temp[0],r_temp[1], r_temp[2]);
        
        /*validate result*/
        if(result == (i*j))
          printf("  Result Correct!");
        else
          printf("  Result Incorrect!");
        
        /*read from terminal*/
        input=getchar();
      }
    }          
  }
  close(fd);
  return 0;
}
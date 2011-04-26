#include <stdio.h>

main()
{
	FILE * pFile = fopen ("encoder.log","r");
	if (pFile!=NULL) {
	    printf("Jubii\n");
	    unsigned char a,b,c,d,e,f,t,g,h,i;
	    int nRead,index=0;
	    do {
	    	index++;
	    	nRead = fscanf(pFile, "%c%c%c%c%c%c%c%c%c", &a,&b,&c,&d,&e,&f,&t,&g,&h,&i);
	    	if(nRead==9) printf("%i: reward = %i\n",index,(unsigned int) 255*g+h);
	    } while(1);
	  }
	  else {
		  printf("Øv\n");
	  }
	  return 0;

}

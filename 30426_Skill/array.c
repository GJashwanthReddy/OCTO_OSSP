// array memory is allocated at compile time
#include <stdio.h>
#include <stdlib.h>

int main(){
   int n;
   int ar[10];
   
   printf("enter size of array\n");
   scanf("%d",&n);
   
   for(int i=0;i<n;i++){
      printf(" %d element\n",(i+1));
      scanf("%d",&ar[i]);
   }
      
      
   for(int i=0;i<n;i++){
      printf(" %d element is: %d\n",(i+1),ar[i]);
   }
   //free(p);
   return 0;
}

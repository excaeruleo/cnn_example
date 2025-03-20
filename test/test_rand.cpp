#include <stdio.h> 
#include <stdlib.h> 
  
int main() 
{ 
    // Generate a random number using the rand() function 
    int value = rand(); 
  
    // Print the generated random value 
    printf("The max Random Value is: %d\n", RAND_MAX); 
    printf("The Random Value is: %d\n", value); 
    printf("The Random Value is: %f\n", value*1.0/RAND_MAX); 
  
    return 0; 
}

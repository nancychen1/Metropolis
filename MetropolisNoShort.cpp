
// This function is found below.
void GetData ();

// "ticker" is a global variable.
char **ticker;

// The covariance matrix is a global variable.
double **V;


#include "Functions.h"
#include <iostream>

int main () {

   int i, seed, U, U2, s;
   double T, U3;
   double **e, **V_inverse, **min, **c, **x,**x2,
          **Varx, **Varx2, **minvar;   

   // Read in and display the tickers and covariance matrix.
   GetData (); 

   // Show the 50 tickers.
   for (i = 1; i <= 50; i++) {
      printf (ticker[i]);
   }
   Pause ();

   // Seed the RNG.
   seed = GetInteger ("RNG Seed?... ");
   MTUniform (seed);

    // Get the temperature parameter.
   printf ("\n");
   T = GetDouble ("What is the temperature?(best = .00001)... ");


   /////////////////////////////////////////////////////////////////////////////
   //Metropolis algorithm 
   //create starting portfolio of equal weights 
   x = Array(50,1);
   for (int i = 1; i<=50; i++){
      x[i][1]= 2.0; 
   }
   //initial energy is initial variance of x 
   Varx = Multiply(Multiply(Transpose(x), V), x);
   

   //prints header for output 
   printf ("         n       Min Var       \n");
   printf ("        ===     =========      \n");

   //begins simulations to find minimum variance portfolio 
   for (int n = 1; n<=10000000;n++ ){
      //holds whether or not there are short positions in the porfolio
      s = 0; 
      x2 = Copy(x);

     //U and U2 are random integers from 1 to 50
     //They will decide which indices of x to change 
      U = (MTUniform(0)*50)/1 + 1;
      U2 = (MTUniform(0)*50)/1 + 1; 
      while (U  == U2)
         U = (MTUniform(0)*50)/1 + 1;
      
      //randomly chooses which index to decrement by a tenth of a penny
      //and the other will be incremented by a tenth of a penny
      //if U3 is less than 0.5, decrements U, else decrements U2
      U3 = MTUniform(0);
      if (U3 < 0.5){
         x2[U][1] = x[U][1] - 0.001; 
         x2[U2][1] = x[U2][1] + 0.001; 
      }
      else{
         x2[U][1] = x[U][1] + 0.001; 
         x2[U2][1] = x[U2][1]- 0.001; 
      }
      //calculates variance of x2 with proposed transition 
      //if there are short positions then sets portfolio variance = 1000
      if (x2[U][1] < 0.0 || x2[U2][1] < 0.0 ){
         Varx2 = Array(1,1);
         Varx2[1][1]= 1000; 
         s = 1; 
      }
      else
         Varx2 = Multiply(Multiply(Transpose(x2), V), x2);
 

      //if variance of proposed transition is less than previous portfolio 
      //accept transition 
      if (Varx2[1][1] < Varx[1][1]){
         Free(x); 
         Free(Varx); 
         x = Copy(x2);
         Varx = Copy(Varx2); 
         
      }
      //small probability of accepting transition unless there is a short pos
      else{
         if (s!=1 && MTUniform(0) < exp(-(Varx2[1][1] - Varx[1][1] )/T) ){
            Free(x); 
            Free(Varx); 
            x = Copy(x2);
            Varx = Copy(Varx2); 
         }
      }
      
      Free(x2); 
      Free(Varx2); 

 
      //prints out minimum variance every 100,000 simulations
      if (n % 100000 == 0){
         printf ("%10.0i   %8.4f \n", n, Varx[1][1]);
      }

   }



   // Report the best-found portfolio and its variance here.
   printf("Best-found portfolio: \n");
   for (int i= 1; i<= 50; i++){
      printf("%8.4f   %s", x[i][1], ticker[i]);
   }
   printf("Variance of best-found portfolio: "); 
   Show(Varx); 

   
   // Pause so the execution window does not close.
   Exit ();

}


////////////////////////////////////////////////////////////////////////////////
// Allocate space for and read in covariance matrix and stock tickers.
////////////////////////////////////////////////////////////////////////////////
void GetData () {

   int i, j;
   double V0;
   char input[100];
   FILE *fp;

   // Allocate array space.
   V = Array (50, 50);

   // Allocate space to hold ticker names; "ticker" is a global variable.
   ticker = (char **) calloc (50+1, sizeof (char *));
   for (i = 0; i <= 50; i++) {
      ticker[i] = (char *) calloc (10, sizeof (char));
   }

   // Read in the data.
   fp = fopen ("V.txt", "r");
   for (i = 1; i <= 50; i++) {

      // Read in stock i's covariance data.

      // Name of the stock ticker.
      fgets (ticker[i], 9, fp);

      // The 50 covariances for stock "i".
      for (j = 1; j <= 50; j++) {

         // Read in V[i][j].
         fgets (input, 99, fp);
         sscanf (input, "%lf", &V0);

         // Put data into the V array.
         V[i][j] = V0;

      }

   }
   fclose (fp);

   return;

}




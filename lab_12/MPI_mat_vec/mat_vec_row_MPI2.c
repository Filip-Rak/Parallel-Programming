#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "mpi.h"

#define WYMIAR 10080 // divisible by 1,2,3,4,5,6,7,8,9,10,12,14,16,18,20,24,28,30,32,36,40,60
#define ROZMIAR (WYMIAR*WYMIAR)
#define ROOT 1

// slightly optimized version of matrix-vector product with possible OpenMP parallelization
void mat_vec(double* a, double* x, double* y, int nn, int nt)
{
  
  register int n=nn;
  register int i;
  //#pragma omp parallel for num_threads(nt) default(none) shared (a,x,y,n)
  for(i=0;i<n;i+=2)
  {
    register double ty1 = 0;
    register double ty2 = 0;
    register int j;

    for(j=0;j<n;j+=2)
    {
      register double t0=x[j];
      register double t1=x[j+1];
      register int k= i*n+j;
      ty1  +=a[k]*t0    +a[k+1]*t1 ;
      ty2  +=a[k+n]*t0  +a[k+1+n]*t1;
    }
    //printf("nr threads = %d\n", omp_get_num_threads());
    y[i]  = ty1;
    y[i+1]+=ty2;
  }
}

int main ( int argc, char** argv )
{
  /* Initialization */
  static double x[WYMIAR],y[WYMIAR],z[WYMIAR]; // z is sized for column decomposition
  double *a;
  double t1;
  int n,nt,i,j;
  const int ione=1;
  const double done=1.0;
  const double dzero=0.0;
  
  int rank, size, source, dest, tag=0; 
  int n_wier, n_wier_last;
  MPI_Status status;
  
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank ); 
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  
  n=WYMIAR;
  
  // x is locally initialized to zero
  for(i=0;i<WYMIAR;i++) x[i]=0.0;

  /* Tasks */
  if(rank==ROOT)
  {  
    a = (double *) malloc((ROZMIAR+1)*sizeof(double));
    
    for(i=0;i<ROZMIAR;i++) 
      a[i]=1.0*i;
    for(i=0;i<WYMIAR;i++) 
      x[i]=1.0*(WYMIAR-i);
       
    //printf("Podaj liczbe watkow: "); scanf("%d",&nt);
    nt=1;
    
    printf("poczatek (wykonanie sekwencyjne)\n");
    
    t1 = MPI_Wtime();
    mat_vec(a,x,y,n,nt);
    t1 = MPI_Wtime() - t1;
    
    printf("\tczas wykonania (zaburzony przez MPI?): %lf, Gflop/s: %lf, GB/s> %lf\n",  
	   t1, 2.0e-9*ROZMIAR/t1, (1.0+1.0/n)*8.0e-9*ROZMIAR/t1);
    
  }
  
  if(size>1)
  {
    
    /************** || block row decomposition || *******************/
    
    // z is initialized for all ranks
    for(i=0;i<WYMIAR;i++) z[i]=0.0;

    MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD );
    // podzial wierszowy
    n_wier = ceil(WYMIAR / size);
    n_wier_last = WYMIAR - n_wier*(size-1);
    
    // for n_wier!=n_wier_last arrays should be oversized to avoid problems
    if(n_wier!=n_wier_last)
    {  
      printf("This version does not work with WYMIAR not a multiple of size!\n");
      MPI_Finalize(); 
      exit(0); 
    }

    // local matrices a_local form parts of a big matrix a
    double *a_local = (double *) malloc(WYMIAR*n_wier*sizeof(double)); 
    for(i=0;i<WYMIAR*n_wier;i++) a_local[i]=0.0;
    
    // Scatter the matrix a to all processes
    MPI_Scatter(a, WYMIAR * n_wier, MPI_DOUBLE, a_local, WYMIAR * n_wier, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    // Broadcast the entire vector x to all processes
    MPI_Bcast(x, WYMIAR, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);    

    if(rank==ROOT) 
    {
      printf("Starting MPI matrix-vector product with block row decomposition!\n");
      t1 = MPI_Wtime();
    }
    
    // local parts of x start at rank*n_wier
    //MPI_Allgather(&x[rank*n_wier], n_wier, MPI_DOUBLE, x, n_wier, MPI_DOUBLE, MPI_COMM_WORLD );
    // MPI_Allgather( MPI_IN_PLACE, n_wier, MPI_DOUBLE, x, n_wier, MPI_DOUBLE, MPI_COMM_WORLD );

    for(i=0;i<n_wier;i++)
    {     
      double t=0.0;
      int ni = n*i;
      
      for(j=0;j<n;j++)
      {
        t+=a_local[ni+j]*x[j];
      }

      z[i]=t;
    }
    
    // just to measure time
    MPI_Barrier(MPI_COMM_WORLD);        
    if(rank==ROOT) 
    {
      t1 = MPI_Wtime() - t1;
      printf("Wersja rownolegla MPI z dekompozycją wierszową blokową\n");
      printf("\tczas wykonania: %lf, Gflop/s: %lf, GB/s> %lf\n", t1, 2.0e-9*ROZMIAR/t1, (1.0+1.0/n)*8.0e-9*ROZMIAR/t1);
    }
    
    MPI_Gather(z, n_wier, MPI_DOUBLE, z, n_wier, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    if(rank==ROOT)
    {
      for(i=0;i<WYMIAR;i++)
      {
	      if(fabs(y[i]-z[i])>1.e-9*z[i]) 
        {
	        printf("Blad! i=%d, y[i]=%lf, z[i]=%lf\n",i, y[i], z[i]);
	      }
      }
    }

    /************** || block column decomposition (collective only) || *******************/

    // Zero result vector z
    for(i=0;i<WYMIAR;i++)
    {
        z[i] = 0.0;
    }

    // Allocate data on all processes
    if (rank != ROOT)
    {
        a = (double *) malloc((ROZMIAR+1)*sizeof(double));
    }

    // Trasnfer data
    MPI_Bcast(x, WYMIAR, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(a, ROZMIAR, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    
    int per_proc = ceil((double)WYMIAR / (double)size);
    int start_col = rank * per_proc;
    int end_col = (rank == size - 1) ? WYMIAR : (rank + 1) * per_proc;

    if(rank==ROOT) 
    {
      printf("Starting MPI matrix-vector product with block column decomposition!\n");
      t1 = MPI_Wtime();
    }

    // Lokalne obliczenia dla przypisanych kolumn
    double *z_local = (double *)malloc(WYMIAR * sizeof(double));
    for (i = 0; i < WYMIAR; i++) {
        z_local[i] = 0.0; // Inicjalizacja lokalnego wyniku
    }

    for (j = start_col; j < end_col; j++) { // Iteracja po przypisanych kolumnach
        for (i = 0; i < WYMIAR; i++) { // Iteracja po wierszach
            z_local[i] += a[i * WYMIAR + j] * x[j]; // Przemnażanie wierszy przez kolumnę
        }
    }

    // Redukcja wyników na procesie ROOT
    MPI_Reduce(z_local, z, WYMIAR, MPI_DOUBLE, MPI_SUM, ROOT, MPI_COMM_WORLD);

    // just to measure time
    MPI_Barrier(MPI_COMM_WORLD);        
    if(rank==ROOT) {
      t1 = MPI_Wtime() - t1;
      printf("Werja rownolegla MPI z dekompozycją kolumnową blokową\n");
      printf("\tczas wykonania: %lf, Gflop/s: %lf, GB/s> %lf\n",  
      	     t1, 2.0e-9*ROZMIAR/t1, (1.0+1.0/n)*8.0e-9*ROZMIAR/t1);
      
    }
    
    // testing - switch on after completing calculations and communcation
    if(rank==ROOT)
    {
      for(i=0;i<WYMIAR;i++)
      {
        printf("a\n");
	      if(fabs(y[i]-z[i])>1.e-9*z[i]) 
        {
	        printf("Blad! i=%d, y[i]=%lf, z[i]=%lf - complete the code for column decomposition\n", i, y[i], z[i]);
	        break;
	        //printf("Blad! i=%d, y[i]=%lf, z[i]=%lf\n",i, y[i], z[i]);
	      }
      } 
    }
  }

  MPI_Finalize(); 
  return(0);
}
#include <stdio.h>
#include <papi.h>
#include <stdlib.h>

#define TAM 2000
#define EVENT_COUNT 4
#define TEST_NUM 5
//#define BLOCKSIZE 16

float A[TAM][TAM], B[TAM][TAM], C[TAM][TAM];
int blocksize = 2000;
//Inicializa las matrices
void iniciarMatrices()
{
  int i,j;
  for(i=0;i<TAM;i++)
    for(j=0;j<TAM;j++)
    {
	 C[i][j]=0;
     B[i][j]=0;
     A[i][j]=0;
    }
}

//Multiplica las dos matrices
void bucle()
{
	
int i,j,k,bi,bj,bk;
//for (bi =0; bi <TAM; bi += blocksize )
	for(bk =0; bk <TAM; bk += blocksize )
		for (bj =0; bj <TAM; bj += blocksize )
			for (i=0; i< TAM ; i++)
				for (k=0; k< blocksize ; k++)
					for(j=0; j< blocksize ; j++)
						C[bi+i][ bj+j] += A[bi+i][ bk+k]*B[bk+k][ bj+j];
}
int main()
{
 
    int retval;
    int i,j;
    int EventSet = PAPI_NULL;
	int blocksizes[] = {4, 16, 50, 100, 2000};
    long long totales[EVENT_COUNT], totalesPerm[EVENT_COUNT];
    
    int events[] = {PAPI_L1_DCM,  PAPI_TOT_CYC, PAPI_L2_DCH, PAPI_L2_DCA};
    long long values[EVENT_COUNT];
    
    
    // Inicializamos la librería PAPI
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    
    if(retval!=PAPI_VER_CURRENT){
	fprintf(stderr, "PAPI library init error!\n");
	exit(1);
    }
    
    
    //Comprobamos si los contadores están disponibles
    for(i=0; i<EVENT_COUNT; i++)
    {
      if (PAPI_OK != PAPI_query_event(events[i])) 
      {
	printf("Cannot count counter %d", i);
	exit(0);
      }
    }

    //iniciamos los vectores de resultados totales
    for(i=0; i<EVENT_COUNT; i++)
    {
      totales[i]=0;
      totalesPerm[i]=0;
    }
    
    
    iniciarMatrices();

    printf("\n --------  Prueba con bucle original ---------\n\n");
  
    for(i=0; i<TEST_NUM; i++)
    {
	blocksize = blocksizes[i];
	//Iniciamos la cuenta de eventos
	if (PAPI_start_counters(events, EVENT_COUNT) != PAPI_OK)
	{
	    fprintf(stderr, "ERROR Starting counters!\n");
	    exit(1);
	}
	
	bucle();
	
	//Leemos el valor de un contador:
	if (PAPI_stop_counters(values, EVENT_COUNT) != PAPI_OK)
	{
	    fprintf(stderr, "ERROR Reading counters!\n");
	    exit(1);
	}
	
	for(j=0; j<EVENT_COUNT; j++)
	{
	  totales[j]+=values[j];      
	}

	printf("Prueba %d - Bloque %i:\n\tL1 ->  Fallos: %lld\n", i, blocksize,values[0]);
	printf("\tL2 -> Accesos: %lld  Aciertos: %lld\n",  values[3], values[2]);
	printf("\tCiclos: %lld\n", values[1]);
    }
    
    //Calculamos los valores medios:
    for(i=0; i<EVENT_COUNT; i++)
    {
      totales[i] = totales[i]/TEST_NUM;
    }
    
    printf("\nValores medios:\n");
    printf("\tCaché L1: \n\t\tFallos: %lld\n",  totales[0]);
    printf("\tCaché L2:\n\t\tAccesos: %lld  \n\t\tAciertos: %lld  \n\t\tPorcentaje de acierto: %lld\n",
	   totales[3], totales[2], 100*totales[2]/(totales[3]));
    printf("\tCiclos: %lld\n", totales[1]);
    
    return 0;
    
}

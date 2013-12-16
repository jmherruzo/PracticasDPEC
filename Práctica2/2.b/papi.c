#include <stdio.h>
#include <papi.h>
#include <stdlib.h>

#define TAM 2200
#define BUCLE 2000
#define EVENT_COUNT 5
#define TEST_NUM 10

int A[TAM][TAM], B[TAM][TAM];

//Inicializa la matriz B
void iniciarMatrizB()
{
  int i,j;
  for(i=0;i<2000;i++)
    for(j=0;j<2000;j++)
    {
      B[i][j]=i*j;
      A[i][j]=i*j;
    }
}

//Realiza el bucle con los coeficientes i j colocados en la mejor posición
void bucle()
{
  int i,j;
  
  for(i=0;i<BUCLE;i++)
    for(j=0;j<BUCLE;j++)
      A[i][j]=B[i][j];
}

//Realiza el bucle con los coeficientes i j permutados
void buclePermutado()
{
  int i,j;
  
  for(j=0;j<BUCLE;j++)
    for(i=0;i<BUCLE;i++)
      A[i][j]=B[i][j];
}

int main()
{
 
    int retval;
    int i,j;
    int EventSet = PAPI_NULL;
    long long totales[EVENT_COUNT], totalesPerm[EVENT_COUNT];
    
    int events[] = {PAPI_L1_DCM, PAPI_L1_DCH, PAPI_L1_DCA, PAPI_L2_DCH, PAPI_L2_DCA};
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
    
    
    //iniciarMatrizB();

    printf("\n --------  Prueba con bucle original ---------\n\n");
  
    for(i=0; i<TEST_NUM; i++)
    {
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

	printf("Prueba %d:\n\tL1 -> Accesos: %lld  Aciertos: %lld  Fallos: %lld\n", i, values[2], values[1], values[0]);
	printf("\tL2 -> Accesos: %lld  Aciertos: %lld\n",  values[4], values[3]);
    }
    
    //Calculamos los valores medios:
    for(i=0; i<EVENT_COUNT; i++)
    {
      totales[i] = totales[i]/TEST_NUM;
    }
    
    printf("\nValores medios:\n");
    printf("\tCaché L1:\n\t\tAccesos: %lld  \n\t\tAciertos: %lld  \n\t\tFallos: %lld \n\t\tPorcentaje de acierto: %lld\n",  
	   totales[2], totales[1], totales[0], 100*totales[1]/totales[2]);
    printf("\tCaché L2:\n\t\tAccesos: %lld  \n\t\tAciertos: %lld  \n\t\tPorcentaje de acierto: %lld\n",
	   totales[4], totales[3], 100*totales[3]/(totales[4]));
    
    
    
    
    printf("\n --------  Prueba con bucle permutado --------- \n\n");

    for(i=0; i<TEST_NUM; i++)
    {
      
	//Iniciamos la cuenta de eventos
	if (PAPI_start_counters(events, EVENT_COUNT) != PAPI_OK)
	{
	    fprintf(stderr, "ERROR Starting counters!\n");
	    exit(1);
	}
	
	
	buclePermutado();
	
	//Leemos el valor de un contador:
	if (PAPI_stop_counters(values, EVENT_COUNT) != PAPI_OK)
	{
	    fprintf(stderr, "ERROR Reading counters!\n");
	    exit(1);
	}
      
      	for(j=0; j<EVENT_COUNT; j++)
	{
	  totalesPerm[j]+=values[j];      
	}
	
	printf("Prueba %d:\n\tL1 -> Accesos: %lld  Aciertos: %lld  Fallos: %lld\n", i, values[2], values[1], values[0]);
	printf("\tL2 -> Accesos: %lld  Aciertos: %lld\n",  values[4], values[3]);

    }

    //Calculamos los valores medios:
    for(i=0; i<EVENT_COUNT; i++)
    {
      totalesPerm[i] = totalesPerm[i]/TEST_NUM;
    
    }
    

    
    printf("\nValores medios:\n");
    printf("\tCaché L1:\n\t\tAccesos: %lld  \n\t\tAciertos: %lld  \n\t\tFallos: %lld \n\t\tPorcentaje de acierto: %lld\n",  
	   totalesPerm[2], totalesPerm[1], totalesPerm[0], 100*totalesPerm[1]/totalesPerm[2]);
    printf("\tCaché L2:\n\t\tAccesos: %lld  \n\t\tAciertos: %lld  \n\t\tPorcentaje de acierto: %lld\n",
	   totalesPerm[4], totalesPerm[3], 100*totalesPerm[3]/(+totalesPerm[4]));
    
    
    return 0;
    
}

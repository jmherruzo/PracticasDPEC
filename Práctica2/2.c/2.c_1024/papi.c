#include <stdio.h>
#include <papi.h>
#include <stdlib.h>

#define TAM 1024
#define BUCLE 600
#define EVENT_COUNT 4
#define TEST_NUM 1

float A[TAM][TAM], B[TAM][TAM];

//Inicializa la matriz B
void iniciarMatrizB()
{
  int i,j;
  for(i=0;i<TAM;i++)
    for(j=0;j<TAM;j++)
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
      A[i][j]=B[j][i];
}

int main()
{
 
    int retval;
    int i,j;
    int EventSet = PAPI_NULL;
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

	printf("Prueba %d:\n\tL1 ->  Fallos: %lld\n", i, values[0]);
	printf("\tL2 -> Accesos: %lld  Aciertos: %lld\n",  values[3], values[2]);
	printf("\tCiclos: %lld", values[1]);
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

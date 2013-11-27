

compile:
	gcc  papi.c -o papi -lpapi -O0
     
run:compile
	./papi
	
log:compile
	./papi > log.txt
	
	
O2:
	gcc  papi.c -o papi -lpapi -O2

runO2:O2
	./papi
	
logO2:runO2
	./papi > logO2.txt
	
#include "nSystem.h"
#include <stdlib.h>

nSem mutex;

int aleatorio(nSubasta s, int print_msg, char *nom, int oferta) {

  int delay;
  nSetTaskName(nom);
  nWaitSem(mutex); 
  delay= rand()%1000;

  nSignalSem(mutex);
  nSleep(delay);
  if (print_msg)
    nPrintf("%s ofrece %d\n", nom, oferta);
  int ret= nOfrecer(s, oferta);

  if (print_msg) {
    if (ret)
      nPrintf("%s se adjudico un item a %d\n", nom, oferta);
    else
      nPrintf("%s fallo con su oferta de %d\n", nom, oferta);
  }
  return ret;
}

int oferente(nSubasta s, int print_msg, char *nom, int oferta) {
  nSetTaskName(nom);
  if (print_msg)
    nPrintf("%s ofrece %d\n", nom, oferta);
  int ret= nOfrecer(s, oferta);
  if (print_msg) {
    if (ret)
      nPrintf("%s se adjudico un item a %d\n", nom, oferta);
    else
      nPrintf("%s fallo con su oferta de %d\n", nom, oferta);
  }
  return ret;
}

int test1(int print_msg, int delay) {
  nSubasta s= nNuevaSubasta(2, delay);
  nTask pedro= nEmitTask(aleatorio, s, print_msg, "pedro", 1, -1);
  nTask juan= nEmitTask(aleatorio, s, print_msg, "juan", 3, -1);
  nTask diego= nEmitTask(aleatorio, s, print_msg, "diego", 4, -1);
  nTask pepe= nEmitTask(aleatorio, s, print_msg, "pepe", 2, -1);
  int u;
  int recaud= nRecaudacion(s, &u);
  if (recaud!=7)
    nFatalError("test1", "La recaudacion debio ser 7 y no %d\n", recaud);
  if (u!=0)
    nFatalError("test1", "Quedaron %d unidades sin vender\n", u);
  if (print_msg)
    nPrintf("El monto recaudado es %d y quedaron %d unidades sin vender\n",
            recaud, u);
  if (nWaitTask(pedro))
    nFatalError("nMain", "pedro debio ganar con 7\n");
  if (!nWaitTask(juan))
    nFatalError("nMain", "juan debio ganar con 6\n");
  if (!nWaitTask(diego))
    nFatalError("nMain", "diego debio perder con 4\n");
  if (nWaitTask(pepe))
    nFatalError("nMain", "pepe debio ganar con 5\n");
  return 0;
}

int test2(int print_msg, int delay) {
  nSubasta s= nNuevaSubasta(3, delay);
  nTask ana= nEmitTask(oferente, s, print_msg, "ana", 7);
  nSleep(1000);
  nTask maria= nEmitTask(oferente, s, print_msg, "maria", 3);
  nSleep(1000);
  nTask ximena= nEmitTask(oferente, s, print_msg, "ximena", 4);
  nSleep(1000);
  nTask erika= nEmitTask(oferente, s, print_msg, "erika", 5);
  if (nWaitTask(maria))
    nFatalError("nMain", "maria debio perder con 3\n");
  nSleep(1000);
  nTask sonia= nEmitTask(oferente, s, print_msg, "sonia", 6);
  int u;
  int recaud= nRecaudacion(s, &u);
  if (recaud!=18)
    nFatalError("test1", "La recaudacion debio ser 18 y no %d\n", recaud);
  if (u!=0)
    nFatalError("test1", "Quedaron %d unidades sin vender\n", u);
  if (print_msg)
    nPrintf("El monto recaudado es %d y quedaron %d unidades sin vender\n",
            recaud, u);
  if (!nWaitTask(ana))
    nFatalError("nMain", "ana debio ganar con 7\n");
  if (nWaitTask(ximena))
    nFatalError("nMain", "ximena debio perder con 4\n");
  if (!nWaitTask(erika))
    nFatalError("nMain", "erika debio ganar con 5\n");
  if (!nWaitTask(sonia))
    nFatalError("nMain", "sonia debio ganar con 6\n");
  return 0;
}

int test3(int print_msg, int delay) {
  nSubasta s= nNuevaSubasta(5, delay);
  nTask tomas= nEmitTask(oferente, s, print_msg, "tomas", 2, -1);
  nSleep(1000);
  nTask monica= nEmitTask(oferente, s, print_msg, "monica", 3, -1);
  nSleep(1000);
  int u;
  int recaud= nRecaudacion(s, &u);
  if (recaud!=5)
    nFatalError("test3", "La recaudacion debio ser 5 y no %d\n", recaud);
  if (u!=3)
    nFatalError("test3", "Quedaron %d unidades sin vender\n", u);
  if (print_msg)
    nPrintf("El monto recaudado es %d y quedaron %d unidades sin vender\n",
            recaud, u);
  if (!nWaitTask(tomas))
    nFatalError("nMain", "tomas debio ganar con 2\n");
  if (!nWaitTask(monica))
    nFatalError("nMain", "monica debio ganar con 3\n");
  return 0;
}

#define N 10
#define M 10000

int nMain() {
  mutex= nMakeSem(1);
  nPrintf("una sola subasta con tiempos aleatorios\n");
  test1(1, 3000);
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");

  nPrintf("una sola subasta con tiempos deterministicos\n");
  test2(1, 3000);
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");

  nPrintf("una sola subasta con menos oferentes que unidades disponibles\n");
  test3(1, 3000);
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");

  nSetTimeSlice(1);
  nPrintf("30 subastas en paralelo\n");
  nTask *tasks1= nMalloc(M*sizeof(nTask));
  nTask *tasks2= nMalloc(M*sizeof(nTask));
  nTask *tasks3= nMalloc(M*sizeof(nTask));
  int k;
  tasks1[0]= nEmitTask(test1, 1, 5000);
  tasks2[0]= nEmitTask(test2, 1, 5000);
  tasks3[0]= nEmitTask(test3, 1, 5000);
  for (k=1; k<N; k++) {
    tasks1[k]= nEmitTask(test1, 0, 5000);
    tasks2[k]= nEmitTask(test2, 0, 5000);
    tasks3[k]= nEmitTask(test3, 0, 5000);
  }
  for (k=0; k<N; k++) {
    nWaitTask(tasks1[k]);
    nWaitTask(tasks2[k]);
    nWaitTask(tasks3[k]);
  }
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");
  nPrintf("%d subastas en paralelo\n", M*3);
  tasks1[0]= nEmitTask(test1, 1, 20000);
  tasks2[0]= nEmitTask(test2, 1, 20000);
  tasks3[0]= nEmitTask(test3, 1, 20000);
  for (k=1; k<M; k++) {
    tasks1[k]= nEmitTask(test1, 0, 20000);
    tasks2[k]= nEmitTask(test2, 0, 20000);
    tasks3[k]= nEmitTask(test3, 0, 20000);
  }
  nWaitTask(tasks1[0]);
  nWaitTask(tasks2[0]);
  nWaitTask(tasks3[0]);
  nPrintf("Enterrando tareas.  Cada '.' son 30 tareas enterradas.\n");
  for (k=1; k<M; k++) {
    nWaitTask(tasks1[k]);
    nWaitTask(tasks2[k]);
    nWaitTask(tasks3[k]);
    if (k%10==0) nPrintf(".");
  }
  nPrintf("\ntest aprobado\n");
  nPrintf(  "-------------\n");
  nPrintf("Felicitaciones: paso todos los tests\n");
  return 0;
}

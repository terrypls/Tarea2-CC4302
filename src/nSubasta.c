#include "nSysimp.h"
#include <nSystem.h>

typedef struct {
  nTask *vec;
  int size;
} *PriQueue;

PriQueue MakePriQueue();
nTask PriGet(PriQueue pq);
void PriPut(PriQueue pq, nTask t, int pri);
int PriBest(PriQueue pq);
int EmptyPriQueue(PriQueue pq);

typedef struct subasta{
  int tiempo;
  int elementos;
  nTask recaudacion;
  PriQueue fila;
} *nSubasta;



nSubasta nNuevaSubasta(int n, int delay);
int nOfrecer(nSubasta s, int oferta);
int nRecaudacion(nSubasta s, int *punidades);



/* ... agregue aca la implementacion del sistema de subastas ... */

nSubasta nNuevaSubasta(int n,int delay){
  nSubasta ns=nMalloc(sizeof(*ns)); //asigno memoria a subasta
  ns->tiempo=delay;   //tiempo de espera antes de cerrar subasta
  ns->elementos=n;    //cantidad de elementos en la subasta
  ns->fila= MakePriQueue(n); //cola de prioradidad para la subasta
  ns->recaudacion = NULL; //empieza vacia, se llena con nRecaudacion
  return ns;
  printf("%i\n",n );
}

int nOfrecer(nSubasta s, int oferta){
  START_CRITICAL();


  nTask this_task = current_task;
  this_task->status = OFERTA_ACTIVA;
  PriPut(s->fila,this_task,oferta);
    int holis= s->fila->size;
  printf("\nTAMAÑO ACTUAL DE LA QUEUE %i\n",holis );
  ResumeNextReadyTask();

  

/*Como llamaron a oferta, debo resetear el timer de nRecaudacion*/

  END_CRITICAL();
}
int nRecaudacion(nSubasta s, int *punidades){
  START_CRITICAL();
  nTask this_task=current_task; //tarea actual
  s->recaudacion=this_task; //asigno tarea actual a subasta
  s->recaudacion->status=WAIT_TASK;
  ProgramTask(s->tiempo);//timer para terminar la subasta
  ResumeNextReadyTask(); //dejo la cpu para los demas procesos

  int total=s->elementos; /*Cantidad maxima de ofertas validas de la PriQueue*/
  int largo= s->fila->size; /*Cantidad de elementos en la cola*/
  int recaudado=0;
nTask tarea1=PriGet(s->fila);
nTask tarea2=PriGet(s->fila);
nTask tarea3=PriGet(s->fila);
nTask tarea4=PriGet(s->fila);

printf("Tarea 1%i\n",tarea1->oferta);
nTask tarea1=PriGet(s->fila);


  /*for(int i=0;i<total;i++){
      int aux = PriBest(s->fila);
      recaudado+=aux;
      printf("%i\n",aux );
  }*/

  *punidades=total-largo; /*devuelve la cantidadd de elementos restantes en la cola*/
  END_CRITICAL();

  return recaudado;
}

/*----------------------------------------------------------------------------------------------------------------------*/
PriQueue MakePriQueue(int maxsize) {
  PriQueue pq= nMalloc(sizeof(*pq));
  pq->vec= nMalloc(sizeof(nTask)*(maxsize+1));
  pq->size= 0;
  return pq;
}

nTask PriGet(PriQueue pq) {
  nTask t;
  int k;
  if (pq->size==0)
    printf("Nulo\n");
    return NULL;
  t= pq->vec[0];
  pq->size--;
  for (k= 0; k<pq->size; k++)
    pq->vec[k]= pq->vec[k+1];
  return t;
}

void PriPut(PriQueue pq, nTask t, int oferta) {
  int k;
  t->oferta= oferta;
  for (k= pq->size-1; k>=0; k--) {
    nTask t= pq->vec[k];
    if (oferta > t->oferta)
      break;
    else
      pq->vec[k+1]= t;
  }
  pq->vec[k+1]= t;
  pq->size++;


}

int PriBest(PriQueue pq) {
  return pq->size==0 ? 0 : pq->vec[0]->oferta;
}

int EmptyPriQueue(PriQueue pq) {
  return pq->size==0;
}


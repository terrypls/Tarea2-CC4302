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
  ns->fila= MakePriQueue(2*n); //cola de prioradidad para la subasta
  ns->recaudacion = NULL; //empieza vacia, se llena con nRecaudacion
  
  return ns;
}

int nOfrecer(nSubasta s, int oferta){
  START_CRITICAL();
/*Como llego una nueva oferta, debo resetear el timer de nRecaudacion*/
  nTask this_task = current_task;
  if(s->recaudacion!=NULL){
    current_task=s->recaudacion;
    CancelTask(current_task);
    ProgramTask(s->tiempo);
    current_task=this_task;
  }
//Cambio estatus de la tarea y lo agrego a la cola de prioridad
  this_task->status=WAIT_TASK;
  PriPut(s->fila,this_task,oferta);

  nTask minimo;

    //si soy mejor que el ultimo, y la fila es mas grande que la oferta, saco el minimo y lo meto a la cola
  if(PriBest(s->fila) <= oferta && s->fila->size > s->elementos){
    minimo=PriGet(s->fila);
    minimo->status=READY;
    PutTask(ready_queue,minimo);
  }
//Cedo la CPU para el siguiente proceso en la cola
  ResumeNextReadyTask();
//me acaba de llegar la cpu de nuevo
//Tengo que comprobar que la fila sea distinto de 0 para matar, de lo contrarios 
  if(s->fila->size!=0){
    return FALSE;
  }
  END_CRITICAL();
  return TRUE;

}

int nRecaudacion(nSubasta s, int *punidades){
  START_CRITICAL();
  nTask this_task=current_task; //tarea actual
  s->recaudacion=this_task; //asigno tarea actual a subasta
  s->recaudacion->status=WAIT_TASK;
  ProgramTask(s->tiempo);//timer para terminar la subasta
  ResumeNextReadyTask(); //dejo la cpu para los demas procesos

  int total=s->fila->size;  //Tamaño de la fila
  int largo=s->elementos;   //Cantidad de elementos subastados
  int recaudado=0;

  while(s->fila->size > 0){
    nTask aux= PriGet(s->fila);
    recaudado+=aux->oferta;
    aux->status=READY;
    PutTask(ready_queue,aux);
  }


  int dif= largo-total;
  if(dif<0){
    *punidades=0;  
  }
  else{
    *punidades=dif;
  }
  PutTask(ready_queue,this_task);
  ResumeNextReadyTask();
   /*devuelve la cantidadd de elementos restantes en la cola*/

  int empty= EmptyPriQueue(s->fila);  
  nFree(s);


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

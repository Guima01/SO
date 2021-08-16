/*
*  lottery.c - Implementacao do algoritmo Lottery Scheduling e sua API
*
*  Autores: Guilherme Marques, Daniel Machado, Gabriel Bronte
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*
*/

#include "lottery.h"
#include <stdio.h>
#include <string.h>

//Nome unico do algoritmo. Deve ter 4 caracteres.
int slot;
const char lottName[] = "LOTT";

//=====Funcoes Auxiliares=====

//=====Funcoes da API=====

//Funcao chamada pela inicializacao do S.O. para a incializacao do escalonador
//conforme o algoritmo Lottery Scheduling
//Deve envolver a inicializacao de possiveis parametros gerais
//Deve envolver o registro do algoritmo junto ao escalonador
void lottInitSchedInfo()
{
	SchedInfo *sched = malloc(sizeof(SchedInfo));
	strcpy(sched->name, lottName);
	sched->initParamsFn = lottInitSchedParams;
	sched->scheduleFn = lottSchedule;
	sched->releaseParamsFn = lottReleaseParams;
	slot = schedRegisterScheduler(sched);
}

//Inicializa os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' associado ao slot de Lottery
void lottInitSchedParams(Process *p, void *params)
{
	processSetSchedParams(p,params);
	processSetSchedSlot(p,slot);
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo Lottery
Process *lottSchedule(Process *plist)
{
	int ticketsTotal = 0;
	Process *proc = plist;
	while(proc != NULL){
		LotterySchedParams *schedParams = processGetSchedParams(proc);

		int status = processGetStatus(proc);

		if (status!= 2){
			ticketsTotal = ticketsTotal + schedParams->num_tickets;
		}
		proc = processGetNext(proc);
	}

	int ticket = rand() % ticketsTotal;
	int aux = 0;
	Process *proc2 = plist;
	while(proc2 != NULL){
		LotterySchedParams *schedParams = processGetSchedParams(proc2);
		int status = processGetStatus(proc2);
		if (status != 2){
			aux = aux + schedParams->num_tickets;
			if (ticket <= aux)
				return proc2;
		}
		proc2 = processGetNext(proc2);
	}
	return NULL;
}

//Libera os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' desassociado do slot de Lottery
//Retorna o numero do slot ao qual o processo estava associado
int lottReleaseParams(Process *p)
{
	LotterySchedParams *lspOld = processGetSchedParams(p);
	free(lspOld);
	return slot;
}

//Transfere certo numero de tickets do processo src para o processo dst.
//Retorna o numero de tickets efetivamente transfeirdos (pode ser menos)
int lottTransferTickets(Process *src, Process *dst, int tickets)
{
	LotterySchedParams *schedParams1 = processGetSchedParams(src);
	LotterySchedParams *schedParams2 = processGetSchedParams(dst);

	if(schedParams1->num_tickets > 0){
		if(schedParams1->num_tickets - tickets < 0){
			tickets = schedParams1->num_tickets - 1;
		}
		schedParams1->num_tickets -= tickets;
		schedParams2->num_tickets += tickets;
		return tickets;
	}
	return 0;
}

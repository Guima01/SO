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
const char lottName[] = "LOTT";
int slot;

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
	schedSetScheduler(p, params, slot);
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo Lottery
Process *lottSchedule(Process *plist)
{
	int ticketsTotal = 0;
	for (Process *p = plist; p != NULL; p = processGetNext(p))
	{
		LotterySchedParams *sched_params = processGetSchedParams(p);
		int status = processGetStatus(p);
		if (status!= PROC_WAITING)
		{
			ticketsTotal += sched_params->num_tickets;
		}
	}

	int ticket = rand() % ticketsTotal;
	int aux = 0;
	for (Process *p = plist; p != NULL; p = processGetNext(p))
	{
		LotterySchedParams *sched_params = processGetSchedParams(p);
		int status = processGetStatus(p);
		if (status != PROC_WAITING)
		{
			aux += sched_params->num_tickets;
			if (ticket <= aux)
				return p;
		}
	}
	return NULL;
}

//Libera os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' desassociado do slot de Lottery
//Retorna o numero do slot ao qual o processo estava associado
int lottReleaseParams(Process *p)
{
	free(processGetSchedParams(p));
	processSetSchedParams(p, NULL);
	processSetSchedSlot(p, -1);

	return slot;
}

//Transfere certo numero de tickets do processo src para o processo dst.
//Retorna o numero de tickets efetivamente transfeirdos (pode ser menos)
int lottTransferTickets(Process *src, Process *dst, int tickets)
{
	int ticketsTransferidos = 0;
	LotterySchedParams *sched_params1 = processGetSchedParams(src);
	LotterySchedParams *sched_params2 = processGetSchedParams(dst);

	if (sched_params1->num_tickets >= tickets)
	{
		sched_params1->num_tickets -= tickets;
		sched_params2->num_tickets += tickets;
		ticketsTransferidos = tickets;
	}
	else
	{
		sched_params2->num_tickets = sched_params1->num_tickets;
		ticketsTransferidos = sched_params1->num_tickets;
		sched_params1->num_tickets = 0;
	}

	return ticketsTransferidos;
}

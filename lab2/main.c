#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>
#define DABS(X) (X > 0) ? X : -X
#define THRASH 1<<31
#define STACK_IS_NOT_FREE (sp>0)
struct stks {
    long double sA;
    long double sB;
    long double sfA;
    long double sfB;
    long double ss;
};
struct stks stk[1000];
#define PUT_INTO_GL_STACK(sp, A,B,fA,fB,s) do {\
    stk[sp].sA=A;\
    stk[sp].sB=B;\
    stk[sp].sfA=fA;\
    stk[sp].sfB=fB;\
    stk[sp].ss=s;\
sp++;\
}while(0)

#define GET_FROM_GL_STACK(sp, A,B,fA,fB,s) do {\
    sp--;\
    A=stk[sp].sA;\
    B=stk[sp].sB;\
    fA=stk[sp].sfA;\
    fB=stk[sp].sfB;\
    s=stk[sp].ss;\
}while(0)

#define PUT_INTO_LC_STACK(sp, A,B,fA,fB,s) do {\
    stkl[sp].sA=A;\
    stkl[sp].sB=B;\
    stkl[sp].sfA=fA;\
    stkl[sp].sfB=fB;\
    stkl[sp].ss=s;\
sp++;\
}while(0)

#define GET_FROM_LC_STACK(sp, A,B,fA,fB,s) do {\
    sp--;\
    A=stkl[sp].sA;\
    B=stkl[sp].sB;\
    fA=stkl[sp].sfA;\
    fB=stkl[sp].sfB;\
    s=stkl[sp].ss;\
}while(0)

long double eps = 0;
int nproc = 0;
const int SPK = 10;


struct shdat {
    int ntask;
    int nactive;
    int maxtask;
    int list_of_tasks;
    sem_t sem_sum;
    sem_t sem_list;
    sem_t sem_task_present;   
    long double s_all;
};
struct shdat sdat;

long double fun(long double x) {
    return 1/x/x*sin(1/x)*sin(1/x);
    //return x*x;
}

int BreakCond(long double sacb, long double sab) {
    long double T1 = sacb-sab;
    long double T2 = eps*sacb;
    T1 = DABS(T1);
    T2 = DABS(T2);
    if (T1 > T2)
        return 0;
    else
        return 1;
}





void* slave_thr()
{
    struct stks stkl[1000];
    int sp=0;
    int i = 0;
    long double s=0;
    long double c = 0, fc = 0, sac = 0, scb = 0, sacb = 0;
    long double a = 0, b = 0, fa= 0, fb = 0, sab = 0;

    long double t1, t2, t3, t4, t5;

    while(1) {
        sem_wait(&sdat.sem_task_present);
        sem_wait(&sdat.sem_list);
        sdat.ntask--;
        GET_FROM_GL_STACK(sdat.ntask, a,b,fa,fb,sab);
        //printf("GL a,b:%Lf, %Lf\n", a, b);
        if(sdat.ntask)
            sem_post(&sdat.sem_task_present);
        if(a<=b)
            sdat.nactive++;
        sem_post(&sdat.sem_list);

        if(a>b)
            break;
        while(1) {
            c=(a+b)/2;
            fc=fun(c);
            sac=(fa+fc)*(c-a)/2;
            scb=(fc+fb)*(b-c)/2;
            sacb=sac+scb;
            if(BreakCond(sacb,sab)) {
                s+=sacb;
                if(!sp)
                    break;
                sp--;
                GET_FROM_LC_STACK(sp, a, b, fa, fb, sab);

            }
            else
            {
                PUT_INTO_LC_STACK(sp, a, c, fa, fc, sac);
                sp++;
                a=c;
                fa=fc;
                sab=scb;
            }
            if((sp>SPK) && (!sdat.ntask))
            {
                sem_wait(&sdat.sem_list);
                if(!sdat.ntask) {
                    sem_post(&sdat.sem_task_present);
                }
                while((sp>1) && (sdat.ntask<sdat.maxtask)){
                    sp--;
                    GET_FROM_LC_STACK(sp,t1,t2,t3,t4,t5);
                    PUT_INTO_GL_STACK(sdat.ntask,t1,t2,t3,t4,t5);
                    sdat.ntask++;
                }
                sem_post(&sdat.sem_list);
            }
        }
        sem_wait(&sdat.sem_list);
        sdat.nactive--;
        if( (!sdat.nactive) && (!sdat.ntask) ) {
            for(i=0;i<nproc;i++) {
                PUT_INTO_GL_STACK(sdat.ntask,2,1,THRASH,THRASH,THRASH);
                sdat.ntask++;
            }
            sem_post(&sdat.sem_task_present);
        }
        sem_post(&sdat.sem_list);
    }

    sem_wait(&sdat.sem_sum);
    //printf("psum:%Lf\n", s);
    sdat.s_all+=s;
    sem_post(&sdat.sem_sum);
}



int main(int argc, char* argv[]){
    nproc = atoi(argv[1]);
    eps = 0.00001;
    long double A = 0.0001, B = 1;
    sdat.ntask=0;
    sdat.maxtask = 1000;
    pthread_t thr[nproc];
    int i = 0;
    sdat.nactive=0;

    sem_init(&sdat.sem_sum, 0, 1);
    sem_init(&sdat.sem_list, 0, 1);
    sem_init(&sdat.sem_task_present,0, 0);
    sdat.s_all=0;

    PUT_INTO_GL_STACK(sdat.ntask, A,B,fun(A),fun(B),(fun(A)+fun(B))*(B-A)/2.);
    sdat.ntask++;
    sem_post(&sdat.sem_task_present);
    for (i = 0; i < nproc; i++)
        pthread_create(&thr[i], NULL, slave_thr, NULL);

    for(i = 0; i< nproc; ++i)
        pthread_join(thr[i], NULL);
    long double J = sdat.s_all;
    printf("ANS %Lf\n", J);
    return 0;
}
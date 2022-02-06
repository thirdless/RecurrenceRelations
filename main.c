#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "unistd.h"
#include "semaphore.h"
#include "queue.h"
#include "message.h"
#include "stdatomic.h"
#include "stdbool.h"
#include "stdint.h"
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

// parte parametrizabila
// !!!!!!!!

#define PAIRS 4

int x0[] = {
    1, -1, 0, 3
};

int x1[] = {
    1, 2, 3, 5
};

#define N 15 // numarul de nuclee - 1 = valorea maxima a lui n

// !!!!!!!!

// ec 15
#define A 8
#define B -55
#define C 7
typedef long long int msgtype; // message type
#define MCOUNT 3 // message vector index count
int MSIZE = sizeof(msgtype) * MCOUNT; //message size

#define QSIZE 2 * N - 1//queue size
#define BSIZE 128 //buffer size
queue_t Q[QSIZE];
semaphore_t sem[QSIZE];
char buff[QSIZE][BSIZE];

atomic_flag printsem;

void prints(const char * format, ...){
    P(&printsem);
    va_list valist;
    va_start(valist, format);
    vfprintf(stderr, format, valist);
    va_end(valist);
    V(&printsem);
}

int overflowEquation(msgtype operand1, msgtype operand2, msgtype operand3){
    int ret = 0;

    // overflow daca:
    // - daca termenii sunt pozitivi iar rezultatul este negativ
    // - daca termenii sunt negativi iar rezultatul este pozitiv
    if(
        (operand1 > 0 && operand2 > 0 && operand1 + operand2 < 0) ||
        (operand1 < 0 && operand2 < 0 && operand1 + operand2 > 0)
    ){
        ret = 1;

        if(
            !ret &&
            ((operand2 > 0 && operand3 > 0 && operand1 + operand2 + operand3 < 0) ||
            (operand2 < 0 && operand3 < 0 && operand1 + operand2 + operand3 > 0))
        )
        {
            ret = 1;
        }
    }

    return ret;
}

int overflowMultiplication(msgtype operand1, msgtype operand2, msgtype operand3){
    // a * b = c
    // a == c / b
    int ret = 0;
    msgtype temp = operand1 * operand2;

    // verificare daca orice element din inmultire este 0
    // rezultatul este 0 => nu este overflow
    // deasemenea se evita impartirea la 0 in urmatoarele calcule
    if(!operand1 || !operand2 || !operand3)
        return 0;
    
    if(operand1 == temp / operand2){
        msgtype temp2 = temp * operand3;

        if(temp != temp2 / operand3)
            ret = 1;
    }
    else
        ret = 1;

    return ret;
}

// functie ce returneaza un string cand overflowul este detectat, 0 cand nu este
char * getOverflow(int op1, int op2, int op3, int den, int calc){
    if(op1)
        return "Operand1";
    if(op2)
        return "Operand2";
    if(op3)
        return "Operand3";
    if(den)
        return "Denominator";
    if(calc)
        return "Calculation";
    
    return 0;
}

// msg - 0 - indice pereche + overflow pe ultimul bit
//         1 - numarator
//         2 - numitor

// fiecare nod isi va depune rezultatul pe coada cu numarul
// propriu de core si pe coada N + core-ul curent

// functia ce va fi utilizata pe primele 2 core-uri;
// pun valorile in coada pentru primii 2 termeni
void core_init(int id){
    int pair = 0;
    msgtype msg[MCOUNT];

    while(pair < PAIRS){
        msg[0] = pair << 1;
        msg[2] = 1;

        if(!id){ // core 0
            msg[1] = x0[pair];
            queue_put_b(&Q[N], (char*)msg, MSIZE);
        }
        else{ // core 1
            msg[1] = x1[pair];
            queue_put_b(&Q[1], (char*)msg, MSIZE);
            queue_put_b(&Q[N + 1], (char*)msg, MSIZE);
        }

        prints("Core%d %d x%d = %d\n", id, pair, id, msg[1]);

        pair++;
    }
}

// functie ce va fi rulata de restul core-urilor, calculeaza urmatorii termeni
void core_code(int id){
    msgtype msg0[MCOUNT],
            msg1[MCOUNT];
    int pair = 0;

    do{
        queue_get_b(&Q[id - 1], (char*)msg0, MSIZE);
        queue_get_b(&Q[N + id - 2], (char*)msg1, MSIZE);

        pair = msg0[0] >> 1;

        // daca ultimul core nu a raportat overflow
        if(msg0[0] % 2 == 0){
            msgtype denominator = msg1[2] * 10, // numitorul este inmultit cu 10 deoarece B este inmultit cu 10 pentru a evita virgula
                operand1 = A * (denominator / msg0[2]) * msg0[1], // se aduce la acelasi numitor inmultind cu raportul dintre cel curent si cel precedent
                operand2 = B * msg1[1], // este deja adus la acelasi numitor, deoarece B este deja inmultit cu 10 pentru a evita virgula
                operand3 = C * denominator * id, // ultimul termen nu depinde de fractii precedente, deci trebuie inmultit cu numitorul curent

                calc = operand1 + operand2 + operand3,
                whole = calc / denominator, // parte intreaga
                decimal = calc % denominator; // parte fractionara

            //partea fractionara nu poate fi negativa
            if(decimal < 0)
                decimal = -decimal;

            //verificare overflow pentru fiecare termen in parte, numitorul fractiei si adunarea termenilor
            int overflowOp1 = overflowMultiplication(A, (denominator / msg0[2]), msg0[1]),
                overflowOp2 = overflowMultiplication(B, msg1[1], 1), // putem adauga elementul neutru al inmultirii pentru a utiliza functia cu 3 parametri
                overflowOp3 = overflowMultiplication(C, denominator, id),
                overflowDeno = overflowMultiplication(msg1[2], 10, 1),
                overflowCalc = overflowEquation(operand1, operand2, operand3);

            char * overflow = getOverflow(overflowOp1, overflowOp2, overflowOp3, overflowDeno, overflowCalc);

            if(overflow){
                msg0[0] += 1; // bitul de overflow este pus pe 1
                prints("OVERFLOW DETECTED ON X%d IN SET %d AT %s!\n", id, pair, overflow);
            }
            else{
                msg0[1] = calc;
                msg0[2] = denominator;
                prints("Core%d %d x%d = %lld.%lld\n", id, pair, id, whole, decimal);
            }
        }

        // se depun valorile in cozile respective doar daca nucleul curent nu calculeaza ultimul termen dorit
        if(id != N){
            queue_put_b(&Q[id], (char*)msg0, MSIZE);
            queue_put_b(&Q[N + id], (char*)msg0, MSIZE);
        }
    }
    while(pair < PAIRS - 1);
}

void core_nop(int id) {
    // prints("SimTime: %lld Core%d start\n", *mtime, nid);
    // prints("SimTime: %lld Core%d end\n", *mtime, nid);
}

void (*cores[])(int) = { 
    core_init, core_init, core_nop, core_nop,

    core_nop, core_nop, core_nop, core_nop, 
    core_nop, core_nop, core_nop, core_nop,
    core_nop, core_nop, core_nop, core_nop
};

atomic_int ok = 16;

int main(int hart_id) {
    if (hart_id == 1) {
        // initializare nuclee de calcul
        for(int i = 2; i <= N; i++)
            cores[i] = core_code;

        for (int i = 0; i < QSIZE; i++) {
            queue_init(&Q[i], buff[i], BSIZE, &sem[i]);
        }
    }

    --ok;
    while(ok);

    cores[hart_id](hart_id);

    return 0;
}
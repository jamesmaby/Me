
#include "MeFiFo.h"


void meFiFo_Init(void * pfifo){

    MeFiFo_t * fifo = (MeFiFo_t*)pfifo;

    fifo->rd = 0;            // Index du premier élément que je peux lire
    fifo->wr = 0;            // Index du premier élément que je peux écrire
}


void meFiFo_Write(void * pfifo, char element){

    MeFiFo_t * fifo = (MeFiFo_t*)pfifo;
    uint16_t next_wr = (fifo->wr + 1) & fifo->mask;      // Si j'écris dans la dernière case de mon buffer circulaire, alors repartir à l'indice 0
                                                        // 10000 & 1111 = 00000000

    if ( next_wr == fifo->rd ){  // Je ne peux pas rééecrire par dessus le dernier élément de mon buffer circulaire
        return;
    }

    fifo->buff[fifo->wr] = element;  // Remplir l'indice wr du tableau avec l'élément
    fifo->wr = next_wr;              // Passer à l'élément suivant
}

char meFiFo_Read(void * pfifo){

    MeFiFo_t * fifo = (MeFiFo_t*)pfifo;

    if (fifo->rd == fifo->wr){       // Si je lis au même endroit que j'écris, alors return
        return 0;
    }

    char element = fifo->buff[fifo->rd];  // Lire l'élément
    fifo->rd = (fifo->rd + 1) & fifo->mask;  // Mettre à jour `rd` en mode circulaire 

    return element;
    
}

bool meFiFo_Empty(void * pfifo){

    MeFiFo_t * fifo = (MeFiFo_t*)pfifo;

    if (fifo->rd == fifo->wr){
        return true;
    }else return false;
}

bool meFiFo_Full(void * pfifo){

    MeFiFo_t * fifo = (MeFiFo_t*)pfifo;

    if (fifo->rd == ( (fifo->wr + 1) & fifo->mask )){
        return true;
    }else return false;
}

bool meFiFo_CanWrite(void * pfifo){

    MeFiFo_t * fifo = (MeFiFo_t*)pfifo;

    if (fifo->rd == ( (fifo->wr + 1) & fifo->mask )){
        return false;
    }else return true;
}

bool meFiFo_CanRead(void * pfifo){

    MeFiFo_t * fifo = (MeFiFo_t*)pfifo;

    if (fifo->rd == fifo->wr){
        return false;
    }else return true;
}

uint16_t meFiFo_Count(void * pfifo){

    MeFiFo_t * fifo = (MeFiFo_t*)pfifo;
    uint16_t i = 0;
    uint16_t rd_next = fifo->rd;

    while ( rd_next != fifo->wr){
        i++;
        rd_next++;
    }
    return i;
}

uint16_t meFiFo_Free(void * pfifo){
    
    MeFiFo_t * fifo = (MeFiFo_t*)pfifo;

    if (fifo->wr >= fifo->rd){
        return fifo->mask - (fifo->wr - fifo->rd);
    }else return fifo->mask - (fifo->wr - fifo->rd);
}



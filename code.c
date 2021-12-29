//import libraries
#include <stdio.h>
#include <stdlib.h>
#ifndef __MMU__H__
#define __MMU__H__
//define size of VM and size of a page
#define SIZE 65536
#define PAGE_TABLE 128 //size of a page
//define type of variable
typedef short byte_t; //byte_t as short
typedef int address_t; //address_t as int
//define structure hole with one address, one size and the address of the next and previous hole
typedef struct hole {
    address_t adr;
    int sz;
    struct hole *next;
    struct hole *prev;
} hole_t;
// define structure of the memory :
// 1. the physical memory (RAM)
// 2. the frame which tells if each a specific frame is free in the RAM
// 3. the page table, its size is equal to the number of page in the Virtual Memory
// 4. the root which is related to the first hole
typedef struct {
    byte_t physmem[SIZE/2];
    byte_t frame[SIZE/PAGE_TABLE];
    byte_t tablePage[SIZE/PAGE_TABLE];
    hole_t *root; // holes list
} mem_t;
// dynamically allocates a mem_t structure and initializes its content
mem_t *initMem();
// allocates space in bytes (byte_t) using First-Fit in the virtual memory
address_t myAllocCont(mem_t *mp, int sz);
// puts correct addresses in the page table and the frame table
address_t myAlloc(mem_t *mp, int sz);
// release memory that has already been allocated previously
hole_t* myContFree(mem_t *mp, address_t p, int sz);
// check if page is free and do the necessary in the page table and the frame table
void myFree(mem_t *mp, address_t p, int sz);
// assign a value to a byte
void myWrite(mem_t *mp, address_t p, byte_t val);
// read memory from a byte
byte_t myRead(mem_t *mp, address_t p);
#endif
//display results of the simulation
void printEvrth(mem_t *mp){
    //get the first hole
    hole_t *h1 = mp->root;
    //declare variable i for the display
    int i=1;
    //while the hole is not NULL
    while(h1){
        //display hole number
        printf("Hole number %d :\n",i);
        i++;
        //display the first and last addresses of the hole in an interval
        printf("[ %d, %d ]\n",h1->adr,h1->adr+h1->sz);
        //go to next hole
        h1 = h1->next;
    }
/*
printf("///////PAGE TABLE///////\n");
for(i=0;i<SIZE/PAGE_TABLE;i++){
printf("%d -> %d\t",i*PAGE_TABLE,mp->tablePage[i]);
if(mp->frame[i]==0)
printf("frame is empty\n");
else printf("frame is used\n");
}*/
}
int main() {
//initialize the memory for the exercise and return the mem variable
    mem_t *mem = initMem();
    //allocate three different elements in the memory with different sizes and
    return the address
    address_t adr1 = myAlloc(mem, 5); //allocate a size of 5 and return the
    address where it was allocated
    address_t adr2 = myAlloc(mem, 10);
    address_t adr3 = myAlloc(mem, 100);
    //free a specific size at an address specified
    myFree(mem, adr2, 10);//here we want to free in the memory an element with a
    size of 5 and at the address adr2 returned in the second allocation
    myFree(mem, adr1, 5);
    //write in the physical memory
    myWrite(mem, adr3, 543); // write on the 1st byte
    myWrite(mem, adr3+9, 34); // write on the 10th byte
    //read in the physical memory
    byte_t val1 = myRead(mem, adr3); // get the byte at address adr3 by
    returning it
    byte_t val2 = myRead(mem, adr3+9);// get the byte at address adr3+9
    //display the result of the simulation
    printEvrth(mem);
    return 0;
}
// assign a value to a byte
void myWrite(mem_t *mp, address_t p, byte_t val){
    int beg = p/PAGE_TABLE;//get the index of the page table of the address
    //assign value
    //here we assign the value in physmem at the corresponding address (translate with tablePage) and at the correct offset (using the result of p%PAGE_TABLE)
    mp->physmem[mp->tablePage[beg]+(p%PAGE_TABLE)] = val;
}
//read the value of a certain byte
byte_t myRead(mem_t *mp, address_t p){
    int beg = p/PAGE_TABLE;//get the index of the page table of the address
    //return the byte of the address corresponding to address in the VM
    //get address with value at index of page table + offset of the address
    return mp->physmem[mp->tablePage[beg]+(p%PAGE_TABLE)];
}
//create holes and free table page(s) if necessary
void myFree(mem_t *mp, address_t p, int sz){
//get the hole released we the specific address and size
    hole_t *h1 = myContFree(mp, p, sz);
    //if the hole exists we go on or we abort the operation
    if(!h1){
        return;
    }
//we define the first and last page indexes released thanks to the hole returned
//it defines a range of pages (from the beg to end) that must be released in the table page to tell that they are now available
    int beg = h1->adr/PAGE_TABLE;
    int end = (h1->adr+sz+(p-h1->adr))/PAGE_TABLE;
//if there is many pages between the first and last page (at least one) we enter in the loop
// we start at the page with index "beg"
    for(int i=beg+1;i<end;i++){
    //if page table is not empty condition is verified
        if(mp->tablePage[i]!=-1){
        //frame is now available and the table page at the index i is also available (set respectively to 0 and -1)
            mp->frame[mp->tablePage[i]/PAGE_TABLE]=0;
            mp->tablePage[i]=-1;
        }
    }
//if the last and first page are different and the last page is entirely free, we free the page table at the last index and we free the frame
    if(beg!=end && h1->sz%PAGE_TABLE==0){
        mp->frame[mp->tablePage[end]/PAGE_TABLE]=0;
        mp->tablePage[end]=-1;
    }
//if the first page is free : its first address is the first offset and the size is bigger than a page size we free the page table at this index
    if(h1->adr%PAGE_TABLE==0 && sz>PAGE_TABLE-1){
        mp->frame[mp->tablePage[beg]/PAGE_TABLE]=0;
        mp->tablePage[beg]=-1;
    }
}
//creating the holes
hole_t* myContFree(mem_t *mp, address_t p, int sz){
//initialize the hole h1 as the root (first hole of the VM)
    hole_t *h1 = mp->root;
    //allocate memory for the new hole if it is created
    hole_t *h2 = malloc(sizeof(*h2));
    int condi=0;
    //allocate third hole, this one will be the one returned to myFree function
    hole_t *h3 = malloc(sizeof(*h3));
    h3 = NULL;//initialize at NULL
    //we go from one hole to another and we check if the address where we want to free the size is before the address of the current one
    while(h1 && condi==0){
    //if the address of the element we want to free is in a hole, we return null
        if(p>=h1->adr && p<((h1->adr)+(h1->sz))){
            h1=NULL;
            condi=1;//we get out of the loop
        }
        else{
        //if we are at the good place -> the hole's address is after the address of the element to free, we get out of the loop
            if(p<h1->adr)
            {
                condi=1;
            }
            else h1 = h1->next;//else we go to the next hole
        }
    }
    //if h1 is not null - it means we found the place to free, we enter in the condition
    if(h1){
        //we define the address, size of the new hole h2
        h2->adr = p;
        h2->sz = sz;
        h2->next = h1;
        h2->prev = (h1->prev);
        //check if we are not at the root to avoid error
        if(h1->prev!=NULL)
            (h1->prev)->next = h2;
        //define previous hole
        h1->prev = h2;
        //the hole to return for the moment is the hole h3
        h3 = h2;
        //if my new hole joins the next hole the condition is verified
        if(h2->adr+h2->sz>=h1->adr)
        {
        //we join the hole h1 and h2 so we redefine the size and the address of the new hole which is h1
            h1->adr=h2->adr;
            h1->sz+=h2->sz;
            h1->prev = h2->prev;
            if(h2->prev!=NULL)
            (h2->prev)->next=h1;
            //we free h2 because we don't need it anymore
            free(h2);
            //the hole return will be h1
            h3 = h1;
            if(h1->prev != NULL){
                //we check now if the new hole also joins the previous hole
                if(h1->adr <= h1->prev->adr + h1->prev->sz){
                //if it is the case we redefine the configuration of the big hole
                    h1->adr = h1->prev->adr;
                    h1->sz += h1->prev->sz;
                    h3 = h1;//the hole returned is h1 (we don't need the prev and next attribute in the function myFree so we can give it this value)
                    h1 = h1->prev;
                    //we go on the previous hole and we check if it the root
                    if(h1 == mp->root){
                        //if it is we redefine the root and we free h1 because we dont need it anymore
                        h1->next->prev = h1->prev;
                        mp->root = h1->next;
                        free(h1);
                    }
                    else{
                        //else the previous hole is now equal to the big hole and we can free h1
                        h1->next->prev = h1->prev;
                        free(h1);
                    }
                }
            }
        }
        else if(h2->prev != NULL){
        //we check if the new hole joins the previous hole, if it is the case we redefine the new hole
            if(h2->adr <= h2->prev->adr + h2->prev->sz){
                //we define the new size and we dont need to define new address
                h2->prev->sz += h2->sz;
                h2->prev->next = h1;
                h1->prev = h2->prev;
                //once it is defined we can free h2 because the new hole is the previous one
                free(h2);
                h3 = h1;
            }
        }
        else if(h1==mp->root)
            mp->root = h2;//if h1 is the root, the new root is the previous hole: h2
        //h2 is inserted before h1 so if it is not free it is the new root
    }
    //return the hole inserted with join
    return h3;
}
//initialize memory
mem_t *initMem(){
//allocate memory for the struct mem_t
    mem_t *mt = malloc(sizeof(*mt));
    //allocate memory to create the first hole
    hole_t *ht = malloc(sizeof(*ht));
    //initialize the first hole with the address (= 0), the size of the VM and the hole around it
    ht->adr=0;
    ht->sz=SIZE;
    ht->next=NULL;
    ht->prev=NULL;
    //assign the root as the first hole
    (mt->root)= ht;
    //initialize the tablePage and the frame
    //their initial values tell that the place is free at this index (-1 for tablePage and 0 for frame)
    for(int i=0;i<SIZE/PAGE_TABLE;i++){
        mt->tablePage[i]=-1;
        mt->frame[i]=0;
    }
    //return the mem_t struct
    return mt;
}
//allocation of memory in VM and association to the page table
address_t myAlloc(mem_t *mp, int sz){
//we get the address of where the size has been allocated to in VM
    address_t addr = myAllocCont(mp, sz);
    //we define the first and last page index
    int beg = addr/PAGE_TABLE;
    int end = (addr+sz)/PAGE_TABLE;
    //define i as the index of first page
    int i=beg;
    //force enter in loop at least once
    do{
        //if the tablePage is empty it means that we have to link the VM to a frame and enter its frame address in the page table
        if(mp->tablePage[i]==-1){
            int k;
            //loop to check where the frame is free thanks to the frame table (allow multiple processes to check it at same time)
            for(k=0;k<SIZE/PAGE_TABLE;k++){
            //if the frame is empty
                if(mp->frame[k]==0){
                    //frame is now used
                    mp->frame[k]=-1;
                    //give frame address to the page table at the index of the VM
                    mp->tablePage[i]=k*PAGE_TABLE;
                    break;//get out of the loop
                }
            }
        }
        i++;//increment i to go to the next page
    }while(i<=end);//while we haven't looped through all the pages allocated in the VM (from the first page to the last page)
    //return the address where the size has been allocated
        return addr;
}

//allocate an element with a certain size to the virtual memory
address_t myAllocCont(mem_t *mp, int sz){
    //nvx hole is the first hole (root)
    hole_t *nvx = (mp->root);
    address_t addr;//declare address
    //while nvx is not null and we don't have enough place to allocate the size we go from one hole to the next
    while(nvx && nvx->sz < sz){
        nvx = nvx->next;
    }
    //if we found a place, nvx is not null and the condition is verified (it means we found a place to allocate the size)
    if(nvx){
    //we define the address as the hole where size is big enough to be allocated
        addr = nvx->adr;
        //if size is big enough and the hole is not empty
        if(nvx->sz > sz){
        //we remove available size because we have allocated some so the hole is shorter
            nvx->sz -= sz;
            //the address where we can allocate the memory is now has a bigger value (e.g. the address was at 10 and we allocate 5 so now the address where the hole is available is at 15)
            nvx->adr +=sz;
        }
        else{
        //if we filled the hole
        //the previous hole has as next hole the next hole of the current hole
            if(nvx->prev!=NULL)
                (nvx->prev)->next = nvx->next;
            //the next hole has as previous hole the previous hole of the current hole
            if(nvx->next!=NULL)
                (nvx->next)->prev = nvx->prev;
            //we free the current hole because it is filled
            free(nvx);
        }
        return addr;//we return the address of the allocation
    }
    else
        return -1;//else we return -1 to explain that we didn't find any place to allocate our element
}
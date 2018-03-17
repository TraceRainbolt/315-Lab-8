#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define CACHE_SIZE 256
#define ASSOCIATIVITY 4 
#define AMAX 10
#define CACHESIM 1    /* Set to 1 if simulating Cache */

#define BLOCK_BITS 4

/* memory management, code density, Cache emulation - statistics generation */
/* Generated for CSC 315 Lab 5 */

typedef struct Fields {
   unsigned byte;
   unsigned index;
   unsigned tag;
} Fields;

typedef struct CacheLine {
   unsigned tag;
   unsigned data;
   unsigned valid;
} CacheLine;

typedef struct CacheSet {
   CacheLine cacheLines[ASSOCIATIVITY];
} CacheSet;

CacheSet cache[CACHE_SIZE]; 

bool is64Bit;
int index_bits;

void get_fields(Fields *fields, int *pointer){
   unsigned address = (unsigned long) pointer;
   unsigned tag_shift = is64Bit ? 64 - index_bits - BLOCK_BITS : 32 - index_bits - BLOCK_BITS;

   fields->byte = address & 0xF;
   fields->index = ((address >> BLOCK_BITS) & (0xFFFFFFFF >> index_bits)) % CACHE_SIZE;
   fields->tag = (address >> tag_shift);
}

/* This function gets called with each "read" reference to memory */
void mem_read(int *mp){
   Fields fields;
   CacheSet set;
   int i;

   getFields(&fields, mp);
   /* printf("Memory read from location %p\n", mp);  */
   printf("Set number: %0X\n", fields.index);
   set = cache[fields.index];

   for(i = 0; i < ASSOCIATIVITY; i++){
      CacheLine line = set.cacheSet[i];
   }

}


/* This function gets called with each "write" reference to memory */
void mem_write(int *mp) {
   int index; 
   Fields fields;
   CacheSet cacheset;
   CacheLine cacheline;

   writes++;
   get_fields(&fields, mp);
   cacheset = cache[fields.index];
   index = random_replce(cacheset);
   /* printf("Memory write to location %p\n", mp); */
   cacheline = cacheset.cacheLines[index];

   cacheline.valid = 1;
   cacheline.tag = fields.tag;
   cacheline.data = *mp;

}


/* Statically define the arrays a, b, and mult, where mult will become the cross product of a and b, i.e., a x b. */

static int a[AMAX][AMAX], b[AMAX][AMAX], mult[AMAX][AMAX];


void matmul(int r1, int c1, int c2){
   int i,j,k;
   int *mp1, *mp2, *mp3;

   /* Initializing elements of matrix mult to 0.*/
   for(i=0; i<r1; ++i)
      for(j=0; j<c2; ++j){
         mult[i][j]=0;
      }

   /* Multiplying matrix a and b and storing in array mult. */
   for(i=0; i<r1; ++i)
      for(j=0; j<c2; ++j)
         for(k=0; k<c1; ++k){
            #if CACHESIM      /* "Hooks" to measure memory references - enabled if CACHESIM  */
               mp1 = &mult[i][j];
               mp2 = &a[i][k];
               mp3 = &b[k][j];   
               mem_read(mp1);
               mem_read(mp2);
               mem_read(mp3);
               mem_write(mp1); 
            #endif
              mult[i][j]+=a[i][k]*b[k][j];
         }
}


int main() {
   int r1, c1, r2, c2, i, j, k;

   int *mp1, *mp2, *mp3;

   index_bits = ceil(log2(CACHE_SIZE));

   printf("Size of pointer is: %d\n\n", (unsigned) sizeof(mp1));

   is64Bit = (unsigned) sizeof(mp1) == 16;

   printf("Enter rows and column for first matrix: ");
   scanf("%d%d", &r1, &c1);
   printf("Enter rows and column for second matrix: ");
   scanf("%d%d",&r2, &c2);

   /* If column of first matrix in not equal to row of second matrix, asking user to enter the size of matrix again. */
   while (c1 != r2) {
      printf("Error! column of first matrix not equal to row of second.\n");
      printf("Enter rows and column for first matrix: ");
      scanf("%d%d", &r1, &c1);
      printf("Enter rows and column for second matrix: ");
      scanf("%d%d",&r2, &c2);
   }

   /* Storing elements of first matrix. */

   printf("\nEnter elements of matrix 1:\n");
 
   for(i=0; i<r1; ++i)
      for(j=0; j<c1; ++j){
         // printf("Enter elements a%d%d: ", i + 1, j + 1);
         // scanf("%d",&a[i][j]);
         a[i][j] = i+j; // build sample data
      }

   /* Storing elements of second matrix. */
   printf("\nEnter elements of matrix 2:\n");

   for(i=0; i<r2; ++i)
     for(j=0; j<c2; ++j) {
         // printf("Enter elements b%d%d: ", i + 1, j + 1);
         // scanf("%d",&b[i][j]);
         b[i][j] = 10 + i + j;
      }


   matmul(r1, c1, c2);     /* Invoke matrix multiply function */  

   /* Displaying the multiplication of two matrix. */
   printf("\nOutput Matrix:\n");

   for(i=0; i<r1; ++i)
     for(j=0; j<c2; ++j){
        printf("%d  ",mult[i][j]);
        if(j==c2-1) printf("\n\n");
     }

   return 0;
}
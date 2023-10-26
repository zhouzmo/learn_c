typedef union
{
   long i;
   int k[5];
   char c;
} DATE; // 20

struct data
{
   int cat;
   DATE cow;
   double dog;
} too;// 4 20 8

DATE max; 
int main(int argc, char const *argv[])
{
    printf("%d",sizeof(struct data)+sizeof(max));// 20 + 32 = 52
    return 0;
}


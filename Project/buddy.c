#include "headers.h"
#include "sortedLinkedList.c"
#include <math.h>

int split(int starting_index, int avaliable_size, int required_size);

struct linkedList *free_Lists[11];
void intialize_free_lists()
{
    /*10:1024
    9:512
    8:256
    7:128
    6:64
    5:32
    4:16
    3:8
    2:4
    1:2
    0:
    */
    for (int i = 1; i < 11; i++)
    {
        free_Lists[i] = creatLinkedList();
    }
    // alloctae 1024 bytes
    InsertOrdered(free_Lists[10], 0); // 0-1024
}

int nearest_power_of_2(int memsize)
{
    //    int n=pow(2,ceil(log2(200)));
    if (memsize == 1)
        return 1;

    int n = ceil((log2(memsize)));
    // printf("%d",n);
    return n;
}

int allocate(int memsize, int *end_mem_index)
{
    // getting nearestt power of 2 for this memsize
    int power_of_2 = nearest_power_of_2(memsize); // 5  30==32

    int index_of_free_list = power_of_2;
    while (isempty(free_Lists[index_of_free_list]))
    {
        index_of_free_list++;
        if (index_of_free_list == 11)
            return -1; // No space for this process
    }

    int start_index_of_Allocation = free_Lists[index_of_free_list]->head->data;
    DeleteNode(free_Lists[index_of_free_list], start_index_of_Allocation);

    int available_size = pow(2, index_of_free_list); // 1024
    int req_size = pow(2, power_of_2);               // 32
    // 0       1024        32
    *end_mem_index = split(start_index_of_Allocation, available_size, req_size); //
    // 10          5
    // 1024       32
    // split 1024 32

    return start_index_of_Allocation;
}

int split(int starting_index, int avaliable_size, int required_size)
{
    // Exit condition
    if (required_size == avaliable_size)
    {
        return starting_index + required_size - 1;
    }
    // 0       size 1024
    // 512    512()512-1023
    // 1.add the right 512 to th free list of 512
    int half = avaliable_size / 2;
    int right_start_index = starting_index + half; // 0 +1024/2= 265
    // add this new index to the free list of 512
    int index_of_free_list = ceil(log2(half));

    InsertOrdered(free_Lists[index_of_free_list], right_start_index); // 0-1024
    // 2.further split left half
    return split(starting_index, half, required_size);
}
int deallocate(int memsize, int mem_start_index, int mem_end_index)
{
    int start, end, size;
    int power_of_2 = nearest_power_of_2(memsize);
    int index_of_list = power_of_2;
    if (mem_start_index % 2 != 0)
        return -1;
    start = mem_start_index;
    end = mem_end_index;
    size = pow(2, power_of_2);
    for (int i = 0; i < 11; i++)
    {
        if ((start / size) % 2 == 0)
        {
            if (findNode(free_Lists[index_of_list], end + 1))
            {
                DeleteNode(free_Lists[index_of_list], end + 1);
                start = start;
                end = end + pow(2, power_of_2);
                size = size * 2;
            }
            else
                break;
        }
        else
        {
            if (findNode(free_Lists[index_of_list], start - pow(2, power_of_2)))
            {
                DeleteNode(free_Lists[index_of_list], start - pow(2, power_of_2));
                start = start - pow(2, power_of_2);
                end = end;
                size = size * 2;
            }
            else
                break;
        }
        index_of_list++;
    }
    index_of_list = ceil(log2(size));
    InsertOrdered(free_Lists[index_of_list], start);
    return 0;
}
// int main(int argc, char *argv[])
// {

//     intialize_free_lists();

//     print_All_Empty_Lists(11, free_Lists);
//     int acutalsize,acutalsize1,acutalsize2,acutalsize3,acutalsize4,acutalsize5;
//     printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nAllocate 200");
//     int w = allocate(200, &acutalsize5);
//     print_All_Empty_Lists(11, free_Lists);
//     printf("\n\n%d\n\n", w);


//     printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nAllocate 150");
//    int x = allocate(150, &acutalsize1);
//     print_All_Empty_Lists(11, free_Lists);
//     printf("\n\n%d\n\n", x);

//     printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nDeallocate 150");
//     x = deallocate(150,x,acutalsize1 );
//     print_All_Empty_Lists(11, free_Lists);
//     printf("\n\n%d\n\n", x);

//     printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nAllocate 100");
//    int y = allocate(100, &acutalsize2);
//     print_All_Empty_Lists(11, free_Lists);
//     printf("\n\n%d\n\n", y);

//     printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nAllocate 80");
//    int z = allocate(80, &acutalsize4);
//     print_All_Empty_Lists(11, free_Lists);
//     printf("\n\n%d\n\n", z);

//     printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nAllocate 300");
//     x = allocate(300, &acutalsize3);
//     print_All_Empty_Lists(11, free_Lists);
//     printf("\n\n%d\n\n", x);
//     printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nDeallocate 100");
//     y= deallocate(100,y,acutalsize2 );
//     print_All_Empty_Lists(11, free_Lists);
//     printf("\n\n%d\n\n", y);
//     printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nDeallocate 300");
//     x= deallocate(300,x,acutalsize3 );
//     print_All_Empty_Lists(11, free_Lists);
//     printf("\n\n%d\n\n", x);
//     printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nDeallocate 80");
//     z= deallocate(80,z,acutalsize4);
//     print_All_Empty_Lists(11, free_Lists);
//     printf("\n\n%d\n\n", z);
//      printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nDeallocate 200");
//     w= deallocate(200,w,acutalsize5 );
//     print_All_Empty_Lists(11, free_Lists);
//     printf("\n\n%d\n\n", w);
//     // printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nAllocate 150");
//     // x = allocate(150, &acutalsize);
//     // print_All_Empty_Lists(11, free_Lists);
//     // printf("\n\n%d\n\n", x);

//     // printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nAllocate 2 bytes");
//     //     int start_index = allocate(2, &end_index);
//     //     //print_All_Empty_Lists(11, free_Lists);
//     //     printf("\n\n from %d to %d\n\n", start_index,end_index);

//     //     printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \nAllocate 1 bytes");
//     //     start_index = allocate(1, &end_index);
//     //     //print_All_Empty_Lists(11, free_Lists);
//     //    printf("\n\n from %d to %d\n\n", start_index,end_index);
//     return 0;
// }

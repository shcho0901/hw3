#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void insertion_C(int* arr, int n);
void insertion_ASM(int* arr, int n);
void randomgenerate(int* arr_1, int* arr_2, int n);

int main(int argc, char *argv[])
{
    srand(time(NULL));

    int n = atoi(argv[1]); // user input
    int *arr_c = (int *)malloc(sizeof(int) * n);
    int *arr_asm = (int *)malloc(sizeof(int) * n);

    int i = 0;
    double runtime_c = 0;
    double runtime_asm = 0;
    struct timespec begin_c, end_c, begin_asm, end_asm;


    for (i = 0; i < n; i++)
    {
        *(arr_c + i) = 0;
        *(arr_asm + i) = 0;
    } // array initialization

    randomgenerate(arr_c, arr_asm, n); // make randomly ordered integer array

    if (n <= 20)
    {
        printf("Before sort    : [ ");
        for (i = 0; i < n; i++)
        {
            printf("%d ", *(arr_c + i));
        }
        printf("]\n"); // print data before sorting
    }

    clock_gettime(CLOCK_MONOTONIC, &begin_c);// measuring time
    insertion_C(arr_c, n);
    clcock_gettime(CLOCK_MONOTONIC, &end_c);// measuring time
    runtime_c = ((double)end_c.tv_sec + 1.0e-9*end_c.tv_nsec) - ((double)begin_c.tv_sec + 1.0e-9*begin_c.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC, &begin_asm);// measuring time
    insertion_ASM(arr_asm, n);
    clock_gettime(CLOCK_MONOTONIC, &end_asm);// measuring time
    runtime_asm = ((double)end_asm.tv_sec + 1.0e-9*end_asm.tv_nsec) - ((double)begin_asm.tv_sec + 1.0e-9*begin_asm.tv_nsec);


    if (n <= 20)
    {
        printf("Asfter sort  (C): [ ");
        for (i = 0; i < n; i++)
        {
            printf("%d ", *(arr_c + i));
        }
        printf("]\n");

        printf("After sort (ASM): [ ");
        for (i = 0; i < n; i++)
        {
            printf("%d ", *(arr_asm + i));
        }
        printf("]\n");
    } // print data after sorting

    printf("Execution Time   (C): %f[s]\n", runtime_c);
    printf("Execution Time (ASM): %f[s]\n", runtime_asm); /// print run time

    return 0;
}

void insertion_C(int* arr, int n)
{
    int i, j, temp;

    for (i = 1; i < n; i++) //맨 처음 값 건너 뛰고 2번째 값부터!
    {
        temp = arr[i]; // temp에 insert하려는 값을 저장해두고
        j = i-1; //j에 한 칸 앞 인덱스 저장

        while(j >= 0 && arr[j] > temp) // j가 0 이상이고, arr[j]가 insert하려는 값보다 크면
        {
            arr[j+1] = arr[j]; //arr[j]를 한 칸 오른쪽으로 당기고
            j--; //j는 1 decrement
        }
        arr[j+1] = temp; //j가 -1까지 왔거나, arr[j]가 insert하려는 값보다 작으면 while문 빠져나와서 arr[j+1]에 temp 저장
    }
}

void insertion_ASM(int* arr, int n)
{
    // C 언어 부분에서 오는 것: arr, n
    // 레지스터에서 새로 정의: i, j, arr[i], arr[j], arr[j+1], offset_i, offset_j
    // r4: offest_i, r5: i, r6: n, r7: arr[i], r8: j, r9: offset_j, r10: arr[j], r11: offset_j+1

    asm(
        "mov r4, #4\n\t"   // reg for address offset (for i)
        "mov r5, #1\n\t"   // i = 1;
        "ldr r6, %[n]\n\t" // reg_n = n;

        "L1:\n\t"                  // Loop 1
            "ldr r7, [%[arr], r4]\n\t" // reg_arr[i] = arr[i]
            "sub r8, r5, #1\n\t"       // j = i - 1
            "sub r9, r4, #4\n\t"       // reg for address offset (for j)
            "add r11, r9, #4\n\t"      // reg for address offset (for j+1)

            "L2:\n\t"                  // Loop 2
                "ldr r10 [%[arr], r9]\n\t" // reg_arr[j] = arr[j]

                "cmp r8, #0\n\t"
                "jb end_L2\n\t" // check condition 1 (j >= 0)

                "cmp r10, r7\n\t"
                "jna end_L2\n\t" // check condition 2 (reg_arr[j] > reg_arr[i])

                "str r10, [%[arr],r11]\n\t" // arr [j+1] = arr[j]
                "sub r8, r8, #1\n\t"        // j--
                "sub r9, r9, #4\n\t"        // offset_j -= 4
                "sub r11, r11, #4\n\t"      // offset_j+1 -= 4

            "end_L2:\n\t" // end Loop 2

            "str r4, [%[arr], r11]\n\t" // arr[j+1] = arr[i]

            "add r5, r5, #1\n\t" // i++
            "add r4, r4, #4\n\t" // offset_i += 4

            "cmp r5, r6\n\t" // check i < reg_n
            "jb L1\n\t"

        :

        :
        [n] "m"(n), [arr] "r"(arr)
        :
        "r4", "r5", "r7", "r8", "r9", "r10", "r11"
    );
}

void randomgenerate(int* arr_1, int* arr_2, int n)
{
    int num = 0;
    int i = 0;
    int j = 0;

    for (i = 0; i < n; i++)   
    {
        num = rand()%n + 1;
        arr_1 = num;
        arr_2 = num;

        for (j = 0; j < i; j++)
        {
            if (arr_1[i] == arr_1[j])
            {
                arr_1[i] = 0;
                arr_2[i] = 0;
                i--;
                break;
            }
        }
    }
}

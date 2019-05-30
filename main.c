#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>

typedef struct
{
    int start_point;
    int end_point;
    int counter;
}range;

typedef struct
{
    range *current_range;
    struct list_of_ranges *next;
}list_of_ranges;


int choise[3];

void save_it(void **point, size_t size);
int def_lucky(int **lucky_num, int n);
range* add_data(int num, int k);
void add_node(list_of_ranges **head, int num, int k);
void arrange_list(list_of_ranges **head);
int verify_num(list_of_ranges **head, int num);
void display_list(list_of_ranges *head);
void read_from_file(int k, list_of_ranges **head);
void def_ranges(int stats_num, int **file_stats);
int randomize(int n, int k, int *lucky_num, int lucky_range);


int main(void)
{
    int k = 1,                                                  //What interval is it
        n = 1;                                                  //How much is this interval
    int *lucky_num;
    int lucky_range;


    while(k <= 3)
    {
        n *= 10;
        if(n != 10)
            printf("\n\n");
        printf("Range:\n0 - %d\n", n);
        lucky_range = def_lucky(&lucky_num, n);
        printf("Steps: %d", randomize(n, k, lucky_num, lucky_range));
        free(lucky_num);
        k++;
    }

    return 1;
}

void save_it(void **point, size_t size)
{
    void *temp = realloc(*point, size);
    if(temp == NULL)
        printf("Error realloc!");
    else
        *point = temp;
}

int def_lucky(int **lucky_num, int n)                           //Lucky numbers: 1 3 7 9 13 15 21 25...
{                                                               //Every even number excluded (2 4 6 8 ...)
    int temp = 0, crit = 1,                                     //Every 3 odd number excluded (5 11 17 ...)
        k = 0, i;                                               //Every 7 odd number excluded (19 ...)
    int range = n;
    int *temp_num;
        temp_num = (int *) malloc(sizeof(int) * range);
    for(i = 0; i < n; i++)
    {
        if(i%2)
            if(temp < 2 && crit != 7)
            {
                if(k >= range)
                {
                    range += n/5;
                    save_it((void **)&temp_num, sizeof(int) * range);
                }

                temp_num[k] = i;
                temp++;
                crit++;
                k++;
            }
            else if(temp == 2)
                temp = 0;
            else
            {
                temp++;
                crit = 1;
            }
    }

    //Add heads: 0 - 10 = {0, 10};
    temp_num[k] = 0;
    temp_num[k + 1] = n;
    k += 2;

    *lucky_num = temp_num;

    return k;
}

range* add_data(int num, int k)
{
    range *new_range = malloc(sizeof(range));

    //Define interval's head
    if(num - pow(10, k-1) <= 0)
        new_range->start_point = 0;
    else
        new_range->start_point = num - pow(10, k-1);

    //Define interval's tale
    if(num + pow(10, k-1) >= (float)pow(10, k))
        new_range->end_point = (float)pow(10, k);
    else
        new_range->end_point = num + pow(10, k-1);

    new_range->counter = 1;

    return new_range;
}

void add_node(list_of_ranges **head, int num, int k)
{
    list_of_ranges *new_range;
    list_of_ranges *temp_range;

    new_range = malloc(sizeof(list_of_ranges));
    new_range->current_range = add_data(num, k);
    new_range->next = NULL;

    if(*head == NULL)
        *head = new_range;
    else
    {
        temp_range = *head;
        while(temp_range->next != NULL)
            temp_range = temp_range->next;

        temp_range->next = new_range;
    }
}

void arrange_list(list_of_ranges **head)
{
    list_of_ranges *current = *head;
    list_of_ranges *cursor;

    int current_counter, cursor_counter;
    range *change;

    while(current->next != NULL)
    {
        cursor = *head;
        while(cursor->next != NULL)
        {
            current_counter = current->current_range->counter;
            cursor_counter = cursor->current_range->counter;

            if(cursor_counter < current_counter)
            {
                change = current->current_range;
                current->current_range = cursor->current_range;
                cursor->current_range = change;
            }
            cursor = cursor->next;
        }
        current = current->next;
    }
}

int verify_num(list_of_ranges **head, int num)
{
    if(*head == NULL)
        return 0;

    list_of_ranges *temp = *head;


    int st_p, end_p;

    while(temp->next != NULL)
    {
        st_p = temp->current_range->start_point;
        end_p = temp->current_range->end_point;

        if(num >= st_p && num <= end_p)
        {
            temp->current_range->counter++;
            //arrange_list(head, temp->current_range);

            return 1;
        }

        temp = temp->next;
    }

    return 0;
}

void display_list(list_of_ranges *head)
{
    while(head != NULL)
    {
        printf("(%d, %d, %d)\n", head->current_range->start_point,
                                 head->current_range->end_point,
                                 head->current_range->counter);
        head = head->next;
    }
}

int rest_num(int n, char lost_num[n], int **lucky_num)
{
    int i, k = 0;
    int *arr;
    int temp = n/2;

    arr = (int *)malloc(sizeof(int) * temp);
    for(i = 0; i < n; i++)
        if(lost_num[i] != '1')
        {
            arr[k] = i;
            k++;

            if(k >= temp)
            {
                temp += n/5;
                save_it(&arr, sizeof(int) * temp);
            }
        }

    *lucky_num = arr;
    return k;
}

void read_from_file(int k, list_of_ranges **head)
{
    int i;

    FILE *frand = fopen("stats.txt", "a+");
    if(frand == NULL)
        printf("File error!");

    int temp;

    while(!feof(frand))
        for(i = 0; i < 3; i++)
        {
            fscanf(frand, "%d", &temp);
            if(i == k-1)
            {
                if(verify_num(head, temp))
                    continue;
                add_node(head, temp, k);
            }
        }

    arrange_list(head);

    //Read all user chooses in file
    if(k == 3)
    {
        fprintf(frand, "\n");
        for(i = 0; i < 3; i++)
        {
            if(i)
                fprintf(frand, " ");
            fprintf(frand, "%d", choise[i]);
        }
    }

    fclose(frand);
}

int randomize(int n, int k, int *lucky_num, int lucky_range)
{
    int i, j, rand_num;

    int step = 1;

    //List of ranges
    list_of_ranges *temp_list,
                   *head = NULL;

    //Array with all numbers
    //If it's 1 number was past
    char lost_num[n];
    for(i = 0; i < n; i++)
        lost_num[i] = '0';

    printf("Enter your choice:\n");
    scanf("%d", &choise[k-1]);

    printf("Game start:\n");
    //Read data from file and define ranges
    read_from_file(k, &head);

    //Display list
    display_list(head);

    //Temporal variables
    int counter = 0;
    int *temp_arr = malloc(sizeof(int) * n);
    int temp_arr_len = 0;

    while(1)
    {
        //Choose number in ranges from list
        if(!temp_arr_len)
            while(!temp_arr_len)
                temp_arr_len = choose_data(head, &temp_arr, &lucky_num, &counter, lucky_range, n);

        //Random number from 0 to length(lucky_num)
        srand(time (NULL));
        rand_num = rand() % (temp_arr_len);

        printf("%d\n", temp_arr[rand_num]);

        if(choise[k-1] == temp_arr[rand_num])
            break;

        //Bubble change
        lost_num[temp_arr[rand_num]] = '1';                    //lucky_num[6] = {1, 3, 7, 9, 0, 10}
        temp_arr_len--; lucky_range--;                         //Random number = 7
        temp_arr[rand_num] = temp_arr[temp_arr_len];           //lucky_num[5] = {1, 3, 10, 9, 0}

        if(!lucky_range)
        {
            free(lucky_num);
            counter = 0;
            lucky_range = rest_num(n, lost_num, &lucky_num);
        }

        step++;
    }

    free(head);
    free(temp_arr);

    return step;
}

int choose_data(list_of_ranges *head, int **temp_arr, int **lucky_num, int *counter, int lucky_range, int n)
{
    int i = 0, k = 0;
    int st_p, end_p;
    int *arr = *temp_arr;
    int *lucky = *lucky_num;

    while(i < *counter)
    {
        head = head->next;
        if(head == NULL)
            break;

        i++;
    }

    //If head is NULL define interval [0 - n]
    if(head == NULL)
    {
        st_p = 0;
        end_p = n;
    }
    else
    {
        st_p = head->current_range->start_point;
        end_p = head->current_range->end_point;
    }

    //Choose number in interval
    for(i = 0; i < lucky_range; i++)
        if(lucky[i] >= st_p && lucky[i] <= end_p)
        {
            arr[k] = lucky[i];
            lucky_range--;
            lucky[i] = lucky[lucky_range];
            k++;
        }

    *temp_arr = arr;
    *lucky_num = lucky;

    (*counter)++;

    return k;
}

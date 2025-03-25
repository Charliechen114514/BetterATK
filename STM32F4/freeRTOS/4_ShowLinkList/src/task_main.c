#include "include/FreeRTOS.h"
#include "include/task.h"
#include "task_main.h"
#include "user_init.h"
#include <stdio.h>

TaskHandle_t
    reverse_led0_state_tskhandle,
    reverse_led1_state_tskhandle,
    key_handling_tskhandle;

#define ITEMS_N (3)
#if ITEMS_N < 3
#error "For Display Issue, ITEMS_N can not be less then 3!"
#endif

void init_my_items(ListItem_t *items)
{
    int begin_value = 10;
    for (uint16_t i = 0; i < ITEMS_N; i++)
    {
        vListInitialiseItem(&items[i]);
        items->xItemValue = begin_value;
        begin_value += 10;
    }
}

static uint8_t inline shell_continue(void)
{
    return isKeyPress(&key1, KEY_LONG_PRESS);
}

#define DISPLAY_BUFFER_SZ (70)
static void printListWithItems(
    List_t *list, ListItem_t *itemsArray,
    const char *header, const char *tails)
{
    char buffer[DISPLAY_BUFFER_SZ];
    send_message(&uart1, header);

    snprintf(buffer, DISPLAY_BUFFER_SZ, "list:                      %#X\n", (int)list);
    send_message(&uart1, buffer);
    snprintf(buffer, DISPLAY_BUFFER_SZ, "list->pxIndex:             %#X\n", (int)list->pxIndex);
    send_message(&uart1, buffer);
    snprintf(buffer, DISPLAY_BUFFER_SZ, "list->xListEnd:            %#X\n", (int)(&list->xListEnd));
    send_message(&uart1, buffer);

    for (uint16_t i = 0; i < ITEMS_N; i++)
    {
        snprintf(buffer, DISPLAY_BUFFER_SZ, "Items %d:                      %#X\n", i, (int)&itemsArray[i]);
        send_message(&uart1, buffer);
    }

    send_message(&uart1, tails);
}

static void insertAndDisplay(
    List_t *list, ListItem_t *itemsArray,
    const char *header, const char *tails)
{
    char buffer[DISPLAY_BUFFER_SZ];
    send_message(&uart1, header);
    for (uint16_t i = 0; i < ITEMS_N; i++)
    {
        vListInsert(list, &itemsArray[i]);
        snprintf(buffer, DISPLAY_BUFFER_SZ, "list->xListEnd->pxNext             %#x\n", (int)(list->xListEnd.pxNext));
        send_message(&uart1, buffer);
        snprintf(buffer, DISPLAY_BUFFER_SZ, "ListItem%d->pxNext                     %#x\n", i, (int)(itemsArray[i].pxNext));
        send_message(&uart1, buffer);
        snprintf(buffer, DISPLAY_BUFFER_SZ, "list->xListEnd->pxPrevious         %#x\n", (int)(list->xListEnd.pxPrevious));
        send_message(&uart1, buffer);
        snprintf(buffer, DISPLAY_BUFFER_SZ, "ListItem%d->pxNext                     %#x\n", i, (int)(itemsArray[i].pxPrevious));
        send_message(&uart1, buffer);

        send_message(&uart1, "press buttons to continue...\n");
        while (!shell_continue())
            vTaskDelay(pdMS_TO_TICKS(10));
    }
    send_message(&uart1, tails);
}

static void __del_and_reinsert(
    List_t *list, ListItem_t *itemsArray,
    const char *header, const char *tails)
{
    char buffer[DISPLAY_BUFFER_SZ];
    uxListRemove(&itemsArray[2]); // 删除itemsArray[2]
    snprintf(buffer, DISPLAY_BUFFER_SZ, "remove list items2\r\n");
    send_message(&uart1, buffer);
    snprintf(buffer, DISPLAY_BUFFER_SZ, "list->xListEnd->pxNext        %#x                    \r\n", (int)(list->xListEnd.pxNext));
    send_message(&uart1, buffer);
    snprintf(buffer, DISPLAY_BUFFER_SZ, "itemsArray[1]->pxNext                 %#x                    \r\n", (int)(itemsArray[1].pxNext));
    send_message(&uart1, buffer);
    snprintf(buffer, DISPLAY_BUFFER_SZ, "itemsArray[3]->pxNext                 %#x                    \r\n", (int)(itemsArray[3].pxNext));
    send_message(&uart1, buffer);
    snprintf(buffer, DISPLAY_BUFFER_SZ, "list->xListEnd->pxPrevious    %#x                    \r\n", (int)(list->xListEnd.pxPrevious));
    send_message(&uart1, buffer);
    snprintf(buffer, DISPLAY_BUFFER_SZ, "itemsArray[1]->pxPrevious             %#x                    \r\n", (int)(itemsArray[1].pxPrevious));
    send_message(&uart1, buffer);
    snprintf(buffer, DISPLAY_BUFFER_SZ, "itemsArray[3]->pxPrevious             %#x                    \r\n", (int)(itemsArray[3].pxPrevious));
    send_message(&uart1, buffer);

    send_message(&uart1, "press buttons to continue...\n");
    while (!shell_continue())
        vTaskDelay(pdMS_TO_TICKS(10));

    list->pxIndex = list->pxIndex->pxNext;
    vListInsertEnd(list, &itemsArray[2]);

    snprintf(buffer, DISPLAY_BUFFER_SZ, "list->pxIndex                 %#x                    \r\n", (int)list->pxIndex);
    send_message(&uart1, buffer);

    snprintf(buffer, DISPLAY_BUFFER_SZ, "list->xListEnd->pxNext        %#x                    \r\n", (int)(list->xListEnd.pxNext));
    send_message(&uart1, buffer);

    snprintf(buffer, DISPLAY_BUFFER_SZ, "itemsArray[2]->pxNext                 %#x                    \r\n", (int)(itemsArray[2].pxNext));
    send_message(&uart1, buffer);

    snprintf(buffer, DISPLAY_BUFFER_SZ, "itemsArray[1]->pxNext                 %#x                    \r\n", (int)(itemsArray[1].pxNext));
    send_message(&uart1, buffer);

    snprintf(buffer, DISPLAY_BUFFER_SZ, "itemsArray[3]->pxNext                 %#x                    \r\n", (int)(itemsArray[3].pxNext));
    send_message(&uart1, buffer);

    snprintf(buffer, DISPLAY_BUFFER_SZ, "list->xListEnd->pxPrevious    %#x                    \r\n", (int)(list->xListEnd.pxPrevious));
    send_message(&uart1, buffer);

    snprintf(buffer, DISPLAY_BUFFER_SZ, "itemsArray[2]->pxPrevious             %#x                    \r\n", (int)(itemsArray[2].pxPrevious));
    send_message(&uart1, buffer);

    snprintf(buffer, DISPLAY_BUFFER_SZ, "itemsArray[1]->pxPrevious             %#x                    \r\n", (int)(itemsArray[1].pxPrevious));
    send_message(&uart1, buffer);

    snprintf(buffer, DISPLAY_BUFFER_SZ, "itemsArray[3]->pxPrevious             %#x                    \r\n", (int)(itemsArray[3].pxPrevious));
    send_message(&uart1, buffer);
}

void task_main_1(void *params)
{
    List_t list;
    ListItem_t items[3];
    vListInitialise(&list);
    init_my_items(items);

    printListWithItems(
        &list, items,
        "Display the lists address with items address!\n",
        "Display the lists address with items address Done!\n");

    send_message(&uart1, "press buttons to continue...\n");
    while (!shell_continue())
        vTaskDelay(pdMS_TO_TICKS(10));

    insertAndDisplay(&list, items,
                     "Display the lists insertion!\n",
                     "Display the lists insertion Done!\n");

    __del_and_reinsert(&list, items,
                       "Display the lists insertion!\n",
                       "Display the lists insertion Done!\n");
    while (1)
    {
        reverse_ccgpio_pinstate(&led0);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void task_main_2(void *params)
{
    (void)params;
    while (1)
    {
        reverse_ccgpio_pinstate(&led1);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    // if the task quit anyway, we need to delete it
    vTaskDelete(NULL);
}

// create task
void start_task(void *pvParams)
{
    (void)pvParams; // oh, we don't use it
    // to let the task running always the first
    // or, set the tasks with less priorities, that's all ok
    taskENTER_CRITICAL();
    send_message(&uart1, "\nTask shell create one demo task to show lists\n");

    xTaskCreate(
        (TaskFunction_t)task_main_1,
        "displaylists",
        (const StackType_t)(configMINIMAL_STACK_SIZE * 7),
        NULL,
        (UBaseType_t)configMAX_PRIORITIES - 1,
        &reverse_led0_state_tskhandle);
    send_message(&uart1, "creating the task1 done!\n");

    xTaskCreate(
        (TaskFunction_t)task_main_2,
        "flipled",
        (const StackType_t)(configMINIMAL_STACK_SIZE),
        NULL,
        (UBaseType_t)configMAX_PRIORITIES - 1,
        &reverse_led0_state_tskhandle);
    send_message(&uart1, "creating the task2 done!\n");

    // at the end, we removed the task depatcher
    vTaskDelete(NULL);

    taskEXIT_CRITICAL();
}
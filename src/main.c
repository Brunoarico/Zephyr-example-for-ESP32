#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/sem.h>
#include <zephyr/sys/printk.h>
#include <zephyr/random/rand32.h>

const int ON = 1;
const int OFF = 0;

//#define ENABLE_SEM
//#define EQUAL_PRIORITY

#ifdef EQUAL_PRIORITY
    #define A_PRIORITY 7
    #define B_PRIORITY 7
#endif

#ifndef EQUAL_PRIORITY
    #define A_PRIORITY 1
    #define B_PRIORITY 7
#endif

#define DT_DRV_COMPAT espressif_esp32_gpio
#define SLEEP_TIME    1000
#define LED_PIN       2
#define STACKSIZE     1024

#define MATRIX_SIZE 1000

//
const struct device *led;

K_THREAD_STACK_DEFINE(threadA_stack_area, STACKSIZE);
static struct k_thread threadA_data;

K_THREAD_STACK_DEFINE(threadB_stack_area, STACKSIZE);
static struct k_thread threadB_data;

#ifdef ENABLE_SEM
K_SEM_DEFINE(on_sem, 0, 1);
K_SEM_DEFINE(off_sem, 0, 1);
#endif


//Function to intensive cpu consumption using matrix multiplication
void matrixMulSum() {
    int i, j, k;
    int multiply[MATRIX_SIZE][MATRIX_SIZE];
    int first[MATRIX_SIZE][MATRIX_SIZE];
    int second[MATRIX_SIZE][MATRIX_SIZE];
    int sum = 0;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            first[i][j] = sys_rand32_get() % 100;
            second[i][j] = sys_rand32_get() % 100;
        }
    }
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            for (k = 0; k < MATRIX_SIZE; k++) {
                sum = sum + first[i][k] * second[k][j];
            }
            multiply[i][j] = sum;
            sum = 0;
        }
    }
}

void configLed(){
	int ret;
	led = DEVICE_DT_GET(DT_NODELABEL(gpio0));
	if (led == NULL) {
		printk("Error led\n");
		return;
	}

	ret = gpio_pin_configure(led, LED_PIN, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Error ret\n");
		return;
	}
}

void worker(void *p1, void *p2, void *p3) {
    char *name = (char *)p1;
    int action = *((int *)p2);
	ARG_UNUSED(p3);

	int cpu_number = arch_curr_cpu()->id;
	uint64_t time_stamp;
	int64_t delta_time;
    int i=0;
	while (1) {
        #ifdef ENABLE_SEM
		k_sem_take(&off_sem, K_FOREVER);
        #endif

		time_stamp = k_uptime_get();
		matrixMulSum();
		delta_time = k_uptime_delta(&time_stamp);

		gpio_pin_set(led, LED_PIN, action);
        printk("Thread %s running on cpu %d turn led ON (iteration %d time: %lld)\n", name, cpu_number, i, delta_time);
		
        k_sleep(K_MSEC(500));
        #ifdef ENABLE_SEM
		k_sem_give(&on_sem);
        #endif
        i++;
	}
}

void configThreads() {
	k_thread_create(&threadA_data, 
                    threadA_stack_area,
                    K_THREAD_STACK_SIZEOF(threadA_stack_area), 
                    worker, 
                    "A", 
                    (void *)&ON, 
                    NULL, 
                    A_PRIORITY, 
                    0,
                    K_FOREVER);
	k_thread_name_set(&threadA_data, "thread_a");

	k_thread_create(&threadB_data, 
                    threadB_stack_area,
			        K_THREAD_STACK_SIZEOF(threadB_stack_area), 
                    worker, 
                    "B", 
                    (void *)&OFF, 
                    NULL, 
                    B_PRIORITY, 
                    0,
			        K_FOREVER);
	k_thread_name_set(&threadB_data, "thread_b");
}

void main() {
	configLed();

	configThreads();

	k_thread_start(&threadA_data);
	k_thread_start(&threadB_data);
    #ifdef ENABLE_SEM
	k_sem_give(&on_sem);
    #endif
}
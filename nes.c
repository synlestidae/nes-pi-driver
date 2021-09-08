#include <wiringPi.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define LATCH 18
#define CLOCK 4
#define SERIAL 23

#define CONTROL_DELAY (16 * 1000)
#define LATCH_DELAY 20
#define PULSE_DELAY 20

//#define DEBUG
#define PRINT printf

#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_SELECT 2
#define BUTTON_START 3
#define BUTTON_UP 4
#define BUTTON_DOWN 5
#define BUTTON_LEFT 6
#define BUTTON_RIGHT 7

#define UDELAY usleep

void setupbuttons(void) {
  #ifdef DEBUG
  PRINT("Setting up pins");
  #endif
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);   
  pinMode(SERIAL, INPUT);   
}

char readbuttons(void) 
{
  char output = 0;

  digitalWrite(LATCH, HIGH); 
  UDELAY(LATCH_DELAY);

  int val = digitalRead(SERIAL);

  #ifdef DEBUG
  PRINT("Val: %d\b", val);
  #endif

  output = output | (val^1);

  digitalWrite(LATCH, LOW);
  UDELAY(LATCH_DELAY);

  //return output;

  for (int i = BUTTON_B; i <= BUTTON_RIGHT; i++) {
    //pulse the clock high
    digitalWrite(CLOCK, HIGH);
    #ifdef DEBUG
    PRINT("Clock = 1");
    getchar();
    #endif
    UDELAY(PULSE_DELAY);

    // it is zero if button pressed
    output = output | ((digitalRead(SERIAL) ^ 1) << i);

    // clear the clock for next iteration
    digitalWrite(CLOCK, LOW);
    UDELAY(PULSE_DELAY);
    #ifdef DEBUG
    PRINT("Clock = 0");
    getchar();
    #endif
  }
  
  return output;
}

int main (void)
{
  wiringPiSetupGpio();
  setupbuttons();

  digitalWrite(LATCH, LOW);
  digitalWrite(CLOCK, LOW);

  while(1) {
    PRINT("Input: %d\n", readbuttons());

    UDELAY(CONTROL_DELAY);
  }
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert W. Oliver II");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.0.1");

static int __init lkm_example_init(void) {
 printk(KERN_INFO "Hello, World!\n");
 return 0;
}

static void __exit lkm_example_exit(void) {
 printk(KERN_INFO "Goodbye, World!\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);

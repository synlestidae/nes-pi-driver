//#include <wiringPi.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define LATCH 18
#define CLOCK 4
#define SERIAL 23

#define CONTROL_DELAY (16 * 1000)
#define LATCH_DELAY 20
#define PULSE_DELAY 20

#define PRINT printk

#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_SELECT 2
#define BUTTON_START 3
#define BUTTON_UP 4
#define BUTTON_DOWN 5
#define BUTTON_LEFT 6
#define BUTTON_RIGHT 7

#define UDELAY usleep

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

// I/O access
volatile unsigned *gpio;

void setupio()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;


}

void setupbuttons() {
  INP_GPIO(LATCH);
  OUT_GPIO(LATCH);

  INP_GPIO(CLOCK);
  OUT_GPIO(CLOCK);

  INP_GPIO(SERIAL);
  OUT_GPIO(SERIAL);
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
    #endif
    UDELAY(PULSE_DELAY);

    // it is zero if button pressed
    output = output | ((digitalRead(SERIAL) ^ 1) << i);

    // clear the clock for next iteration
    digitalWrite(CLOCK, LOW);
    UDELAY(PULSE_DELAY);
    #ifdef DEBUG
    PRINT("Clock = 0");
    #endif
  }
  
  return output;
}

void digitalWrite(int pin, int level) {
}

int digitalRead(int pin) {
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mate Antunovic");
MODULE_DESCRIPTION("A simple Raspberry PI NES Controller Driver.");
MODULE_VERSION("0.0.1");

static int __init lkm_example_init(void) {
  printk(KERN_INFO "Hello, World!\n");
  setupio();
  setupbuttons();
  return 0;
}

static void __exit lkm_example_exit(void) {
  printk(KERN_INFO "Goodbye, World!\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);

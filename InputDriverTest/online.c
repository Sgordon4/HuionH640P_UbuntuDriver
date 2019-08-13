/** @file example2.c
 *
 * \brief second example of input subsystem driver
 *
 * See Linux Journal Feb. 2003
 *
 * Requires
 *   insmod input
 *   insmod keybdev
 */

#include <linux/input.h>   /* input_dev */
#include <linux/module.h>
/* #include <linux/modversions.h> */
#include <linux/init.h>

/* MODULE_LICENSE( "GPL" ); */

struct input_dev example2_dev;

void example2_timeout( unsigned long unused /* UNUSED */ )
{
  int x;

  for (x=0; x<4; x++) {  /* letter 'A' */
    input_report_key( &example2_dev, KEY_A, 1 );
    /* input_sync( &example2_dev ); */
    input_report_key( &example2_dev, KEY_A, 0 );
    /* input_sync( &example2_dev ); */
  }
  for (x=0; x<4; x++) {  /* letter 'B' */
    input_report_key( &example2_dev, KEY_B, 1 );
    /* input_sync( &example2_dev ); */
    input_report_key( &example2_dev, KEY_B, 0 );
    /* input_sync( &example2_dev ); */
  }

  mod_timer( &example2_dev.timer, jiffies + 10*HZ );
}

static int __init example2_init( void )
{
  /* extra safe initialization */
  memset( &example2_dev, 0, sizeof(struct input_dev) );
#if 0
  init_input_dev( &example2_dev );
#endif


  /* set up descriptive labels */
  example2_dev.name = "Example 2 device";
#if 0
  example2_dev.phys = "A/Fake/Path";
  example2_dev.id.bustype = BUS_HOST;
  example2_dev.id.vendor  = 0x0001;
  example2_dev.id.product = 0x0002;  // our second product
  example2_dev.id.version = 0x0101;  // version 1.1
#else
  example2_dev.idbus     = BUS_ISA;
  example2_dev.idvendor  = 0x0001;
  example2_dev.idproduct = 0x0002;  // our second product
  example2_dev.idversion = 0x0101;  // version 1.1
#endif

  set_bit( EV_KEY, example2_dev.evbit );
  set_bit( KEY_A,  example2_dev.keybit );
  set_bit( KEY_B,  example2_dev.keybit );
  set_bit( KEY_C,  example2_dev.keybit );

  input_register_device( &example2_dev );

  init_timer( &example2_dev.timer );
  example2_dev.timer.function = example2_timeout;
  example2_dev.timer.expires  = jiffies + HZ;
  add_timer( &example2_dev.timer );

  return 0;
}

static void __exit example2_exit( void )
{
  del_timer_sync( &example2_dev.timer );
  input_unregister_device( &example2_dev );
}

module_init( example2_init );
module_exit( example2_exit );

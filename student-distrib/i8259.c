/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = MASK_VALUE; /* IRQs 0-7  */
uint8_t slave_mask = MASK_VALUE;  /* IRQs 8-15 */


/* Initialize the 8259 PIC */
void i8259_init(void) {

    //mask all interrupts
    outb(MASK_VALUE, MASTER_PORT_2);
    outb(MASK_VALUE, SLAVE_PORT_2);

    //send ICWs to master
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_PORT_2);   //map to 0x20 to 0x27
    outb(ICW3_MASTER, MASTER_PORT_2);
    outb(ICW4, MASTER_PORT_2);

    //send ICWs to slave
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_PORT_2); //map to 0x28 to 0x2F
    outb(ICW3_SLAVE, SLAVE_PORT_2);
    outb(ICW4, SLAVE_PORT_2);

    //we can manually enable individual interrupts here
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    if(irq_num > SLAVE_IRQ_NUM || irq_num < 0) {
        //printk("irq_num invalid\n");
        return;
    }
    //if irq_num corresponds to secondary PIC, unmask the secondary PIC
    if(irq_num > MASTER_IRQ_NUM) {
        slave_mask = slave_mask & ~(1 << (irq_num - NUM_IR));
        outb(slave_mask, SLAVE_PORT_2);
    
    }
    //irq_num corresponds to primary PIC
    else {
        master_mask = master_mask & ~(1 << irq_num);
        outb(master_mask, MASTER_PORT_2);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    if(irq_num > SLAVE_IRQ_NUM || irq_num < 0) {
        //printk("irq_num invalid\n");
        return;
    }
    //if irq_num corresponds to secondary PIC, mask the secondary PIC
    if(irq_num > MASTER_IRQ_NUM) {
        slave_mask = slave_mask | (1 << (irq_num - NUM_IR));
        outb(slave_mask, SLAVE_PORT_2);
    
    }
    //irq_num corresponds to primary PIC
    else {
        master_mask = master_mask | (1 << irq_num);
        outb(master_mask, MASTER_PORT_2);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    //irq_num is greater than the number on the PICs or less than 0, is invalid
    if(irq_num > SLAVE_IRQ_NUM || irq_num < 0) {
        printk("irq_num invalid\n");
        return;
    }
    //means that the irq_num corresponds to the secondary PIC
    if(irq_num > MASTER_IRQ_NUM) {

        outb(EOI | (irq_num - NUM_IR), SLAVE_8259_PORT); //sends to slave; we subtract my 8 to obtain the correct IRQ number (0-7) for the secondary PIC
        outb(EOI | IR2, MASTER_8259_PORT);  //lets the master know the secondary interrupt was serviced

    }
    //irq corresponds to primary PIC
    else {
        outb(EOI | irq_num, MASTER_8259_PORT);
    }
}

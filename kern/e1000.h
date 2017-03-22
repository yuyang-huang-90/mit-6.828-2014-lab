#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>
#include <kern/pmap.h>
#include <inc/string.h>

/* PCI Device IDs */
#define E1000_DEV_ID_82540EM             0x100E
/* Register Set. (82543, 82544)
 *
 * Registers are defined to be 32 bits and  should be accessed as 32 bit values.
 * These registers are physically located on the NIC, but are mapped into the
 * host memory address space.
 *
 * RW - register is both readable and writable
 * RO - register is read only
 * WO - register is write only
 * R/clr - register is read only and is cleared when read
 * A - register array
 *
 * The register addr is divided by 4 for use as uint32_t[] indices.
 */
#define E1000_STATUS   (0x00008/4)  /* Device Status - RO */
#define E1000_STATUS   (0x00008/4)  /* Device Status - RO */
#define E1000_TCTL     (0x00400/4)  /* TX Control - RW */
#define E1000_TIPG     (0x00410/4)  /* TX Inter-packet gap -RW */
#define E1000_TDBAL    (0x03800/4)  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    (0x03804/4)  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    (0x03808/4)  /* TX Descriptor Length - RW */
#define E1000_TDH      (0x03810/4)  /* TX Descriptor Head - RW */
#define E1000_TDT      (0x03818/4) /* TX Descripotr Tail - RW */
#define E1000_RAL      (0x05400/4)  /* Receive Address Low 32 bits - RW Array */
#define E1000_RAH      (0x05404/4)  /* Receive Address High 32 bits- RW Array */
#define E1000_MTA      (0x05200/4)  /* Multicast Table Array - RW Array */
#define E1000_RDBAL    (0x02800/4)  /* RX Descriptor Base Address Low - RW */
#define E1000_RDBAH    (0x02804/4)  /* RX Descriptor Base Address High - RW */
#define E1000_RDLEN    (0x02808/4)  /* RX Descriptor Length - RW */
#define E1000_RDH      (0x02810/4)  /* RX Descriptor Head - RW */
#define E1000_RDT      (0x02818/4)  /* RX Descriptor Tail - RW */
#define E1000_RCTL     (0x00100/4)  /* RX Control - RW */
#define E1000_IMS      (0x000D0/4)  /* Interrupt Mask Set - RW */
#define E1000_ICR      (0x000C0/4)  /* Interrupt Cause Read - R/clr */
#define E1000_IMS      (0x000D0/4)  /* Interrupt Mask Set - RW */
#define E1000_ICR      (0x000C0/4)  /* Interrupt Cause Read - R/clr */


 /* Transmit Descriptor bit definitions */
#define E1000_TXD_DEXT	0x20 /* bit 5 in CMD section */
#define E1000_TXD_RS		0x8 /* bit 3 in CMD section */
#define E1000_TXD_DD		0x1 /* bit 0 in STATUS section */
#define E1000_TXD_EOP		0x1 /* bit 0 of CMD section */

/* Transmit Control Registers Bits (TCTL) */
#define E1000_TCTL_EN     0x00000002    /* enable tx */
#define E1000_TCTL_PSP    0x00000008    /* pad short packets */
#define E1000_TCTL_CT     0x00000100    /* collision threshold, set to 0x10 */
#define E1000_TCTL_COLD   0x00040000    /* collision distance, set to 0x40 */

// Transmission IPG offset bits (TIPG)
#define E1000_TIPG_IPGT 0
#define E1000_TIPG_IPGR1 10
#define E1000_TIPG_IPGR2 20

// Receive control bits
#define E1000_RCTL_EN           0x00000002 /* enable*/
#define E1000_RCTL_LBM_NO       0xffffff3f /* no loopback mode, 6 & 7 bit set to 0 */
#define E1000_RCTL_SZ_2048      0xfffcffff /* buffer size at 2048 by setting 16 and 17 bit to 0 */
#define E1000_RCTL_SECRC        0x04000000 /* strip CRC by setting 26 bit to 1 */
#define E1000_RCTL_LPE_NO       0xffffffdf /* disable long packet mode*/

// Receive descriptor status bits
#define E1000_RXD_STATUS_DD		0x1
#define E1000_RXD_STATUS_EOP	0x2

// Receive Timer Interrupt mask
#define E1000_IMS_RXT0	0x80 /* 7th bit */
#define E1000_ICR_RTX0  0x80

// RAH valid bit
#define E1000_RAH_AV  0x80000000        /* Receive descriptor valid */


#define NELEM_MTA 128

/* Transmit Descriptor */
struct e1000_tx_desc
{
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
};

/* Receive Descriptor */
struct e1000_rx_desc {
	uint64_t addr;
	uint16_t length;
	uint16_t csum;
	uint8_t status;
	uint8_t errors;
	uint16_t special;
};

// the ring size
#define TX_RING_SIZE 32
#define RX_RING_SIZE 128

// the packet buffer size
#define PKT_BUF_SIZE 2048

// packet buf
struct packet
{
	char buffer[PKT_BUF_SIZE];
};



int e1000_pci_network_attach(struct pci_func *pcif);

int e1000_transmit(void *addr, size_t length);
int e1000_receive(void *addr, size_t *length);
void e1000_trap_handler(void);
#endif	// JOS_KERN_E1000_H

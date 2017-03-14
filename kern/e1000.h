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
	uint64_t buffer_addr; /* Address of the descriptor's data buffer */
	uint16_t length;     /* Length of data DMAed into data buffer */
	uint16_t csum;       /* Packet checksum */
	uint8_t status;      /* Descriptor status */
	uint8_t errors;      /* Descriptor Errors */
	uint16_t special;
};

// the tx ring size
#define RING_SIZE 32

// the packet buffer size
#define PKT_BUF_SIZE 2048

// packet buf
struct packet
{
	char buffer[PKT_BUF_SIZE];
};



int e1000_pci_network_attach(struct pci_func *pcif);

int e1000_transmit(void *addr, size_t length);

#endif	// JOS_KERN_E1000_H

#include <kern/e1000.h>

// LAB 6: Your driver code here
volatile uint32_t *e1000;
// tx ring Length is 512B (128B aligned)
struct e1000_tx_desc txq[RING_SIZE] __attribute__ ((aligned (16)));
struct packet tx_pkts[RING_SIZE];

static void init_desc();
static void e1000_init();

int e1000_pci_network_attach(struct pci_func *pcif) {
	pci_func_enable(pcif);
	e1000 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
	init_desc();
	//cprintf("e1000 status: 0x%x\n", e1000[E1000_STATUS]);
	e1000_init();
	return 0;
}

void e1000_init()
{
	// init TDBAL
	e1000[E1000_TDBAL] = PADDR(txq);
	e1000[E1000_TDBAH] = 0x0;
	// init TDLEN
	e1000[E1000_TDLEN] = RING_SIZE * sizeof(struct e1000_tx_desc);
	// init TDH TDT
	e1000[E1000_TDH] = 0x0;
	e1000[E1000_TDT] = 0x0;
	// Initialize the Transmit Control Register with the following
	// set enable bit to 1 (TCTL.EN)
	// set pad short packets bit to 1 (TCTL.PSP)
	// set collision threshold to 0x10 (TCTL.CT)
	// set collision distance to 0x40 (TCTL.COLD)
	e1000[E1000_TCTL] |= E1000_TCTL_EN;
	e1000[E1000_TCTL] |= E1000_TCTL_PSP;
	e1000[E1000_TCTL] |= E1000_TCTL_CT;
	e1000[E1000_TCTL] |= E1000_TCTL_COLD;

	// Initialize the Transmit IPG register. The TIPG is broken up into three:
	// Set IPGT, bits 0-9, set to 0xA
	// Set IPGR1, bits 19-10, set to 0x8 (2/3 * 0xc)
	// Set IPGR2, bits, 19-20, set to 0xc
	e1000[E1000_TIPG] |= (0xA << E1000_TIPG_IPGT);
	e1000[E1000_TIPG] |= (0x8 << E1000_TIPG_IPGR1);
	e1000[E1000_TIPG] |= (0xC << E1000_TIPG_IPGR2);

}

void init_desc()
{
	int i;

	memset((void *)txq, 0, sizeof(struct e1000_tx_desc) * RING_SIZE);

	for (i = 0; i < RING_SIZE; i++) {
		txq[i].addr = PADDR(&tx_pkts[i]);
		txq[i].cmd |= E1000_TXD_RS;
		txq[i].status = E1000_TXD_DD;
	}
}

int e1000_transmit(void *addr, size_t length) {
	if (length > PKT_BUF_SIZE)
		panic("e1000_transmit: too large buffer size");

	uint32_t tail_idx = e1000[E1000_TDT];
	// txq is full
	if (!(txq[tail_idx].status & E1000_TXD_DD))
		return -1;

	// has useable slot
	memmove((void *) &tx_pkts[tail_idx], addr, length);
	//turn off DD bit
	txq[tail_idx].status &= ~E1000_TXD_DD;
	// mark the end of pkt
	txq[tail_idx].cmd |= E1000_TXD_EOP;

	txq[tail_idx].length = length;

	// inc tail idx
	e1000[E1000_TDT] = (tail_idx + 1) % RING_SIZE;

	return 0;
}

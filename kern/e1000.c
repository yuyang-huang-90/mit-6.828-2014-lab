#include <kern/e1000.h>

// LAB 6: Your driver code here
volatile uint32_t *e1000;
// tx ring Length is 512B (128B aligned)
struct e1000_tx_desc txq[TX_RING_SIZE] __attribute__ ((aligned (16)));
struct packet tx_pkts[TX_RING_SIZE];

struct e1000_rx_desc rxq[RX_RING_SIZE] __attribute__ ((aligned (16)));
struct packet rx_pkts[RX_RING_SIZE];


static void init_desc();
static void init_recv();
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
	int i;
	// init trasmit
	// init TDBAL
	e1000[E1000_TDBAL] = PADDR(txq);
	e1000[E1000_TDBAH] = 0x0;
	// init TDLEN
	e1000[E1000_TDLEN] = TX_RING_SIZE * sizeof(struct e1000_tx_desc);
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


	// init receive
	// init RAL and RAH
	e1000[E1000_RAL] = 0x12005452;
	e1000[E1000_RAH] = 0x00005634 | E1000_RAH_AV;

	// Initialize MTA
	for (i = 0; i < NELEM_MTA; i++) {
		e1000[E1000_MTA + i] = 0x0;
	}

	// init RBDEL
	e1000[E1000_RDBAL] = PADDR(&rxq);
	e1000[E1000_RDBAH] = 0x0;

	init_recv();

	// init RDLEN
	e1000[E1000_RDLEN] = RX_RING_SIZE * sizeof(struct e1000_rx_desc);

	// init RDH and RDT
	e1000[E1000_RDH] = 0;
	e1000[E1000_RDT] = RX_RING_SIZE - 1;
	// Set RCTL
	// - set RCTL.LPE to 0 for normal operation
	// - set RCTL.BSIZE to 2048 bytes
	// - set RCTL.SECRC bit
	// - disable RCTL.LPE
	// - enable RCTL.EN
	e1000[E1000_RCTL] &= E1000_RCTL_LBM_NO;
	e1000[E1000_RCTL] &= E1000_RCTL_SZ_2048;
	e1000[E1000_RCTL] |= E1000_RCTL_SECRC;
	e1000[E1000_RCTL] &= E1000_RCTL_LPE_NO;
	e1000[E1000_RCTL] |= E1000_RCTL_EN;
}

void init_recv()
{
	int i;
	memset((void *)rxq, 0, sizeof(struct e1000_rx_desc) * RX_RING_SIZE);

	for (i = 0; i < RX_RING_SIZE; ++i) {
		rxq[i].addr = PADDR(&rx_pkts[i]);
	}
}

void init_desc()
{
	int i;

	memset((void *)txq, 0, sizeof(struct e1000_tx_desc) * TX_RING_SIZE);

	for (i = 0; i < TX_RING_SIZE; i++) {
		txq[i].addr = PADDR(&tx_pkts[i]);
		txq[i].cmd |= E1000_TXD_RS;
		txq[i].status = E1000_TXD_DD;
	}
}

int e1000_transmit(void *addr, size_t length)
{
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
	e1000[E1000_TDT] = (tail_idx + 1) % TX_RING_SIZE;

	return 0;
}

int e1000_recv(void *addr, size_t *length)
{
	size_t tail_idx = (e1000[E1000_RDT] + 1) % RX_RING_SIZE;

	//rxq is empty
	if ((rxq[tail_idx].status & E1000_RXD_STATUS_DD) == 0)
		return -1;

	if ((rxq[tail_idx].status & E1000_RXD_STATUS_EOP) == 0)
		panic("e1000_recv: EOP flag not set, all packets should fit in one buffer");

	*length = rxq[tail_idx].length;

	memmove(addr, &rx_pkts[tail_idx], *length);

		// clear DD and EOP bits
	rxq[tail_idx].status &= ~(E1000_RXD_STATUS_DD);
	rxq[tail_idx].status &= ~(E1000_RXD_STATUS_EOP);

	e1000[E1000_RDT] = tail_idx;
	return 0;
}

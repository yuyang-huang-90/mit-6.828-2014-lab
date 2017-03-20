#include "ns.h"

extern union Nsipc nsipcbuf;

void
input(envid_t ns_envid)
{
	binaryname = "ns_input";

	// LAB 6: Your code here:
	// 	- read a packet from the device driver
	//	- send it to the network server
	// Hint: When you IPC a page to the network server, it will be
	// reading from it for a while, so don't immediately receive
	// another packet in to the same physical page.
	int perm = PTE_P | PTE_W | PTE_U;
	size_t length;
	int r;
	char pkt_buf[2048]; //PKT_BUF_SIZE

	while (1) {
		//alloc nsipcbuf
		if ((r = sys_page_alloc(0, &nsipcbuf, perm)) < 0)
			panic("input; unable to allocate new page. error %e", r);

		//keep receive
		while (sys_e1000_receive((void*) pkt_buf, &length) != 0);

		memmove(nsipcbuf.pkt.jp_data, pkt_buf, length);
		nsipcbuf.pkt.jp_len = length;

		ipc_send(ns_envid, NSREQ_INPUT, &nsipcbuf, perm);
	}
}

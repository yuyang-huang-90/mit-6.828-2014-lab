#include "ns.h"

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	envid_t from_envid;
	uint32_t req;
	int perm = 0;

	while (1) {
		req = ipc_recv(&from_envid, &nsipcbuf, &perm);
		if ((from_envid == 0) || (perm == 0)) {
			cprintf("output: receive invalid ipc %e\n", req);
			continue;
		}

		if (from_envid != ns_envid) {
			cprintf("Received IPC from envid %08x, expected to receive from %08x\n",
				from_envid, ns_envid);
			continue;
		}
		if (req == NSREQ_OUTPUT) {
			while(sys_e1000_transmit(nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len) < 0) {
				sys_yield();
			}
		}
	}
}

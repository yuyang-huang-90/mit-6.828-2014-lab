// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

extern void _pgfault_upcall(void);


//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.

	if (!(err & FEC_WR))
		panic("pgfault: not a write fault at address [%08x]\n", addr);

	void *addr_aligned = (void *) ROUNDDOWN(addr, PGSIZE);
	uint32_t pgnum  = PGNUM(addr_aligned);
	if (!(uvpt[pgnum] & PTE_COW))
		panic("pgfault: not a copy on write page\n");
	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.

	if ((r = sys_page_alloc(0, PFTEMP, PTE_P | PTE_U | PTE_W)) < 0)
			panic("sys_page_alloc: %e\n",r);

	// copy the content to PFTEMP
	memmove(PFTEMP, addr_aligned, PGSIZE);

	// remap
	if ((r = sys_page_map(0, PFTEMP, 0, addr_aligned, PTE_P | PTE_U | PTE_W)) < 0)
		panic("sys_page_map: %e\n", r);

	// unmap
	if ((r = sys_page_unmap(0, PFTEMP)) < 0)
		panic("sys_page_unmap: %e", r);

}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	uint32_t old_perm = uvpt[pn] & PTE_SYSCALL;
	if ((uvpt[pn] & PTE_COW) || (uvpt[pn] & PTE_W) || !(uvpt[pn] & PTE_SHARE)) {
		uint32_t new_perm = (old_perm & (~PTE_W)) | PTE_COW;
				// map page COW in child
		if ((r = sys_page_map(0, (void *) (pn*PGSIZE), envid, (void *) (pn*PGSIZE), new_perm)) < 0)
			panic("sys_page_map: %e\n", r);

		// map page in parent
		if ((r = sys_page_map(0, (void *) (pn*PGSIZE), 0, (void *) (pn*PGSIZE), new_perm)) < 0)
			panic("sys_page_map: %e\n", r);
	} else {
		// map page in child
		if ((r = sys_page_map(0, (void *) (pn*PGSIZE), envid, (void *) (pn*PGSIZE), old_perm)) < 0)
			panic("sys_page_map: %e\n", r);
	}
	return 0;
}

//
// User-level fork with copy-on-writek
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	int r;
	envid_t child_envid;

	set_pgfault_handler(pgfault);

	child_envid = sys_exofork();
	if (child_envid < 0)
		panic("sys_exofork: %e\n", child_envid);
	if (child_envid == 0) {
		//child
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}

	// map pages under UTOP - PGSIZE
	for (int i = 0; i < UTOP - PGSIZE; i += PGSIZE) {
		if ((uvpd[PDX(i)] & PTE_P) && (uvpt[PGNUM(i)] & PTE_P)) {
			duppage(child_envid, PGNUM(i));
		}
	}
	//alloc exception stack
	if ((r = sys_page_alloc(child_envid, (void *) (UXSTACKTOP - PGSIZE), PTE_P | PTE_U | PTE_W)) < 0)
		panic("sys_page_alloc: %e\n", r);

	// Set page fault handler for the child
	if ((r = sys_env_set_pgfault_upcall(child_envid, _pgfault_upcall)) < 0)
		panic("sys_env_set_pgfault_upcall: %e\n", r);

	// Mark child environment as runnable
	if ((r = sys_env_set_status(child_envid, ENV_RUNNABLE)) < 0)
		panic("sys_env_set_status: %e\n", r);

	return child_envid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}

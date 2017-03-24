
## Q.1
The implemented output environment loop forever try to send the pkt first, if the transmit is failed (Usually when the transmit ring is full). The output environment yield the process and retransmit the pkt when this environment take control of CPU again.

## Q.2
When the receive queue is empty and a user environment requests the next
incoming packet. I suspend the calling environment and make such environment
RUNNABLE again when the interrupt happens (the interrupt generated when packet
arrived).


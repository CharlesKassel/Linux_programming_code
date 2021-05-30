/****************************************************************************
 * lsrv2.c  
 * License server server program version 2
 */

#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MSGLEN 128 /* Size of our datagrams */
#define RECLAIM_INTERVAL 20

int main(int ac, char *av[])
{
	struct sockaddr_in client_addr;
	socklen_t addrlen;
	char buf[MSGLEN];
	int ret, sock;
	void ticket_reclaim();
	unsigned time_left;

	sock = setup();
	signal(SIGALRM, ticket_reclaim);
	alarm(RECLAIM_INTERVAL);

	while (1)
	{
		addrlen = sizeof(client_addr);
		ret = recvfrom(sock, buf, MSGLEN, 0, (struct sockaddr *)&client_addr, &addrlen);
		if (ret != -1)
		{
			buf[ret] = '\0';
			narrate("GOT:", buf, &client_addr);
			time_left = alarm(0);
			handle_request(buf, &client_addr, addrlen);
			alarm(time_left);
		}
		else if (errno != EINTR)
			perror("recvfrom");
	}
}
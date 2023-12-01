#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)

int main(int argc, char **argv) 
{
	int sockfd, n;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;

		int BUFSIZE = -1;

	int port = -1;
	char ip[16] = {'\0'};
	while (1) 
	{
		int current_optind = optind ? optind : 1;

		static struct option options[] = {{"port", required_argument, 0, 0},
																			{"bufsize", required_argument, 0, 0},
																			{"ip",required_argument,0,0},
																			{0, 0, 0, 0}};

		int option_index = 0;
		int c = getopt_long(argc, argv, "", options, &option_index);

		if (c == -1)
			break;

		switch (c) 
		{
		case 0: 
		{
			switch (option_index) 
			{
			case 0:
				port = atoi(optarg);
				if (!(port>0))
				return 0;
				break;
			case 1:
				BUFSIZE = atoi(optarg);
				if (!(BUFSIZE>0))
				return 0;
				break;
			case 2:
				strcpy(ip, optarg);
				if (!(strlen(ip)>0))
				return 0;
				break;
			default:
				printf("Index %d is out of options\n", option_index);
			}
		} 
			break;
		case '?':
			printf("Unknown argument\n");
			break;
		}
	}

	if (port == -1 || BUFSIZE == -1 || strlen(ip)==0) 
	{
		fprintf(stderr, "Using: %s --port 20001 --bufsize 4 --ip 127.0.0.1\n", argv[0]);
		return 1;
	}

	char sendline[BUFSIZE], recvline[BUFSIZE + 1];

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0) 
	{
		perror("inet_pton problem");
		exit(1);
	}
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		perror("socket problem");
		exit(1);
	}

	write(1, "Enter string\n", 13);

	while ((n = read(0, sendline, BUFSIZE)) > 0) 
	{
		if (sendto(sockfd, sendline, n, 0, (SADDR *)&servaddr, SLEN) == -1) 
		{
			perror("sendto problem");
			exit(1);
		}

		if (recvfrom(sockfd, recvline, BUFSIZE, 0, NULL, NULL) == -1) 
		{
			perror("recvfrom problem");
			exit(1);
		}

		printf("REPLY FROM SERVER= %s\n", recvline);
	}
	close(sockfd);
}

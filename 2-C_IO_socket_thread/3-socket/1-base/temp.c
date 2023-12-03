void printfIPAndPort(int sockfd)
{
    struct sockaddr_in localaddr;
    socklen_t addrlen = sizeof(localaddr);
    if (getsockname(sockfd, (struct sockaddr *)&localaddr, &addrlen) < 0)
        ERR_EXIT("getsockname");
    printf("pid:%d,ip=%s port=%d\n", getpid(), inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
}

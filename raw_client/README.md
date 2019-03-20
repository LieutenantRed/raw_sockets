Notes
--------------

Mathematics of the Internet Checksum

For the mathematically inclined, the set of 16-bit hexadecimal values V = {0001, . . . , FFFF} and the one’s complement sum operation + together form an Abelian group. The following properties are obeyed:

    For any X,Y in V, (X + Y) is in V [closure]
    For any X,Y,Z in V, X + (Y + Z) = (X + Y) + Z [associativity]
    For any X in V, e + X = X + e = X where e = FFFF [identity]
    For any X in V, there is an X′ in V such that X + X′ = e [inverse]
    For any X,Y in V, (X + Y) = (Y + X) [commutativity]

Note that in the set V and the group <V,+>, number 0000 deleted the from consideration. If we put the number 0000 in the set V, then <V,+> is not a group any longer. 
_____________

TCP header:

![Image alt](https://github.com/LieutenantRed/raw_sockets/blob/master/raw_client/png/tcp_header.png)

_____________

UDP header:

![Image alt](https://github.com/LieutenantRed/raw_sockets/blob/master/raw_client/png/udp_header.png)

_____________

IP header:


![Image alt](https://github.com/LieutenantRed/raw_sockets/blob/master/raw_client/png/ip_header.png)
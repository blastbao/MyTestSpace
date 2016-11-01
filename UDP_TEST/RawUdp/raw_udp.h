/* raw_udp.h
 *
 * Raw UDP packet sender api
 */

#ifndef __RAW_UDP_HDR__
#define __RAW_UDP_HDR__

/*
 * The sending function
 * Arguments:
 *  buf      - the content of a UDP packet
 *  buf_len  - the length of buf
 *  src_addr - the sending address(the sin_addr should be equal to the output interface's IP address)
 *  dst_addr - the destination address
 * Returns:
 *  >=0 - successful, the number of bytes sent
 *  < 0 - failed, please call raw_udp_errorstr() to get the reason for it
 */
int raw_udp_send(char *buf, int buf_len, struct sockaddr_in *src_addr, struct sockaddr_in *dst_addr);

/*
 * Retrieving the error msg when raw_send_udp() failed
 */
const char *raw_udp_errorstr();


#endif

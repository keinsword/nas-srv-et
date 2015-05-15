#ifndef NAS_SRV_ET_LIB_HDR_NETNS_FUNC_H_
#define NAS_SRV_ET_LIB_HDR_NETNS_FUNC_H_

#include "../../../nas-srv-et-lib/include/nas-srv-et-lib.h"
#include "server-func.h"
#include <sched.h>

int netns_change(char *ns_name, int fd_global_netns);

int* open_socket_in_netns(char **ns_names, int num_netns);

#endif /* NAS_SRV_ET_LIB_HDR_NETNS_FUNC_H_ */

/**
 * conntail
 *
 * tracks netfilter events
 * load nf_conntrack_netlink.ko first
**/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libnetfilter_conntrack/libnetfilter_conntrack.h>

#define CONNTAIL_MAX_LINE 1024

static int callback(enum nf_conntrack_msg_type type,
                    struct nf_conntrack *ct, void *data)
{
    char buffer[CONNTAIL_MAX_LINE];

    nfct_snprintf(buffer, sizeof(buffer), ct, type, NFCT_O_XML,
                  NFCT_OF_TIME);
    printf("%s\n", buffer);
    return NFCT_CB_CONTINUE;
}

int main(int argc, char **argv)
{
    struct nfct_handle *handle;

    handle = nfct_open(CONNTRACK, NFCT_ALL_CT_GROUPS);
    if (!handle) {
        perror("nfct_open");
        goto err;
    }

    nfct_callback_register(handle, NFCT_T_NEW | NFCT_T_DESTROY, callback,
                           NULL);

    if (nfct_catch(handle) < 0) {
        perror("nfct_catch");
        goto err;
    }

    nfct_close(handle);
    return EXIT_SUCCESS;
  err:
    return EXIT_FAILURE;
}

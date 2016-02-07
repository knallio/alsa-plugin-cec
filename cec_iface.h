#include <stdio.h>

#include <libcec/cecc.h>


typedef struct cec_iface
{
	libcec_configuration	config;
	libcec_connection_t	connection;
	cec_logical_address	receiver;
	char strPort[50];
	int cecLogLevel;
	ICECCallbacks callbacks;

} cec_iface_t;


static int cb_cec_log_message(void* lib, const cec_log_message message);
static int cb_cec_command(void* lib, const cec_command command);


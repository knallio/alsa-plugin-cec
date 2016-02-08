#include "cec_iface.h"

#include <unistd.h>

static cec_iface_t g_cec_iface;

static receiver_t g_receiver;

static int cb_cec_command(void* lib, const cec_command command)
{

	printf("\ncommand received:\n");

	if (command.destination!=g_cec_iface.cec_adapter)
	{
		printf("not for me: %x\n",command.destination);
		return 0;
	}

	if (command.initiator!=g_cec_iface.receiver)
	{
		printf("not from audio system: %x\n",command.initiator);
		return 0;
	}

	switch (command.opcode)
	{
		case CEC_OPCODE_REPORT_AUDIO_STATUS :
			printf("audio status received: %x\n",command.parameters.data[0]);
			g_receiver.volume=command.parameters.data[0];
			break;
		default:
			printf("opcode: %x\n",command.opcode);
			printf("parameters.size: %d\n",command.parameters.size);
			printf("parameters.data[0]: %x\n",command.parameters.data[0]);
			printf("opcode_set: %d\n",command.opcode_set);
      			break;
	}

}

static int cb_cec_log_message(void* lib, const cec_log_message message)
{
	if ((message.level & g_cec_iface.cecLogLevel) == message.level)
	{
		const char* strLevel;
		switch (message.level)
		{
			case CEC_LOG_ERROR:
				strLevel = "ERROR:   ";
				break;
			case CEC_LOG_WARNING:
				strLevel = "WARNING: ";
				break;
			case CEC_LOG_NOTICE:
				strLevel = "NOTICE:  ";
				break;
			case CEC_LOG_TRAFFIC:
				strLevel = "TRAFFIC: ";
				break;
			case CEC_LOG_DEBUG:
				strLevel = "DEBUG:   ";
				break;
			default:
				break;
		}

		printf("%s[%16lld]\t%s\n", strLevel, message.time, message.message);
	}
	return 1;
}

int cec_send_command(cec_opcode opcode, cec_datapacket parameter)
{
	cec_command command;

//	cec_logical_address initiator;        /**< the logical address of the initiator of this message */
//	cec_logical_address destination;      /**< the logical address of the destination of this message */
//	int8_t              ack;              /**< 1 when the ACK bit is set, 0 otherwise */
//	int8_t              eom;              /**< 1 when the EOM bit is set, 0 otherwise */
//	cec_opcode          opcode;           /**< the opcode of this message */
//	cec_datapacket      parameters;       /**< the parameters attached to this message */
//	int8_t              opcode_set;       /**< 1 when an opcode is set, 0 otherwise (POLL message) */
//	int32_t             transmit_timeout; /**< the timeout to use in ms */

	command.initiator = g_cec_iface.cec_adapter;
	command.destination = g_cec_iface.receiver;

	command.opcode = opcode;
	command.opcode_set=1;
	command.parameters=parameter;

	printf("\nsend_command %x\n",command.opcode);
	int result=libcec_transmit(g_cec_iface.connection, &command);

	printf("\n result: %d \n",result);

}


int cec_request_volume()
{
	cec_opcode opcode=CEC_OPCODE_GIVE_AUDIO_STATUS;
	cec_datapacket parameter = {{},0};
	cec_send_command(opcode,parameter);
}


int cec_volume_up()
{
	printf(" volume up\n");
	cec_opcode opcode=CEC_OPCODE_USER_CONTROL_PRESSED;
	cec_datapacket parameter = {{CEC_USER_CONTROL_CODE_VOLUME_UP},1};
	cec_send_command(opcode,parameter);
}

int cec_volume_down()
{
	printf(" volume down\n");
	cec_opcode opcode=CEC_OPCODE_USER_CONTROL_PRESSED;
	cec_datapacket parameter = {{CEC_USER_CONTROL_CODE_VOLUME_DOWN},1};
	cec_send_command(opcode,parameter);
}

int cec_mute()
{
	return 0;
}


int cec_iface_init()
{
	g_cec_iface.strPort[0] = 0;
	g_cec_iface.cecLogLevel = CEC_LOG_TRAFFIC;
	//g_cec_iface.cecLogLevel = CEC_LOG_ALL;

	g_cec_iface.callbacks.CBCecLogMessage = cb_cec_log_message;
	g_cec_iface.callbacks.CBCecCommand = cb_cec_command;

	//g_cec_iface.config.callbacks=&g_cec_iface.callbacks;
	g_cec_iface.receiver = 5;

	//set configuration:

	libcec_clear_configuration(&(g_cec_iface.config));

	g_cec_iface.config.callbacks=&g_cec_iface.callbacks;

	g_cec_iface.config.clientVersion=LIBCEC_VERSION_CURRENT;
	g_cec_iface.config.bActivateSource = 0;
	g_cec_iface.config.deviceTypes.types[0] = CEC_DEVICE_TYPE_PLAYBACK_DEVICE;
	strncpy(g_cec_iface.config.strDeviceName, "alsa-cec", 8);

	//initialize:
	g_cec_iface.connection=libcec_initialise(&(g_cec_iface.config));

	libcec_init_video_standalone(g_cec_iface.connection);

	//find adapters
	cec_adapter devices[10];
	int8_t iDevicesFound;
	iDevicesFound = libcec_find_adapters(g_cec_iface.connection, devices, sizeof(devices) / sizeof(devices), NULL);

	if (iDevicesFound <= 0)
	{
	        printf("autodetect ");
		printf("FAILED\n");
		libcec_destroy(g_cec_iface.connection);
		return 1;
	} else
	{
		printf("\n path:     %s\n com port: %s\n\n", devices[0].path, devices[0].comm);
		strcpy(g_cec_iface.strPort, devices[0].comm);
	}

	printf("OPEN:\n");


	if (!libcec_open(g_cec_iface.connection, g_cec_iface.strPort, 5000))
	{
		printf("unable to open the device on port %s\n", g_cec_iface.strPort);
		libcec_destroy(g_cec_iface.connection);
		return 1;
	}

	printf("INIT_SUCCESS\n");

	cec_logical_addresses my_logical_addresses = libcec_get_logical_addresses(g_cec_iface.connection);
	for (int i=0;i<16;i++)
	{
		if (my_logical_addresses.addresses[i]!=0)
		{
			g_cec_iface.cec_adapter=i;
			break;
		}
	}

	printf("My logical address: %d",g_cec_iface.cec_adapter);

//	printf("wake0:%x\n",g_cec_iface.config.wakeDevices.addresses[0]);
//	printf("%x\n",g_cec_iface.config.wakeDevices.addresses[1]);
//	printf("%x\n",g_cec_iface.config.wakeDevices.addresses[2]);
//	printf("%x\n",g_cec_iface.config.wakeDevices.addresses[3]);
//	printf("%x\n",g_cec_iface.config.wakeDevices.addresses[4]);
//	printf("%x\n",g_cec_iface.config.wakeDevices.addresses[5]);


}



/*public functions which can be called from ctl_cec*/
int receiver_get_volume()
{
	return g_receiver.volume;
}

int receiver_set_volume(int volume)
{
	cec_request_volume();
	sleep(1);	//90000 seems to be enough... no idea how robust...
	int volume_diff=g_receiver.volume-volume;
	printf("requested volume: %d, current volume: %d, volume difference: %d\n",volume,g_receiver.volume,volume_diff);

	if (volume_diff>0)
	{
		for (int i=0;i<volume_diff;i++)
		{
			cec_volume_down();
			sleep(1);
		}
	} else if (volume_diff<0)
	{
		for (int i=0;i<-volume_diff;i++)
		{
			sleep(1);
			cec_volume_up();
		}
	}


	cec_request_volume();
	sleep(1);	//90000 seems to be enough... no idea how robust...
	volume_diff=g_receiver.volume-volume;
	printf("requested volume: %d, current volume: %d, volume difference: %d\n",volume,g_receiver.volume,volume_diff);


	return 0;
	while(volume_diff!=0)
	{
		cec_request_volume();
		printf("requested volume: %d, current volume: %d, volume difference: %d\n",volume,g_receiver.volume,volume_diff);
		if (volume_diff>0)
		{
			cec_volume_down();
		} else if (volume_diff<0)
		{
			cec_volume_up();
		}
		//usleep(200000);
		volume_diff=g_receiver.volume-volume;
	}


}

int receiver_get_mute()
{
	return g_receiver.mute;

}

int receiver_set_mute()
{

}


int main(int argc, char *argv[])
{

	cec_iface_init();


//	cec_volume_down();
//	cec_request_volume();


	receiver_set_volume(50);

//	printf("start waiting...");

/*
	while (1)
	{
		printf("waiting...\n");
		cec_request_volume();
		printf("receiver_volume: %d\n", g_receiver.volume);
		sleep(1);
	}

*/
	libcec_destroy(g_cec_iface.connection);
}

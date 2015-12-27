/*
 * ALSA -> CEC control plugin
 *
 * Copyright (c) 2015 by Winfried Lorenzen <knallio@gmx.de>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#include <sys/socket.h>
#include <alsa/asoundlib.h>
#include <alsa/control_external.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
/*

#define MIN(a, b)     ((a) < (b) ? (a) : (b))
#define MID(a, b, c)  ((a) < (b) ? ((b) < (c) ? (b) : ((a) < (c) ? (c) : (a))) \
                                 : ((b) > (c) ? (b) : ((a) > (c) ? (c) : (a))))
*/

static const char* cec_name =		"CEC";

typedef struct snd_ctl_cec {
	snd_ctl_ext_t		ext;
	int			port_fd;
//	int			shm_id;
//	const char*		port;
//	pthread_t		server;
} snd_ctl_cec_t;

static void cec_close(snd_ctl_ext_t *ext)
{
	snd_ctl_cec_t *cec = ext->private_data;

	//close file
	printf("closing file\n");
	if (cec->port_fd >= 0)
		close(cec->port_fd);

	free(cec);
}
static int cec_elem_count(snd_ctl_ext_t *ext)
{
	//snd_ctl_cec_t *cec = ext->private_data;

	//return ARRAY_SIZE(arcam_av_zone1);

	//Nur Volume geplant:
	return 1;
}

static int cec_elem_list(snd_ctl_ext_t *ext, unsigned int offset, snd_ctl_elem_id_t *id)
{
//	snd_ctl_cec_t *cec = ext->private_data;

	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);

	if (offset < 1)
		snd_ctl_elem_id_set_name(id, "VOLUME");
	return 0;
}

static snd_ctl_ext_key_t cec_find_elem(snd_ctl_ext_t *ext,
					    const snd_ctl_elem_id_t *id)
{
//	snd_ctl_cec_t *cec = ext->private_data;
	unsigned int numid;

	numid = snd_ctl_elem_id_get_numid(id);
	if (numid > 0) {
		return 0;
	}


	return SND_CTL_EXT_KEY_NOT_FOUND;
}

static int cec_get_attribute(snd_ctl_ext_t *ext ATTRIBUTE_UNUSED,
				  snd_ctl_ext_key_t key, int *type,
				  unsigned int *acc, unsigned int *count)
{
	*type = SND_CTL_ELEM_TYPE_INTEGER;
	*acc = SND_CTL_EXT_ACCESS_READWRITE;
	*count = 1;
	return 0;
}

static int cec_get_integer_info(snd_ctl_ext_t *ext,
				     snd_ctl_ext_key_t key,
				     long *imin, long *imax, long *istep)
{
	*istep = 1;
	*imin = 0;
	*imax = 100;
	return 0;
}

static int cec_read_integer(snd_ctl_ext_t *ext, snd_ctl_ext_key_t key, long *value)
{
	snd_ctl_cec_t *cec = ext->private_data;
	char buf[3];
	int volume=0;
//	printf("reading integer\n");
	read(cec->port_fd,buf,2);
	volume=atoi(buf);

//	printf("read(buf): %s\n",buf);
//	printf("read(int): %d\n",volume);

	*value = volume;
	return 0;
}

static int cec_write_integer(snd_ctl_ext_t *ext, snd_ctl_ext_key_t key, long *value)
{
	snd_ctl_cec_t *cec = ext->private_data;
	char buf[3];
	printf("%d\n",*value);
	sprintf(buf,"%d\n",*value);
	printf("writing integer %d\n",buf);
	//TODO: check value against current volume. not clear current volume from where? locally stored variable? in future polled from cec?
	//TODO: rewind file?
	if (write(cec->port_fd,buf,3)<0)
	{
		printf("error printing");
		return -1;
	} else
	{
		printf("no error printing, rewinding");
		lseek(cec->port_fd,0,SEEK_SET);
		return 1;
	}
}

/*
static int arcam_av_read_event(snd_ctl_ext_t *ext, snd_ctl_elem_id_t *id, unsigned int *event_mask)
{
	snd_ctl_arcam_av_t *arcam_av = ext->private_data;
	unsigned int elem;
	int result = 0;

	switch(arcam_av->zone) {
	case ARCAM_AV_ZONE1:
		for (elem = 0; elem < ARRAY_SIZE(arcam_av_zone1); ++elem) {
			if (arcam_av->local.zone1.state[elem] != arcam_av->global->zone1.state[elem]) {
				snd_ctl_elem_id_set_name(id, arcam_av_zone1[elem].name);
				snd_ctl_elem_id_set_numid(id, elem + 1);
				arcam_av->local.zone1.state[elem] = arcam_av->global->zone1.state[elem];
				result = 1;
				break;
			}
		}		
		break;

	case ARCAM_AV_ZONE2:
		for (elem = 0; elem < ARRAY_SIZE(arcam_av_zone2); ++elem) {
			if (arcam_av->local.zone2.state[elem] != arcam_av->global->zone2.state[elem]) {
				snd_ctl_elem_id_set_name(id, arcam_av_zone2[elem].name);
				snd_ctl_elem_id_set_numid(id, elem + 1);
				arcam_av->local.zone2.state[elem] = arcam_av->global->zone2.state[elem];
				result = 1;
				break;
			}
		}
		break;
	}

	if (!result) {
		char buf[10];
		if (recv(arcam_av->ext.poll_fd, buf, sizeof(buf), 0) <= 0) {
			close(arcam_av->ext.poll_fd);
			arcam_av->ext.poll_fd = arcam_av_client(arcam_av->port);
			if (arcam_av->ext.poll_fd > 0)
				fcntl(arcam_av->ext.poll_fd, F_SETFL, O_NONBLOCK);
		}

		result = -EAGAIN;
	} else {
		snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
		*event_mask = SND_CTL_EVENT_MASK_VALUE;
	}

	return result;
}

*/

static snd_ctl_ext_callback_t cec_ext_callback = {
	.close = cec_close,
	.elem_count = cec_elem_count,
	.elem_list = cec_elem_list,
	.find_elem = cec_find_elem,
	.get_attribute = cec_get_attribute,
	.get_integer_info = cec_get_integer_info,
//	.get_enumerated_info = arcam_av_get_enumerated_info,
//	.get_enumerated_name = arcam_av_get_enumerated_name,
	.read_integer = cec_read_integer,
//	.read_enumerated = arcam_av_read_enumerated,
	.write_integer = cec_write_integer,
//	.write_enumerated = arcam_av_write_enumerated,
//	.read_event = arcam_av_read_event,
};

SND_CTL_PLUGIN_DEFINE_FUNC(cec)
{
	snd_config_iterator_t it, next;
	int err;
	snd_ctl_cec_t *cec = NULL;
	const char *filename = "/home/knallio/alsa_cec.test";

	snd_config_for_each(it, next, conf) {
		snd_config_t *n = snd_config_iterator_entry(it);
		const char *id;
		if (snd_config_get_id(n, &id) < 0)
			continue;
		if (strcmp(id, "comment") == 0 || strcmp(id, "type") == 0 || strcmp(id, "hint") == 0)
			continue;
		if (strcmp(id, "file") == 0) {
			if (snd_config_get_string(n, &filename) < 0) {
				SNDERR("Invalid type for %s", id);
				return -EINVAL;
			}
			continue;
		}
		SNDERR("Unknown field %s", id);
		return -EINVAL;
	}

	if (access(filename, R_OK | W_OK) < 0) {
		err = -errno;
		goto error;
	}
	//test, ob printfs beim aufrufen von amixer in die konsole kommen.
	//printf("test\n");

	cec = calloc(1, sizeof(*cec) + strlen(filename) + 1);

	if (!cec)
		return -ENOMEM;

	cec->ext.version = SND_CTL_EXT_VERSION;
	cec->ext.card_idx = 0;
	strncpy(cec->ext.id, cec_name, sizeof(cec->ext.id) - 1);
	strncpy(cec->ext.name, cec_name, sizeof(cec->ext.name) - 1);
	strncpy(cec->ext.longname, cec_name, sizeof(cec->ext.longname) - 1);
	strncpy(cec->ext.mixername, cec_name, sizeof(cec->ext.mixername) - 1);
	cec->ext.poll_fd = -1;
	cec->ext.callback = &cec_ext_callback;
	cec->ext.private_data = cec;

	printf("opening file\n");
	cec->port_fd=open(filename,O_RDWR);
	if (cec->port_fd<0)
	{
		err = -errno;
		goto error;
	}
/*
	arcam_av->shm_id = -1;
	arcam_av->port_fd = -1;
	arcam_av->port = strcpy((char*)(arcam_av + 1), port);
	arcam_av->zone = zone != 2 ? ARCAM_AV_ZONE1 : ARCAM_AV_ZONE2;

	arcam_av->port_fd = arcam_av_connect(arcam_av->port);
	if (arcam_av->port_fd < 0) {
		err = -errno;
		goto error;
	}

	if (arcam_av_server_start(&arcam_av->server, arcam_av->port)) {
		err = -errno;
		goto error;
	}

	arcam_av->ext.poll_fd = arcam_av_client(arcam_av->port);
	if (arcam_av->ext.poll_fd < 0) {
		err = -errno;
		goto error;
	}

	fcntl(arcam_av->ext.poll_fd, F_SETFL, O_NONBLOCK);

	arcam_av->global = arcam_av_state_attach(arcam_av->port);
	if (!arcam_av->global) {
		err = -errno;
		goto error;
	}
*/

	err = snd_ctl_ext_create(&cec->ext, name, mode);
	if (err < 0)
		goto error;

	*handlep = cec->ext.handle;
	return 0;

 error:
	perror("cec()");
	//arcam_av_close(&arcam_av->ext);
	return err;
}

SND_CTL_PLUGIN_SYMBOL(cec);

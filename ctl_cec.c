/*
 * ALSA -> CEC control plugin
 *
 * Copyright (c) 2009 by Peter Stokes <linux@dadeos.co.uk>
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

/*
static const struct {
	arcam_av_cc_t			code;
	const char*			name;
} arcam_av_zone1[] = {
	{ARCAM_AV_POWER,			"Power Switch"				},
	{ARCAM_AV_VOLUME_SET,			"Master Playback Volume"		},
	{ARCAM_AV_MUTE,				"Master Playback Switch"		},
	{ARCAM_AV_DIRECT,			"Direct Playback Switch"		},
	{ARCAM_AV_SOURCE,			"Source Playback Route"			},
	{ARCAM_AV_SOURCE_TYPE,			"Source Type Playback Route"		},
	{ARCAM_AV_STEREO_DECODE,		"Stereo Decode Playback Route"		},
	{ARCAM_AV_MULTI_DECODE,			"Multi-Channel Decode Playback Route"	},
	{ARCAM_AV_STEREO_EFFECT,		"Stereo Effect Playback Route"		}
};

static const struct {
	arcam_av_cc_t			code;
	const char*			name;
} arcam_av_zone2[] = {
	{ARCAM_AV_POWER,			"Power Switch"				},
	{ARCAM_AV_VOLUME_SET,			"Master Playback Volume"		},
	{ARCAM_AV_MUTE,				"Master Playback Switch"		},
	{ARCAM_AV_SOURCE,			"Source Playback Route"			}
};

static const struct {
	arcam_av_source_t		code;
	const char*			name;
} arcam_av_sources[] = {
	{ARCAM_AV_SOURCE_DVD,			"DVD"			},
	{ARCAM_AV_SOURCE_SAT,			"SAT"			},
	{ARCAM_AV_SOURCE_AV,			"AV"			},
	{ARCAM_AV_SOURCE_PVR,			"PVR"			},
	{ARCAM_AV_SOURCE_VCR,			"VCR"			},
	{ARCAM_AV_SOURCE_CD,			"CD"			},
	{ARCAM_AV_SOURCE_FM,			"FM"			},
	{ARCAM_AV_SOURCE_AM,			"AM"			},
	{ARCAM_AV_SOURCE_DVDA,			"DVDA"			}
};

static const struct {
	arcam_av_source_type_t		code;
	const char*			name;
} arcam_av_source_types[] = {
	{ARCAM_AV_SOURCE_TYPE_ANALOGUE,		"Analogue"		},
	{ARCAM_AV_SOURCE_TYPE_DIGITAL,		"Digital"		}
};

static const struct {
	arcam_av_direct_t		code;
	const char*			name;
} arcam_av_direct[] = {
	{ARCAM_AV_DIRECT_DISABLE,		"Disable"		},
	{ARCAM_AV_DIRECT_ENABLE,		"Enable"		}
};

static const struct {
	arcam_av_stereo_decode_t	code;
	const char*			name;
} arcam_av_stereo_decode_modes[] = {
	{ARCAM_AV_STEREO_DECODE_MONO,		"Mono"			},
	{ARCAM_AV_STEREO_DECODE_STEREO,		"Stereo"		},
	{ARCAM_AV_STEREO_DECODE_PLII_MOVIE,	"Pro Logic II Movie"	},
	{ARCAM_AV_STEREO_DECODE_PLII_MUSIC,	"Pro Logic II Music"	},
	{ARCAM_AV_STEREO_DECODE_PLIIx_MOVIE,	"Pro Logic IIx Movie"	},
	{ARCAM_AV_STEREO_DECODE_PLIIx_MUSIC,	"Pro Logic IIx Music"	},
	{ARCAM_AV_STEREO_DECODE_DOLBY_PL,	"Dolby Pro Logic"	},
	{ARCAM_AV_STEREO_DECODE_NEO6_CINEMA,	"Neo:6 Cinema"		},
	{ARCAM_AV_STEREO_DECODE_NEO6_MUSIC,	"Neo:6 Music"		}
};

static const struct {
	arcam_av_multi_decode_t		code;
	const char*			name;
} arcam_av_multi_decode_modes[] = {
	{ARCAM_AV_MULTI_DECODE_MONO,		"Mono down-mix"		},
	{ARCAM_AV_MULTI_DECODE_STEREO,		"Stereo down-mix"	},
	{ARCAM_AV_MULTI_DECODE_MULTI_CHANNEL,	"Multi-channel"		},
	{ARCAM_AV_MULTI_DECODE_PLIIx,		"Pro Logic IIx"		}
};

static const struct {
	arcam_av_stereo_effect_t	code;
	const char*			name;
} arcam_av_stereo_effects[] = {
	{ARCAM_AV_STEREO_EFFECT_NONE,		"None"			},
	{ARCAM_AV_STEREO_EFFECT_MUSIC,		"Music"			},
	{ARCAM_AV_STEREO_EFFECT_PARTY,		"Party"			},
	{ARCAM_AV_STEREO_EFFECT_CLUB,		"Club"			},
	{ARCAM_AV_STEREO_EFFECT_HALL,		"Hall"			},
	{ARCAM_AV_STEREO_EFFECT_SPORTS,		"Sports"		},
	{ARCAM_AV_STEREO_EFFECT_CHURCH,		"Church"		}
};

*/

typedef struct snd_ctl_cec {
	snd_ctl_ext_t		ext;
//	int			port_fd;
//	int			shm_id;
//	const char*		port;
//	pthread_t		server;
} snd_ctl_cec_t;

static void cec_close(snd_ctl_ext_t *ext)
{
	snd_ctl_cec_t *cec = ext->private_data;

	//close file
	//if (cec->port_fd >= 0)
	//	close(cec->port_fd);

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

/*

static int arcam_av_read_integer(snd_ctl_ext_t *ext, snd_ctl_ext_key_t key, long *value)
{
	snd_ctl_arcam_av_t *arcam_av = ext->private_data;

	switch(key) {
	case ARCAM_AV_POWER:
		switch(arcam_av->zone) {
		case ARCAM_AV_ZONE1:
			arcam_av->local.zone1.power = arcam_av->global->zone1.power;
			*value = !(arcam_av->local.zone1.power == ARCAM_AV_POWER_STAND_BY);
			break;

		case ARCAM_AV_ZONE2:
			arcam_av->local.zone2.power = arcam_av->global->zone2.power;
			*value = !(arcam_av->local.zone2.power == ARCAM_AV_POWER_STAND_BY);
			break;
		}
		break;

	case ARCAM_AV_VOLUME_SET:
		switch(arcam_av->zone) {
		case ARCAM_AV_ZONE1:
			arcam_av->local.zone1.volume = arcam_av->global->zone1.volume;
			*value = MID(0, arcam_av->local.zone1.volume - ARCAM_AV_VOLUME_MIN, 100);
			break;

		case ARCAM_AV_ZONE2:
			arcam_av->local.zone2.volume = arcam_av->global->zone2.volume;
			*value = MID(20, arcam_av->local.zone2.volume - ARCAM_AV_VOLUME_MIN, 83);
			break;
		}
		break;

	case ARCAM_AV_MUTE:
		switch(arcam_av->zone) {
		case ARCAM_AV_ZONE1:
			arcam_av->local.zone1.mute = arcam_av->global->zone1.mute;
			*value = !(arcam_av->local.zone1.mute == ARCAM_AV_MUTE_ON);
			break;

		case ARCAM_AV_ZONE2:
			arcam_av->local.zone2.mute = arcam_av->global->zone2.mute;
			*value = !(arcam_av->local.zone2.mute == ARCAM_AV_MUTE_ON);
			break;
		}
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int arcam_av_write_integer(snd_ctl_ext_t *ext, snd_ctl_ext_key_t key, long *value)
{
	snd_ctl_arcam_av_t *arcam_av = ext->private_data;
	unsigned char volume = ARCAM_AV_VOLUME_MIN;

	switch(key) {
	case ARCAM_AV_POWER:
		switch(arcam_av->zone) {
		case ARCAM_AV_ZONE1:
			arcam_av->local.zone1.power = ARCAM_AV_POWER_STAND_BY + *value;
			if (arcam_av->global->zone1.power == arcam_av->local.zone1.power)
				return 0;
			break;

		case ARCAM_AV_ZONE2:
			arcam_av->local.zone2.power = ARCAM_AV_POWER_STAND_BY + *value;
			if (arcam_av->global->zone2.power == arcam_av->local.zone2.power)
				return 0;
			break;
		}
		break;

	case ARCAM_AV_VOLUME_SET:
		switch(arcam_av->zone) {
		case ARCAM_AV_ZONE1:
			arcam_av->local.zone1.volume = ARCAM_AV_VOLUME_MIN + *value;
			if (arcam_av->global->zone1.volume == arcam_av->local.zone1.volume)
				return 0;

			if (arcam_av->global->zone1.mute == ARCAM_AV_MUTE_ON) {
				arcam_av->global->zone1.volume = arcam_av->local.zone1.volume;
				return 1;
			}
			break;

		case ARCAM_AV_ZONE2:
			arcam_av->local.zone2.volume = ARCAM_AV_VOLUME_MIN + *value;
			if (arcam_av->global->zone2.volume == arcam_av->local.zone2.volume)
				return 0;

			if (arcam_av->global->zone2.mute == ARCAM_AV_MUTE_ON) {
				arcam_av->global->zone2.volume = arcam_av->local.zone2.volume;
				return 1;
			}
			break;
		}
		break;

	case ARCAM_AV_MUTE:
		switch(arcam_av->zone) {
		case ARCAM_AV_ZONE1:
			arcam_av->local.zone1.mute = ARCAM_AV_MUTE_ON + *value;
			if (arcam_av->global->zone1.mute == arcam_av->local.zone1.mute)
				return 0;

			volume = arcam_av->global->zone1.volume;
			break;

		case ARCAM_AV_ZONE2:
			arcam_av->local.zone2.mute = ARCAM_AV_MUTE_ON + *value;
			if (arcam_av->global->zone2.mute == arcam_av->local.zone2.mute)
				return 0;

			volume = arcam_av->global->zone2.volume;
			break;
		}

		if (*value)
			arcam_av_send(arcam_av->port_fd, ARCAM_AV_VOLUME_SET, arcam_av->zone, volume);
		break;

	default:
		return -EINVAL;
	}

	if (!arcam_av_send(arcam_av->port_fd, key, arcam_av->zone, '0' + *value))
		return 1;
	else
		return -1;
}

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
//	.close = arcam_av_close,
	.elem_count = cec_elem_count,
	.elem_list = cec_elem_list,
	.find_elem = cec_find_elem,
	.get_attribute = cec_get_attribute,
	.get_integer_info = cec_get_integer_info,
//	.get_enumerated_info = arcam_av_get_enumerated_info,
//	.get_enumerated_name = arcam_av_get_enumerated_name,
//	.read_integer = arcam_av_read_integer,
//	.read_enumerated = arcam_av_read_enumerated,
//	.write_integer = arcam_av_write_integer,
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

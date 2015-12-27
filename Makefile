all:libasound_module_ctl_cec.so

libcec: ctl_cec.c
	gcc -Wall -g -shared -o libasound_module_ctl_cec.so -fPIC ctl_cec.c

ctl_cec.o: ctl_cec.c
	gcc -DHAVE_CONFIG_H -I. -I.. -Wall -g -I/usr/include/alsa -O2 -Wall -W -pipe -g -MD -MP -c $< -fPIC -DPIC -o $@

libasound_module_ctl_cec.so: ctl_cec.o
	gcc -shared -fPIC -DPIC $< -lasound  -g -O2 -g -Wl,-soname -Wl,$@ -o $@	

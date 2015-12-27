all:libasound_module_ctl_cec.so

ctl_cec.o: ctl_cec.c
	gcc -DHAVE_CONFIG_H -I. -I.. -Wall -g -I/usr/include/alsa -O2 -Wall -W -pipe -g -c $< -fPIC -DPIC -o $@

libasound_module_ctl_cec.so: ctl_cec.o
	gcc -shared -fPIC -DPIC $< -lasound  -g -O2 -g -Wl,-soname -Wl,$@ -o $@	

install: libasound_module_ctl_cec.so
	cp $< /usr/lib/alsa-lib/$<

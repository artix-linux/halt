PREFIX ?= /usr

all:
	$(CC) $(CFLAGS) halt.c -o halt $(LDFLAGS)

install:
	install -d $(DESTDIR)$(PREFIX)/bin
	install -d $(DESTDIR)$(PREFIX)/share/libalpm/scripts
	install -m755 halt $(DESTDIR)$(PREFIX)/bin/halt
	ln -sf halt $(DESTDIR)$(PREFIX)/bin/shutdown
	ln -sf halt $(DESTDIR)$(PREFIX)/bin/poweroff
	ln -sf halt $(DESTDIR)$(PREFIX)/bin/reboot
	install -m755 initswitch $(DESTDIR)$(PREFIX)/bin/initswitch
	install -m755 initswitch-hook $(DESTDIR)$(PREFIX)/share/libalpm/scripts/initswitch
	install -d $(DESTDIR)$(PREFIX)/share/man/man8
	install -m644 halt.8 $(DESTDIR)$(PREFIX)/share/man/man8/halt.8
	ln -sf halt.8 $(DESTDIR)$(PREFIX)/share/man/man8/shutdown.8
	ln -sf halt.8 $(DESTDIR)$(PREFIX)/share/man/man8/poweroff.8
	ln -sf halt.8 $(DESTDIR)$(PREFIX)/share/man/man8/reboot.8

clean:
	-rm -f halt

.PHONY: all install clean

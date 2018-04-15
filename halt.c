/*
 * halt.c: A halt/poweroff/reboot wrapper for Artix Linux
 *         Based on halt.c from void-runit
 *
 * Copyright (C) 2018 Muhammad Herdiansyah
 *           (C) 2018 Artix Linux Developers
 *
 * To see the license terms of this program, see COPYING
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <sys/reboot.h>

extern char *__progname;

typedef enum {NOOP, HALT, REBOOT, POWEROFF} action_type;
typedef enum {OPENRC, RUNIT} initsys;

const char* get_init()
{
    char *name = (char *)calloc(1024,sizeof(char));
    if(name) {
        sprintf(name, "/proc/1/cmdline");
        FILE *f = fopen(name, "r");
        if (f) {
            size_t size;
            size = fread(name, sizeof(char), 1024, f);
            if (size > 0) {
                if ('\n' == name[size-1])
                    name[size-1]='\0';
            }
            fclose(f);
        }
    }
    return name;
}

int main(int argc, char *argv[])
{
    if (getuid() != 0)
        errx(1, "You must be root to do that!");

    int do_sync = 1;
    int do_force = 0;
    int opt;
    action_type action = NOOP;
    initsys init;
    const char *initfile = get_init();
    char openrc_options[50];

    if (strcmp(initfile, "runit") == 0)
        init = RUNIT;
    else init = OPENRC;

    if (strcmp(__progname, "halt") == 0)
        action = HALT;
    else if (strcmp(__progname, "reboot") == 0)
        action = REBOOT;
    else if (strcmp(__progname, "poweroff") == 0 || strcmp(__progname, "shutdown") == 0)
        action = POWEROFF;
    else
        warnx("No default behavior, needs to be called as halt/reboot/poweroff/shutdown.");

    while ((opt = getopt(argc, argv, "dfhinw")) != -1)
        switch (opt) {
        case 'n':
            do_sync = 0;
            strcat(openrc_options, "--no-write");
            break;
        case 'w':
            if (init == RUNIT)
                action = NOOP;
            do_sync = 0;
            strcat(openrc_options, "--write-only");
            break;
        case 'd':
            strcat(openrc_options, "--no-write");
            break;
        case 'h':
        case 'i':
            break;
        case 'f':
            do_force = 1;
            break;
        default:
            errx(1, "Usage: %s [-n] [-f]", __progname);
        }

    if (do_sync)
        sync();

    switch (action) {
        case HALT:
            if (do_force)
                reboot(RB_HALT_SYSTEM);
            else
                switch (init) {
                    case RUNIT:
                        execl("/usr/bin/runit-init", "init", "0", (char*)0);
                        break;
                    case OPENRC:
                        execl("/usr/bin/openrc-shutdown", "openrc-shutdown", "--halt", openrc_options, (char*)0);
                        break;
                }
            err(1, "halt failed");
            break;
        case POWEROFF:
            if (do_force)
                reboot(RB_POWER_OFF);
            else
                switch (init) {
                    case RUNIT:
                        execl("/usr/bin/runit-init", "init", "0", (char*)0);
                        break;
                    case OPENRC:
                        execl("/usr/bin/openrc-shutdown", "openrc-shutdown", "--poweroff", openrc_options, (char*)0);
                        break;
                }
            err(1, "poweroff failed");
            break;
        case REBOOT:
            if (do_force)
                reboot(RB_AUTOBOOT);
            else
                switch(init) {
                    case RUNIT:
                        execl("/usr/bin/runit-init", "init", "6", (char*)0);
                        break;
                    case OPENRC:
                        execl("/usr/bin/openrc-shutdown", "openrc-shutdown", "--reboot", openrc_options, (char*)0);
                        break;
                }
            err(1, "reboot failed");
            break;
        case NOOP:
            break;
    }

    return 0;
}

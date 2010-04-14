#ifndef h_pnd_io_ioctl_h
#define h_pnd_io_ioctl_h

// this is a simple 'is key pressed?' type routine, using standard ioctl; if you don't want to
// monitor the events and just ewant to see whats up, this can be handy.
// returns -1 on error, 0 for 'not down', and >0 for 'is down'
int pnd_is_key_down ( int fd, int key );

#endif

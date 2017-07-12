#ifndef DEVICE_H
#define DEVICE_H

struct pdevinit
{
    void (*pdev_attach)(int);   // attach function
    int pdev_count; // number of devices;
};

void slattach(int);
extern void loopattach(int);

struct pdevinit pdevinit[] = {
    {slattach, 1},
    {loopattach, 1},
    {0, 0}
};

void dev_init(void *framep);
#endif  // DEVICE_H


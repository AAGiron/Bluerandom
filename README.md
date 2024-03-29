# Bluerandom

Repository for the source code of Bluerandom: Bluetooth RSSI-Based Scalable Entropy Source.

If you use it with academic purposes please check the [paper](https://000626cf-7296-4b40-ae6b-d1a550c81174.usrfiles.com/ugd/000626_a407008605d5455ba1ad035cfbd5f19a.pdf), presented at [SBSeg 2020](https://www.ceseg.org/anais-trilha-principal). 
You can also see the experiments and more explanations there.

![alt text](https://github.com/AAGiron/Bluerandom/blob/master/bluerandom.png)


IoT platforms does not always have plenty of entropy sources available, and could be risky to rely only on one entropy source. Here is where stands the benefit of Bluerandom: to be used as an additional entropy source to the system.

# How can I use it?

First things first: **Attention**! Bluerandom has low throughput (feel free to contribute!) and the test results suggests its use as an additional source of entropy (not as a single RNG).

Your computer or IoT board needs libbluetooth-dev and to the BLE compatible. Bluerandom outputs go to stdout (like `cat /dev/random`). You can add to the system (through IOCTL - see Tools - or rngd) or to your application. Basically Bluerandom generate numbers based on RSSI readings from Bluetooth Low Energy (BLE) devices nearby.

The src/ directory contains Bluerandom source code:
- Bluerandom.c: initial code
- BLEScan.c: code which scans for BLE devices and launch an extraction method;
- extraction.c: contains extraction methods (see extraction.h to choose one from three possibilities);
- scan.c: not used, but it scans in continuously inquiry mode to gather RSSI from BR/EDR devices. Since BLE advertise in shorter intervals, BR/EDR is not used here by default. 

The Tools/ directory contains some tools created to test Bluerandom and used in the paper.
- AddEntropyIOCTL: reads from stdin and use the linux system call (RNDADDENTROPY) to add entropy to the system.
- AES_PRNG, SHA_based_PRNG: both are PRNG implementations which were tested with Bluerandom as the seed source (Results can be found in the paper).
-- AES is from mcrypt, and SHA is from openssl. Compile flags for gcc: ```-lcrypto -lmcrypt -lltdl```
-- Both read from stdin, so to run these tools (example): ```cat /dev/urandom | ./a.out ```
- LERandLauncher: the Bluetooth internal PRNG can be accessible by the LE Rand command, so this tool send this commmand to the BLE controller and generates random numbers. In the paper this tool was used to compare it with Bluerandom seeding a PRNG.
-- Must compile it with ```-lbluetooth```


Compile the src/ code with `make` and run it with `sudo ./bluerandom`. The generation process has low throughput and depends on how much BLE devices are nearby (see the paper for details).

If "Error on opening HCI device. No such device" appears you might need to start it (```sudo bluetoothctl; power on; exit``` - or with blueman program).


Suggestions are welcome.


## Bluerandom App

There's an app being developed, integrating bluerandom with Flutter: https://github.com/WorstOne0/Bluerandom-Flutter

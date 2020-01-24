# Bluerandom

Repository for the source code of Bluerandom: Bluetooth RSSI-Based Scalable Entropy Source.

If you use it with academic purposes please cite the paper ... You can also see the experiments and explanations there.
![alt text](https://github.com/AAGiron/Bluerandom/blob/master/bluerandom.png)


IoT platforms does not always have plenty of entropy sources available, and could be risky to rely only on one entropy source. Here is where stands the benefit of Bluerandom: to be used as an additional entropy source to the system.

# How can I use it?

Bluerandom outputs go to stdout. You can add to the system (through IOCTL - see Tools - or rngd) or to your application.

Basically Bluerandom generate numbers based on RSSI readings from Bluetooth Low Energy (BLE) devices nearby. Attention! Bluerandom has low throughput (feel free to contribute!) and the test results suggests its use as an additional source of entropy (not as a single RNG).

The src/ directory contains Bluerandom source code:
- Bluerandom.c: initial code
- BLEScan.c: code which scans for BLE devices and launch an extraction method;
- extraction.c: contains extraction methods (see extraction.h to choose one from three possibilities);
- scan.c: not used, but it scans in continuously inquiry mode to gather RSSI from BR/EDR devices. Since BLE advertise in shorter intervals, BR/EDR is not used here by default. 

The linux_module_test/ directory contains an attempt to create a kernel module for Bluerandom. 

The Tools/ directory contains some tools created to test Bluerandom and used in the paper.
- AddEntropyIOCTL: reads from stdin and use the linux system call (RNDADDENTROPY) to add entropy to the system.
- AES_PRNG, SHA_based_PRNG: both are PRNG implementations which were tested with Bluerandom as the seed source (Results can be found in the paper).
- LERandLauncher: the Bluetooth internal PRNG can be accessible by the LE Rand command, so this tool send this commmand to the BLE controller and generates random numbers. In the paper this tool was used to compare it with Bluerandom seeding a PRNG.


Compile the src/ code with make and run it with sudo ./bluerandom. The generation process has low throughput and depends on how much BLE devices are nearby (see the paper for details).

If "Error on opening HCI device. No such device" appears you might need to start it (sudo bluetoothctl; power on; exit - or with blueman program).


Suggestions are welcome!


# Author

Alexandre Giron

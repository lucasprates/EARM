# EARM - Energy and Reachability Managament of Mining Rigs (version 1.0.0)

This Repo is an attempt to make public a software I coded for the Wemos D1 ESP8266 board. It's a software that controls the energy and Internet reachability of mining rig(s), automatically adjusting itself so that you don't have to keep checking (and rebooting) it manually when something goes unexpected.
For it to work you will need to get an ESP8266 board and add 2 (two) High voltage realys into it (one for the internet modem/router/switch bundle and another for the mining rig bundle).

### What is it?
EARM is a software that checks reachability of (1) Outside servers to guarantee internet access to the mining rig, and (2) ping the local mining rig to check if it is still stable and reliably working.
Then, after that, we turn the High Voltage relay #1 and/or #2 On+Off to get internet access back on, or, reboot the computer system.
It does so automatically and also via a tiny webservice that can be used to trigger manually (and change settings) while on a distance from the device / mining rig / internet router/modem/switches.

### Deploy
- Using the Arduino IDE, add the package of ESP8266 boards.
- Plug your ESP8266 board.
- Open the main EARM.ino file
-  Modify the Strings related to your own wireless SSID and password
- Modify the password String of the webservice required to access it
- Modify other information for what suits your better
- how many tryouts before turning the relay on/off
- how long it takes to test internet reachability
- how long it takes to test mining rig reachbility
- and many other variables avaliable for you to modify at EARM.ino
- Compile the code and flash it into your ESP8266 board.
- Turn on your ESP8266

That's all. Simple and easy, no?

### Usage

To use this project, simply turn on the flashed ESP8266 and wait for it to boot (should take less than 20 seconds).

It will automatically work on a loop schedule looking for any reachability issues, and prompting jobs to fix the issue. You don't have to do a thing.

But if you want to change something, there is a small webservice API avaliable for you to use.

Just get the IP address of the board and, using a computer with local network access and a browser, type in the IP address of the EARM Wemos D1 device (the port to access the webservice is the standard 80 http).  

The little webservice protocol is accessable as following:
- /rebootMiningRig
- It manually reboots the mining rig by triggering its direct relay
- /rebootInternet
- It manually reboots the internet modem/router/switches by triggering its relay
- /checkMiningRigReachability
- It checks, and tells you, if the mining rig is reachable at that specific moment
- /checkInternetReachability
- It checks, and tells you, if the internet modem/router/switches are reachable at that specific moment
- /getLogFile"
- It gives you all the log information in a TXT file (**not implemented yet**)

### Tip
Use a strong webservice password and open the IP Address of the EARM ESP8266 device in your router so that you can access it outside your network. It will allow you to control the energy and functionability of your device when distance with a somewhat security.

### Development

Want to contribute? Great! Just let me know and I'll let you in.
Also, if you want to start your own from this one, just fork it and get it going...

Found a bug or think of improvements? Just create an issue and I'll gladly answer and implement it.

### Todos
- Make the code compatible with ESP32 boards also

### License
MIT

### Created by
Lucas Quintiliano Prates

----

**Hope it helps you as it has helped me. Cheers!**

----

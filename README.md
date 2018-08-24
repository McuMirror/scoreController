# scoreController

This is the **"Score Controller"** part of the **"Score Panels System"**.
The **“Score Controller”** software controls, via Network, one or more **“Score Panels”**.

Each **"Score Panel"** is individually configurable in such a way to show the score information only or instead the score
and, on request, **slides** and **videos** as dictated by the **"Score Controller"** or even **live images** of the game field.

Here you can see the Control Panel for the _Volley_:

![Volley Score Controller Panel](/images/ScoreController.png)

The corresponding screen that is shown on each of the **"Score Panels"** is:

![Volley Score Panel](/images/ScorePanel.png)

The **"Score Controller"** software can be run on Linux computers (tested on UBUNTU 18.04 LTS) or on an Android tablet or else on a Raspberry Pi3 connected to a monitor, mouse and keyboard.

The **"Score Controller"** and the **"Score Panels"** communicate via Network (either wired or wireless).

At present there are only 3 sports that have the corresponding scoreboard:

* Volley
* Handball
* Basket

but the system can be easily extended to many other sports.

The program is written in C++ by using the powerfull framework Qt. I have installed the version 5.11.1 on my Ubuntu laptop and version 5.7.2 on my Raspberry Pi3.

## Installation
There are already built versions for Android, Ubuntu and Raspberry Pi3 at:

https://github.com/salvato/ScorePanel_executables

For Ubuntu and Raspberry you can simply download the executable in a directory of your choice and the run the App provided that all the needed libraries are presents.

To check if there are missing libraries you can run the following command:

`ldd ./scoreController`

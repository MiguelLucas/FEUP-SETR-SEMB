# Project

This project is a (supposedly) Worms game in a dual display, using an Arduino UNO as microcontroller, with a Bluefruit LE Shield for bluetooth connection.

### Requirements

* Arduino UNO
* Bluefruit LE Shield module
* A fuckton of cables
* Android smartphone with [Adafruit Bluefruit LE app](https://play.google.com/store/apps/details?id=com.adafruit.bluefruit.le.connect) and bluetooth
* 2 Nokia 5510/3110 displays ([tutorial](https://learn.adafruit.com/nokia-5110-3310-monochrome-lcd/overview))
* Patience

### Procedures

* Follow the wiring of the tutorial, or of the [Report](https://github.com/MiguelLucas/FEUP-SETR-SEMB/tree/master/Project/Report) in this repository
* Run the project into your Arduino
* Wait for the "Waiting for bluetooth connection" message
* Install the app in your phone and turn on Bluetooth
* Open the app and search for Bluefruit LE
* Select "Controller"
* Select "Control Pad"
* Play (up and down arrows to jump, left and right arrows to move, button 1 to aim/shoot, button 2 to cancel

### Contents

* [Code](https://github.com/MiguelLucas/FEUP-SETR-SEMB/tree/master/Project/Project)
* [Report](https://github.com/MiguelLucas/FEUP-SETR-SEMB/tree/master/Project/Report)
* [BitmapEncoder](https://github.com/MiguelLucas/FEUP-SETR-SEMB/tree/master/Project/BitmapEncoder) - This program is used to convert bitmaps into a bit array, if you want to change the images in the game
* [Images](https://github.com/MiguelLucas/FEUP-SETR-SEMB/tree/master/Project/Images) - Original image used. Yes it looks like what you're thinking.

### Disclaimer

DO NOT FOLLOW the software architecture of this project, unless you want to have a bad grade. This uses a hybrid (cyclic + interrupts) approach, and you should use a more sophisticated architecture for your project, even if you didn't know you had (i didn't)

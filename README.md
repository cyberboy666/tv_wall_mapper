# tv_wall_mapper

_an open-source cross-platform application for creating synchronized tv wall instalations_

![image](https://github.com/cyberboy666/tv_wall_mapper/assets/12017938/c92391b6-5b20-4313-99b3-b30fac11a27d)

- this software is distributed by __UNDERSCORES__ - compiled downloads for linux, windows, mac and arm64 are available to purchase from underscores.shop [put link to shop page here]
- the source code can be viewed [here](/src/)
- consider [donating](https://opencollective.com/underscores) to the underscores project to help us continue creating for the commons

## background

this application was originally written for an installation i worked on with __the dojo stage__ at [Twisted Frequency Festival](https://twistedfrequency.nz/) NYE 2023. we used 8 recycled tvs, a cheap [aliexpress hdmi_controller](https://www.aliexpress.com/item/1005005643772042.html) and this software to achieve the _synchronized tv wall effect_ seen here:

![tv_wall_gif](http://underscores.shop/wp-content/uploads/2024/02/tv_wall_gif_04.gif)

you can find some more background on this install and process here: [WORKS IN PROGRESS 02 - talk about tv_wall_mapper software and installation](https://videos.scanlines.xyz/w/fUE9iVRaK9EojUMZ2XjySZ) 

## demo video

[coming soon]

## hardware options

<details><summary>hardware options</summary>

to achieve the _synchronized tv wall effect_ you need a seperate video output per tv in the wall. depending on the requirements of your installation there are a few different ways you can combine this software with hardware to achieve the desired results. feel free to [email me](mailto:tim@cyberboy666.com) if you would like individual consulting on this.

### using a computer with multiple display outputs

![image](https://github.com/cyberboy666/tv_wall_mapper/assets/12017938/5a8784c8-75d2-4ceb-b22e-aa63f8e5e471)

if you only need to map a few tvs or have graphics cards that allow multiple display outputs this can be the simplest way to achieve the effect

![image](https://github.com/cyberboy666/tv_wall_mapper/assets/12017938/33baf3d1-9655-462d-bfe9-2fca79397bae)

- connect each display output to an input on one of your tv's (you may need to convert the signal format depending on if they are natively compatable)
- in __tv_wall_mapper__ interface set the `NUMBER OF MONITORS` to the number of physical outputs you have connected
- in __tv_wall_mapper__ interface, for each `MONITOR X` set the `MONITOR DISPLAY` to the physical output it corresponds to
- in __tv_wall_mapper__ interface, for each `MONITOR X` set the `TV LAYOUT` to `1`
- finally use the __tv_wall_mapper__ interface to create _the map_ between the physical layout of your tv's (more on this in software settings below)

pros/cons of this __hardware option__:

- quite simple for a small amount of displays - becomes less fesable the more tv's you want
- cheapest if using hardware you already have
- gpu's can be an expensive way to add display outputs
- it is likely to need to convert signals if using tv's without display ports etc...

### using cheap Aliexpress TV Video wall Controllers

![image](https://github.com/cyberboy666/tv_wall_mapper/assets/12017938/2fbe86ad-6029-4433-ae10-f51f5b6afee5)


you can buy quite affordable off-the-shelf hardware for taking a single HDMI video input and splitting this into a grid of multiple HDMI video outputs. although there are many of these the only ones i personally have tested look like this:

- [2x2 HDMI Video Wall Controller](https://www.aliexpress.com/item/1005006156950887.html) for around __25euros__
- [3x3 HDMI Video Wall Controller](https://www.aliexpress.com/item/1005006156950887.html) for around __60euros__

![image](https://github.com/cyberboy666/tv_wall_mapper/assets/12017938/04c68c95-20fa-4096-910b-fa41db912b8e)

![image](https://github.com/cyberboy666/tv_wall_mapper/assets/12017938/cfed83e9-87ab-43a4-b672-918f908d6c4e)

- connect a single output from the computer to the __video wall controller__ input
- connect each of the __video wall controller__ outputs to an input on one of your tv's (__NOTE: these controllers only output HDMI 1080p @60Hz - this is not compatable with some older tv's and a converter may be needed__)
- in __tv_wall_mapper__ interface, for the monitor you connected set the `TV LAYOUT` to `2x2` or `3x3` (depending on the controller you are using)
- finally use the __tv_wall_mapper__ interface to create _the map_ between the physical layout of your tv's (more on this in software settings below)

pros/cons of this __hardware option__:

- best value for mapping larger tv walls ( 9 outputs for __60euros__ )
- can save on cabling by installing controller close to tv wall (only short cables from tvs to controller)
- will need addition signal convertions unless all the tvs support __1080p @60Hz__
- can add many more outputs by combining this method with the multiply display outputs covered above (ie 3x 3x3 controllers plus a computer with 3 hdmi outputs can map 27 tvs !)

### using a cheap sbc per tv and a network switch

![image](https://github.com/cyberboy666/tv_wall_mapper/assets/12017938/01f658c7-1730-41ee-9a11-3a509efe5165)

_NOTE: this method is untested by me but in theory should work_

since the __tv_wall_mapper__ software supports sending and recieving _network video_ over [NDI](https://ndi.video/tools/) another option would be to install a _Single Board Computer_ (such as a raspberry pi) to each tv in the wall - then that SBC can run the mapping software, recieve the source video over NDI and output the mapped video directly to the tv.

NDI video can easliy be split into multiple signals to send to each SBC using a cheap _network switch_ (for example [heres](https://www.aliexpress.com/item/1005007009152040.html) a 16 port switch for only 25euros)

_NOTE: currently the software is supported on 64bit versions of raspberry pi - it runs well on pi4 and up. however i tested it on a pi_zero2 and although it runs it needs to be too low resolution to be usable_

possilbe use case:

i plan to test running this app on an [Orange Pi Zero 3](https://www.aliexpress.com/item/1005007009152040.html) which cost around __20euro__ each.

![image](https://github.com/cyberboy666/tv_wall_mapper/assets/12017938/a12905ec-23d0-401d-b04d-d474d02a27a3)


__(big) if i confirm this runs ok__ this could be a viable alternative to the __ hardware controller__ approach:

- is cheap enough to buy multiple (6x tvs = only __120euros__ on hardware)
- they have rj45 ethernet ports built in
- can output multiple video formats (including potentially composite video) - meaning you may save on signal converters
- ethernet cables cheaper / longer than HDMI cables
- possible to add PoE to these to save on power cabling
- can reuse the SBC for more other tasks than a hardware controller

to set the __tv_wall_mapper__ app to work for this hardware arrangement you can first create the mapping:

- and then set `NUMBER OF MONITORS` to `1` and `TV LAYOUT` to `1`
- then the same config file can be copied across all the SBCs - you can find all the settings stored as human readable JSON file in `bin/data/config.json`
- the only setting that needs to change between the SBCs is `FIRST TV ON OUTPUT:` = whichever tv this SBC is connecting to

</details>

## how to use this app

<details><summary>how to use this app</summary>

### inputing source video

__tv_wall_mapper__ app can take input source from video devices on your computer (such as webcams and external capture cards)

if you have the [NDI SDKs installed](https://ndi.video/for-developers/ndi-sdk/download/) it will also be able to take input source from NDI

many programs for creating video such as resolume and touchdesigner can output to NDI (and to VIRTUAL CAMERA) giving you options for linking with __tv_wall_mapper__

if you are unsure where you will be getting source video from a good program to start with is [OBS](https://obsproject.com/download) - this can play videos, screencapture and much more plus can output to VIRTUAL CAMERA with __obs virtual cam__ and to NDI with [OBS NDI plugin](https://obsproject.com/forum/resources/obs-ndi-newtek-ndi%E2%84%A2-integration-into-obs-studio.528/) installed.

_NOTE: currently there is a [known bug](https://github.com/cyberboy666/tv_wall_mapper/issues/1) where windows version of __tv_wall_mapper__ crashes when taking input from __obs virtual cam__ - if you want to input source from obs on windows you will need to use NDI_

### creating _the map_

_the map_ is how you input where the tvs are placed relative to each other so that the software can correctly crop the image required for each one.

start by setting the `NUMBER OF TVS` to match your install.

after you have physically set up your tvs you will need to take some measurements with a _tape measure_:

- __canvas width__ and __canvas height__ is the total size of the space that the video is being mapped onto - if you imagine that the tv_wall is one big screen (with gaps between the tvs) then this is the size of that screen. a good place to start is to imagine the `X AXIS` as the horizontal line running across from the top-most pixel of the installation, and the `Y AXIS` as the vertical line running up from left most pixel of the installation

- you can also measure and enter the `TV I WIDTH` and `TV I HEIGHT` for each tv found under `TV VALUES` heading if you click on the `TV I`

- finally you can measure the `TV I X POS` - the distance from `Y AXIS` to first pixel on _tv i_, and `TV I Y POS` - the distance from `X AXIS` to first pixel on _tv i_

_NOTE: the position measurements do not need to be exact - this will likely be adjusted slightly - its more important to get good width/height measurements so the ratios are correct_

![Screenshot from 2024-06-06 09-09-23](https://github.com/cyberboy666/tv_wall_mapper/assets/12017938/5e7b8730-aaae-48e3-8ea8-d602008280d8)

once these are entered the __MAPTEST__ view in the app - black box with blue/red squares representing tvs - should be looking like your tv wall

- if you select __testcard 01__ from `VIDEO SOURCE` and the monitor settings are correct you should now see a test card being spread out over the tvs in the wall. small adjustments so size/position can be made here so lines appear to line up between tvs.

### controls for adjusting the map

this info can be seen in the app by clicking the `HELP` tab at top of the interface:

MAPPING WITH A MOUSE:
- CLICK on a TV to select it (RED is selected)
- CLICK and drag a TV to position
- hold SHIFT and drag to resize it
- or RIGHT CLICK and drag to resize it
MAPPING WITH A KEYBOARD:
- press ALT plus LEFT/RIGHT to select a TV
- press ARROW KEYS to position it
- press SHIFT plus ARROW KEYS to resize it
- press ALT plus UP/DOWN to set grid size (how much each press moves)
- NOTE: TVs with width less than height are automatically rotated
- NOTE: resizing can be negative to support all possible orientations
OTHER KEYBOARD SHORTCUTS:
- press F to toggle ON/OFF FULLSCREEN
- press R to toggle ON/OFF FPS ON OUTPUT

### info about every setting in the interface:

- __HELP__ - this opens/closes info about mapping controls
- __save config__ - saves the current config to a json file (located at `bin/data/config.json`)
- __load config__ - loads the config from the file at `bin/data/config.json`
- __refresh inputs/outputs__ - this checks for any new `VIDEO SOURCE` or `MONITOR DISPLAY`s since the app started - useful if you hotplug a capture card or new monitor
- __VIDEO SOURCE__ - select where the source video is coming from - can take input from cameras/capture cards or from a NDI source
- _INPUT RESOLUTION_ - this displays the source resolution - usually it will match the monitor or a resolution that `VIDEO SOURCE` supports
- __NUMBER OF TVS__ - set the amount of tvs being used in the tv wall
- __HIDE MAPTEST__ - removes the MAPTEST - box showing the mapping - from the interface
- __HIDE PREVIEW__ - removes the PREVIEW - box showing video output - from the interface
- __OVERWRITE DEFAULTS__ - lets you play around with `INPUT WIDTH`, `INPUT HEIGHT` and `FRAMERATE` on the __video source__ - could be useful if having performance issues
- __NUMBER OF MONITORS__ - sets the amount of seperate windows this app should create - allowing you to map different parts of input source to different __displays__ - see _using a computer with multiple display outputs_ in __hardware options__ for more info
- __MONITOR 0 | MONITOR 1 | ...__ - lets you toggle between the different monitor outputs so they can be configured seperately (ie it is possible that one monitor will be going to a 2x2 video wall so needs `TV LAYOUT: 2x2` but another is going straight to a tv so needs `TV LAYOUT: 1` )
- __only show tvs for this monitor__ - toggle whether all tvs are shown in the MAPTEST or filter them only for this monitor
- __OUTPUT TO MONITOR__ - toggle whether this monitor should have a window showing its output
- __OUTPUT TO NDI__ - toggle whether this monitor should output its content to NDI (useful if sending video over network rather than HDMI)
- __MONITOR DISPLAY__ - select which connected display this monitor should output to
- __monitor fullscreen__ - toggles this monitors window to being fullscreen - fullscreen is needed for tv wall effect but it could be easier when setting things up to havee it windowed
- __show framerate__ - outputs the framerate of the app to this monitor - could be useful when testing that the tv wall is responsive enough
-  __TV LAYOUT__ - use this setting when the monitor is outputing to a __tv wall controller__ so multiple tv images are sent in `2x2` or `3x3` grid (see _using cheap Aliexpress TV Video wall Controllers_ under __hardware options__)
-  __NUMBER OF TVS ON OUTPUT__ - this should automatically update with __TV LAYOUT__ (ie `2x2` = 4, `3x3` = 9 etc) however could be useful if you are only mapping say 7 tvs from a 3x3 controller etc...
-  __FIRST TV ON OUTPUT__ - this can be useful if you want to use the same mapping across multiple devices running __tv_wall_mapper_ app and just need to set which tv each one is mapping (see _using a cheap sbc per tv and a network switch_ under __hardware options__)
-  __CANVAS WIDTH__ - this is the width of the "screen" that video is mapped onto - if the tv wall was one big screen
-  __CANVAS HEIGHT__ - this is the height of the "screen" that video is mapped onto - if the tv wall was one big screen
-  __SELECTED TV__ - this is the currently highlighted tv in MAPTEST (red) - the one that will be affected by key presses etc
-  __GRID SIZE__ - this is the amount one key press will move the `SELECTED TV`
-  __TV I__ - click on a __tv__ to see values for it:
-  __TV I WIDTH__ - set the physical width of `TV I`
-  __TV I HEIGHT__ - set the physical height of `TV I`
-  __TV I X POS__ - set the distance from left most pixel to `Y AXIS`
-  __TV I Y POS__ - set the distance from top most pixel to `X AXIS`

</details>

## how to install from source

<details><summary>how to install from source</summary>

you can support this project by buying compiled versions of this app from underscores.shop [put link to shop page here] - with a compiled version you only need to download & unzip a folder, then just run the app - no installing required!

you can also install and compile it yourself from the source code and instructions provided below. if this software is useful to you consider giving back by [donating](https://opencollective.com/underscores/donate) or contributing in other ways.

the app has been tested on:

- linux (pop_os, ubuntu, debian & xubuntu)
- windows 10
- mac_os Monterey (intel) - currently untested on ARM
- raspberry_pi_4 running _raspios-bookworm-arm64_
- orange_pi_5 running _debian_

this code is written in [openFrameworks](https://openframeworks.cc/) - an open source cpp library for creative coding. for the most part you can followw their [install instructions](https://openframeworks.cc/download/) for your chosen platform.

### install on linux:

- install and compile __openFrameworks12__ - in a terminal:

```
cd ~
wget https://github.com/openframeworks/openFrameworks/releases/download/0.12.0/of_v0.12.0_linux64gcc6_release.tar.gz
mkdir openframeworks
tar vxfz of_v0.12.0_linux64gcc6_release.tar.gz -C openframeworks --strip-components 1
cd openframeworks/scripts/linux/<YOUR DISTRIBUTION HERE>/
sudo ./install_dependencies.sh -y
make Release -C ~/openframeworks/libs/openFrameworksCompiled/project
```

- install NDI SDK (if using NDI - recommended):

download [NDI SDK](https://ndi.video/for-developers/ndi-sdk/download/) and unzip: `NDI SDK for Linux`

then copy the sdk into usr/lib: `sudo cp ~/NDI\ SDK\ for\ Linux/lib/x86_64-linux-gnu/* /usr/lib/`

- download __tv_wall_mapper__ and dependanices:

```
<INSTALL GIT IF YOU DO NOT HAVE IT>
cd ~/openframeworks/apps/myApps
git clone git@github.com:cyberboy666/tv_wall_mapper.git
cd tv_wall_mapper
./install_addons.sh
make
```

now it is compiled can run with `make run` or with `./bin/tv_wall_mapper`

### install on windows 10

follow the [openframeworks visual studio](https://openframeworks.cc/setup/vs/) guide:

- download [visual studio community 2019](https://www.visualstudio.com/thank-you-downloading-visual-studio/?sku=Community&rel=16) and install with _desktop development with c++_ plus other settings as listed on that oF page

install openframeworks and tv_wall_mapper:

- download and install [git for windows](https://gitforwindows.org/)
- download and unzip [openframeworks 12 for visual studio](https://github.com/openframeworks/openFrameworks/releases/download/0.12.0/of_v0.12.0_vs_release.zip)
- open up `git bash` and navigate to the openframeworks folder:

```
cd ~/path/to/of12_folder/
cd apps/myApps/
git clone git@github.com:cyberboy666/tv_wall_mapper.git
cd tv_wall_mapper
./install_addons.sh
```

install NDI SDK (if using NDI - recommended)

- download the [sdk](https://ndi.video/for-developers/ndi-sdk/download/)
- you may need to copy `Processing.NDI.Lib.x64.dll` from here into `tv_wall_mapper/bin` if it doesnt load automatically

build __tv_wall_mapper__ app:

- open the openframeworks __ProjectGenerator__ in top level of openframeworks folder
- from here import the __tv_wall_mapper__ folder and generate project
- now you can open the project in _visual studio_ with the file `tv_wall_mapper.sln`
- inside _visual studio_ find __BUILD__ / __RUN__  

### install on mac_os

follow the [openframeworks xcode](https://openframeworks.cc/setup/xcode/) guide:

- download xcode (14.1 or later) + Xcode's command line tools... (check internet archive if you can not access apple store)
- download and unzip [openframeworks 12 for xcode](https://github.com/openframeworks/openFrameworks/releases/download/0.12.0/of_v0.12.0_osx_release.zip)
- open up terminal and navigate to the openframeworks folder:

```
cd ~/path/to/openframeworks
cd apps/myApps/
git clone git@github.com:cyberboy666/tv_wall_mapper.git
cd tv_wall_mapper
./install_addons.sh
```

install NDI SDK (if using NDI - recommended)

- download the [sdk](https://ndi.video/for-developers/ndi-sdk/download/)

build __tv_wall_mapper__ app:

- open the openframeworks __ProjectGenerator__ in top level of openframeworks folder
- from here import the __tv_wall_mapper__ folder and generate project
- now you can open the project in _xcode_ with the file `Project.xcconfig`
- inside _xcode_ find __BUILD__ / __RUN__  

### install on raspberry_pi_4 or similar arm64:

- download the the raspberry pi os image: `2024-03-15-raspios-bookworm-arm64.img.xz` and flash to the sd card
- once the rpi is booted / connected to internet etc then you can download openframeworks:

```
cd ~
wget https://github.com/openframeworks/openFrameworks/releases/download/0.12.0/of_v0.12.0_linuxaarch64_release.tar.gz
mkdir openframeworks
tar vxfz of_v0.12.0_linuxaarch64_release.tar.gz -C openframeworks --strip-components 1
cd openframeworks/scripts/linux/debian/
sudo ./install_dependencies.sh -y
make Release -C ~/openframeworks/libs/openFrameworksCompiled/project
``` 

- install NDI SDK (if using NDI - recommended):

download [NDI SDK](https://ndi.video/for-developers/ndi-sdk/download/) and unzip: `NDI SDK for Linux`

then copy the sdk into usr/lib: `sudo cp ~/NDI\ SDK\ for\ Linux/lib/aarch64-rpi4-linux-gnueabi/* /usr/lib/`

- download __tv_wall_mapper__ and dependanices:

```
<INSTALL GIT IF YOU DO NOT HAVE IT>
cd ~/openframeworks/apps/myApps
git clone git@github.com:cyberboy666/tv_wall_mapper.git
cd tv_wall_mapper
./install_addons.sh
```

__TURN ON FLAG TO COMPILE FOR GLES__

since openframeworks12 on rpi doesnt work rn with standard glsl we need to switch to using GLES - this limits the program to only having one window so it can not be used with multiple display outputs

```
cd cd ~/openframeworks/apps/myApps/tv_wall_mapper
nano src/ofApp.h
```

and UNCOMMENT LINE 9:
```
#define PROJECT_TARGET_OPENGLES
```
and save (`CTRL+X`, `Y`, `ENTER`) - now compile the program:

```
make
```

now it is compiled can run with `make run` or with `./bin/tv_wall_mapper`


</details>

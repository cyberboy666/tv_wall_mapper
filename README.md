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



</details>

## how to install from source

<details><summary>how to install from source</summary>



</details>

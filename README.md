# slide

Shows a slideshow of pictures that reside in a given directory (recursively) in fullscreen using opengl. 

## dependencies

`sudo pacman -S glfw-wayland` // or x11 respectively

`sudo pacman -S mesa`

## how to build

`make slide`

## how to run

`./slide <PATH_TO_YOUR_PICTURES>`

## configuration

I am mostly using this on a raspberry pi 3 to run a slideshow of my thousands of photos. 

`sudo nano /etc/X11/xinit/xinitrc`

comment out this line 

`. /etc/X11/xsession`

add this line to start openbox

`exec openbox-session`

then to autostart the slideshow

`sudo nano /etc/xdg/openbox/autostart`

then add the program

`/path/to/slide <PATH_TO_YOUR_PICTURES> &`

this is from here: https://raspberrypi.stackexchange.com/questions/52099/using-openbox-to-autostart-gui-application-raspberry-pi-3

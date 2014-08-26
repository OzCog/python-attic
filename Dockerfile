# Replace all instances of shujingke with your github / docker.io / system username.
# Replace hk.archive.ubuntu.com with your own country code, e.g. nl.archive.ubuntu.com
# adduser shujingke
# cd ~ && git clone http://shujingke@github.com/shujingke/opencog && cd opencog && git pull
# docker build -t shujingke/opencog-dev-qt .
# xhost +
# docker run --rm -i -v /tmp/.X11-unix/X0:/tmp/.X11-unix/X0 -v /etc/passwd:/etc/passwd -v /etc/shadow:/etc/shadow -v /etc/group:/etc/group -v /etc/group-:/etc/group- -v /home/shujingke:/home/shujingke -e DISPLAY=:0.0 -t shujingke/opencog-dev-qt 
# remote
# ssh -L 17001:localhost:17001 -XC hostname 'docker run --rm -i -v /home/shujingke:/home/shujingke -e DISPLAY=$DISPLAY -p 17001:17001 -t shujingke/opencog-dev-qt'
# for gnome-panel fix, install locally, configure panels (top panel alt-super-right-click: delete this panel; bottom panel: alt-super-right-click: add to panel: main menu), then re-run container

FROM ubuntu:14.04
MAINTAINER Alex van der Peet "alex.van.der.peet@gmail.com"
MAINTAINER David Hart "dhart@opencog.org"

RUN sed 's/archive.ubuntu.com/hk.archive.ubuntu.com/' -i /etc/apt/sources.list

RUN apt-get -y update
RUN apt-get -y install software-properties-common git

ADD scripts/ocpkg install-dependencies-trusty
RUN chmod +x /install-dependencies-trusty
RUN /install-dependencies-trusty

RUN apt-get -y install wget tmux
RUN apt-get -y install gitg
RUN apt-get -y install git-gui
RUN apt-get -y install meld
RUN apt-get -y install qtcreator
RUN apt-get -y install gnome-session
RUN apt-get -y install gnome-panel
RUN apt-get -y install gnome-terminal
RUN apt-get -y install nautilus
RUN apt-get -y install vim-gnome
RUN apt-get -y remove brasero gnome-media

RUN adduser --disabled-password --gecos "Shujing Ke,,," shujingke

WORKDIR /home/shujingke
USER shujingke
ENV USER shujingke
ENV HOME /home/shujingke

ENV STARTSCRIPT "\
echo evaluating startup script... ;\
cd $HOME;\
tmux new-session -d '/usr/bin/gnome-panel&/bin/bash' ;\
tmux set -g set-remain-on-exit on ;\
tmux set-option -g set-remain-on-exit on ;\
tmux bind-key R respawn-window ;\
tmux split-window -d -v -p 25 '/bin/bash' ;\
tmux select-layout even-vertical ;\
tmux attach \
"

CMD /bin/bash -l -c "eval $STARTSCRIPT"

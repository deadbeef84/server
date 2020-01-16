ARG IMAGE_BASE

FROM ${IMAGE_BASE} as BUILD
    ARG PROC_COUNT=8

    ADD http://opensource.spotify.com/cefbuilds/cef_binary_75.1.14%2Bgc81164e%2Bchromium-75.0.3770.100_linux64_minimal.tar.bz2 /opt/cef.tar.bz2
    # ADD http://opensource.spotify.com/cefbuilds/cef_binary_3.3683.1920.g9f41a27_linux64_minimal.tar.bz2 /opt/cef.tar.bz2
	# ADD http://opensource.spotify.com/cefbuilds/cef_binary_3.3578.1870.gc974488_linux64_minimal.tar.bz2 /opt/cef.tar.bz2
	WORKDIR /opt
	RUN tar -jxf cef.tar.bz2 && mv /opt/cef_binary_* /opt/cef
    RUN mkdir /opt/build
    WORKDIR /opt/build
    RUN cmake /opt/cef && make -j $PROC_COUNT
    RUN cp /opt/build/libcef_dll_wrapper/libcef_dll_wrapper.a /opt/cef/Release/
    # RUN strip /opt/cef/Release/libcef.so

FROM scratch
	COPY --from=BUILD /opt/cef /opt/cef

# FROM ubuntu:18.04

# RUN apt update && apt install -y \
#   curl build-essential flex g++ git-svn libcairo2-dev libglib2.0-dev \
#   libcups2-dev libgtkglext1-dev git-core libglu1-mesa-dev libnspr4-dev \
#   libnss3-dev libgnome-keyring-dev libasound2-dev gperf bison libpci-dev \
#   libkrb5-dev libgtk-3-dev libxss-dev python libpulse-dev ca-certificates \
#   default-jre wget

# # accept MS license for TTF fonts
# RUN echo ttf-mscorefonts-installer msttcorefonts/accepted-mscorefonts-eula select true | debconf-set-selections

# # RUN curl 'https://chromium.googlesource.com/chromium/src/+/master/build/install-build-deps.sh?format=TEXT' | base64 -d > install-build-deps.sh
# # RUN bash install-build-deps.sh --no-prompt --no-chromeos-fonts
# # RUN apt install -y libgtkglext1-dev

# RUN mkdir -p /code/automate
# RUN wget https://bitbucket.org/chromiumembedded/cef/raw/master/tools/automate/automate-git.py -O /code/automate/automate-git.py

# ENV CEF_USE_GN=1
# # branch=3904
# RUN python /code/automate/automate-git.py --download-dir=/code --no-distrib --no-build --branch=3683 --force-update

# ENV GN_DEFINES="use_x11=false use_ozone=true"
# ENV PATH=$PATH:/code/depot_tools
# WORKDIR /code/chromium/src/cef
# RUN bash cef_create_projects.sh

# WORKDIR /code/chromium/src
# RUN ninja -C out/Debug_GN_x64 cef chrome_sandbox

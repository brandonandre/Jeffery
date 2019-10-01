# Start with a ubuntu linux image...
FROM ubuntu:latest

# Install the required packages for the image...
RUN apt-get update && \
    apt-get -y install sudo sed bash python make binutils rsync

# Add all the files from the compile folder to the linux image.
COPY ./compile .

# Run the compiler batch file to install everything...
RUN sudo chmod +x oecore-x86_64-cortexa9-vfpv3-toolchain-5.0.sh
RUN bash oecore-x86_64-cortexa9-vfpv3-toolchain-5.0.sh

# Move all the developer files
RUN pwd
WORKDIR /development
RUN pwd
VOLUME ["/development"]
COPY ./src /development
COPY ./makefile /development
WORKDIR /development/myrio
COPY ./lib /development/myrio

# Compile the code...
WORKDIR /development
RUN ls
RUN make

# Send to the remote robotics
RUN rsync -alPvvz main.out user@127.16.0.1:remote_path
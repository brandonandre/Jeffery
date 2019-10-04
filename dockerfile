# Start with a ubuntu linux image...
FROM ubuntu:latest

# Install the required packages for the image...
RUN apt-get update && \
    apt-get -y install sudo sed bash python make binutils rsync sshpass

# Add all the files from the compile folder to the linux image.
COPY ./compile .

# Run the compiler batch file to install everything...
RUN sudo chmod +x oecore-x86_64-cortexa9-vfpv3-toolchain-5.0.sh
RUN bash oecore-x86_64-cortexa9-vfpv3-toolchain-5.0.sh

# Move all the developer files
RUN pwd
WORKDIR /development
RUN pwd
COPY ./src /development
COPY ./makefile /development
WORKDIR /development/myrio
COPY ./lib /development/myrio

# Compile the code...
WORKDIR /development
RUN ls
RUN make
RUN ls

# Send to the remote robotics
RUN sshpass -p "roboboto" rsync -alPvvz main.out admin@172.16.0.1:/home/compiled
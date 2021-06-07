# Get the base Ubuntu image from Docker Hub
FROM ubuntu:latest

# Update apps on the base image
RUN apt-get -y update && apt-get install -y

# Install the Clang compiler
RUN apt-get -y install gcc

RUN apt-get -y install make

# Copy the current folder which contains C++ source code to the Docker image under /usr/src
COPY . /usr/src/threads

# Specify the working directory
WORKDIR /usr/src/threads

# Use Clang to compile the Test.cpp source file
RUN make exec_matrix

# Run the output program from the previous step
CMD ["./Test"]

FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get -y --fix-missing update
RUN apt-get -y upgrade
RUN apt-get -y install build-essential
RUN apt-get -y install valgrind
RUN apt-get -y install git
RUN apt-get -y install openjdk-8-jdk
RUN apt-get -y install python3 python3-pip
RUN apt-get -y install libboost-all-dev
RUN pip3 install networkx
RUN pip3 install matplotlib

# maven 3.6.3
RUN apt-get -y install wget
RUN wget https://repo.maven.apache.org/maven2/org/apache/maven/apache-maven/3.6.3/apache-maven-3.6.3-bin.tar.gz
RUN tar -xvf apache-maven-3.6.3-bin.tar.gz
RUN mv apache-maven-3.6.3 /opt/
RUN rm apache-maven-3.6.3-bin.tar.gz
ENV PATH="/opt/apache-maven-3.6.3/bin:$PATH"

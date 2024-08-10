#Create the Chroot Directory
sudo mkdir -p ~/chroot/{bin,lib,lib64,dev}

#Compile the program
gcc -o server server.c -lssl -lcrypto -lpthread

#Copy Required Binaries for running 'bash' and 'server'
sudo cp /bin/bash ~/chroot/bin/
sudo cp server ~/chroot/bin/

#Use 'ldd' to find the required libraries and copy them into appropriate directories
sudo cp /lib/x86_64-linux-gnu/libc.so.6 ~/chroot/lib/
sudo cp /lib/x86_64-linux-gnu/libtinfo.so.6 ~/chroot/lib/
sudo cp /lib/x86_64-linux-gnu/libgcc_s.so.1 ~/chroot/lib/
sudo cp /lib64/ld-linux-x86-64.so.2 ~/chroot/lib64/
sudo cp /lib/x86_64-linux-gnu/libssl.so.3 ~/chroot/lib/
sudo cp /lib/x86_64-linux-gnu/libcrypto.so.3 ~/chroot/lib/

#Create necessary device nodes
sudo mknod -m 666 ~/chroot/dev/null c 1 3
sudo mknod -m 666 ~/chroot/dev/zero c 1 5
sudo mknod -m 666 ~/chroot/dev/random c 1 8
sudo mknod -m 666 ~/chroot/dev/urandom c 1 9

#Set Up Configuration Files
sudo mkdir -p ~/chroot/etc/ssl
sudo cp -r /etc/ssl/servercerts ~/chroot/etc/ssl/

#Running the Serrver in a Chroot Environment
sudo chroot ~/chroot /bin/server

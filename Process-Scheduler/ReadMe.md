# Process-Scheduler 
This part aims to introduce connect to your target **Linux** system in **Visual Studio**


# SSH in Linux
First of all, 
SSH Command in Linux The ssh command provides a **secure encrypted connection between two hosts over an insecure network**.

## Other CMD in Necessity
`gcc`, `g++`, `gdb`, `gdbserver`, and `zip` 

## Installing
```code
sudo apt-get install openssh-client
$ sudo apt-get install openssh-server
```
## Configure
Using command :
```code
sudo nano /etc/ssh/sshd_config 
```
and modify the file by adding the words :
```text
PermitRootLogin no  #prohibit root login and configure other user
AllowUsers xxx #xxx(your user name)
PasswordAuthentication yes #default using password login
ChallengeResponseAuthentication no
```
The configure is not unique following your Linux devices.
 
Now try to run or restart ssh service :
```code
sudo service ssh start
sudo service ssh --full-restart
```
Also, you can check `ssh` status by some methods:

1. ```code sudo systemctl status ssh ```
2. ```code ps -aux | grep ssh ```

To set `ssh` when opening your devices :
```code
sudo systemctl enable ssh
```
# Visual Studio Connecting to Linux
To know more about configure, you can refer the website [readme file](readme/readme.html) or [Microsoft Doc](https://docs.microsoft.com/en-us/cpp/linux/connect-to-your-remote-linux-computer?view=msvc-160)

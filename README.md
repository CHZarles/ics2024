# ICS2024 Programming Assignment

This project is the programming assignment of the class ICS(Introduction to Computer System)
in Department of Computer Science and Technology, Nanjing University.

For the guide of this programming assignment,
refer to https://nju-projectn.github.io/ics-pa-gitbook/ics2024/

To initialize, run
```bash
git clone -b 2024  git@github.com:CHZarles/ics2024.git
git branch -m master
bash init.sh nemu
bash init.sh abstract-machine
```
See `init.sh` for more details.

The following subprojects/components are included. Some of them are not fully implemented.
* [NEMU](https://github.com/NJU-ProjectN/nemu)
* [Abstract-Machine](https://github.com/NJU-ProjectN/abstract-machine)
* [Nanos-lite](https://github.com/NJU-ProjectN/nanos-lite)
* [Navy-apps](https://github.com/NJU-ProjectN/navy-apps)

# pa1 record
```
# generate compile_command.json
pip install compiledb
cd ./nemu 
compiledb make 

ref:
    https://pypi.org/project/compiledb/

# install node.js for copilot chat 
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.0/install.sh | bash
export http_proxy=http://your_ip:7890
export https_proxy=https://your_ip:7890
export NVM_NODEJS_ORG_MIRROR=http://nodejs.org/dist
nvm list-remote
nvm install ...

ref:
    https://nodejs.org/en/download/package-manager

```

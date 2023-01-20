# setup
```bash
git clone https://github.com/Asyny/embeddedBridge.git

sudo apt update
sudo apt install libhidapi-dev libncurses5 python3-venv
```

# setup debug
```bash
sudo usermod -a -G dialout,plugdev $USER

cd /etc/udev/rules.d/
sudo wget https://raw.githubusercontent.com/raspberrypi/openocd/rp2040/contrib/60-openocd.rules
sudo systemctl reload udev && sudo udevadm trigger
```

# UF2
```bash
embeddedBridge/.pio/build/picoprobe/firmware.uf2
embeddedBridge/.pio/build/jlink/firmware.uf2
```

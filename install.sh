
echo "Installing Terminal basic Setting"

#sudo sed -e "/kr.archive.ubuntu.com/ c\mirror.kakao.com" /etc/apt/sources.list
#cat /etc/apt/sources.list
sudo apt update
sudo apt upgrade
sudo apt install net-tools gcc vim git build-essential sysstat gnome-tweak-tool ssh

echo "Install Complete"

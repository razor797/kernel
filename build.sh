rm /home/gustavo/zips/*.zip
/home/gustavo/kernel/build_cm.sh
/home/gustavo/kernel/makezip.sh
make -j3 zImage CONFIG_INITRAMFS_SOURCE="/home/gustavo/kernel/usr/cm_initramfs.twrp.list"
/home/gustavo/kernel/makezip.twrp.sh
make -j3 zImage CONFIG_INITRAMFS_SOURCE="/home/gustavo/kernel/usr/omni_initramfs.list"
/home/gustavo/kernel/makezip_omni.sh
make -j3 zImage CONFIG_INITRAMFS_SOURCE="/home/gustavo/kernel/usr/omni_initramfs.twrp.list"
/home/gustavo/kernel/makezip_omni.twrp.sh


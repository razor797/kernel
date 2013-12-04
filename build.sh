rm /home/gustavo/zips/*.zip
/home/gustavo/kernel/build_kernel.sh
/home/gustavo/kernel/makezip.sh
make -j3 zImage CONFIG_LOCALVERSION="-CM" | CONFIG_INITRAMFS_SOURCE="/home/gustavo/kernel/usr/cm_initramfs.twrp.list"
/home/gustavo/kernel/makezip.twrp.sh
make -j3 zImage CONFIG_LOCALVERSION="-Omni" | CONFIG_INITRAMFS_SOURCE="/home/gustavo/kernel/usr/omni_initramfs.list"
/home/gustavo/kernel/makezip_omni.sh
make -j3 zImage CONFIG_LOCALVERSION="-Omni" | CONFIG_INITRAMFS_SOURCE="/home/gustavo/kernel/usr/omni_initramfs.twrp.list"
/home/gustavo/kernel/makezip_omni.twrp.sh
make -j3 zImage CONFIG_LOCALVERSION="-Slimkat" | CONFIG_INITRAMFS_SOURCE="/home/gustavo/kernel/usr/slimkat_initramfs.list"
/home/gustavo/kernel/makezip_slimkat.sh
make -j3 zImage CONFIG_LOCALVERSION="-Slimkat" | CONFIG_INITRAMFS_SOURCE="/home/gustavo/kernel/usr/slimkat_initramfs.twrp.list"
/home/gustavo/kernel/makezip_slimkat.twrp.sh


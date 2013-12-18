cp /home/gustavo/kernel/arch/arm/configs/kernel_defconfig /home/gustavo/kernel/.config
make -j3
make -j3 zImage CONFIG_INITRAMFS_SOURCE="/home/gustavo/kernel/usr/cm-10.2_initramfs.list"
find -name '*.ko' -exec cp -av {} /home/gustavo/modules/ \;
chmod 644 /home/gustavo/modules/*
/home/gustavo/toolchain/bin/arm-linux-androideabi-strip --strip-unneeded /home/gustavo/modules/*
cp /home/gustavo/modules/* /home/gustavo/zip_cm-10.2/system/lib/modules/
cp /home/gustavo/kernel/arch/arm/boot/zImage /home/gustavo/zip_cm-10.2/

CURRENTDATE=$(date +"%d-%m")
cd /home/gustavo/zip_cm-10.2
rm *.zip
zip -r cm-10.2-kernel-$CURRENTDATE-CWM.zip ./
cp ./*.zip /home/gustavo/zips/cm-10.2-kernel-$CURRENTDATE-CWM.zip

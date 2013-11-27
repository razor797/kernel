find -name '*.ko' -exec cp -av {} /home/gustavo/modules/ \;
chmod 644 /home/gustavo/modules/*
/home/gustavo/toolchain/bin/arm-linux-androideabi-strip --strip-unneeded /home/gustavo/modules/*
cp /home/gustavo/modules/* /home/gustavo/zip_omni/system/lib/modules/
cp /home/gustavo/kernel/arch/arm/boot/zImage /home/gustavo/zip_omni/

CURRENTDATE=$(date +"%d-%m")
cd /home/gustavo/zip_omni
rm *.zip
zip -r omni-kernel-$CURRENTDATE.zip ./

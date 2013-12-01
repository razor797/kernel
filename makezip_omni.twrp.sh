find -name '*.ko' -exec cp -av {} /home/gustavo/modules/ \;
chmod 644 /home/gustavo/modules/*
/home/gustavo/toolchain/bin/arm-linux-androideabi-strip --strip-unneeded /home/gustavo/modules/*
cp /home/gustavo/modules/* /home/gustavo/zip_omni_twrp/system/lib/modules/
cp /home/gustavo/kernel/arch/arm/boot/zImage /home/gustavo/zip_omni_twrp/

CURRENTDATE=$(date +"%d-%m")
cd /home/gustavo/zip_omni_twrp
rm *.zip
zip -r omni-4.4-kernel-$CURRENTDATE-TWRP.zip ./
cp ./*.zip /home/gustavo/zips/omni-4.4-kernel-$CURRENTDATE-TWRP.zip

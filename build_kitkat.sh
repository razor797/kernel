#!/bin/bash

TARGET=$1
if [ "$TARGET" != "" ]; then
	echo "starting your build for $TARGET"
else
	echo ""
	echo "you need to define your device target!"
	echo "example: build_sammy.sh n7100"
	exit 1
fi

version=Devil-$TARGET-KITKAT-1.05_$(date +%Y%m%d)

if [ "$TARGET" = "i9300" ] ; then
CUSTOM_PATH=i9300

else
CUSTOM_PATH=note

fi


if [ -e boot.img ]; then
	rm boot.img
fi

if [ -e compile.log ]; then
	rm compile.log
fi

if [ -e ramdisk.cpio.gz ]; then
	rm ramdisk.cpio.gz
fi

# Set Default Path
KERNEL_PATH=$PWD

# Set toolchain and root filesystem path
if [ "$(whoami)" == "dominik" ]; then
	#TOOLCHAIN_PATH="/home/dominik/android/android_4.2/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin"
	#TOOLCHAIN_PATH="/home/dominik/android/android_4.2/prebuilt/linux-x86/toolchain/android-linaro-toolchain-4.8/bin"
	TOOLCHAIN_PATH="/home/dominik/android/android_4.2/prebuilt/linux-x86/toolchain/android-toolchain-eabi-4.8-2013.09/bin"

elif [ "$(whoami)" == "rollus" ]; then
        TOOLCHAIN_PATH="/home/rollus/android-toolchain-eabi/bin/"
fi
TOOLCHAIN="$TOOLCHAIN_PATH/arm-eabi-"
ROOTFS_PATH="$KERNEL_PATH/ramdisks/ramdisk-kitkat-$TARGET"
MODULES="$ROOTFS_PATH/system/lib/modules"

defconfig=kitkat_"$TARGET"_defconfig

export KBUILD_BUILD_VERSION="$version"
export KERNELDIR=$KERNEL_PATH

export USE_SEC_FIPS_MODE=true

# Set ramdisk files permissions
chmod 750 $ROOTFS_PATH/init*
chmod 644 $ROOTFS_PATH/ueventd*
chmod 644 $ROOTFS_PATH/lpm.rc
chmod 750 $ROOTFS_PATH/sbin/init*


if [ "$2" = "clean" ]; then
echo "Cleaning latest build"
make ARCH=arm CROSS_COMPILE=$TOOLCHAIN -j`grep 'processor' /proc/cpuinfo | wc -l` mrproper
fi
# Cleaning old kernel and modules
find -name '*.ko' -exec rm -rf {} \;
rm -rf $KERNEL_PATH/arch/arm/boot/zImage

# Making our .config
make $defconfig

make -j`grep 'processor' /proc/cpuinfo | wc -l` ARCH=arm CROSS_COMPILE=$TOOLCHAIN || exit -1
# Copying and stripping kernel modules
mkdir -p $MODULES
find -name '*.ko' -exec cp -av {} $MODULES/ \;
        for i in $MODULES/*; do $TOOLCHAIN_PATH/arm-eabi-strip --strip-unneeded $i;done;\


# Copy Kernel Image
rm -f $KERNEL_PATH/releasetools/$CUSTOM_PATH/tar/$KBUILD_BUILD_VERSION.tar
rm -f $KERNEL_PATH/releasetools/$CUSTOM_PATH/zip/$KBUILD_BUILD_VERSION.zip
cp -f $KERNEL_PATH/arch/arm/boot/zImage .


# Create ramdisk.cpio archive
cd $ROOTFS_PATH
find . | cpio -o -H newc | gzip > $KERNEL_PATH/ramdisk.cpio.gz
RAMDISK=ramdisk.cpio.gz
#find . | cpio -o -H newc > $KERNEL_PATH/ramdisk.cpio
cd $KERNEL_PATH

# Make boot.img
./mkbootimg --kernel zImage --ramdisk $RAMDISK --board smdk4x12 --base 0x10000000 --pagesize 2048 --ramdiskaddr 0x11000000 -o $KERNEL_PATH/boot.img

# Copy boot.img
cp boot.img $KERNEL_PATH/releasetools/$CUSTOM_PATH/zip

cp boot.img $KERNEL_PATH/releasetools/$CUSTOM_PATH/tar

# Creating flashable zip and tar
cd $KERNEL_PATH
cd releasetools/$CUSTOM_PATH/zip
zip -0 -r $KBUILD_BUILD_VERSION.zip *
mkdir -p $KERNEL_PATH/release
mv *.zip $KERNEL_PATH/release
cd ..
cd tar
tar cf $KBUILD_BUILD_VERSION.tar boot.img && ls -lh $KBUILD_BUILD_VERSION.tar
mv *.tar $KERNEL_PATH/release

# Cleanup
cd $KERNEL_PATH
rm $KERNEL_PATH/releasetools/$CUSTOM_PATH/zip/boot.img
rm $KERNEL_PATH/releasetools/$CUSTOM_PATH/tar/boot.img
rm $KERNEL_PATH/zImage

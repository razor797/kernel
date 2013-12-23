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

if [ "$TARGET" = "i9300" ] ; then
CUSTOM_PATH=i9300
MODE=DUAL
elif [ "$TARGET" = "i9100" ] ; then
CUSTOM_PATH=i9100
MODE=CM
else
CUSTOM_PATH=note
MODE=DUAL	
fi

displayversion=Devil2-1.0.6
version=$displayversion-$TARGET-$MODE-$(date +%Y%m%d)

if [ -e boot.img ]; then
	rm boot.img
fi

if [ -e compile.log ]; then
	rm compile.log
fi

if [ -e ramdisk.cpio ]; then
	rm ramdisk.cpio
fi

# Set Default Path
KERNEL_PATH=$PWD

# Set toolchain and root filesystem path
if [ "$(whoami)" == "dominik" ]; then
	#TOOLCHAIN_PATH="/home/dominik/android/android_4.2/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin"
	#TOOLCHAIN_PATH="/home/dominik/android/android_4.2/prebuilt/linux-x86/toolchain/android-toolchain-eabi-4.8-2013.07/bin"
	TOOLCHAIN_PATH="/home/dominik/android/android_4.2/prebuilt/linux-x86/toolchain/android-toolchain-eabi-4.8-2013.09/bin"
	#TOOLCHAIN_PATH="/home/dominik/android/android_4.2/prebuilts/gcc/linux-x86/arm/arm-eabi-4.7.2/bin"
elif [ "$(whoami)" == "rollus" ]; then
	TOOLCHAIN_PATH="/home/rollus/android-toolchain-eabi/bin/"
fi
TOOLCHAIN="$TOOLCHAIN_PATH/arm-eabi-"
ROOTFS_PATH="$KERNEL_PATH/ramdisks/$TARGET-combo"
MODULES="$KERNEL_PATH/ramdisks/modules"

defconfig=cyanogenmod_"$TARGET"_defconfig

export KBUILD_BUILD_VERSION="$displayversion"
export KERNELDIR=$KERNEL_PATH

export USE_SEC_FIPS_MODE=true

# Set ramdisk files permissions
chmod 750 $ROOTFS_PATH/roms/*/init*
chmod 644 $ROOTFS_PATH/roms/*/ueventd*
chmod 644 $ROOTFS_PATH/roms/*/lpm.rc
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
if [ "$TARGET" != "i9100" ] ; then
mkdir -p $MODULES/lib/modules
find -name '*.ko' -exec cp -av {} $MODULES/lib/modules/ \;
        for i in $MODULES/lib/modules/*; do $TOOLCHAIN_PATH/arm-eabi-strip --strip-unneeded $i;done;\
else
MODULES=releasetools/$CUSTOM_PATH/zip/system/lib/modules
mkdir -p $MODULES
find -name '*.ko' -exec cp -av {} $MODULES \;
        for i in $MODULES/*; do $TOOLCHAIN_PATH/arm-eabi-strip --strip-unneeded $i;done;\
fi

# Copy Kernel Image
rm -f $KERNEL_PATH/releasetools/$CUSTOM_PATH/tar/$version.tar
rm -f $KERNEL_PATH/releasetools/$CUSTOM_PATH/zip/$version.zip
cp -f $KERNEL_PATH/arch/arm/boot/zImage .

if [ "$TARGET" != "i9100" ] ; then
# Create ramdisk.cpio archive
cd $MODULES
find . | cpio -o -H newc > $KERNEL_PATH/ramdisk.cpio
cd $KERNEL_PATH

# Make boot.img
./mkbootimg --kernel zImage --ramdisk ramdisk.cpio --board smdk4x12 --base 0x10000000 --pagesize 2048 --ramdiskaddr 0x11000000 -o $KERNEL_PATH/boot.img

# Copy boot.img
cp boot.img $KERNEL_PATH/releasetools/$CUSTOM_PATH/zip
cp boot.img $KERNEL_PATH/releasetools/$CUSTOM_PATH/tar
else
cp zImage $KERNEL_PATH/releasetools/$CUSTOM_PATH/zip
cp zImage $KERNEL_PATH/releasetools/$CUSTOM_PATH/tar
fi

# Creating flashable zip and tar
cd $KERNEL_PATH
cd releasetools/$CUSTOM_PATH/zip
zip -0 -r $version.zip *
mkdir -p $KERNEL_PATH/release
mv *.zip $KERNEL_PATH/release
cd ..

if [ "$TARGET" != "i9100" ] ; then
cd tar
tar cf $version.tar boot.img && ls -lh $version.tar
mv *.tar $KERNEL_PATH/release
fi

# Cleanup
cd $KERNEL_PATH
rm $KERNEL_PATH/releasetools/$CUSTOM_PATH/zip/boot.img
rm $KERNEL_PATH/releasetools/$CUSTOM_PATH/tar/boot.img
rm $KERNEL_PATH/zImage

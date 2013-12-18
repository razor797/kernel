#!/system/bin/sh
# Logging
#/sbin/busybox cp /data/user.log /data/user.log.bak
#/sbin/busybox rm /data/user.log
#exec >>/data/user.log
#exec 2>&1

#!/sbin/busybox sh

mkdir /data/.siyah
chmod 0777 /data/.siyah

. /res/customconfig/customconfig-helper

ccxmlsum=`md5sum /res/customconfig/customconfig.xml | awk '{print $1}'`
if [ "a${ccxmlsum}" != "a`cat /data/.siyah/.ccxmlsum`" ];
then
  rm -f /data/.siyah/*.profile
  echo ${ccxmlsum} > /data/.siyah/.ccxmlsum
fi
[ ! -f /data/.siyah/default.profile ] && cp /res/customconfig/default.profile /data/.siyah

read_defaults
read_config

#cpu undervolting
echo "${cpu_undervolting}" > /sys/devices/system/cpu/cpu0/cpufreq/vdd_levels

# disable debugging on some modules
if [ "$logger" == "off" ];then
  rm -rf /dev/log
  echo 0 > /sys/module/ump/parameters/ump_debug_level
  echo 0 > /sys/module/mali/parameters/mali_debug_level
  echo 0 > /sys/module/kernel/parameters/initcall_debug
  echo 0 > /sys//module/lowmemorykiller/parameters/debug_level
  echo 0 > /sys/module/earlysuspend/parameters/debug_mask
  echo 0 > /sys/module/alarm/parameters/debug_mask
  echo 0 > /sys/module/alarm_dev/parameters/debug_mask
  echo 0 > /sys/module/binder/parameters/debug_mask
  echo 0 > /sys/module/xt_qtaguid/parameters/debug_mask
fi

##### EFS Backup #####
(
# make sure that sdcard is mounted
sleep 30
/sbin/busybox sh /sbin/ext/efs-backup.sh
) &

#apply last soundgasm level on boot
/res/uci.sh soundgasm_hp $soundgasm_hp

# apply STweaks defaults
export CONFIG_BOOTING=1
/res/uci.sh apply
export CONFIG_BOOTING=

#usb mode
/res/customconfig/actions/usb-mode ${usb_mode}

### Disables Built In Error Reporting
setprop profiler.force_disable_err_rpt 1
setprop profiler.force_disable_ulog 1

mount -o remount,rw /system
# gpu watch
cp /res/gpuwatch /system/bin/gpuwatch
chown root.system /system/bin/gpuwatch
chmod 0755 /system/bin/gpuwatch

cp /res/gpucat /system/bin/gpucat
chown root.system /system/bin/gpucat
chmod 0755 /system/bin/gpucat
mount -o remount,ro /system

# install lights lib needed by BLN
mount -o remount,rw /system
rm /system/lib/hw/lights.exynos4.so
cp /res/lights.exynos4.so /system/lib/hw/lights.exynos4.so
chown root.root /system/lib/hw/lights.exynos4.so
chmod 0664 /system/lib/hw/lights.exynos4.so
mount -o remount,ro /system

# google dns
setprop net.dns1 8.8.8.8
setprop net.dns2 8.8.4.4

# make sure dynamic fsync is active
echo "1" > /sys/kernel/dyn_fsync/Dyn_fsync_active

# enable JIT compiler for packet filters

echo "1" > /proc/sys/net/core/bpf_jit_enable

# disable hotplug on sleep for zzmoove

echo "1" > /sys/devices/system/cpu/cpufreq/zzmoove/disable_hotplug_sleep

# cgroup timer slack
mount -t cgroup -o timer_slack none /sys/fs/cgroup
ls /sys/fs/cgroup/timer_slack.*
echo "50000" > /sys/fs/cgroup/timer_slack.min_slack_ns
echo "100000" > /sys/fs/cgroup/timer_slack.effective_slack_ns

##### init scripts #####
/system/bin/sh sh /sbin/ext/run-init-scripts.sh

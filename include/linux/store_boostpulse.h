/*
 * Boostpulse support for ondemand based governors, based on Steve Kondik's solution for ondemand,
 * modified to work on multiple governors by Gustavo Santos, 2013.
 */

/*
 * BOOST CONTROL:
 * 1 = ONDEMAND
 * 2 = PEGASUSQ
 * */

#if (BOOST_CONTROL == 1)
u64 freq_boosted_time_ondemand;
static ssize_t store_boosttime(struct kobject *kobj, struct attribute *attr,
				const char *buf, size_t count)
{
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

	dbs_tuners_ins.freq_boost_time = input;
	return count;
}

static ssize_t store_boostpulse(struct kobject *kobj, struct attribute *attr,
        const char *buf, size_t count)
{
  int ret;
  unsigned int input;

  ret = sscanf(buf, "%u", &input);
  if (ret < 0)
    return ret;

  if (input > 1 && input <= MAX_FREQ_BOOST_TIME)
    dbs_tuners_ins.freq_boost_time = input;
  else
    dbs_tuners_ins.freq_boost_time = DEFAULT_FREQ_BOOST_TIME;

  dbs_tuners_ins.boosted = 1;
  freq_boosted_time_ondemand = ktime_to_us(ktime_get());
  return count;
}

static ssize_t store_boostfreq(struct kobject *a, struct attribute *b,
           const char *buf, size_t count)
{
  unsigned int input;
  int ret;
  ret = sscanf(buf, "%u", &input);
  if (ret != 1)
    return -EINVAL;
  dbs_tuners_ins.boostfreq = input;
  return count;
}
#endif

#if (BOOST_CONTROL == 2)
u64 freq_boosted_time_pegasusq;
static ssize_t store_boosttime(struct kobject *kobj, struct attribute *attr,
				const char *buf, size_t count)
{
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

	dbs_tuners_ins.freq_boost_time = input;
	return count;
}
static ssize_t store_boostpulse(struct kobject *kobj, struct attribute *attr,
        const char *buf, size_t count)
{
  int ret;
  unsigned int input;

  ret = sscanf(buf, "%u", &input);
  if (ret < 0)
    return ret;

  if (input > 1 && input <= MAX_FREQ_BOOST_TIME)
    dbs_tuners_ins.freq_boost_time = input;
  else
    dbs_tuners_ins.freq_boost_time = DEFAULT_FREQ_BOOST_TIME;

  dbs_tuners_ins.boosted = 1;
  freq_boosted_time_pegasusq = ktime_to_us(ktime_get());
  return count;
}

static ssize_t store_boostfreq(struct kobject *a, struct attribute *b,
           const char *buf, size_t count)
{
  unsigned int input;
  int ret;
  ret = sscanf(buf, "%u", &input);
  if (ret != 1)
    return -EINVAL;
  dbs_tuners_ins.boostfreq = input;
  return count;
}
#endif



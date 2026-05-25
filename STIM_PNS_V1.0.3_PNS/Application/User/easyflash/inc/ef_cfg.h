#ifndef EF_CFG_H_
#define EF_CFG_H_

#include <stdint.h>
#include <stddef.h>

/* 使用 ENV 功能，从 V4.0 开始默认采用 NG（新一代）模式 */
#define EF_USING_ENV

#ifdef EF_USING_ENV
/* 当当前 ENV 版本号改变时，自动将 ENV 更新到最新的默认值。 */
#define EF_ENV_AUTO_UPDATE

/**
 * ENV 版本号定义。
 * 当您的固件向 default_env_set 添加新的 ENV 时，请修改此值。
 */
#define EF_ENV_VER_NUM         0   /* 注意：您必须为其定义一个值，例如 0 */

/* MCU 字节序配置，默认是小端模式。 */
/* #define EF_BIG_ENDIAN  */

#endif /* EF_USING_ENV */

/* 使用 IAP 功能 */
/* #define EF_USING_IAP */

/* 使用保存日志功能 */
#define EF_USING_LOG

/* 闪存擦除的最小大小。可以是一个闪存扇区的大小。 */
#define EF_ERASE_MIN_SIZE      (0x1000*16)   /* 注意：64k 您必须为其定义一个值 */

/* 闪存写入粒度，单位：位
 * 仅支持 1（NOR 闪存）/ 8（stm32f4）/ 32（stm32f1） */
#define EF_WRITE_GRAN           8  /* 注意：您必须为其定义一个值 */

/* read_env 和 continue_ff_addr 函数使用的缓冲区大小 */
#define EF_READ_BUF_SIZE        512     /* 默认 32，较大的数值可以提高首次分配 ENV 的速度，但需要更多的栈空间 */

/* 备份区起始地址 */
#define EF_START_ADDR           0  /* 注意：您必须为其定义一个值 */

/* ENV 区域大小。它至少需要一个空扇区用于 GC。因此其定义必须大于或等于 2 个闪存扇区大小。 */
#define ENV_AREA_SIZE          (EF_ERASE_MIN_SIZE * 2)   /* 注意：如果您使用 ENV，必须为其定义一个值 */

/* 日志保存区大小 */
#define LOG_AREA_SIZE          (4*1024*1024 - ENV_AREA_SIZE)   /* 注意：如果您使用日志功能，必须为其定义一个值 */

#endif /* EF_CFG_H_ */

/*
 *
 * This all Backup Area Flash storage index. All used flash area configure is under here.
 * |----------------------------|   Storage Size
 * | Environment variables area |   ENV area size @see ENV_AREA_SIZE
 * |----------------------------|
 * |      Saved log area        |   Log area size @see LOG_AREA_SIZE
 * |----------------------------|
 * |(IAP)Downloaded application |   IAP already downloaded application, unfixed size
 * |----------------------------|
 *
 * @note all area sizes must be aligned with EF_ERASE_MIN_SIZE
 *
 * The EasyFlash add the NG (Next Generation) mode start from V4.0. All old mode before V4.0, called LEGACY mode.
 *
 * - NG (Next Generation) mode is default mode from V4.0. It's easy to settings, only defined the ENV_AREA_SIZE.
 * - The LEGACY mode has been DEPRECATED. It is NOT RECOMMENDED to continue using.
 *   Beacuse it will use ram to buffer the ENV and spend more flash erase times.
 *   If you want use it please using the V3.X version.
 */

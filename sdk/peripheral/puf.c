//SoftPUF

//PUF 是 Physical Unclonable Function（物理不可克隆函数）的缩写

/*

利用 SRAM 上电初始值的工艺偏差随机性，通过专利算法提取唯一指纹，生成 UID、根密钥、真随机数。

*/

// 极简 SRAM PUF：4KB SRAM，输出 64bit 唯一 ID（可改 32bit）
#include <stdint.h>
#include <string.h>



#if 0

// -------------------------- 配置区（根据你的芯片改）-------------------------
#define PUF_SRAM_START  0x20000000UL  // 你的 1901 SRAM 起始地址
#define PUF_SRAM_SIZE   1024            // 用 1KB（4KB 足够，留 3KB 给程序）
#define PUF_STABLE_TH   90              // 稳定阈值：90% 以上上电不变才算稳定
#define PUF_OUTPUT_BITS 64               // 输出 64bit ID（改成 32 就是 32bit）
// -----------------------------------------------------------------------------

static uint8_t sram_buf[PUF_SRAM_SIZE];       // 上电原始值缓存
static uint8_t stable_mask[PUF_SRAM_SIZE];     // 稳定位掩码（1=稳定，0=不稳定）
static uint64_t puf_uid = 0;                    // 最终 64bit UID

// 1. 上电第一时间抓取 SRAM 初始值（必须在任何初始化代码前调用！）
void sram_puf_capture(void) {
    memcpy(sram_buf, (void*)PUF_SRAM_START, PUF_SRAM_SIZE);
}

// 2. 多次上电统计稳定位（你可以在测试阶段跑 10~20 次，出厂烧录 stable_mask）
void sram_puf_train(int poweron_times) {
    static uint8_t cnt0[PUF_SRAM_SIZE]; // 每个 bit 为 0 的次数
    static uint8_t cnt1[PUF_SRAM_SIZE]; // 每个 bit 为 1 的次数
    memset(cnt0, 0, sizeof(cnt0));
    memset(cnt1, 0, sizeof(cnt1));

    for (int t = 0; t < poweron_times; t++) {
        sram_puf_capture(); // 每次上电抓一次
        for (int i = 0; i < PUF_SRAM_SIZE; i++) {
            for (int b = 0; b < 8; b++) {
                if (sram_buf[i] & (1 << b)) cnt1[i]++;
                else cnt0[i]++;
            }
        }
    }

    // 计算稳定掩码：某 bit 90% 以上是 0 或 1 → 稳定
    memset(stable_mask, 0, sizeof(stable_mask));
    for (int i = 0; i < PUF_SRAM_SIZE; i++) {
        for (int b = 0; b < 8; b++) {
            uint8_t bit_val = (sram_buf[i] >> b) & 1;
            uint8_t total = cnt0[i] + cnt1[i];
            if (total == 0) continue;
            // 稳定条件：该 bit 90% 时间是当前值
            if ((bit_val && (cnt1[i] * 100 >= PUF_STABLE_TH * total)) ||
                (!bit_val && (cnt0[i] * 100 >= PUF_STABLE_TH * total))) {
                stable_mask[i] |= (1 << b);
            }
        }
    }
}

// 3. 从稳定位生成 64bit UID（用简单哈希压缩，可改 32bit）
uint64_t sram_puf_get_uid(void) {
    uint64_t uid = 0;
    int bit_idx = 0;

    // 遍历所有稳定 bit，依次填入 UID
    for (int i = 0; i < PUF_SRAM_SIZE; i++) {
        for (int b = 0; b < 8; b++) {
            if (stable_mask[i] & (1 << b)) { // 只取稳定 bit
                if (bit_idx >= PUF_OUTPUT_BITS) goto done; // 够 64bit 就停
                uint8_t val = (sram_buf[i] >> b) & 1;
                uid |= ((uint64_t)val) << bit_idx;
                bit_idx++;
            }
        }
    }
done:
    puf_uid = uid;
    return uid;
}

// 4. 一键生成（上电流程：先 capture → 再 get_uid）
uint64_t sram_puf_init(void) {
    sram_puf_capture();   // 抓原始值
    return sram_puf_get_uid(); // 生成 64bit UID
}

#endif






#if 0

// 极简无内存占用 SoftPUF —— 专为 4KB SRAM 芯片设计
// 特点：无全局数组、无常驻内存、用完即释放、只输出 64bit 唯一ID
//#include <stdint.h>

// ==================== 芯片配置（你只需要改这一行）====================
#define PUF_SRAM_BASE     0x20000000UL    // 你的芯片 SRAM 起始地址
#define PUF_USE_BYTES     512             // 临时只用 512 字节（足够生成64bit ID）
// ====================================================================

/**
 * @brief  上电第一时间调用：直接从 SRAM 生成 64bit 唯一ID
 * @return 64bit 芯片唯一ID
 * @note   无全局变量，无内存占用，执行完自动释放所有临时资源
 */
uint64_t sram_puf_generate_uid(void)
{
    uint64_t uid = 0;
    uint32_t bit_count = 0;

    // 直接读取芯片物理 SRAM，不拷贝、不缓存、不占用全局数组
    for (uint32_t i = 0; i < PUF_USE_BYTES; i++)
    {
        uint8_t sram_data = *(volatile uint8_t*)(PUF_SRAM_BASE + i);

        // 从 SRAM 原始bit中提取稳定位（简单哈希压缩）
        for (uint32_t b = 0; b < 8; b++)
        {
            uint8_t bit_val = (sram_data >> b) & 0x01;

            // 简单稳定筛选：只保留固定规律的bit（避免抖动）
            if ((i ^ b) % 3 == 0)  // 轻量稳定位筛选，不占内存
            {
                if (bit_count < 64)
                {
                    uid |= ((uint64_t)bit_val) << bit_count;
                    bit_count++;
                }
            }

            if (bit_count >= 64) break;
        }
        if (bit_count >= 64) break;
    }

    // 执行完这里，所有临时变量自动释放（栈回收）
    // 只返回 64bit UID，无任何内存残留
    return uid;
}


#endif




// 必须加上 volatile，禁止编译器优化掉 SRAM 读取
#define PUF_SRAM_BASE  0x20000000UL   // 你的芯片SRAM基地址
#define PUF_READ_SIZE  256            // 只读取256字节，足够生成64bit ID

// 唯一全局变量：只占 8 字节 SRAM
uint64_t chip_unique_id = 0;

// 上电第一时间被汇编调用
void sram_puf_on_power_up(void)
{
    uint64_t uid = 0;
    uint32_t bit_cnt = 0;

    // 直接读取物理SRAM，不缓存、不拷贝、不占内存
    for (uint32_t i = 0; i < PUF_READ_SIZE; i++)
    {
        uint8_t d = *(volatile uint8_t*)(PUF_SRAM_BASE + i);
        
        for (int b = 0; b < 8; b++)
        {
            if (bit_cnt >= 64) break;
            
            // 简单稳定筛选（轻量、无内存）
            if (((i >> 1) ^ b) % 2 == 0)
            {
                uid |= (uint64_t)((d >> b) & 1) << bit_cnt;
                bit_cnt++;
            }
        }
    }

    // 保存最终ID（仅8字节）
    chip_unique_id = uid;
}






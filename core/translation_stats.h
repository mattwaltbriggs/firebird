#ifndef _H_TRANSLATION_STATS
#define _H_TRANSLATION_STATS

#include <stdint.h>

#define STAT_UNIMP_KEYS 4096u

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TRANSLATION_STATS
extern uint64_t stat_arm_translated_insns __asm__("stat_arm_translated_insns");
extern uint64_t stat_arm_interp_insns     __asm__("stat_arm_interp_insns");
extern uint64_t stat_thumb_interp_insns   __asm__("stat_thumb_interp_insns");
extern uint64_t stat_translation_entries  __asm__("stat_translation_entries");
extern uint64_t stat_unimpl_total         __asm__("stat_unimpl_total");
extern uint64_t stat_unimpl_by_key[STAT_UNIMP_KEYS] __asm__("stat_unimpl_by_key");

void stat_record_unimpl(uint32_t raw, uint32_t pc);
void stat_dump(void);

#define STAT_INC(x)             (++(x))
#define STAT_ADD(x, n)          ((x) += (uint64_t)(n))
#define STAT_RECORD_UNIMPL(r,p) stat_record_unimpl((r), (p))
#define STAT_DUMP()             stat_dump()
#else
#define STAT_INC(x)             ((void)0)
#define STAT_ADD(x, n)          ((void)0)
#define STAT_RECORD_UNIMPL(r,p) ((void)0)
#define STAT_DUMP()             ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif

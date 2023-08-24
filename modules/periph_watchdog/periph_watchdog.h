#ifndef __PERIPH_WATCHDOG_H
#define __PERIPH_WATCHDOG_H

/********************  Bit definition for Periph_State register  *************/

#define PERIPH_STATE_1_Pos      (0U)
#define PERIPH_STATE_2_Pos      (1U)
#define PERIPH_STATE_3_Pos      (2U)
#define PERIPH_STATE_4_Pos      (3U)
#define PERIPH_STATE_5_Pos      (4U)
#define PERIPH_STATE_6_Pos      (5U)
#define PERIPH_STATE_7_Pos      (6U)
#define PERIPH_STATE_8_Pos      (7U)
#define PERIPH_STATE_9_Pos      (8U)
#define PERIPH_STATE_10_Pos     (9U)
#define PERIPH_STATE_11_Pos     (10U)
#define PERIPH_STATE_12_Pos     (11U)
#define PERIPH_STATE_13_Pos     (12U)
#define PERIPH_STATE_14_Pos     (13U)
#define PERIPH_STATE_15_Pos     (14U)
#define PERIPH_STATE_16_Pos     (15U)
#define PERIPH_STATE_17_Pos     (16U)
#define PERIPH_STATE_18_Pos     (17U)
#define PERIPH_STATE_19_Pos     (18U)
#define PERIPH_STATE_20_Pos     (19U)
#define PERIPH_STATE_21_Pos     (20U)
#define PERIPH_STATE_22_Pos     (21U)
#define PERIPH_STATE_23_Pos     (22U)
#define PERIPH_STATE_24_Pos     (23U)
#define PERIPH_STATE_25_Pos     (24U)
#define PERIPH_STATE_26_Pos     (25U)
#define PERIPH_STATE_27_Pos     (26U)
#define PERIPH_STATE_28_Pos     (27U)
#define PERIPH_STATE_29_Pos     (28U)
#define PERIPH_STATE_30_Pos     (29U)
#define PERIPH_STATE_31_Pos     (30U)
#define PERIPH_STATE_32_Pos     (31U)

#define PERIPH_STATE_1          (0x1U << PERIPH_STATE_1_Pos)
#define PERIPH_STATE_2          (0x1U << PERIPH_STATE_2_Pos)
#define PERIPH_STATE_3          (0x1U << PERIPH_STATE_3_Pos)
#define PERIPH_STATE_4          (0x1U << PERIPH_STATE_4_Pos)
#define PERIPH_STATE_5          (0x1U << PERIPH_STATE_5_Pos)
#define PERIPH_STATE_6          (0x1U << PERIPH_STATE_6_Pos)
#define PERIPH_STATE_7          (0x1U << PERIPH_STATE_7_Pos)
#define PERIPH_STATE_8          (0x1U << PERIPH_STATE_8_Pos)
#define PERIPH_STATE_9          (0x1U << PERIPH_STATE_9_Pos)
#define PERIPH_STATE_10         (0x1U << PERIPH_STATE_10_Pos)
#define PERIPH_STATE_11         (0x1U << PERIPH_STATE_11_Pos)
#define PERIPH_STATE_12         (0x1U << PERIPH_STATE_12_Pos)
#define PERIPH_STATE_13         (0x1U << PERIPH_STATE_13_Pos)
#define PERIPH_STATE_14         (0x1U << PERIPH_STATE_14_Pos)
#define PERIPH_STATE_15         (0x1U << PERIPH_STATE_15_Pos))
#define PERIPH_STATE_16         (0x1U << PERIPH_STATE_16_Pos)
#define PERIPH_STATE_17         (0x1U << PERIPH_STATE_17_Pos)
#define PERIPH_STATE_18         (0x1U << PERIPH_STATE_18_Pos)
#define PERIPH_STATE_19         (0x1U << PERIPH_STATE_19_Pos)
#define PERIPH_STATE_20         (0x1U << PERIPH_STATE_20_Pos)
#define PERIPH_STATE_21         (0x1U << PERIPH_STATE_21_Pos)
#define PERIPH_STATE_22         (0x1U << PERIPH_STATE_22_Pos)
#define PERIPH_STATE_23         (0x1U << PERIPH_STATE_23_Pos)
#define PERIPH_STATE_24         (0x1U << PERIPH_STATE_24_Pos)
#define PERIPH_STATE_25         (0x1U << PERIPH_STATE_25_Pos)
#define PERIPH_STATE_26         (0x1U << PERIPH_STATE_26_Pos)
#define PERIPH_STATE_27         (0x1U << PERIPH_STATE_27_Pos)
#define PERIPH_STATE_28         (0x1U << PERIPH_STATE_28_Pos)
#define PERIPH_STATE_29         (0x1U << PERIPH_STATE_29_Pos)
#define PERIPH_STATE_30         (0x1U << PERIPH_STATE_30_Pos)
#define PERIPH_STATE_31         (0x1U << PERIPH_STATE_31_Pos)
#define PERIPH_STATE_32         (0x1U << PERIPH_STATE_32_Pos)

#define PS_ETH_STATE_Pos        PERIPH_STATE_1_Pos
#define PS_ETH_STATE            (0x1U << PS_ETH_STATE_Pos)      /*!< 0x00000001 */
#define PS_RS485_STATE_Pos      PERIPH_STATE_2_Pos
#define PS_RS485_STATE          (0x1U << PS_RS485_STATE_Pos)    /*!< 0x00000002 */
#define PS_ADC_STATE_Pos        PERIPH_STATE_3_Pos
#define PS_ADC_STATE            (0x1U << PS_ADC_STATE_Pos)      /*!< 0x00000004 */
#define PS_EEPROM_STATE_Pos     PERIPH_STATE_4_Pos
#define PS_EEPROM_STATE         (0x1U << PS_EEPROM_STATE_Pos)   /*!< 0x00000008 */

void vPERIPH_WATCHDOG(void *params);

#endif // __PERIPH_WATCHDOG_H
#pragma once

// PRISM (Programmable Reconfigurable Indexed State Machine) peripheral driver
// for TinyQV.  The PRISM is user peripheral 8 on the ttsky26a-prism-qv design.
//
//                        /\.
//                       /  \.
//                   ..-/----\-..
//               --''  /      \  ''--
//                    /________\.
//
// The PRISM executes a 16-state programmable Mealy FSM ("chroma") loaded at
// runtime into a latch based State Information Table (SIT).  Each state is a
// 45-bit State Execution Word (STEW).  The SIT is not randomly addressable:
// STEWs are shifted through the 16 config stages like a shift register.  A
// STEW is written by storing bits [44:32] to CFG_MSW and then bits [31:0] to
// CFG_LSW; the CFG_LSW write triggers the shift of the new word into stage 0
// (moving every other stage forward one position).  Stage 15 (the oldest
// word) is readable back at CFG_MSW/CFG_LSW, so a previously loaded design
// can be validated as it is shifted out while a new design is shifted in.
//
// Chroma arrays are in load order as produced by the PRISM Yosys backend:
// chroma[0]/chroma[1] = MSW/LSW of the first word written, which after a
// complete 16 word load resides in state 15; the last pair is state 0.

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

#define PRISM_PERIPHERAL_NUM    8
#define PRISM_BASE_ADDRESS      ((uintptr_t)PERI_BASE_ADDRESS(PRISM_PERIPHERAL_NUM))

#define PRISM_NUM_STATES        8
#define PRISM_CHROMA_WORDS      (PRISM_NUM_STATES * 2)
#define PRISM_STEW_MSW_MASK     0xFFFu     // STEW bits [43:32]

// ==========================================================================
// Register map (byte offsets from PRISM_BASE_ADDRESS)
// ==========================================================================
#define PRISM_REG_CTRL          0x00    // R/W  Control register (latch based)
#define PRISM_REG_INT_CLR       0x03    // W    Byte write 0x80 clears interrupt
#define PRISM_REG_DBG_CTRL      0x04    // R/W  Debugger control (latch based)
#define PRISM_REG_DBG_STAT      0x0C    // R    Debugger / FSM status
#define PRISM_REG_CFG_LSW       0x10    // W    STEW[31:0], write starts shift
                                        // R    Config stage 7 STEW[31:0]
#define PRISM_REG_CFG_MSW       0x14    // W    STEW[43:32], write before LSW
                                        // R    Config stage 7 STEW[43:32]
#define PRISM_REG_COMM_DATA     0x18    // R/W  8-bit comm register (byte access)
#define PRISM_REG_FIFO_DATA     0x19    // R    Byte read pops 3-byte RX FIFO
#define PRISM_REG_FIFO_STAT     0x1A    // R    FIFO status (byte access)
#define PRISM_REG_HOST_IN       0x1B    // R/W  host_in[1:0] (byte access)
#define PRISM_REG_PRELOAD       0x20    // R/W  24-bit count1 preload / SR load
#define PRISM_REG_HOST_TOGGLE   0x21    // W    Byte write toggles host_in[0],
                                        //      clears interrupt AND writes
                                        //      count2_compare with the byte
#define PRISM_REG_COUNT_VAL     0x24    // R    [23:0]=count1, [31:24]=count2
#define PRISM_REG_COMPARE       0x28    // R/W  8-bit count2 compare (byte access)
#define PRISM_REG_ID            0x30    // R    PRISM architecture ID word
#define PRISM_REG_DECISION      0x34    // R    Decision tree (LUT ins + match)
#define PRISM_REG_OUT_DATA      0x38    // R    [10:0] live FSM output vector
#define PRISM_REG_IN_DATA       0x3C    // R    [15:0] live FSM input vector

// ==========================================================================
// CTRL register bits (write layout; reads return the same bits plus status)
// ==========================================================================
#define PRISM_CTRL_SHIFT_IN_SEL(n)  ((uint32_t)((n) & 3u) << 0)  // shift in from ui_in[n]
#define PRISM_CTRL_SHIFT_OUT_SEL(n) ((uint32_t)((n) & 3u) << 2)  // 0=off, 1..3 => uo_out[4+n]
#define PRISM_CTRL_COND_OUT_SEL(n)  ((uint32_t)((n) & 3u) << 4)  // 0=off, 1..3 => uo_out[1+n]
#define PRISM_CTRL_LOAD4            (1u << 6)   // out[4] loads comm from preload byte
#define PRISM_CTRL_LATCH_IN_OUT     (1u << 7)   // in[13:12] = registered out[6]/out[1]
#define PRISM_CTRL_SHIFT_EN         (1u << 8)   // enable shift operations
#define PRISM_CTRL_SHIFT_DIR        (1u << 9)   // 0=left (MSB first), 1=right
#define PRISM_CTRL_SHIFT_24_EN      (1u << 10)  // shift 24-bit count1, else 8-bit comm
#define PRISM_CTRL_FIFO_24          (1u << 11)  // count1 acts as 3-byte RX FIFO
#define PRISM_CTRL_COUNT2_DEC       (1u << 12)  // enable count2 decrement via out[5]
#define PRISM_CTRL_LATCH_EN         (1u << 13)  // 'latch3': out[2] latches inputs;
                                                //   with FIFO_24, uo_out[1]=fifo_full
#define PRISM_CTRL_ENABLE           (1u << 30)  // PRISM execution enable
#define PRISM_CTRL_INTERRUPT        (1u << 31)  // (read only) interrupt pending
#define PRISM_CTRL_CFG_MASK         0x3FFFu     // all configuration bits

// CTRL read-only status fields
#define PRISM_CTRL_GET_OUT_PINS(v)  (((v) >> 16) & 0x7Fu)   // latched uo_out[7:1]
#define PRISM_CTRL_UI_IN7           (1u << 23)              // direct read of ui_in[7]

// ==========================================================================
// DBG_CTRL register bits.  Bits [11:0] are persistent (latch based);
// NEW_SI/NEW_SI_VAL act only during the write itself.  The debugger only
// operates while the PRISM is enabled.  Any halt raises the user interrupt.
// ==========================================================================
#define PRISM_DBG_HALT_REQ          (1u << 0)   // set to halt; 1->0 edge resumes
#define PRISM_DBG_STEP              (1u << 1)   // 0->1 edge single steps (halted)
#define PRISM_DBG_BP0_EN            (1u << 2)
#define PRISM_DBG_BP1_EN            (1u << 3)
#define PRISM_DBG_BP0_SI(n)         ((uint32_t)((n) & 0x7u) << 4)
#define PRISM_DBG_BP1_SI(n)         ((uint32_t)((n) & 0x7u) << 7)
#define PRISM_DBG_NEW_SI            (1u << 10)  // load NEW_SI_VAL as state index
#define PRISM_DBG_NEW_SI_VAL(n)     ((uint32_t)((n) & 0x7u) << 11)
#define PRISM_DBG_CTRL_MASK         0x3FFu      // persistent latch bits [9:0]

// DBG_STAT register fields
#define PRISM_DBG_STAT_CURR_SI(v)   ((v) & 0x7u)
#define PRISM_DBG_STAT_NEXT_SI(v)   (((v) >> 3) & 0x7u)
#define PRISM_DBG_STAT_HALTED       (1u << 6)
#define PRISM_DBG_STAT_BREAK        (1u << 7)   // halted due to breakpoint 0
#define PRISM_DBG_STAT_BREAK1       (1u << 8)   // halted due to breakpoint 1

// FIFO_STAT register fields (byte read)
#define PRISM_FIFO_STAT_EMPTY       (1u << 0)
#define PRISM_FIFO_STAT_FULL        (1u << 1)
#define PRISM_FIFO_STAT_WR_PTR(v)   (((v) >> 2) & 3u)
#define PRISM_FIFO_STAT_RD_PTR(v)   (((v) >> 4) & 3u)
#define PRISM_FIFO_STAT_COUNT(v)    (((v) >> 6) & 3u)

// ==========================================================================
// PRISM FSM input vector (PRISM_REG_IN_DATA / chroma input mux selects)
//   in[6:0]   ui_in[6:0]
//   in[7]     shift_data (serial output of the active shifter)
//   in[9:8]   host_in[1:0]
//   in[10]    count1 == 0
//   in[11]    count2 >= count2_compare
//   in[13:12] latched inputs (or registered out[6]/out[1] with LATCH_IN_OUT)
//   in[14]    shift bit counter == 0 (all bits shifted)
//   in[15]    count2 == comm_data
//
// PRISM FSM output vector (PRISM_REG_OUT_DATA)
//   out[6:0]  drive uo_out[7:1] (subject to shift/cond output muxes)
//   out[2]    also latches inputs when CTRL LATCH_EN set
//   out[4]    also comm load-from-preload when CTRL LOAD4 set
//   out[5]    also count2 decrement when CTRL COUNT2_DEC set
//   out[6]    also shift enable strobe when CTRL SHIFT_EN set
//   out[7]    count1 decrement
//   out[8]    count1 load from preload (FIFO push in FIFO_24 mode)
//   out[9]    count2 increment
//   out[10]   count2 clear (out[9]+out[10] together raise the interrupt)
// ==========================================================================

// Error bits returned by the config load/verify functions
#define PRISM_ERR_WORD(i)           (1 << (i))  // outgoing word i mismatched
#define PRISM_ERR_FINAL             (1 << 16)   // post-load readback mismatched
#define PRISM_ERR_CTRL              (1 << 17)   // CTRL readback mismatched

// ==========================================================================
// Low level register access
// ==========================================================================
static inline void prism_write32(uint32_t reg, uint32_t value)
{
    *(volatile uint32_t*)(PRISM_BASE_ADDRESS + reg) = value;
}

static inline uint32_t prism_read32(uint32_t reg)
{
    return *(volatile uint32_t*)(PRISM_BASE_ADDRESS + reg);
}

static inline void prism_write8(uint32_t reg, uint8_t value)
{
    *(volatile uint8_t*)(PRISM_BASE_ADDRESS + reg) = value;
}

static inline uint8_t prism_read8(uint32_t reg)
{
    return *(volatile uint8_t*)(PRISM_BASE_ADDRESS + reg);
}

// ==========================================================================
// Configuration (chroma) loading and the shift/latch config space
// ==========================================================================

// Shift one 45-bit STEW into config stage 0 (advances all stages).
void prism_cfg_write(uint32_t msw, uint32_t lsw);

// Read the STEW currently held in config stage 15 (the next word to be
// pushed out by a prism_cfg_write).
void prism_cfg_read(uint32_t *msw, uint32_t *lsw);

// Shift all PRISM_NUM_STATES words of chroma[] into the config array.
// If expected is non-NULL, the word shifted out of stage 15 is compared
// against expected[] before each write - pass the previously loaded chroma
// to validate it as it is displaced by the new one.  Always verifies that
// chroma word 0 arrived in stage 15 when the load completes.  Returns 0 on
// success, else a mask of PRISM_ERR_WORD(i) / PRISM_ERR_FINAL.
// The PRISM should be disabled (or halted) while reloading its config.
int prism_load_config(const uint32_t *chroma, const uint32_t *expected);

// Verify the entire config array against expected[] (chroma load order) by
// recirculating it: each stage 15 word is read, compared and written back,
// so after 16 shifts the array is restored unchanged.  Pass expected=NULL
// to just exercise/restore the array.  Returns 0 or PRISM_ERR_* mask.
// The PRISM should be disabled (or halted) during the recirculation.
int prism_verify_config(const uint32_t *expected);

// Full chroma load as performed during ASIC verification: disable the
// PRISM, shift in the chroma, program the control word (ctrl_reg as emitted
// by the Yosys PRISM backend), verify both, then enable the PRISM.  On any
// error the PRISM is left disabled.  Returns 0 or PRISM_ERR_* mask.
int prism_load_chroma(const uint32_t *chroma, uint32_t ctrl_reg);

// As prism_load_chroma, but additionally validates the previously loaded
// design (expected[], may be NULL) as it is shifted out of the STEW array.
int prism_load_chroma_verify(const uint32_t *chroma, const uint32_t *expected,
                             uint32_t ctrl_reg);

// Self test of the shift-register/latch based configuration space: loads a
// pattern "design", then loads a second pattern while validating the first
// as it is shifted out, then verifies the second by recirculation.
// Leaves the PRISM disabled.  Returns 0 on pass, else PRISM_ERR_* mask.
int prism_test_config(void);

// ==========================================================================
// Control word / enable / interrupt
// ==========================================================================
void prism_set_ctrl(uint32_t ctrl);         // write CTRL (CFG bits + ENABLE)
uint32_t prism_get_ctrl(void);              // raw CTRL readback
void prism_enable(void);                    // set ENABLE, keep config bits
void prism_disable(void);                   // clear ENABLE, keep config bits
                                            //   (resets FSM, counters, FIFO)
bool prism_is_enabled(void);
bool prism_interrupt_pending(void);
void prism_clear_interrupt(void);           // note: chroma interrupt requests
                                            //   are level style and may re-set

// Route uo_out pins to the PRISM: bit n of pin_mask selects uo_out[n]
// (bit 0 is ignored - uo_out[0] is the TinyQV UART TX).
void prism_claim_pins(uint8_t pin_mask);

// Enable/disable the PRISM user interrupt in the RISC-V mie CSR.  Handle it
// by defining:  void tqv_user_interrupt08(void)
void prism_enable_interrupt(void);
void prism_disable_interrupt(void);

// ==========================================================================
// PRISM peripheral values: counters, compare, 24-bit shift register, comm
// register, host input bits and RX FIFO
// ==========================================================================
void prism_set_count1_preload(uint32_t value24);    // preload / 24-bit SR load
uint32_t prism_get_count1_preload(void);
uint32_t prism_get_count1(void);                    // live 24-bit counter / SR
uint8_t prism_get_count2(void);                     // live 8-bit counter
void prism_set_count2_compare(uint8_t value);
uint8_t prism_get_count2_compare(void);

// 24-bit shift register convenience wrappers: the value is staged in the
// preload register (the chroma transfers it with out[8] = count1 load) and
// the shifted/received value is read from the live count1 register.
void prism_shift24_write(uint32_t value24);
uint32_t prism_shift24_read(void);

void prism_comm_write(uint8_t value);               // 8-bit comm register
uint8_t prism_comm_read(void);

void prism_host_write(uint8_t bits);                // host_in[1:0]
uint8_t prism_host_read(void);

// Toggle host_in[0] and clear the interrupt in a single write (the hardware
// auto-toggle at offset 0x21).  The count2 compare value, which shares the
// write port, is preserved.
void prism_host_toggle(void);

uint8_t prism_fifo_status(void);                    // PRISM_FIFO_STAT_* fields
bool prism_fifo_empty(void);
bool prism_fifo_full(void);
uint8_t prism_fifo_level(void);                     // 0..2 buffered bytes... plus
                                                    //   a 3rd in the shift stage
uint8_t prism_fifo_pop(void);                       // unchecked pop (byte read)
int prism_fifo_read(void);                          // -1 if empty, else byte
                                                    //   (PRISM must be running)

// ==========================================================================
// Debugger.  Only functional while the PRISM is enabled.  Halting (by
// request, breakpoint or step) raises the PRISM user interrupt.
// ==========================================================================
uint32_t prism_dbg_status(void);                    // PRISM_DBG_STAT_* fields
uint32_t prism_dbg_get_ctrl(void);                  // persistent DBG_CTRL bits
bool prism_dbg_is_halted(void);
uint8_t prism_dbg_curr_state(void);
uint8_t prism_dbg_next_state(void);
bool prism_dbg_break_active(void);

bool prism_dbg_halt(void);                          // halt, wait until halted
bool prism_dbg_wait_halt(uint32_t timeout_us);
void prism_dbg_resume(void);                        // resume from halt/break
bool prism_dbg_step(void);                          // execute one transition
void prism_dbg_set_breakpoint(int bp, uint8_t state);   // bp = 0 or 1
void prism_dbg_clear_breakpoint(int bp);
void prism_dbg_set_state(uint8_t state);            // force state (while halted)

// Live visibility into the FSM
uint32_t prism_get_inputs(void);                    // 16-bit input vector
uint32_t prism_get_outputs(void);                   // 11-bit output vector
uint32_t prism_get_decision_tree(void);             // LUT inputs + match
uint32_t prism_get_id(void);                        // architecture ID word
uint8_t prism_get_out_pins(void);                   // latched uo_out[7:1]

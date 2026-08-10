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
// See prism.h for the register map.

#include <stddef.h>
#include <stdio.h>
#include "prism.h"
#include "csr.h"

// A CFG_LSW write starts the latch loader, which pulses all 16 stage latch
// enables in sequence (~50 clocks at the peripheral clock).  The written
// data must remain stable until the shift completes, so wait before doing
// any further PRISM access.
#define PRISM_CFG_SHIFT_US  2

// ==========================================================================
// Configuration (chroma) loading
// ==========================================================================

void prism_cfg_write(uint32_t msw, uint32_t lsw)
{
    prism_write32(PRISM_REG_CFG_MSW, msw & PRISM_STEW_MSW_MASK);
    prism_write32(PRISM_REG_CFG_LSW, lsw);
    delay_us(PRISM_CFG_SHIFT_US);
}

void prism_cfg_read(uint32_t *msw, uint32_t *lsw)
{
    *msw = prism_read32(PRISM_REG_CFG_MSW);
    *lsw = prism_read32(PRISM_REG_CFG_LSW);
}

int prism_load_config(const uint32_t *chroma, const uint32_t *expected)
{
    int errors = 0;

    for (int i = 0; i < PRISM_NUM_STATES; ++i) {
        if (expected) {
            uint32_t msw, lsw;
            prism_cfg_read(&msw, &lsw);
            if (msw != (expected[2 * i] & PRISM_STEW_MSW_MASK) ||
                lsw != expected[2 * i + 1])
                errors |= PRISM_ERR_WORD(i);
        }
        prism_cfg_write(chroma[2 * i], chroma[2 * i + 1]);
    }

    // The first word written has now been shifted through to stage 15
    uint32_t msw, lsw;
    prism_cfg_read(&msw, &lsw);
    if (msw != (chroma[0] & PRISM_STEW_MSW_MASK) || lsw != chroma[1])
        errors |= PRISM_ERR_FINAL;

    return errors;
}

int prism_verify_config(const uint32_t *expected)
{
    int errors = 0;

    for (int i = 0; i < PRISM_NUM_STATES; ++i) {
        uint32_t msw, lsw;
        prism_cfg_read(&msw, &lsw);
        if (expected &&
            (msw != (expected[2 * i] & PRISM_STEW_MSW_MASK) ||
             lsw != expected[2 * i + 1]))
            errors |= PRISM_ERR_WORD(i);

        // Recirculate the word so the array is unchanged after 16 shifts
        prism_cfg_write(msw, lsw);
    }

    return errors;
}

int prism_load_chroma_verify(const uint32_t *chroma, const uint32_t *expected,
                             uint32_t ctrl_reg)
{
    prism_disable();

    int errors = prism_load_config(chroma, expected);

    prism_set_ctrl(ctrl_reg & PRISM_CTRL_CFG_MASK);
    if ((prism_get_ctrl() & PRISM_CTRL_CFG_MASK) !=
            (ctrl_reg & PRISM_CTRL_CFG_MASK))
        errors |= PRISM_ERR_CTRL;

    if (errors == 0)
        prism_enable();

    return errors;
}

int prism_load_chroma(const uint32_t *chroma, uint32_t ctrl_reg)
{
    return prism_load_chroma_verify(chroma, NULL, ctrl_reg);
}

static void prism_test_pattern(uint32_t seed, int i, uint32_t *msw, uint32_t *lsw)
{
    uint32_t b = (seed + (uint32_t)i * 0x11u) & 0xFFu;
    uint32_t w = b | (b << 8) | (b << 16) | (b << 24);
    *lsw = w ^ (seed << 16);
    *msw = (w ^ (w >> 3) ^ seed) & PRISM_STEW_MSW_MASK;
}

int prism_test_config(void)
{
    uint32_t msw, lsw, exp_msw, exp_lsw;
    int errors = 0;

    prism_disable();

    // Load the first pattern "design"
    for (int i = 0; i < PRISM_NUM_STATES; ++i) {
        prism_test_pattern(0xA5, i, &msw, &lsw);
        prism_cfg_write(msw, lsw);
    }

    // Load a second pattern, validating the first as it shifts out of the
    // STEW array
    for (int i = 0; i < PRISM_NUM_STATES; ++i) {
        prism_cfg_read(&msw, &lsw);
        prism_test_pattern(0xA5, i, &exp_msw, &exp_lsw);
        printf("Expected 0x%08lX%08lX  got 0x%08lX%08lX\r\n", exp_msw, exp_lsw, msw, lsw);
        if (msw != exp_msw || lsw != exp_lsw)
        {
            errors |= PRISM_ERR_WORD(i);
        }

        prism_test_pattern(0x3C, i, &msw, &lsw);
        prism_cfg_write(msw, lsw);
    }

    printf("\n");
    // Verify the second pattern in place by recirculating it
    for (int i = 0; i < PRISM_NUM_STATES; ++i) {
        prism_cfg_read(&msw, &lsw);
        prism_test_pattern(0x3C, i, &exp_msw, &exp_lsw);
        printf("Expected 0x%08lX%08lX  got 0x%08lX%08lX\r\n", exp_msw, exp_lsw, msw, lsw);
        if (msw != exp_msw || lsw != exp_lsw)
        {
            errors |= PRISM_ERR_FINAL;
        }

        prism_cfg_write(msw, lsw);
    }

    return errors;
}

// ==========================================================================
// Control word / enable / interrupt
// ==========================================================================

void prism_set_ctrl(uint32_t ctrl)
{
    prism_write32(PRISM_REG_CTRL, ctrl & (PRISM_CTRL_CFG_MASK | PRISM_CTRL_ENABLE));
}

uint32_t prism_get_ctrl(void)
{
    return prism_read32(PRISM_REG_CTRL);
}

void prism_enable(void)
{
    prism_set_ctrl((prism_get_ctrl() & PRISM_CTRL_CFG_MASK) | PRISM_CTRL_ENABLE);
}

void prism_disable(void)
{
    prism_set_ctrl(prism_get_ctrl() & PRISM_CTRL_CFG_MASK);
}

bool prism_is_enabled(void)
{
    return (prism_get_ctrl() & PRISM_CTRL_ENABLE) != 0;
}

bool prism_interrupt_pending(void)
{
    return (prism_get_ctrl() & PRISM_CTRL_INTERRUPT) != 0;
}

void prism_clear_interrupt(void)
{
    prism_write8(PRISM_REG_INT_CLR, 0x80);
}

void prism_claim_pins(uint8_t pin_mask)
{
    for (int pin = 1; pin < 8; ++pin)
        if (pin_mask & (1u << pin))
            set_gpio_func(pin, PRISM_PERIPHERAL_NUM);
}

void prism_enable_interrupt(void)
{
    enable_interrupt(PRISM_PERIPHERAL_NUM);
}

void prism_disable_interrupt(void)
{
    disable_interrupt(PRISM_PERIPHERAL_NUM);
}

// ==========================================================================
// Counters, compare, 24-bit shift register, comm register, host bits, FIFO
// ==========================================================================

void prism_set_count1_preload(uint32_t value24)
{
    prism_write32(PRISM_REG_PRELOAD, value24 & 0xFFFFFFu);
}

uint32_t prism_get_count1_preload(void)
{
    return prism_read32(PRISM_REG_PRELOAD) & 0xFFFFFFu;
}

uint32_t prism_get_count1(void)
{
    return prism_read32(PRISM_REG_COUNT_VAL) & 0xFFFFFFu;
}

uint8_t prism_get_count2(void)
{
    return (uint8_t)(prism_read32(PRISM_REG_COUNT_VAL) >> 24);
}

void prism_set_count2_compare(uint8_t value)
{
    prism_write8(PRISM_REG_COMPARE, value);
}

uint8_t prism_get_count2_compare(void)
{
    return prism_read8(PRISM_REG_COMPARE);
}

void prism_shift24_write(uint32_t value24)
{
    prism_write32(PRISM_REG_PRELOAD, value24 & 0xFFFFFFu);
}

uint32_t prism_shift24_read(void)
{
    return prism_read32(PRISM_REG_COUNT_VAL) & 0xFFFFFFu;
}

void prism_comm_write(uint8_t value)
{
    prism_write8(PRISM_REG_COMM_DATA, value);
}

uint8_t prism_comm_read(void)
{
    return prism_read8(PRISM_REG_COMM_DATA);
}

void prism_host_write(uint8_t bits)
{
    prism_write8(PRISM_REG_HOST_IN, bits & 3u);
}

uint8_t prism_host_read(void)
{
    return prism_read8(PRISM_REG_HOST_IN) & 3u;
}

void prism_host_toggle(void)
{
    // The auto-toggle write also lands in the count2 compare latch, so
    // rewrite the current compare value to preserve it
    prism_write8(PRISM_REG_HOST_TOGGLE, prism_get_count2_compare());
}

uint8_t prism_fifo_status(void)
{
    return prism_read8(PRISM_REG_FIFO_STAT);
}

bool prism_fifo_empty(void)
{
    return (prism_fifo_status() & PRISM_FIFO_STAT_EMPTY) != 0;
}

bool prism_fifo_full(void)
{
    return (prism_fifo_status() & PRISM_FIFO_STAT_FULL) != 0;
}

uint8_t prism_fifo_level(void)
{
    return PRISM_FIFO_STAT_COUNT(prism_fifo_status());
}

uint8_t prism_fifo_pop(void)
{
    return prism_read8(PRISM_REG_FIFO_DATA);
}

int prism_fifo_read(void)
{
    if (prism_fifo_empty())
        return -1;
    return prism_fifo_pop();
}

// ==========================================================================
// Debugger
// ==========================================================================

uint32_t prism_dbg_status(void)
{
    return prism_read32(PRISM_REG_DBG_STAT);
}

uint32_t prism_dbg_get_ctrl(void)
{
    return prism_read32(PRISM_REG_DBG_CTRL) & PRISM_DBG_CTRL_MASK;
}

bool prism_dbg_is_halted(void)
{
    return (prism_dbg_status() & PRISM_DBG_STAT_HALTED) != 0;
}

uint8_t prism_dbg_curr_state(void)
{
    return PRISM_DBG_STAT_CURR_SI(prism_dbg_status());
}

uint8_t prism_dbg_next_state(void)
{
    return PRISM_DBG_STAT_NEXT_SI(prism_dbg_status());
}

bool prism_dbg_break_active(void)
{
    return (prism_dbg_status() & PRISM_DBG_STAT_BREAK) != 0;
}

bool prism_dbg_wait_halt(uint32_t timeout_us)
{
    uint32_t deadline = read_time() + timeout_us;
    while (!prism_dbg_is_halted()) {
        if ((int32_t)(deadline - read_time()) <= 0)
            return prism_dbg_is_halted();
    }
    return true;
}

bool prism_dbg_halt(void)
{
    prism_write32(PRISM_REG_DBG_CTRL,
                  prism_dbg_get_ctrl() | PRISM_DBG_HALT_REQ);
    return prism_dbg_wait_halt(100);
}

void prism_dbg_resume(void)
{
    // A falling edge on halt_req resumes and clears any active breakpoint
    uint32_t ctrl = prism_dbg_get_ctrl();
    prism_write32(PRISM_REG_DBG_CTRL, ctrl | PRISM_DBG_HALT_REQ);
    prism_write32(PRISM_REG_DBG_CTRL, ctrl & ~PRISM_DBG_HALT_REQ);
}

bool prism_dbg_step(void)
{
    if (!prism_dbg_is_halted())
        return false;

    // Rising edge on the step bit executes one state transition, after
    // which the FSM halts again
    uint32_t ctrl = prism_dbg_get_ctrl() & ~PRISM_DBG_STEP;
    prism_write32(PRISM_REG_DBG_CTRL, ctrl | PRISM_DBG_STEP);
    prism_write32(PRISM_REG_DBG_CTRL, ctrl);
    return prism_dbg_wait_halt(100);
}

void prism_dbg_set_breakpoint(int bp, uint8_t state)
{
    uint32_t ctrl = prism_dbg_get_ctrl();
    if (bp == 0) {
        ctrl &= ~PRISM_DBG_BP0_SI(0xF);
        ctrl |= PRISM_DBG_BP0_EN | PRISM_DBG_BP0_SI(state);
    } else {
        ctrl &= ~PRISM_DBG_BP1_SI(0xF);
        ctrl |= PRISM_DBG_BP1_EN | PRISM_DBG_BP1_SI(state);
    }
    prism_write32(PRISM_REG_DBG_CTRL, ctrl);
}

void prism_dbg_clear_breakpoint(int bp)
{
    uint32_t ctrl = prism_dbg_get_ctrl();
    ctrl &= (bp == 0) ? ~PRISM_DBG_BP0_EN : ~PRISM_DBG_BP1_EN;
    prism_write32(PRISM_REG_DBG_CTRL, ctrl);
}

void prism_dbg_set_state(uint8_t state)
{
    prism_write32(PRISM_REG_DBG_CTRL,
                  prism_dbg_get_ctrl() | PRISM_DBG_NEW_SI |
                  PRISM_DBG_NEW_SI_VAL(state));
}

// ==========================================================================
// Live visibility
// ==========================================================================

uint32_t prism_get_inputs(void)
{
    return prism_read32(PRISM_REG_IN_DATA) & 0xFFFFu;
}

uint32_t prism_get_outputs(void)
{
    return prism_read32(PRISM_REG_OUT_DATA) & 0x7FFu;
}

uint32_t prism_get_decision_tree(void)
{
    return prism_read32(PRISM_REG_DECISION);
}

uint32_t prism_get_id(void)
{
    return prism_read32(PRISM_REG_ID);
}

uint8_t prism_get_out_pins(void)
{
    return (uint8_t)PRISM_CTRL_GET_OUT_PINS(prism_get_ctrl());
}

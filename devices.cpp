#include "hardware/pwm.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"

#include "devices.h"

#include "video_out.pio.h"


uint32_t video_dma_top[SP_SIZE] = 
{
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, 
    SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_TOP, SP_FINAL
};


uint32_t csync_cc[SYNC_PERIODS] = 
{
    SYNC_VS, SYNC_VS, SYNC_VS, SYNC_VS, SYNC_VS, SYNC_VS, SYNC_VS, SYNC_VS, SYNC_VS, // 1-9
    SYNC_HS, // 10
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 11-20
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 21-30
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 31-40
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 41-50
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 51-60
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 61-70
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, 
    SYNC_DISP | SYNC_HS, SYNC_DISP | SYNC_HS, 
    SYNC_DISP | SYNC_IRQ | SYNC_HS, SYNC_DISP | SYNC_IRQ | SYNC_HS,  // 71-80
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 81-90
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 91-100
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 101-110
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 111-120
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 121-130
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 131-140
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 141-150
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 151-160
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 161-170
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 171-180
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 181-190
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 191-200
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, 
    SYNC_DISP | SYNC_HS, SYNC_DISP | SYNC_HS, SYNC_DISP | SYNC_HS, SYNC_DISP | SYNC_HS, 
    SYNC_HS, SYNC_HS, // 201-210
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 211-220
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 221-230
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 231-240
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 241-250
    SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, SYNC_HS, // 251-260    
    SYNC_HS, SYNC_HS // 261-262
};

#define SYS_TIMER_SLICE 0
#define CSYNC_DMA_CHANNEL 0
#define VIDEO_DMA_DMA_CHANNEL 1
uint sys_timer_slice, csync_timer_slice, csync_timer_channel, beep_timer_slice, beep_timer_channel;
uint video_dma_timer_slice, video_dma_timer_channel;
uint csync_dma_channel, video_dma_dma_channel;
PIO video_out_pio;
uint video_out_sm;

void init_devices() {
    set_sys_clock_khz(128000, true);

    // set functions of GPIO
    gpio_set_function(CSYNC_GPIO, GPIO_FUNC_PWM); // Should be PWM2 A
    gpio_set_function(VIDEO_DMA_GPIO, GPIO_FUNC_PWM); // Should be PWM3 A. For test purpose. 
    gpio_set_function(BEEP_GPIO, GPIO_FUNC_PWM); 

    gpio_init(OUT_Q_GPIO);
    gpio_set_dir(OUT_Q_GPIO, GPIO_OUT);

    gpio_init(IN_EF2_GPIO);
    gpio_set_dir(IN_EF2_GPIO, GPIO_IN);

    gpio_init(BUTTON_GPIO);
    gpio_set_dir(BUTTON_GPIO, GPIO_IN);
    gpio_pull_down(BUTTON_GPIO);

    gpio_init(KEYIN_1_GPIO);
    gpio_set_dir(KEYIN_1_GPIO, GPIO_IN);
    gpio_pull_down(KEYIN_1_GPIO);
    gpio_init(KEYIN_2_GPIO);
    gpio_set_dir(KEYIN_2_GPIO, GPIO_IN);
    gpio_pull_down(KEYIN_2_GPIO);
    gpio_init(KEYIN_3_GPIO);
    gpio_set_dir(KEYIN_3_GPIO, GPIO_IN);
    gpio_pull_down(KEYIN_3_GPIO);
    gpio_init(KEYIN_4_GPIO);
    gpio_set_dir(KEYIN_4_GPIO, GPIO_IN);
    gpio_pull_down(KEYIN_4_GPIO);

    gpio_init(KEYOUT_1_GPIO);
    gpio_set_dir(KEYOUT_1_GPIO, GPIO_OUT);
    gpio_init(KEYOUT_2_GPIO);
    gpio_set_dir(KEYOUT_2_GPIO, GPIO_OUT);
    gpio_init(KEYOUT_3_GPIO);
    gpio_set_dir(KEYOUT_3_GPIO, GPIO_OUT);
    gpio_init(KEYOUT_4_GPIO);
    gpio_set_dir(KEYOUT_4_GPIO, GPIO_OUT);

    // initialize beeper
    beep_timer_slice = pwm_gpio_to_slice_num(BEEP_GPIO);
    beep_timer_channel = pwm_gpio_to_channel(BEEP_GPIO);
    pwm_set_enabled(beep_timer_slice, false);
    pwm_set_irq_enabled(beep_timer_slice, false);
    pwm_set_clkdiv(beep_timer_slice, CLOCK_1802_DIV); // 1.777 MHz

    // initialize PIO0, state machine 0
    video_out_pio = pio0;
    video_out_sm = 0;

    for (int i = 0; i < count_of(video_out_program_instructions); ++i)
        video_out_pio->instr_mem[i] = video_out_program_instructions[i];

    pio_sm_config c = video_out_program_get_default_config(0);

    sm_config_set_out_pins(&c, VIDEO_OUT_GPIO, 1);
    pio_gpio_init(video_out_pio, VIDEO_OUT_GPIO);
    pio_sm_set_consecutive_pindirs(video_out_pio, video_out_sm, VIDEO_OUT_GPIO, 1, true);
    sm_config_set_clkdiv(&c, 72);
    sm_config_set_out_shift(&c, false, true, 32); 
    pio_sm_init(video_out_pio, video_out_sm, 0, &c);
    pio_sm_set_enabled(video_out_pio, video_out_sm, true);
}


void __not_in_flash_func(csync_dma_irq_handler)() {
    dma_hw->ints0 = (1u << csync_dma_channel);

    dma_channel_set_read_addr(csync_dma_channel, csync_cc, true);
}

void __not_in_flash_func(video_dma_dma_irq_handler)() {
    dma_hw->ints0 = (1u << video_dma_dma_channel);

    dma_channel_set_read_addr(video_dma_dma_channel, video_dma_top, true);
}

extern uint16_t registers[];
extern uint8_t memory[];

extern bool video_on;
extern int machine_cycles;

// IRQ from PWM for VIDEO dma signal
void __not_in_flash_func(pwm_irq_handler)() {
    pwm_clear_irq(video_dma_timer_slice);

//    pio_sm_put(pio0, video_out_sm, machine_cycles);

    pio_sm_put(pio0, video_out_sm, 
          (memory[registers[0]++] << 24) 
        | (memory[registers[0]++] << 16) 
        | (memory[registers[0]++] << 8) 
        | (memory[registers[0]++]));
    pio_sm_put(pio0, video_out_sm, 
          (memory[registers[0]++] << 24) 
        | (memory[registers[0]++] << 16) 
        | (memory[registers[0]++] << 8) 
        | (memory[registers[0]++]));
    pio_sm_put(pio0, video_out_sm, 0);
}

void __not_in_flash_func(reset_timer_dma)() {
    dma_channel_config d_c_conf;

    // PWM0: 1 count for a 1802 clock, wrap at 1 machine cycle. 
    // No IRQ issued. Not started yet. 
    sys_timer_slice = SYS_TIMER_SLICE; 
    pwm_set_enabled(sys_timer_slice, false);
    pwm_set_irq_enabled(sys_timer_slice, false);
    pwm_set_clkdiv(sys_timer_slice, CLOCK_1802_DIV);
    pwm_set_wrap(sys_timer_slice, CLOCKS_PER_CYCLE - 1); 
    pwm_set_counter(sys_timer_slice, 0);

    // Timer(PWM) for CSYNC. Wrap at 1 hline. 
    // channel A gives the sync signal, and channel B keeps IRQ/DISP signal. 
    // Data is supplied from csync_dma_channel.
    csync_timer_slice = pwm_gpio_to_slice_num(CSYNC_GPIO);
    csync_timer_channel = pwm_gpio_to_channel(CSYNC_GPIO);

    pwm_set_enabled(csync_timer_slice, false);
    pwm_set_irq_enabled(csync_timer_slice, false);
    pwm_set_clkdiv(csync_timer_slice, CLOCK_1802_DIV);
    // active low for CSYNC
    if (csync_timer_channel == PWM_CHAN_A)
        pwm_set_output_polarity(csync_timer_slice, true, false);
    else
        pwm_set_output_polarity(csync_timer_slice, false, true); 
    pwm_set_chan_level(csync_timer_slice, csync_timer_channel, SYNC_HS);

    // Set up DMA for csync
    csync_dma_channel = CSYNC_DMA_CHANNEL;
    d_c_conf = dma_channel_get_default_config(csync_dma_channel);

    channel_config_set_transfer_data_size(&d_c_conf, DMA_SIZE_32); 
    channel_config_set_read_increment(&d_c_conf, true);
    channel_config_set_write_increment(&d_c_conf, false);
    channel_config_set_dreq(&d_c_conf, pwm_get_dreq(csync_timer_slice));
    channel_config_set_enable(&d_c_conf, true);
    dma_channel_set_config(csync_dma_channel, &d_c_conf, false);

    dma_channel_set_write_addr(csync_dma_channel, &(pwm_hw->slice[csync_timer_slice].cc), false);
    dma_channel_set_trans_count(csync_dma_channel, SYNC_PERIODS, false);

    irq_set_exclusive_handler(DMA_IRQ_0, csync_dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true); 

    // Set up timer/PWM for DMA (to 1861) timing. 
    video_dma_timer_slice = pwm_gpio_to_slice_num(VIDEO_DMA_GPIO);
    video_dma_timer_channel = pwm_gpio_to_channel(VIDEO_DMA_GPIO);

    pwm_set_enabled(video_dma_timer_slice, false);
    pwm_set_irq_enabled(video_dma_timer_slice, false);
    pwm_set_clkdiv(video_dma_timer_slice, CLOCK_1802_DIV);
    pwm_set_chan_level(video_dma_timer_slice, video_dma_timer_channel, SP_DMA_END - SP_DMA_START);

    video_dma_dma_channel = VIDEO_DMA_DMA_CHANNEL;
    d_c_conf = dma_channel_get_default_config(video_dma_dma_channel);

    channel_config_set_transfer_data_size(&d_c_conf, DMA_SIZE_32); 
    channel_config_set_read_increment(&d_c_conf, true);
    channel_config_set_write_increment(&d_c_conf, false);
    channel_config_set_dreq(&d_c_conf, pwm_get_dreq(video_dma_timer_slice));
    channel_config_set_enable(&d_c_conf, true);
    dma_channel_set_config(video_dma_dma_channel, &d_c_conf, false);

    dma_channel_set_write_addr(video_dma_dma_channel, &(pwm_hw->slice[video_dma_timer_slice].top), false);
    dma_channel_set_trans_count(video_dma_dma_channel, SP_SIZE, false);

    irq_set_exclusive_handler(DMA_IRQ_1, video_dma_dma_irq_handler);
    irq_set_enabled(DMA_IRQ_1, true); 

    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_irq_handler);
    irq_set_enabled(PWM_IRQ_WRAP, true); 
}

void __not_in_flash_func(emu_start_clock)() {
    pwm_hw->intr = (1u << sys_timer_slice);
    pwm_set_counter(sys_timer_slice, 0);
    pwm_set_enabled(sys_timer_slice, true);
}

void emu_stop_clock() {
    pwm_set_enabled(sys_timer_slice, false);
}

extern bool video_on;

void __not_in_flash_func(start_display)() {
    if (video_on)
        return;
    video_on = true;
    pwm_set_wrap(csync_timer_slice, VIDEO_CHIP_DELAY - 1); // initial wait
    pwm_set_counter(csync_timer_slice, 0);

    pwm_set_wrap(video_dma_timer_slice, VIDEO_CHIP_DELAY - 1); // initial wait
    pwm_set_counter(video_dma_timer_slice, 0);

    dma_channel_set_read_addr(csync_dma_channel, csync_cc, false);
    dma_channel_set_read_addr(video_dma_dma_channel, video_dma_top, false);

    dma_channel_set_irq0_enabled(csync_dma_channel, true);
    dma_channel_set_irq1_enabled(video_dma_dma_channel, true);

    pwm_set_mask_enabled((1 << sys_timer_slice) | (1 << csync_timer_slice) | (1 << video_dma_timer_slice));
    pwm_set_wrap(csync_timer_slice, SYNC_TOP); // the first value
    pwm_set_wrap(video_dma_timer_slice, SP_INITIAL); // the first value

    dma_channel_start(csync_dma_channel);
    dma_channel_start(video_dma_dma_channel);

    pwm_set_irq_enabled(video_dma_timer_slice, true);
}

void __not_in_flash_func(stop_display)() {
    if (!video_on)
        return;
    video_on = false;
    pwm_set_irq_enabled(video_dma_timer_slice, false);
    pwm_set_enabled(csync_timer_slice, false);
    pwm_set_enabled(video_dma_timer_slice, false);
    dma_channel_abort(csync_dma_channel);
    dma_channel_abort(video_dma_dma_channel);
    dma_channel_set_irq0_enabled(csync_dma_channel, false);
    dma_channel_set_irq1_enabled(video_dma_dma_channel, false);
}

int8_t hexkey_matrix[4][4] = {
    {1, 2, 3, 0xc}, {4, 5, 6, 0xd}, {7, 8, 9, 0xe}, {0xa, 0, 0xb, 0xf}
};

int8_t get_hexkey() {
    for(int i = 0; i < 4; i++) {
        switch(i) {
            case 0:
                gpio_clr_mask((1 << KEYOUT_2_GPIO) | (1 << KEYOUT_3_GPIO) |(1 << KEYOUT_4_GPIO));
                gpio_set_mask((1 << KEYOUT_1_GPIO));
                break;
            case 1:
                gpio_clr_mask((1 << KEYOUT_1_GPIO) | (1 << KEYOUT_3_GPIO) |(1 << KEYOUT_4_GPIO));
                gpio_set_mask((1 << KEYOUT_2_GPIO));
                break;
            case 2:
                gpio_clr_mask((1 << KEYOUT_1_GPIO) | (1 << KEYOUT_2_GPIO) |(1 << KEYOUT_4_GPIO));
                gpio_set_mask((1 << KEYOUT_3_GPIO));
                break;
            case 3:
                gpio_clr_mask((1 << KEYOUT_1_GPIO) | (1 << KEYOUT_2_GPIO) |(1 << KEYOUT_3_GPIO));
                gpio_set_mask((1 << KEYOUT_4_GPIO));
                break;
        }

        sleep_ms(5);
        if (gpio_get(KEYIN_1_GPIO)) {
            return hexkey_matrix[i][0];
        } 
        if (gpio_get(KEYIN_2_GPIO)) {
            return hexkey_matrix[i][1];
        } 
        if (gpio_get(KEYIN_3_GPIO)) {
            return hexkey_matrix[i][2];
        } 
        if (gpio_get(KEYIN_4_GPIO)) {
            return hexkey_matrix[i][3];
        }
    }
    return -1;
}

void __not_in_flash_func(set_tone)(uint16_t t) {
    pwm_set_wrap(beep_timer_slice, ((t + 1) << 6) - 1); 
    pwm_set_chan_level(beep_timer_slice, beep_timer_channel, (t + 1) << 5);    
}


#ifndef __SSD1331_HPP__
#define __SSD1331_HPP__

// Class SSD1331
// Driver of SSD1331 for ESP32(VSPI)
// 

#define DEFAULT_PIN_DC  6  // Data - Command Select
#define DEFAULT_PIN_RST 7  // Reset

//#define __DRAW_COMMANDS_ENABLE__ // NOT IMPLEMENTED

#include "SPI.h" // Arduino environment

class SSD1331{

    private:
    static const char width = 96;
    static const char height = 64;
    static const char max_w = width-1;
    static const char max_h = height-1;
    // pin assign for ESP32 VSPI
    static const int pin_SCLK = 18; // IO18 fixed
    static const int pin_SDIN = 23; // IO23 fixed
    int pin_DCCntl; // data or command
    int pin_RST;    // reset
    int pin_CS;     // chip select

    public:
    // initialize pin setting, display settings
    //   pin_DCCntl: pin number for data/command control
    //   pin_RST:    pin number for reset
    //   pin_CS:     pin number for chip selection
    void init( int pin_DCCntl, int pin_RST, int pin_CS );

    // Turn On the Display / ディスプレイ ON
    void on();
    // Turn On the display with dim mode / ディスプレイ ON (dim mode), セッティングは、set_dim_mode()
    void dim_mode();
    // ディスプレイ Sleep (dim mode)
    void sleep();

    // 通常画面方向(シルクと同じ向き, 画像データは左上原点の時)
    void dir_normal();
    // 回転
    void rotate();
    // 水平方向反転状態に設定
    void h_flip();
    // 垂直方向反転状態に設定
    void v_flip();

    // フルフレームデータ送信 for 65536色モード
    void send_frame(unsigned short *p_data); // send full frame (96x64x2bytes)
    // フルフレームデータ送信 for 256色モード
    void send_frame(unsigned char *p_data);  // send full frame (96x64x1bytes)


    private:
    void send_data( unsigned char val );
    void send_data( unsigned short val );
    void send_data( unsigned short *val, size_t n_pixel );
    void send_data( unsigned char *val, size_t n_bytes );
    void send_command( unsigned char val );
    void set_colmun_address( unsigned char start, unsigned char end); // 00-95
    void set_row_address( unsigned char start, unsigned char end); // 00-63
    void set_contrasts( unsigned char contrast_a, unsigned char contrast_b, unsigned char contrast_c ); // 0-255
    void set_master_current( unsigned char current ); // 0 - 15
    void set_precharge( unsigned char a, unsigned char b, unsigned char c ); // 0-255
    enum VERTICAL_DIR{
        TB_NORMAL = 1,
        TB_FLIP = 0
    };
    enum HORIZONTAL_DIR{
        LR_NORMAL = 1,
        LR_FLIP = 0
    };
    void set_remap_color_depth( 
        HORIZONTAL_DIR h_dir = HORIZONTAL_DIR::LR_NORMAL,
        VERTICAL_DIR v_dir = VERTICAL_DIR::TB_NORMAL
    );
    void set_display_start_line( unsigned char line ); // 0-63
    void set_display_offset( unsigned char line ); // 0-63
    void set_display_mode( unsigned char mode ); // 0: normal, 1:turn on, 2: turn off, 3: inverse
    void set_multiplex_ratio( unsigned char n_mux ); // 15 to 63
    void set_dim_mode( 
        unsigned char contrast_a, //0-255
        unsigned char contrast_b, //0-255
        unsigned char contrast_c, //0-255
        unsigned char voltage    //0-31
    );
    void set_master_configuration( unsigned char configuration );
    enum class DISPLAY_POWER : unsigned char{
        DISPLAY_ON_DIM_MODE = 0,
        DISPLAY_ON = 3,
        DISPLAY_OFF = 2
    };
    // ディスプレイのON, OFF
    // DISPLAY_OFF, DISPLAY_ON, DISPLAY_ON_DIM_MODE, 
    void set_display_on_off( DISPLAY_POWER mode ); 
 
    enum class POWER_SAVE_MODE{
        POWER_SAVE_ON,
        POWER_SAVE_OFF
    };
    void set_power_save_mode( POWER_SAVE_MODE mode ); // 0x1A: on, 0x0B: off
    void set_phase_period_adjustment( unsigned char period1, unsigned char period2 ); // 1-15, 1-15
    void set_display_clock_divider_osc_freq( unsigned char d, unsigned char freq ); //0-15, 0-15
    void set_gray_scale_table( unsigned char level[32 ]); // 0-125
    void set_enable_linear_gray_scale_table( );
    void set_precharge_level( unsigned char level ); //0-31
    void set_vcomh( unsigned char level ); //0-31
    void set_command_lock( unsigned char lock ); // 0:unlock, 1: lock

    // command

    // draw command
#ifdef __DRAW_COMMANDS_ENABLE__
    void clear_window( const unsigned char sx, const unsigned char sy, const unsigned char ex, const unsigned char ey );
#endif

};
#endif // __SSD1331_HPP__

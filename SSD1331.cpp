#include "Arduino.h"
#include "SSD1331.hpp"
void SSD1331::init( int pin_DCCntl, int pin_RST, int pin_CS ){
    // pin setting
    this->pin_DCCntl = pin_DCCntl;
    this->pin_RST = pin_RST;
    this->pin_CS = pin_CS;
    pinMode(this->pin_DCCntl,OUTPUT);
    pinMode(this->pin_RST,OUTPUT);

    // RESET
    digitalWrite(pin_CS, LOW);
    digitalWrite(this->pin_RST, HIGH);
    digitalWrite(this->pin_RST, LOW);
    delay(1);
    digitalWrite(this->pin_RST, HIGH);
    digitalWrite(pin_CS, HIGH);

    SPI.begin( pin_SCLK, -1, pin_SDIN, pin_CS );
    if( pin_CS == HARDWARE_CS0_PIN ){
      SPI.setHwCs(true);
    }else{
      SPI.setHwCs(false);
    }
    SPI.setFrequency(6600000); //SSD1331's SPI Clock Cycle Time : 150ns at least
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE3);

    set_display_on_off(DISPLAY_POWER::DISPLAY_OFF);
    set_remap_color_depth( HORIZONTAL_DIR::LR_NORMAL, VERTICAL_DIR::TB_NORMAL );
    set_display_start_line(0);
    set_display_offset(0);
    set_display_mode(0);
    set_multiplex_ratio(63);
    set_master_configuration(0);
    set_power_save_mode(POWER_SAVE_MODE::POWER_SAVE_OFF);
    set_phase_period_adjustment(0x7,0x4);
    set_display_clock_divider_osc_freq( 0, 0xf );
    set_precharge(0x81,0x82,0x83);
    set_precharge_level(0x31);
    set_vcomh(31);
    set_master_current(1);
    set_colmun_address(0,max_w);
    set_row_address(0,max_h);
    set_contrasts( 255, 255, 255 );
    set_dim_mode( 255, 255, 255, 3 );
    //Serial.println("DEISPLAY ON");
    //set_display_on_off(DISPLAY_POWER::DISPLAY_ON); // on
    delay(108);
}

void SSD1331::on(){
    set_display_on_off(DISPLAY_POWER::DISPLAY_ON); // on
}
void SSD1331::dim_mode(){
    set_display_on_off(DISPLAY_POWER::DISPLAY_ON_DIM_MODE); // dim mode
}
void SSD1331::sleep(){
    set_display_on_off(DISPLAY_POWER::DISPLAY_OFF); // off, sleep
}


void SSD1331::dir_normal(){
    set_remap_color_depth( HORIZONTAL_DIR::LR_NORMAL, VERTICAL_DIR::TB_NORMAL );
}
void SSD1331::h_flip(){
    set_remap_color_depth( HORIZONTAL_DIR::LR_FLIP, VERTICAL_DIR::TB_NORMAL );
}
void SSD1331::v_flip(){
    set_remap_color_depth( HORIZONTAL_DIR::LR_NORMAL, VERTICAL_DIR::TB_FLIP );    
}
void SSD1331::rotate(){
    set_remap_color_depth( HORIZONTAL_DIR::LR_FLIP, VERTICAL_DIR::TB_FLIP );    
}

void SSD1331::send_frame(unsigned short *p_data){
    set_colmun_address( 0, max_w );
    set_row_address( 0, max_h );
    send_data( p_data, 12288 ); // 96 x 64 x 2
}

void SSD1331::send_frame(unsigned char *p_data){
    set_colmun_address( 0, width );
    set_row_address( 0, max_h );
    send_data( p_data, 6144 ); // 96 x 64
}

// 部分データ送信 for 65536色
void SSD1331::send_partial_data( unsigned short *p_data, const char start_x, const char start_y, const char end_x, const char end_y ){
    // 
    try{
        // first copy data to buffer then send it 
        unsigned int size = ( end_x - start_x + 1 ) * ( end_y - start_y + 1 );
        unsigned short *buffer = new unsigned short[ size ];
        unsigned short *p = buffer;
        for( char y = start_y; y <= end_y; y++ ){
            for( char x = start_x; x <= end_x; x++ ){
                *p = p_data[ y * width + x ];
                p++;
            }
        }
        set_colmun_address( start_x, end_x );
        set_row_address( start_y, end_y );
        send_data( buffer, 2 * size );
        delete [] buffer; 
    }catch(std::bad_alloc){
        set_colmun_address( start_x, end_x );
        set_row_address( start_y, end_y );
        // send line by line
        int size_in_bytes = 2 * ( end_x - start_x + 1 );
        for( char y = start_y; y <= end_y; y++ ){
            unsigned short *p = p_data + y * width + start_x;
            send_data( p, size_in_bytes );
        }
    }
}
// 部分データ送信 for 256色
void SSD1331::send_partial_data( unsigned char *p_data, const char start_x, const char start_y, const char end_x, const char end_y ){
    try{
        // first copy data to buffer then send it 
        unsigned int size = ( end_x - start_x + 1 ) * ( end_y - start_y + 1 );
        unsigned char *buffer = new unsigned char[ size ];
        unsigned char *p = buffer;
        for( char y = start_y; y <= end_y; y++ ){
            for( char x = start_x; x <= end_x; x++ ){
                *p = p_data[ y * width + x ];
                p++;
            }
        }
        set_colmun_address( start_x, end_x );
        set_row_address( start_y, end_y );
        send_data( buffer, 2 * size );
        delete [] buffer; 
    }catch(std::bad_alloc){
        set_colmun_address( start_x, end_x );
        set_row_address( start_y, end_y );
        // send line by line
        int size_in_bytes = 2 * ( end_x - start_x + 1 );
        for( char y = start_y; y <= end_y; y++ ){
            unsigned char *p = p_data + y * width + start_x;
            send_data( p, size_in_bytes );
        }
    }
}

void SSD1331::send_data( unsigned char val ){
    digitalWrite(pin_CS,LOW);  
    digitalWrite( pin_DCCntl, HIGH );
    SPI.write( val );
    digitalWrite(pin_CS,HIGH);  
}
void SSD1331::send_data( unsigned short val ){
    digitalWrite(pin_CS,LOW);  
    digitalWrite( pin_DCCntl, HIGH );
    SPI.write16( val );
    digitalWrite(pin_CS,HIGH);  
}
// 
void SSD1331::send_data( unsigned short *val, size_t n_bytes ){
    void *p = reinterpret_cast<void*>(val);
    digitalWrite(pin_CS,LOW);  
    digitalWrite( pin_DCCntl, HIGH );
    SPI.writePixels(p,n_bytes);
    digitalWrite(pin_CS,HIGH);  
}
// 
void SSD1331::send_data( unsigned char *val, size_t n_bytes ){
    digitalWrite( pin_CS,LOW);  
    digitalWrite( pin_DCCntl, HIGH );
    SPI.writeBytes(val,n_bytes);
    digitalWrite(pin_CS,HIGH);  
}
void SSD1331::send_command( unsigned char val ){
    //Serial.print("SSD1331:send_command(");
    //Serial.print(val);
    //Serial.println(")");
    digitalWrite(pin_CS,LOW);
    digitalWrite( pin_DCCntl, LOW );
    SPI.write( val );
    digitalWrite(pin_CS,HIGH);
}

void range_check( unsigned char & val, const unsigned char min, const unsigned char max ){
    if( val < min ) val = min;
    if( val > max ) val = max;
}
void SSD1331::set_colmun_address( unsigned char start, unsigned char end){
    range_check( start, 0, max_w );
    range_check( end, 0, max_w );
    send_command( 0x15 );
    send_command( start );
    send_command( end );
}
void SSD1331::set_row_address( unsigned char start, unsigned char end){
    range_check( start, 0, max_h );
    range_check( end, 0, max_h );
    send_command( 0x75 );
    send_command( start );
    send_command( end );
}
void SSD1331::set_contrasts( unsigned char contrast_a, unsigned char contrast_b, unsigned char contrast_c ){
    send_command( 0x81 );
    send_command( contrast_a );
    send_command( 0x82 );
    send_command( contrast_a );
    send_command( 0x83 );
    send_command( contrast_a );
}
// 0-15
void SSD1331::set_master_current( unsigned char current ){
    range_check( current, 0, 15 );
    send_command( 0x87 );
    send_command( current );
}
void SSD1331::set_precharge( unsigned char a, unsigned char b, unsigned char c ){
    range_check( a, 0, 0x80 );
    range_check( b, 0, 0x80 );
    range_check( c, 0, 0x80 );
    send_command( 0x8a );
    send_command( a );
    send_command( 0x8b );
    send_command( b );
    send_command( 0x8c );
    send_command( c );
}


void SSD1331::set_remap_color_depth( 
    HORIZONTAL_DIR h_dir,        
    VERTICAL_DIR v_dir
){
    unsigned char direction = 0; 
    unsigned char color_order = 0; 

    unsigned char signal = direction + (h_dir << 1) + (color_order << 2 ) + (v_dir << 4) + (1<<5)+ (1<<6);
    send_command( 0xa0 );
    send_command( signal );

}
void SSD1331::set_display_start_line( unsigned char line ){
    range_check( line, 0, max_h );
    send_command( 0xa1 );
    send_command( line );
}
void SSD1331::set_display_offset( unsigned char line ){
    range_check( line, 0, max_h );
    send_command( 0xa2 );
    send_command( line );
}
// 0: normal, 1:turn on, 2: turn off, 3: inverse
void SSD1331::set_display_mode( unsigned char mode ){
    if( mode == 0 ){
        send_command( 0xa4 );
    }else if( mode == 1 ){
        send_command( 0xa5 );
    }else if( mode == 2 ){
        send_command( 0xa6 );
    }else if( mode == 3 ){
        send_command( 0xa7 );
    }else{
        send_command( 0xa4 ); // default
    }
}
// 15 to 63
void SSD1331::set_multiplex_ratio( unsigned char n_mux ){
     range_check( n_mux, 15, 63 );
     send_command( 0xa8 );
     send_command( n_mux );
}
void SSD1331::set_dim_mode( 
    unsigned char contrast_a, //0-255
    unsigned char contrast_b, //0-255
    unsigned char contrast_c, //0-255
    unsigned char voltage    //0-31
){
    unsigned char reserved = 0;
    range_check( reserved, 0, 0 );
    range_check( voltage, 0, 31 );
    send_command( 0xab );
    send_command( reserved );
    send_command( contrast_a );
    send_command( contrast_b );
    send_command( contrast_c );
    send_command( voltage );
}
// 0:vcc 1:reset
void SSD1331::set_master_configuration( unsigned char configuration ){
    if( configuration == 0 ){
        send_command( 0xad );
        send_command( 142 );
    }else{
        send_command( 0xad );
        send_command( 143 );
        send_command( 0xad );
        send_command( 142 );
    }
}
// DISPLAY_ON_DIM_MODE: on in dim mode
// DISPLAY_OFF:         off(sleep)
// DISPLAY_ON:          on in normal mode
void SSD1331::set_display_on_off( DISPLAY_POWER mode ){
    if( mode == DISPLAY_POWER::DISPLAY_ON_DIM_MODE ){
        send_command( 0xac );
    }else if( mode == DISPLAY_POWER::DISPLAY_OFF ){
        send_command( 0xae );
    }else if( mode == DISPLAY_POWER::DISPLAY_ON ){
        send_command( 0xaf ); 
    }else{
        send_command( 0xae );
    }
}
// 0x1A: enable, 0x0B: disable
void SSD1331::set_power_save_mode( POWER_SAVE_MODE mode ){
    send_command( 0xb0 ); 
    if( mode == POWER_SAVE_MODE::POWER_SAVE_ON ){
        send_command( 0x0b ); // enable
    }else{
        send_command( 0x1a ); // disable
    }
}
//1-15, 1-15
void SSD1331::set_phase_period_adjustment( unsigned char period1, unsigned char period2 ){
    range_check( period1, 1, 15 );
    range_check( period2, 1, 15 );
    send_command( 0xb1 ); 
    send_command( ( period2 << 4 ) + period1 );
}
//0-15, 0-15
void SSD1331::set_display_clock_divider_osc_freq( unsigned char d, unsigned char freq ){
    range_check( d, 0, 15 );
    range_check( freq, 0, 15 );
    send_command( 0xb3 ); 
    send_command( ( freq << 4 ) + d );
}
void SSD1331::set_gray_scale_table( unsigned char level[32 ]){
    // 0-125
    send_command( 0xb8 );
    for( int n = 0; n < 32; n++ ){
        range_check( level[n], 1, 125 );
        send_command( level[n] );        
    }
}
void SSD1331::set_enable_linear_gray_scale_table( ){
    send_command( 0xb9 );
}
// level [0,31]
void SSD1331::set_precharge_level( unsigned char level ){
    range_check( level, 0, 31 );
    send_command( 0xbb );
    send_command( level << 1 );
}
// level [0,31]
void SSD1331::set_vcomh( unsigned char level ){
    range_check( level, 0, 31 );
    send_command( 0xbe );
    send_command( level << 1 );
}
// 0:unlock, 1: lock
void SSD1331::set_command_lock( unsigned char lock ){
    send_command( 0xfd );
    if( lock == 0 ){
        send_command( 18 );
    }else{
        send_command( 22 );
    }
}

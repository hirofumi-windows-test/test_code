#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "nrf_delay.h"

/* SDA、SCLが出力状態の時にhigh = 1、low = 0を設定できるマクロ */
#define SDA(status)	(nrf_gpio_pin_write(29, status))
#define SCL(status)	(nrf_gpio_pin_write(28, status))

/* SDAのhigh、Lowを読み取るマクロ */
#define SDA_read	(nrf_gpio_pin_read(29))

/* SDAの入力と出力を設定するマクロ */
#define SDA_input	(nrf_gpio_cfg_input(29, GPIO_PIN_CNF_PULL_Disabled))	//プルアップはなし
#define SDA_output	(nrf_gpio_cfg_output(29))


/* 調整用定数 */
//#define CONTRAST	(0x18)	//for 5.0V
#define CONTRAST	(0x30)	//for 3.3V

/* アイコンの定義 */
const unsigned char ICON[14][2] = {
	{0x00, 0x10},		//アンテナ
	{0x02, 0x10},		//電話
	{0x04, 0x10},		//無線
	{0x06, 0x10},		//ジャック
	{0x07, 0x10},		//△
	{0x07, 0x08},		//▽
	{0x07, 0x18},		//△▽
	{0x09, 0x10},		//鍵
	{0x0B, 0x10},		//ピン
	{0x0D, 0x02},		//電池なし
	{0x0D, 0x12},		//容量少
	{0x0D, 0x1A},		//容量中
	{0x0D, 0x1E},		//容量多
	{0x0F, 0x10}		//丸
};


/*------------------------
* 関数プロトタイプ
*-----------------------*/

/* I2Cについての関数 */
void i2c_start(void);
void i2c_stop(void);
unsigned char i2c_out(unsigned char data);
unsigned char i2c_rcv(unsigned char ack);

/* LCDについての関数 */
void lcd_init(void);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_str(const unsigned char *ptr);
void lcd_clear(void);
void lcd_icon(unsigned char num, unsigned char onoff);
void delay_100ms(unsigned int time);
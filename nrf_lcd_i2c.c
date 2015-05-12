/******************************************************************************
* I2Cについての関数
******************************************************************************/

/*-----------------------------
* I2Cスタート条件出力
*-----------------------------*/

void i2c_start(void)
{
	SCL(1);
	SDA(0);								//SCLをhighの状態でSCLをlow
	SDA_output;
}

/*-----------------------------
* I2Cストップ条件出力
*-----------------------------*/

void i2c_stop(void)
{
	SCL(0);
	nrf_delay_us(1);
	SDA(0);
	SDA_output;
	SCL(1);
	nrf_delay_us(1);
	SDA(1);
	nrf_delay_us(10);
}


/*-----------------------------
* I2Cで1バイト出力
* スレーブからのACKを戻り値とする
*-----------------------------*/

unsigned char i2c_out(unsigned char data)
{
	int i;
	unsigned char bit_pos, ack;

	/* data out */
	SDA_output;
	bit_pos = 0x80						//初回0b10000000
	for(i = 0; i < 8; i++) {			//dataのbitのhigh lowに合わせて送る
		SCL(0);
		if((data & bit_pos) != 0)		
			SDA(1);						//dataの1bit目が1 -> 1 (0b1xxxxxxx) 初回
		else
			SDA(0);						//dataの1bit目が0 -> 0 (0b0xxxxxxx) 初回

		bit_pos = bit_pos >> 1;			//bit_pos = b01000000
		SCL(1);
	}

	/* ACKチェック */
	SCL(0);
	SDA_input;
	nrf_delay_us(2);
	SCL(1);
	nrf_delay_us(2);
	ack = SDA_read;	
	return ack;				

}


/*-----------------------------
* I2Cで1バイト入力
* パラメータでACK/NAKを返送
*-----------------------------*/

unsigned char i2c_rcv(unsigned char ack)
{
	int i;
	unsigned char bit_pos, data;

	data = 0;
	bit_pos = 0x80;
	for(i = 0; i < 8; i++) {
		SCL(0);
		SDA_input;
		nrf_delay_us(3);
		SCL(1);

		if(SDA_read)
			data |= bit_pos;
		bit_pos = bit_pos >> 1;
	}

	/* ACK/NAK出力 */
	SCL(0);
	SDA(ack);
	SDA_output
	nrf_delay_us(2);
	SCL(1);
	return data;
}



/******************************************************************************
* LCDについての関数
******************************************************************************/

/*-----------------------------
* 初期化関数
*-----------------------------*/

void lcd_init(void)
{
	delay_100ms(1);			
	lcd_cmd(0x38);			//8bit 2line Normal mode
	lcd_cmd(0x39);			//8bit 2line Extend mode
	lcd_cmd(0x14);			//OSC 183Hz BIAS 1/5
	/* コントラスト設定 */
	lcd_cmd(0x70 + (CONTRAST & 0X0F));
	lcd_cmd(0x5C + (CONTRAST >> 4));
//	lcd_cmd(0x6A);			//Follower for 5.0V
	lcd_cmd(0x6B);			//Follower for 3.3V
	delay_100ms(3);
	lcd_cmd(0x38);			//Set Normal mode
	lcd_cmd(0x0C);			//Display On
	lcd_cmd(0x01);			//Clear Display	

}


/*-----------------------------
* 液晶へ1コマンド出力
*-----------------------------*/

void lcd_cmd(unsigned char cmd)
{
	i2c_start();
	i2c_out(0x7C);					//0x7C = 0b01111100 7bitAddress：0b0111110
	i2c_out(cmd);
	i2c_stop();

	/* ClearかHomeか */
	if((cmd == 0x01) || (cmd == 0x02))
		nrf_delay_ms(2);
	else
		nrf_delay_us(30);
}


/*-----------------------
* 液晶へ1文字表示データ出力
*----------------------*/

void lcd_data(unsigned char data)
{
	i2c_start();				//スタート
	i2c_out(0x7C);				//アドレス
	i2c_out(0x40);				//表示データ指定
	I2C_out(data);				//表示データ出力
	i2c_stop();					//ストップ
	nrf_delay_us(30);			//遅延
}


/*-----------------------
* 文字列表示関数
*----------------------*/

void lcd_str(const unsigned char *ptr)
{
	while(*ptr != 0)		//文字取り出し
		lcd_data(*ptr++);	//文字表示
}


/*-----------------------
* 全消去関数
*----------------------*/

void lcd_clear(void)
{
	lcd_cmd(0x01);			//初期化コマンド出力
}


/*-----------------------
* アイコン表示制御関数
*----------------------*/

void lcd_icon(unsigned char num, unsigned char onoff)
{
	lcd_cmd(0x39);					//Extend mode
	lcd_cmd(0x40 | ICON[num][0]);	//アイコンアドレス指定
	if(onoff)
		lcd_data(ICON[num][1]);		//アイコンオンデータ
	else
		lcd_data(0x00)				//アイコンオフデータ
	lcd_cmd(0x38);					//Normal Mode
}


/*-----------------------
* 100msec 遅延関数
*----------------------*/
void delay_100ms(unsigned int time)
{
	time *= 4;				//4倍
	while(time) {
		nrf_delay_ms(25);	//25msec
		time--;				//100msec × time
	}
}

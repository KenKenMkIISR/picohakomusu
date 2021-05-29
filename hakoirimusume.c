//箱入り娘パズル  for Raspberry Pi Pico by K.Tanaka

// LCD : ILI9341 (240x340) SPI I/F
//  Pico         LCD
//  GPIO11       CS
//  GPIO12       DC
//  GPIO13       RESET
//  GPIO18(SCK)  CLK
//  GPIO19(MOSI) MOSI
//  GPIO16(MISO) MISO(not use)
//  GND          GND

// Button それぞれGPIOxとGNDに接続（内部PULL UP設定あり）
//  Pico         Button
//  GPIO0        UP
//  GPIO1        LEFT
//  GPIO2        RIGHT
//  GPIO3        DOWN
//  GPIO4        START
//  GPIO5        FIRE

// スピーカー GPIO6

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "LCDdriver.h"
#include "graphlib.h"

extern const uint8_t *Image[10]; //駒画像ポインタの配列
extern const unsigned int Color[10]; //駒画像の色指定

// 入力ボタンのビット定義
#define GPIO_KEYUP 0
#define GPIO_KEYLEFT 1
#define GPIO_KEYRIGHT 2
#define GPIO_KEYDOWN 3
#define GPIO_KEYSTART 4
#define GPIO_KEYFIRE 5
#define KEYUP (1<<GPIO_KEYUP)
#define KEYLEFT (1<<GPIO_KEYLEFT)
#define KEYRIGHT (1<<GPIO_KEYRIGHT)
#define KEYDOWN (1<<GPIO_KEYDOWN)
#define KEYSTART (1<<GPIO_KEYSTART)
#define KEYFIRE (1<<GPIO_KEYFIRE)
#define KEYSMASK (KEYUP|KEYLEFT|KEYRIGHT|KEYDOWN|KEYSTART|KEYFIRE)

#define SOUNDPORT 6

#define clearscreen() LCD_Clear(0)

#define HAKOSIZEX 4 //箱の横サイズ
#define HAKOSIZEY 5 //箱の縦サイズ
#define GOALX 1 //ゴールX座標
#define GOALY 3 //ゴールY座標
#define BLOCKSIZEX 32 //駒の縦ドット数
#define BLOCKSIZEY 32 //駒の横ドット数
#define BLOCKNUM 10 //駒数

#define LEFTX 58 //箱表示 左上のX座標
#define TOPY 25 //箱表示 左上のY座標
#define FRAMESIZEX 7 //箱フレームの幅（上下）
#define FRAMESIZEY 7 //箱フレームの幅（左右）

#define COLOR_FRAME 12 //フレームの色
#define COLOR_BACK 13 //箱底板の色
#define COLOR_BLOCK1 9 //駒の色
#define COLOR_BLOCK2 10 //駒の色（影部分）
#define COLOR_BLOCK3 11 //駒の色（ハイライト部分）
#define COLOR_CURSOR 2 //選択中の駒を囲む色

// 駒の初期配置（数字は駒番号）
// 空き場所は負数
const int8_t map[HAKOSIZEY][HAKOSIZEX]={
	{1,0,0,2},
	{1,0,0,2},
	{3,4,4,5},
	{3,6,7,5},
	{8,-1,-1,9}
};

//駒の構造体定義
typedef struct {
	int x; //駒x座標
	int y; //駒y座標
	int xsize; //駒xサイズ
	int ysize; //駒yサイズ
	const int8_t *image; //駒画像へのポインタ
	unsigned int color; //駒画像の描画色
} _Block;

// グローバル変数定義
_Block block[BLOCKNUM]; //駒の状態を保持
int8_t hako[HAKOSIZEY][HAKOSIZEX]; //箱内の配置を保持
int cursorx,cursory; //カーソル選択位置
unsigned short step; //手数
int8_t lastblock; //前回移動した駒番号
unsigned short keystatus,keystatus2,oldkey; //最新のボタン状態と前回のボタン状態
int soundcount; //駒移動時の音声出力時間カウンタ

#define PWM_WRAP 4000 // 125MHz/31.25KHz
uint pwm_slice_num;

void sound_on(uint16_t f){
	pwm_set_clkdiv_int_frac(pwm_slice_num, f>>4, f&15);
	pwm_set_enabled(pwm_slice_num, true);
}
void sound_off(void){
	pwm_set_enabled(pwm_slice_num, false);
}
void wait60thsec(unsigned short n){
	// 60分のn秒ウェイト
	uint64_t t=to_us_since_boot(get_absolute_time())%16667;
	sleep_us(16667*n-t);
}
void keycheck(void){
//ボタン状態読み取り
//keystatus :現在押されているボタンに対応するビットを1にする
//keystatus2:前回押されていなくて、今回押されたボタンに対応するビットを1にする
	oldkey=keystatus;
	keystatus=~gpio_get_all() & KEYSMASK;
	keystatus2=keystatus & ~oldkey; //ボタンから手を離したかチェック
}
void putblock(int8_t d){
//駒番号dの駒を表示

	int x1,y1,x2,y2;
	int x,y,i;
	const uint8_t *p;
	uint8_t f;

	//(x1,y1)：駒左上座標、(x2,y2)：駒右下座標
	x1=LEFTX+FRAMESIZEX+block[d].x*BLOCKSIZEX;
	y1=TOPY+FRAMESIZEY+block[d].y*BLOCKSIZEY;
	x2=x1+block[d].xsize*BLOCKSIZEX-1;
	y2=y1+block[d].ysize*BLOCKSIZEY-1;

	//塗りつぶし長方形描画（上下左右に影、ハイライト付き）
	boxfill(x1,y1,x2,y2,COLOR_BLOCK1);
	boxfill(x1,y2-1,x2,y2,COLOR_BLOCK3);
	boxfill(x2-1,y1,x2,y2,COLOR_BLOCK3);
	boxfill(x1,y1,x1+1,y2,COLOR_BLOCK2);
	boxfill(x1,y1,x2,y1+1,COLOR_BLOCK2);

	//駒の上に画像（文字）を表示
	p=block[d].image;
	i=0;
	for(y=y1;y<=y2;y++){
		for(x=x1;x<=x2;x++){
			if((i&7)==0) f=*p++;
			if(f&0x80) pset(x,y,block[d].color);
			f<<=1;
			i++;
		}
	}
}
void eraseblock(int8_t d){
//駒番号dの駒を消去

	int x1,y1,x2,y2;

	//(x1,y1)：駒左上座標、(x2,y2)：駒右下座標
	x1=LEFTX+FRAMESIZEX+block[d].x*BLOCKSIZEX;
	y1=TOPY+FRAMESIZEY+block[d].y*BLOCKSIZEY;
	x2=x1+block[d].xsize*BLOCKSIZEX-1;
	y2=y1+block[d].ysize*BLOCKSIZEY-1;

	//塗りつぶし長方形描画（底板色）
	boxfill(x1,y1,x2,y2,COLOR_BACK);
}
void putspace(int x,int y){
//駒座標x,yの位置を空白で埋める（空白位置のカーソル消去で使用）

	int x1,y1,x2,y2;

	x1=LEFTX+FRAMESIZEX+x*BLOCKSIZEX;
	y1=TOPY+FRAMESIZEY+y*BLOCKSIZEY;
	x2=x1+BLOCKSIZEX-1;
	y2=y1+BLOCKSIZEY-1;
	boxfill(x1,y1,x2,y2,COLOR_BACK);
}
void drawcursor(void){
//カーソル表示
//選択中駒の周りを色で囲む
	int8_t d;
	int x1,y1,x2,y2;

	d=hako[cursory][cursorx];
	if(d>=0){
		//駒を選択中の場合、駒を四角で囲む
		x1=LEFTX+FRAMESIZEX+block[d].x*BLOCKSIZEX;
		y1=TOPY+FRAMESIZEY+block[d].y*BLOCKSIZEY;
		x2=x1+block[d].xsize*BLOCKSIZEX-1;
		y2=y1+block[d].ysize*BLOCKSIZEY-1;
	} else {
		//空き場所を選択中の場合、最小駒1つ分の大きさの四角を表示
		x1=LEFTX+FRAMESIZEX+cursorx*BLOCKSIZEX;
		y1=TOPY+FRAMESIZEY+cursory*BLOCKSIZEY;
		x2=x1+BLOCKSIZEX-1;
		y2=y1+BLOCKSIZEY-1;
	}
	//太さ2ドット分の長方形を表示
	boxfill(x1,y1,x1+1,y2,COLOR_CURSOR);
	boxfill(x2-1,y1,x2,y2,COLOR_CURSOR);
	boxfill(x1,y1,x2,y1+1,COLOR_CURSOR);
	boxfill(x1,y2-1,x2,y2,COLOR_CURSOR);
}

void move(void){
//駒またはカーソル移動
	int8_t d;
	int x,y,i,j;

	if(keystatus2 && (KEYUP | KEYDOWN |KEYLEFT | KEYRIGHT)==0) return;//方向ボタン押していない
	d=hako[cursory][cursorx]; //カーソル場所の駒番号
	if(keystatus & KEYFIRE){ //FIREボタンを押している場合
		//駒移動のチェックと移動
		if(d<0) return;//空き場所選択中の場合無効
		x=block[d].x;
		y=block[d].y;
		switch(keystatus2){
			case KEYUP: //上ボタンの場合
				y--;
				if(y<0) return; //一番上なので動かせない
				for(i=0;i<block[d].xsize;i++){
					if(hako[y][x+i]>=0) return; //上が詰まっているので動かせない
				}
				break;
			case KEYDOWN: //下ボタンの場合
				y+=block[d].ysize; //選択中の駒の1つ下のY座標
				if(y>=HAKOSIZEY) return; //一番下なので動かせない
				for(i=0;i<block[d].xsize;i++){
					if(hako[y][x+i]>=0) return; //下が詰まっているので動かせない
				}
				y=block[d].y+1;
				break;
			case KEYLEFT: //左ボタンの場合
				x--;
				if(x<0) return; //一番左なので動かせない
				for(i=0;i<block[d].ysize;i++){
					if(hako[y+i][x]>=0) return; //左が詰まっているので動かせない
				}
				break;
			case KEYRIGHT: //右ボタンの場合
				x+=block[d].xsize; //選択中の駒の1つ右のX座標
				if(x>=HAKOSIZEX) return; //一番右なので動かせない
				for(i=0;i<block[d].ysize;i++){
					if(hako[y+i][x]>=0) return; //右が詰まっているので動かせない
				}
				x=block[d].x+1;
				break;
			default:
				return;
		}

	//駒の移動
		//いったん駒を消去
		for(i=block[d].y;i<block[d].y+block[d].ysize;i++){
			for(j=block[d].x;j<block[d].x+block[d].xsize;j++){
				hako[i][j]=-1;
			}
		}
		eraseblock(d); //駒の表示消去
		block[d].x=x; //移動後の座標に変更
		block[d].y=y;

		//箱の中身を変更
		for(i=y;i<y+block[d].ysize;i++){
			for(j=x;j<x+block[d].xsize;j++){
				hako[i][j]=d;
			}
		}
		putblock(d); //駒を表示
		cursorx=x; //カーソルも合わせて移動
		cursory=y;
		drawcursor(); //カーソル表示
		soundcount=7; //効果音の持続時間設定（ループ7回分）

		//手数更新
		//前回と同じ駒を移動した場合は、1回の移動とみなすので更新しない
		if(lastblock!=d){
			lastblock=d;
			step++;
			printnum2(144,248,7,0,step,6);//6桁の数字を表示
		}
	}
	else{
		//カーソル移動
		x=cursorx;
		y=cursory;
		switch(keystatus2){
			case KEYUP: //上ボタンの場合
				while(y>0){
					//1つ上に移動。カーソル位置の駒と同じ場合は更に上へ。一番上の場合は動かない
					y--;
					if(hako[y][x]!=d || hako[y][x]<0) break;
				}
				break;
			case KEYDOWN: //下ボタンの場合
				while(y<HAKOSIZEY-1){
					//1つ下に移動。カーソル位置の駒と同じ場合は更に下へ。一番下の場合は動かない
					y++;
					if(hako[y][x]!=d || hako[y][x]<0) break;
				}
				break;
			case KEYLEFT: //左ボタンの場合
				while(x>0){
					//1つ左に移動。カーソル位置の駒と同じ場合は更に左へ。一番左の場合は動かない
					x--;
					if(hako[y][x]!=d || hako[y][x]<0) break;
				}
				break;
			case KEYRIGHT: //右ボタンの場合
				while(x<HAKOSIZEX-1){
					//1つ右に移動カーソル位置の駒と同じ場合は更に右へ。一番右の場合は動かない
					x++;
					if(hako[y][x]!=d || hako[y][x]<0) break;
				}
				break;
			default:
				return;
		}
		if(x==cursorx && y==cursory) return; //移動がなかった場合終わり
		if(d>=0){ //カーソル移動前の場所が駒の場合
			putblock(d); //番号dの駒を再表示（カーソルを消すため）
		}
		else{ //カーソル移動前が空き場所の場合
			putspace(cursorx,cursory); //カーソルを消すためスペースを表示
		}
		cursorx=x; //カーソルを移動
		cursory=y;
		drawcursor(); //移動後の場所にカーソルを表示
	}
}
int goalcheck(void){
//パズル完成チェック
//戻り値　未完成:0、完成:1
	int i;

	if(block[0].x !=GOALX || block[0].y!=GOALY) return 0;
	//block[0]＝「娘」の駒

	//完成の場合の処理
	//停止して「ピンポンピンポン」
	sound_on(1012); //音程Bの音を鳴らす
	wait60thsec(10); //60分の10秒待ち
	sound_on(1276); //音程Gの音を鳴らす
	wait60thsec(10); //60分の10秒待ち
	sound_on(1012); //音程Bの音を鳴らす
	wait60thsec(10); //60分の10秒待ち
	sound_on(1276); //音程Gの音を鳴らす
	wait60thsec(10); //60分の10秒待ち
	sound_off(); //音声停止

	eraseblock(0); //「娘」の駒の表示消去
	block[0].y++;
	putblock(0); //1ブロック下に移動して表示
	printstr(64,4,7,0,"CONGRATULATIONS!"); //文字列表示

	//スタートボタンが押されるまで、0.5秒ごとに文字列表示と消去を繰り返す
	i=0;
	while(!(keystatus2 & KEYSTART)){
		if(i==0) printstr(64,14,6,0,"PLAY AGAIN:START");
		else if(i==30) printstr(64,14,0,0,"                ");
		if(++i==60) i=0;
		wait60thsec(1);//ビデオ信号出力終了待ち（60分の1秒ウェイト代わり）
		keycheck();
	}
	return 1; //パズルが完成したことをメインルーチンに返す
}

void sound(void){
//効果音出力
	unsigned short pr;

	if(soundcount==0) return; //効果音なしの時
	soundcount--;
	//最初の60分の3秒間はタイマー周期1000、残りの60分の3秒間はタイマー周期900で
	//方形波を鳴らす
	if(soundcount==0) pr=0; //サウンド停止
	else if(soundcount>=4) pr=1000;
	else pr=900;

	if(pr>0) sound_on(pr); //実際に周期変更
	else sound_off();
}

void gameinit2(void){
//ゲームスタート時に呼ばれる初期化
	int i,j;
	int8_t d;

	clearscreen();//画面消去
	//ここから箱の表示
	//外枠描画
	boxfill(LEFTX,TOPY,
		LEFTX+HAKOSIZEX*BLOCKSIZEX+FRAMESIZEX*2-1,TOPY+HAKOSIZEY*BLOCKSIZEY+FRAMESIZEY*2-1,COLOR_FRAME);
	//底板描画
	boxfill(LEFTX+FRAMESIZEX,TOPY+FRAMESIZEY,
		LEFTX+HAKOSIZEX*BLOCKSIZEX+FRAMESIZEX-1,TOPY+HAKOSIZEY*BLOCKSIZEY+FRAMESIZEY-1,COLOR_BACK);
	//ゴール描画
	boxfill(LEFTX+FRAMESIZEX+GOALX*BLOCKSIZEX,TOPY+HAKOSIZEY*BLOCKSIZEY+FRAMESIZEY,
		LEFTX+FRAMESIZEX+GOALX*BLOCKSIZEX+block[0].xsize*BLOCKSIZEX-1,TOPY+HAKOSIZEY*BLOCKSIZEY+FRAMESIZEY*2-1,COLOR_BACK);

	//手数表示
	printstr(160,240,7,0,"STEP");
	printnum2(144,248,7,0,0,6);

	// 駒を初期配置にする
	// block.x, block.yを駒の左上座標に設定
	for(d=0;d<BLOCKNUM;d++){
		block[d].x=-1;//いったん全てあり得ない値でクリア
	}
	for(i=0;i<HAKOSIZEY;i++){
		for(j=0;j<HAKOSIZEX;j++){
			//初期配置をコピー
			d=map[i][j];
			hako[i][j]=d;

			//左上座標に設定
			if(d>=0 && block[d].x==-1){
				block[d].x=j;
				block[d].y=i;
			}
		}
	}
	for(d=0;d<BLOCKNUM;d++) putblock(d);//駒を表示
	step=0;//手数クリア
	lastblock=BLOCKNUM;//あり得ない番号を指定
	cursorx=0;//カーソル初期位置設定
	cursory=4;
	soundcount=0;//効果音は鳴っていない
}

void gameinit(void){
//起動時1回だけ呼ばれる初期化
	int i,j,k;
	int8_t d;

	//駒サイズ、画像の初期化
	for(d=0;d<BLOCKNUM;d++){
		block[d].xsize=0;//いったんあり得ない値でクリア
		block[d].image=Image[d];//駒画像（文字）へのポインタ
		block[d].color=Color[d];//駒画像の色
	}
	for(i=0;i<HAKOSIZEY;i++){
		for(j=0;j<HAKOSIZEX;j++){
			d=map[i][j];
			if(d>=0 && block[d].xsize==0){
				//駒の横サイズ、縦サイズを探索して設定する
				block[d].xsize=1;
				block[d].ysize=1;
				for(k=j+1;k<HAKOSIZEX && map[i][k]==d;k++) block[d].xsize++;
				for(k=i+1;k<HAKOSIZEY && map[k][j]==d;k++) block[d].ysize++;
			}
		}
	}
	//カラーパレット設定
	//set_palette(色番号,B,R,G)
	set_palette(COLOR_BLOCK1,180,240,210);
	set_palette(COLOR_BLOCK2,150,200,170);
	set_palette(COLOR_BLOCK3,200,255,230);
	set_palette(COLOR_FRAME,0,255,140);
	set_palette(COLOR_BACK,120,180,130);

	//ボタン連続押し防止の初期設定
	keystatus=KEYUP | KEYDOWN | KEYLEFT | KEYRIGHT | KEYSTART | KEYFIRE;
}
void main(void){
    stdio_init_all();

	// ボタン用GPIO設定
	gpio_init_mask(KEYSMASK);
	gpio_set_dir_in_masked(KEYSMASK);
	gpio_pull_up(GPIO_KEYUP);
	gpio_pull_up(GPIO_KEYLEFT);
	gpio_pull_up(GPIO_KEYRIGHT);
	gpio_pull_up(GPIO_KEYDOWN);
	gpio_pull_up(GPIO_KEYSTART);
	gpio_pull_up(GPIO_KEYFIRE);

	// サウンド用PWM設定
	gpio_set_function(SOUNDPORT, GPIO_FUNC_PWM);
	pwm_slice_num = pwm_gpio_to_slice_num(SOUNDPORT);
	pwm_set_wrap(pwm_slice_num, PWM_WRAP-1);
	// duty 50%
	pwm_set_chan_level(pwm_slice_num, PWM_CHAN_A, PWM_WRAP/2);

	// 液晶用ポート設定
    // Enable SPI 0 at 40 MHz and connect to GPIOs
    spi_init(SPICH, 40000 * 1000);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

	gpio_init(LCD_CS);
	gpio_put(LCD_CS, 1);
	gpio_set_dir(LCD_CS, GPIO_OUT);
	gpio_init(LCD_DC);
	gpio_put(LCD_DC, 1);
	gpio_set_dir(LCD_DC, GPIO_OUT);
	gpio_init(LCD_RESET);
	gpio_put(LCD_RESET, 1);
	gpio_set_dir(LCD_RESET, GPIO_OUT);

	init_graphic(); //液晶利用開始
	LCD_WriteComm(0x37); //画面中央にするためスクロール設定
	LCD_WriteData2(272);

	//ここからゲーム処理
	gameinit();//最初の1回だけの初期化
	while(1){
		gameinit2();//ゲーム開始ごとの初期化
		drawcursor();//カーソル枠表示

		//ここからメインループ
		while(1){
			wait60thsec(1);//ビデオ信号出力終了待ち（60分の1秒ウェイト代わり）
			keycheck();	//ボタン読み取り
			move();		//駒またはカーソル移動
			sound();	//効果音を鳴らす
			if(goalcheck()) break; //パズルが解けたらループを抜ける
		}
		sound_off();	//サウンド停止
	}
}

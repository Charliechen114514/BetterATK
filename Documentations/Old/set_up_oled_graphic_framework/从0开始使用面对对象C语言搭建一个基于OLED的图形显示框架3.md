# 从0开始使用面对对象C语言搭建一个基于OLED的图形显示框架（OLED设备层驱动封装）

## OLED设备层驱动开发

​	现在，我们终于来到了最难的设备层驱动开发。在这里，我们抽象出来了一个叫做OLED_Device的东西，我们终于可以关心的是一块OLED，他可以被打开，被设置，被关闭，可以绘制点，可以绘制面，可以清空，可以反色等等。（画画不是这个层次该干的事情，要知道，绘制一个图形需要**从这个设备可以被绘制开始，也就是他可以画点，画面开始**！）

​	所以，离我在这篇总览中https://blog.csdn.net/charlie114514191/article/details/145397231提到的绘制一个多级菜单还是有一些遥远的。饭一口口吃，事情一步步做，这急不得，一着急反而会把我们精心维护的抽象破坏掉。

​	代码在[MCU_Libs/OLED/library/OLED at main · Charliechen114514/MCU_Libs (github.com)](https://github.com/Charliechen114514/MCU_Libs/tree/main/OLED/library/OLED)，两个文件夹都有所涉及，所以本篇的代码量会非常巨大。请各位看官合理安排。

### 如何抽象一个OLED

​	协议层上，我们抽象了一个IIC协议。现在在设备层上，我们将进一步抽象一个OLED。上面笔者提到了，一个OLED可以被开启，关闭，画点画面，反色等等操作，他能干！他如何干是我们马上要做的事情。现在，我们需要一个OLED句柄。这个OLED句柄代表了**背后使用的通信协议和它自身相关的属性信息**，而不必要外泄到其他模块上去。所以，封装一个这样的抽象变得很有必要。

​	OLED的品种很多，分法也很多，笔者顺其自然，打算封装一个这样的结构体

```
typedef struct __OLED_Handle_Type{
    /* driver types announced the way we explain the handle */
    OLED_Driver_Type        stored_handle_type;
    /* handle data types here */
    OLED_Handle_Private     private_handle;
}OLED_Handle;
```

​	让我来解释一下：首先，我们的OLED品种很多，程序如何知道你的OLED如何被解释呢？stored_handle_type标识的类型来决定采取何种行动解释。。。什么呢？解释我们的private_handle。

```
typedef enum {
    OLED_SOFT_IIC_DRIVER_TYPE,
    OLED_HARD_IIC_DRIVER_TYPE,
    OLED_SOFT_SPI_DRIVER_TYPE,
    OLED_HARD_SPI_DRIVER_TYPE
}OLED_Driver_Type;

/*  
    to abstract the private handle base 
    this is to isolate the dependencies of
    the real implementations
*/
typedef void* OLED_Handle_Private;
```

​	也就是说，笔者按照采取的协议进行抽象，将OLED**本身的信息属性差异封装到文件内部去**，作为使用不同的片子，只需要使用编译宏编译不同的文件就好了。现在，OLED_Handle就是我们的OLED，拿到这个结构体，我们就掌握了整个OLED。所以，整个OLED结构体必然可以做到如下的事情

```
#ifndef OLED_BASE_DRIVER_H
#define OLED_BASE_DRIVER_H

#include "oled_config.h"

typedef struct __OLED_Handle_Type{
    /* driver types announced the way we explain the handle */
    OLED_Driver_Type        stored_handle_type;
    /* handle data types here */
    OLED_Handle_Private     private_handle;
}OLED_Handle;

/*
    oled_init_hardiic_handle registers the hardiic commnications
handle: 
    Pointer to an OLED_Handle structure that represents the handle 
    for the OLED display, used for managing 
    and controlling the OLED device.
    programmers should pass a blank one!

config: 
    Pointer to an OLED_HARD_IIC_Private_Config structure 
    that contains the configuration settings 
    for initializing the hardware interface, 
    typically related to the I2C communication 
    parameters for the OLED display.
*/
// 按照硬件IIC进行初始化
void oled_init_hardiic_handle(
    OLED_Handle* handle, 
    OLED_HARD_IIC_Private_Config* config);

/*
    oled_init_hardiic_handle registers the hardiic commnications
handle: 
    Pointer to an OLED_Handle structure that represents the handle 
    for the OLED display, used for managing 
    and controlling the OLED device.
    programmers should pass a blank one!

config: 
    Pointer to an OLED_SOFT_IIC_Private_Config structure 
    that contains the configuration settings 
    for initializing the hardware interface, 
    typically related to the I2C communication 
    parameters for the OLED display.
*/
// 按照软件IIC进行初始化
void oled_init_softiic_handle(
    OLED_Handle* handle,
    OLED_SOFT_IIC_Private_Config* config
);

/* 可以清空 */
void oled_helper_clear_frame(OLED_Handle* handle);
void oled_helper_clear_area(OLED_Handle* handle, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/* 需要刷新，这里采用了缓存机制 */
void oled_helper_update(OLED_Handle* handle);
void oled_helper_update_area(OLED_Handle* handle, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/* 可以反色 */
void oled_helper_reverse(OLED_Handle* handle);
void oled_helper_reversearea(OLED_Handle* handle, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/* 可以绘制 */
void oled_helper_setpixel(OLED_Handle* handle, uint16_t x, uint16_t y);
void oled_helper_draw_area(OLED_Handle* handle, 
        uint16_t x, uint16_t y, uint16_t width, 
        uint16_t height, uint8_t* sources);

/* 自身的属性接口，是我们之后要用的 */
uint8_t     oled_support_rgb(OLED_Handle* handle);
uint16_t    oled_width(OLED_Handle* handle);
uint16_t    oled_height(OLED_Handle* handle);

#endif
```

​	说完了接口，下面就是实现了。

### 完成OLED的功能

#### 初始化OLED

​	整个事情我们终于开始翻开我们的OLED手册了。我们的OLED需要一定的初始化。让我们看看江科大代码是如何进行OLED的初始化。

```
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置
 
	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);
 
	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);
 
	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);
 
	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭
 
	OLED_WriteCommand(0xA6);	//设置正常/倒转显示
 
	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);
 
	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}
```

​	好长一大串，麻了，代码真的不好看。我们为什么不使用数组进行初始化呢？

```
uint8_t oled_init_commands[] = {
    0xAE,  // Turn off OLED panel
    0xFD, 0x12,  // Set display clock divide ratio/oscillator frequency
    0xD5,  // Set display clock divide ratio
    0xA0,  // Set multiplex ratio
    0xA8,  // Set multiplex ratio (1 to 64)
    0x3F,  // 1/64 duty
    0xD3,  // Set display offset
    0x00,  // No offset
    0x40,  // Set start line address
    0xA1,  // Set SEG/Column mapping (0xA0 for reverse, 0xA1 for normal)
    0xC8,  // Set COM/Row scan direction (0xC0 for reverse, 0xC8 for normal)
    0xDA,  // Set COM pins hardware configuration
    0x12,  // COM pins configuration
    0x81,  // Set contrast control register
    0xBF,  // Set SEG output current brightness
    0xD9,  // Set pre-charge period
    0x25,  // Set pre-charge as 15 clocks & discharge as 1 clock
    0xDB,  // Set VCOMH
    0x34,  // Set VCOM deselect level
    0xA4,  // Disable entire display on
    0xA6,  // Disable inverse display on
    0xAF   // Turn on the display
};
#define CMD_TABLE_SZ ( (sizeof(oled_init_commands)) / sizeof(oled_init_commands[0]) )
```

​	现在，我们只需要按部就班的按照顺序发送我们的指令。以hardiic的初始化为例子

```
void oled_init_hardiic_handle(
    OLED_Handle* handle, 
    OLED_HARD_IIC_Private_Config* config)
{
	// 传递使用的协议句柄, 以及告知我们的句柄类型 
    handle->private_handle = config;
    handle->stored_handle_type = OLED_HARD_IIC_DRIVER_TYPE;
    // 按部就班的发送命令表
    for(uint8_t i = 0; i < CMD_TABLE_SZ; i++)
    	// 这里我们协议的send_command就发力了, 现在我们完全不关心他是如何发送命令的
        config->operation.command_sender(config, oled_init_commands[i]);
    // 把frame清空掉
    oled_helper_clear_frame(handle);
    // 把我们的frame commit上去
    oled_helper_update(handle);
}
```

​	这里我们还剩下最后两行代码没解释，为什么是oled_helper_clear_frame和update要分离开来呢？我们知道，频繁的刷新OLED屏幕非常占用我们的单片机内核，也不利于我们合并绘制操作。比如说，我想绘制两个圆，为什么不画完一起更新上去呢？比起来画一个点更新一下，这个操作显然更合理。所以，为了完成这样的技术，我们需要一个Buffer缓冲区。

```
uint8_t OLED_GRAM[OLED_HEIGHT][OLED_WIDTH];
```

​	他就承担了我们的缓存区。多大呢？这个事情跟OLED的种类有关系，一些OLED的大小是128 x 64，另一些是144 x 64，无论如何，我们需要根据chip的种类，来选择我们的OLED的大小，更加严肃的说，是OLED的属性和它的功能。

​	所以，这就是为什么笔者在[MCU_Libs/OLED/library/OLED/Driver/oled_config.h at main · Charliechen114514/MCU_Libs (github.com)](https://github.com/Charliechen114514/MCU_Libs/blob/main/OLED/library/OLED/Driver/oled_config.h)文件中，引入了这样的控制宏

```
#ifndef SSD1306_H
#define SSD1306_H

/* hardware level defines */
#define PORT_SCL    GPIOB
#define PORT_SDA    GPIOB
#define PIN_SCL     GPIO_PIN_8
#define PIN_SDA     GPIO_PIN_9

#define OLED_ENABLE_GPIO_SCL_CLK() __HAL_RCC_GPIOB_CLK_ENABLE()
#define OLED_ENABLE_GPIO_SDA_CLK() __HAL_RCC_GPIOB_CLK_ENABLE()

#define OLED_WIDTH  (128)
#define OLED_HEIGHT (8)

#define POINT_X_MAX     (OLED_WIDTH)
#define POINT_Y_MAX     (OLED_HEIGHT * 8)

#endif
```

​	这个文件是ssd1306.h，这个文件专门承载了关于SSD1306配置的一切。现在，我们将OLED的配置系统建立起来了，当我们的chip是SSD1306的时候，只需要定义SSD1306的宏

```
#ifndef OLED_CONFIG_H
#define OLED_CONFIG_H

...

/* oled chips selections */

#ifdef SSD1306

#include "configs/ssd1306.h"

#elif SSD1309
#include "configs/ssd1309.h"
#else
#error "Unknown chips, please select in compile time using define!"
#endif

#endif
```

​	现在，我们的configure就完整了，我们只需要依赖config文件就能知道OLED自身的全部信息。如果你有IDE，现在就可以看到，当我们定义了SSD1306的时候，我们的OLED_GRAM自动调整为`OLED_GRAM[8][128]`的数组，另一放面，如果我们使用了SSD1309，我们自动会更新为`OLED_GRAM[8][144]`,此事在ssd1309.h中亦有记载

#### 清空屏幕

​	显然，我们有一些人对C库并不太了解，memset函数负责将一块内存设置为给定的值。一般而言，编译器实现将会使用独有的硬件加速优化，使用上，绝对比手动设置值只快不慢。

> 软件工程的一大原则：复用！能不自己手搓就不自己手搓，编译器提供了就优先使用编译器提供的

```
void oled_helper_clear_frame(OLED_Handle* handle)
{
    memset(OLED_GRAM, 0, sizeof(OLED_GRAM));
}
```

#### 刷新屏幕与光标设置1

​	设置涂写光标，就像我们使用Windows的绘图软件一样，鼠标在哪里，左键嗯下就从那里开始绘制，我们的set_cursor函数就是干设置鼠标在哪里的工作。查询手册，我们可以这样书写（笔者是直接参考了江科大的实现）

```
/*
    set operating cursor
*/
void __pvt_oled_set_cursor(
    OLED_Handle* handle, 
    const uint8_t y,
    const uint8_t x)
{   
    const uint8_t new_x = x + 2;
    OLED_Operations op_table;
    __on_fetch_oled_table(handle, &op_table);
    op_table.command_sender(handle->private_handle, 0xB0 | y);
	op_table.command_sender(handle->private_handle,0x10 | ((new_x & 0xF0) >> 4));	//设置X位置高4位
	op_table.command_sender(handle->private_handle,0x00 | (new_x & 0x0F));			//设置X位置低4位
}
```

#### 刷新屏幕与光标设置2

​	不对，这个代码没有看懂！其一原因是我没有给出__on_fetch_oled_table是什么。

```
static void __on_fetch_oled_table(
    const OLED_Handle* handle, 
    OLED_Operations* blank_operations)
{
    switch (handle->stored_handle_type)
    {
        case OLED_HARD_IIC_DRIVER_TYPE:
        {
            OLED_HARD_IIC_Private_Config* config = 
                (OLED_HARD_IIC_Private_Config*)(handle->private_handle);
            blank_operations->command_sender = config->operation.command_sender;
            blank_operations->data_sender = config->operation.data_sender;
        }break;
        case OLED_SOFT_IIC_DRIVER_TYPE:
        {
            OLED_SOFT_IIC_Private_Config* config = 
                (OLED_SOFT_IIC_Private_Config*)(handle->private_handle);
            blank_operations->command_sender = config->operation.command_sender;
            blank_operations->data_sender = config->operation.data_sender;
        }break;
		... // ommited spi seletctions
        }break;
        default:
            break;
    }
}
```

​	这是干什么呢？答案是：根据OLED的类型，选择我们的操作句柄。这是因为C语言没法自动识别void*的原貌是如何的，我们必须将`C++`中的虚表选择手动的完成

> 题外话：接触过C++的朋友都知道继承这个操作，实际上，这里就是一种继承。无论是何种IIC操作，都是IIC操作。他都必须遵守可以发送字节的接口操作，现在的问题是：他到底是哪样的IIC？需要执行的是哪样IIC的操作呢？所以，__on_fetch_oled_table就是把正确的操作函数根据OLED的类型给筛选出来。也就是C++中的虚表选择操作

```
/*
    set operating cursor
*/
void __pvt_oled_set_cursor(
    OLED_Handle* handle, 
    const uint8_t y,
    const uint8_t x)
{   
    const uint8_t new_x = x + 2;
    OLED_Operations op_table;
    __on_fetch_oled_table(handle, &op_table);
    op_table.command_sender(handle->private_handle, 0xB0 | y);
	op_table.command_sender(handle->private_handle,0x10 | ((new_x & 0xF0) >> 4));	//设置X位置高4位
	op_table.command_sender(handle->private_handle,0x00 | (new_x & 0x0F));			//设置X位置低4位
}
```

​	现在回到上面的代码，我们将正确的操作句柄选择出来之后，可以发送设置“鼠标”的指令了。

> 复习一下位操作的基本组成
>
> - &是一种萃取操作，任何数&0就是0，&1则是本身，说明可以通过对应&1保留对应位，&0抹除对应位
> - |是一种赋值操作，任何数&1就是1，|0是本身，所以|可以起到对应位置1的操作。
>
> 所以，保留高4位只需要 & 0xF0（0b11110000），保留低四位只需要&0x0F就好了（0b00001111）

#### 刷新屏幕与光标设置3

​	现在让我们看看刷新屏幕是怎么做的

```
void oled_helper_update(OLED_Handle* handle)
{
    OLED_Operations op_table;
    __on_fetch_oled_table(handle, &op_table);
    	
	for (uint8_t j = 0; j < OLED_HEIGHT; j ++)
	{
		/*设置光标位置为每一页的第一列*/
		__pvt_oled_set_cursor(handle, j, 0);
		/*连续写入128个数据，将显存数组的数据写入到OLED硬件*/
		// 有趣的是,这里笔者埋下了一个伏笔,我为什么没写OLED_WIDTH呢?尽管在SSD1306这样做是正确的
		// 但那也是偶然,笔者在移植SSD1309的时候就发现了这样的不一致性,导致OLED死机.
		// 笔者提示: OLED长宽和可绘制区域的大小不一致性
		op_table.data_sender(handle->private_handle, OLED_GRAM[j], 128);
	}
}
```

​	刷新整个屏幕就是将鼠标设置到开头，然后直接向后面写入128个数据结束我们的事情，这比一个个写要快得多！

#### 绘制一个点

​	实际上，就是将对应的数组的位置放上1就好了，这需要牵扯到的是OLED独特的显示方式。

​	OLED自身分有页这个概念，一个页8个像素，由传递的比特控制。举个例子，我想显示的是第一个像素亮起来，就需要在一个字节的第一个比特置1余下置0，这就是为什么OLED_HEIGHT的大小不是64而是8，也就意味着setpixel函数不是简单的

```
OLED[height][width] = val
```

​	而实需要进行一个复杂的计算。我们分析一下，给定一个Y的值。它落在的页就是 Y / 8。比如说，Y为5的时候落在第0页的第六个比特上，Y为9的时候落在第一个页的第一个第二个比特上（注意我们的Y从0开始计算），我们设置的位置也就是：`OLED_GRAM[y / 8][x]`，设置的值就是Y给定的比特是`0x01 << (y % 8)`

```
void oled_helper_setpixel(OLED_Handle* handle, uint16_t x, uint16_t y)
{
    // current unused
    (void)handle;
    if( 
        0 <= x && x <= POINT_X_MAX &&
        0 <= y && y <= POINT_Y_MAX
    )
        OLED_GRAM[y / 8][x] |= 0x01 << (y % 8);
}
```

> (void)T是一种常见的放置maybe_unused的写法，现代编译器支持`[[maybe_unused]]`的指示符，表达的是这个参数可能不被用到，编译器不需要为此警告我，这在复用中很常见，一些接口的参数可能不被使用，这样的可读性会比传递空更加的好读，为了遵循ISO C，笔者没有采取，保证任何编译器都可以正确的理解我们的意图。

#### 反色

​	反色就很简单了。只需要异或即可，首先，当给定的比特是0的时候，我们异或1，得到的就是相异的比较，所以结果是1：即0变成了1。我们给定的比特是1的时候，我们还是异或1，得到了相同的结果，所以结果是0，即1变成了0，这样不就实现了一个像素的反转吗！

```
void oled_helper_reverse(OLED_Handle* handle)
{
    for(uint8_t i = 0; i < OLED_HEIGHT; i++)
    {
        for(uint8_t j = 0; j < OLED_WIDTH; j++)
        {
            OLED_GRAM[i][j] ^= 0xFF;
        }
    }
}
```

> 能使用memset吗？为什么？所以memset是在什么情况下能使用呢？
>
> 我都这样问了，那显然不能，因为设置的值跟每一个字节的内存强相关，memset的值必须跟内存的值没有关系。

#### 区域化操作

​	我们还有区域化操作没有实现。基本的步骤是

> 思考需要的参数：需要知道对
>
> - 哪个OLED：OLED_Handle* handle,
> - 起头在哪里：uint16_t x, uint16_t y, 
> - 长宽如何：uint16_t width, uint16_t height
> - 对于置位，则需要一个连续的数组进行置位，它的大小就是描述了区域矩形的大小

​	我们先来看置位函数

##### 区域置位

```
void oled_helper_draw_area(OLED_Handle* handle, 
        uint16_t x, uint16_t y, uint16_t width, 
        uint16_t height, uint8_t* sources)
{
    // 确保绘制区域的起点坐标在有效范围内，如果超出最大显示坐标则直接返回
    if(x > POINT_X_MAX)  return;
    if(y > POINT_Y_MAX)  return;

    // 在设置图像前，先清空绘制区域
    oled_helper_clear_area(handle, x, y, width, height); 

    // 遍历绘制区域的高度，以8像素为单位划分区域
    for(uint16_t j = 0; j < (height - 1) / 8 + 1; j++)
    {
        for(uint16_t i = 0; i < width; i++)
        {
            // 如果绘制超出屏幕宽度，则跳出循环
            if(x + i > OLED_WIDTH) { break; }
            // 如果绘制超出屏幕高度，则直接返回
            if(y / 8 + j > OLED_HEIGHT - 1) { return; }

            // 将sources中的数据按位移方式写入OLED显存GRAM
            // 当前行显示，低8位数据左移与显存当前内容进行按位或
            OLED_GRAM[y / 8 + j][x + i] |= sources[j * width + i] << (y % 8);

            // 如果绘制数据跨页（8像素一页），处理下一页的数据写入
            if(y / 8 + j + 1 > OLED_HEIGHT - 1) { continue; }

            // 将高8位数据右移后写入下一页显存
            OLED_GRAM[y / 8 + j + 1][x + i] |= sources[j * width + i] >> (8 - y % 8);
        }
    }
}
```

```
            // 如果绘制超出屏幕宽度，则跳出循环
            if(x + i > OLED_WIDTH) { break; }
            // 如果绘制超出屏幕高度，则直接返回
            if(y / 8 + j > OLED_HEIGHT - 1) { return; }

            // 将sources中的数据按位移方式写入OLED显存GRAM
            // 当前行显示，低8位数据左移与显存当前内容进行按位或
            OLED_GRAM[y / 8 + j][x + i] |= sources[j * width + i] << (y % 8);

            // 如果绘制数据跨页（8像素一页），处理下一页的数据写入
            if(y / 8 + j + 1 > OLED_HEIGHT - 1) { continue; }

            // 将高8位数据右移后写入下一页显存
            OLED_GRAM[y / 8 + j + 1][x + i] |= sources[j * width + i] >> (8 - y % 8);
```

​	我们正常来讲，传递的会是一个二维数组，C语言对于二维数组的处理是连续的。也就是说。对于一个被声明为`OLED[WIDTH][HEIGHT]`的数组，访问`OLED[i][j]`本质上等价于`OLED + i * WIDTH + j`，这个事情如果还是不能理解可以查照专门的博客进行学习。笔者默认在这里看我写的东西已经不会被这样基础的知识所困扰了。所以，我们的所作的就是将出于低页的内容拷贝到底页上

> `OLED_GRAM[y / 8 + j][x + i]`：这是显存二维数组的索引访问。
>
> - `y / 8 + j` 计算出当前数据位于哪个页（OLED通常按8个像素一页分块存储），通过整除将 `y` 坐标映射到显存页。
> - `x + i` 表示横向的列位置。
>
> `sources[j * width + i]`：这是源图像数据数组的索引访问。
>
> - `j * width + i` 计算当前像素在 `sources` 数据中的位置偏移。
>
> `<< (y % 8)`：将当前像素数据向左移动 `(y % 8)` 位，以确保源数据对齐到目标位置。
>
> - `y % 8` 获取绘制的起点在当前页中的垂直偏移。
>
> `|=`：按位或运算符，将偏移后的数据合并到 `OLED_GRAM` 中现有内容。
>
> 如果 `y = 5`，那么 `y % 8 = 5`，表示当前像素从第5位开始绘制。例如：
>
> - 如果 `sources[j * width + i]` 的值是 `0b11000000`，经过 `<< 5` 位移后变为 `0b00000110`，再与 `OLED_GRAM` 的原有数据合并，从而只影响目标位置上的两个像素。

​	先试一下分析`OLED_GRAM[y / 8 + j + 1][x + i] |= sources[j * width + i] >> (8 - y % 8);`，笔者的分析如下

> 1. `OLED_GRAM[y / 8 + j + 1][x + i]`：
>    - 这是下一页显存中的对应位置。
>    - `y / 8 + j + 1` 表示当前绘制位置的下一页。
>    - `x + i` 仍为当前列位置。
> 2. `sources[j * width + i]`：
>    - 源图像数据中当前像素的数据。
>    - `j * width + i` 计算出当前像素在源数据中的位置。
> 3. `>> (8 - y % 8)`：
>    - 将数据右移 `(8 - y % 8)` 位，将超出当前页的高位部分对齐到下一页。
>    - `8 - y % 8` 计算需要移入下一页的位数。
> 4. `|=`：
>    - 按位或，将偏移后的数据合并到下一页显存中，以保留已有内容。
>
> 假设 `y = 5`，那么 `8 - y % 8 = 3`。如果 `sources[j * width + i]` 为 `0b10110000`，右移 3 位得到 `0b00010110`，这部分数据写入下一页显存。

##### 区域反色

```
void oled_helper_reversearea(OLED_Handle* handle, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    // 确认起点坐标是否超出有效范围
    if(x > POINT_X_MAX)  return;
    if(y > POINT_Y_MAX)  return;

    // 确保绘制区域不会超出最大范围，如果超出则调整宽度和高度
    if(x + width > POINT_X_MAX)     width = POINT_X_MAX - x;
    if(y + height > POINT_Y_MAX)    height = POINT_Y_MAX - y;

    // 遍历高度范围中的每个像素行
    for(uint8_t i = y; i < y + height; i++)
    {
        for(uint8_t j = x; j < x + width; j++)
        {
            // 反转显存GRAM中的指定像素位（按位异或）
            OLED_GRAM[i / 8][j] ^= (0x01 << (i % 8));
        }
    }
}
```

##### 区域更新

```
void oled_helper_update_area(OLED_Handle* handle, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    // 检查起点坐标是否超出有效范围
    if(x > POINT_X_MAX)  return;
    if(y > POINT_Y_MAX)  return;

    // 确认绘制区域不超出最大范围
    if(x + width > POINT_X_MAX)     width = POINT_X_MAX - x;
    if(y + height > POINT_Y_MAX)    height = POINT_Y_MAX - y;

    // 定义OLED操作表变量
    OLED_Operations op_table;
    // 获取对应的操作函数表
    __on_fetch_oled_table(handle, &op_table);

    // 遍历绘制区域中的每个页（8像素一页）
    for(uint8_t i = y / 8; i < (y + height - 1) / 8 + 1; i++)
    {
        // 设置光标到指定页及列的位置
        __pvt_oled_set_cursor(handle, i, x);
        // 从显存中读取指定页和列的数据，通过data_sender发送到OLED硬件
        op_table.data_sender(handle, &OLED_GRAM[i][x], width);        
    }
}
```

​	也就是将光标对应到位置上刷新width个数据，完事！

#### 区域清空

```
void oled_helper_clear_area(OLED_Handle* handle, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    // 检查起点坐标是否超出有效范围
    if(x > POINT_X_MAX)  return;
    if(y > POINT_Y_MAX)  return;

    // 确保绘制区域不超出最大范围
    if(x + width > POINT_X_MAX)     width = POINT_X_MAX - x;
    if(y + height > POINT_Y_MAX)    height = POINT_Y_MAX - y;

    // 遍历高度范围内的所有像素
    for(uint8_t i = y; i < y + height; i++)
    {
        for(uint8_t j = x; j < x + width; j++)
        {
            // 清除显存中的指定像素位（按位与非操作）
            OLED_GRAM[i / 8][j] &= ~(0x01 << (i % 8));
        }
    }
}

```

> 1. `OLED_GRAM[i / 8][j]`：
>    - 访问显存缓冲区中指定位置的字节。
>    - `i / 8` 确定当前像素所在的页，因为 OLED 每页存储 8 个垂直像素。
>    - `j` 为水平方向的列位置。
> 2. `0x01 << (i % 8)`：
>    - 生成一个掩码，将 `0x01` 左移 `(i % 8)` 位。
>    - `i % 8` 计算出在当前页中的垂直位偏移。
> 3. `~(0x01 << (i % 8))`：
>    - 对掩码取反，生成一个用于清零的掩码。例如，如果 `i % 8 == 2`，则 `0x01 << 2` 为 `0b00000100`，取反后得到 `0b11111011`。
> 4. `&=`：
>    - 按位与运算，将显存当前位置对应的像素清零，而其他位保持不变。
>
> 假设 `i = 10`，`j = 5`：
>
> - `i / 8 = 1` 表示访问第 2 页（页索引为 1）；
> - `i % 8 = 2` 表示需要清除该页第 3 位的像素；
> - `0x01 << 2 = 0b00000100`，取反得到 `0b11111011`；
> - `OLED_GRAM[1][5] &= 0b11111011` 会将第 3 位清零，其余位保持不变。

### 测试我们的抽象

​	现在，我们终于可以开始测试我们的抽象了。完成了既可以使用软件IIC，又可以使用硬件IIC进行通信的OLED抽象，我们当然迫不及待的想要测试一下我们的功能是否完善。笔者这里刹住车，耐下性子听几句话。

​	首先，测试不是一番风顺的，我们按照我们的期望对着接口写出了功能代码，基本上不会一番风顺的得到自己想要的结果，往往需要我们进行调试，找到其中的问题，修正然后继续测试。

#### 整理一下，我们应该如何使用？

​	首先回顾接口。我们需要指定一个协议按照我们期望的方式进行通信。在上一篇博客中，我们做完了协议层次的抽象，在这里，我们只需要老老实实的注册接口就好了。

> 指引：如果你忘记了我们上一篇博客在做什么的话，请参考https://blog.csdn.net/charlie114514191/article/details/145397569!

​	笔者建议，新建一个Test文件夹，书写一个文件叫:`oled_test_hard_iic.c`和`oled_test_soft_iic.c`测试我们的设备层和协议层是正确工作的。笔者这里以测试硬件IIC的代码为例子。

​	新建一个CubeMX工程，只需要简单的配置一下IIC就好了（笔者选择的是Fast Mode，为了方便以后测试我们的组件刷新），之后，只需要

```
#include "OLED/Driver/hard_iic/hard_iic.h"
#include "Test/OLED_TEST/oled_test.h"
#include "i2c.h"
/* configs should be in persist way */
OLED_HARD_IIC_Private_Config config;

void user_init_hard_iic_oled_handle(OLED_Handle* handle)
{
    bind_hardiic_handle(&config, &hi2c1, 0x78, HAL_MAX_DELAY);
    oled_init_hardiic_handle(handle, &config);
}
```

​	`bind_hardiic_handle`注册了使用硬件IIC通信的协议实体,我们将一个空白的config，注册了配置好的iic的HAL库句柄，提供了IIC地址和最大可接受的延迟时间

​	`oled_init_hardiic_handle`则是进一步的从协议层飞跃到设备层，完成一个OLED设备的注册，即，我们注册了一个使用硬件IIC通信的OLED。现在，我们就可以直接拿这个OLED进行绘点了。

```
void test_set_pixel_line(
    OLED_Handle* handle, uint8_t xoffset, uint8_t y_offset)
{
    for(uint8_t i = 0; i < 20; i++)
        oled_helper_setpixel(handle,xoffset * i, y_offset * i);
    oled_helper_update(handle);
}

void test_oled_iic_functionalities()
{
    OLED_Handle handle;
    // 注册了一个使用硬件IIC通信的OLED
    user_init_hard_iic_oled_handle(&handle);
    // 绘制一个
    test_set_pixel_line(&handle, 1, 2);
    HAL_Delay(1000);
    test_clear(&handle);
    test_set_pixel_line(&handle, 2, 1);
    HAL_Delay(1000);
    test_clear(&handle);
}
```

​	这个测试并不全面，自己可以做修改。效果就是在导言当中的视频开始的两条直线所示。

​	笔者的OLED设备层的代码已经全部开源到[MCU_Libs/OLED/library/OLED at main · Charliechen114514/MCU_Libs (github.com)](https://github.com/Charliechen114514/MCU_Libs/tree/main/OLED/library/OLED)，感兴趣的朋友可以进一步研究。


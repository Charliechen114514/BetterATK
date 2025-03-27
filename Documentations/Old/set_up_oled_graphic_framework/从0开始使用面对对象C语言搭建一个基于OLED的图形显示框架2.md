# 从0开始使用面对对象C语言搭建一个基于OLED的图形显示框架2

​	关于架构设计概述等内容，笔者放到了：https://blog.csdn.net/charlie114514191/article/details/145397231

## 协议层设计，以IIC为例子

​	我们先按照最经典的软硬件IIC为例子！笔者大部分接触到的都是4针脚的使用IIC协议通信的OLED片子。所以，笔者打算优先的搭建起来IIC部分的代码。所有完整的代码放到了：[MCU_Libs/OLED/library/OLED/Driver at main · Charliechen114514/MCU_Libs (github.com)](https://github.com/Charliechen114514/MCU_Libs/tree/main/OLED/library/OLED/Driver)，这个文件夹内部都是协议层的代码。

## 关于软硬件IIC

​	软硬件IIC都是完成IIC通信协议的东西。但区别在于，我们到底是使用自己手动模拟的IIC还是使用专门硬件特化的IIC。

​	关于IIC，看到这里的朋友都很熟悉了：IIC（Inter-Integrated Circuit）是一种常用的串行通信总线协议，用于微控制器与传感器、显示模块等外设之间的通信。而我们的软件IIC就是使用GPIO来模拟IIC时序。

> 优点：
>
> 1. 灵活性强，可以使用任意引脚进行通信，不受特定硬件限制。
> 2. 适用于不具备硬件IIC模块的微控制器。
> 3. 可以方便地调节时序，兼容性较好。
>
> 缺点：
>
> 1. 通信效率较低，占用CPU资源较多。
> 2. 对实时性要求高的应用不太适合。
> 3. 稳定性较差，容易受程序时序影响。

​	硬件IIC则是将IIC应答处理委托给了专门的硬件。

> 优点
>
> 1. 通信速度快，效率高，因为由专用硬件处理时序。
> 2. 占用CPU资源少，适合需要高实时性的场合。
> 3. 通信稳定可靠，不易受到程序时序干扰。
>
> 缺点：
>
> 1. 只能使用特定的IIC引脚，不够灵活。
> 2. 不同微控制器之间的硬件IIC兼容性可能存在差异。
> 3. 部分微控制器可能没有硬件IIC模块，导致无法使用硬IIC。

​	我们大概清楚了。代码上的实现就不会复杂。下面我们就可以开始聊一聊设计了。

## 设计的一些原则

​	你认为这样的代码好看吗？

```
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
    uint8_t i = 0, j = 0;
    int16_t Page, Shift;
     
    /*将图像所在区域清空*/
    OLED_ClearArea(X, Y, Width, Height);
     
    /*遍历指定图像涉及的相关页*/
    /*(Height - 1) / 8 + 1的目的是Height / 8并向上取整*/
    for (j = 0; j < (Height - 1) / 8 + 1; j ++)
    {
        /*遍历指定图像涉及的相关列*/
        for (i = 0; i < Width; i ++)
        {
            if (X + i >= 0 && X + i <= 127)       //超出屏幕的内容不显示
            {
                /*负数坐标在计算页地址和移位时需要加一个偏移*/
                Page = Y / 8;
                Shift = Y % 8;
                if (Y < 0)
                {
                    Page -= 1;
                    Shift += 8;
                }
                 
                if (Page + j >= 0 && Page + j <= 7)       //超出屏幕的内容不显示
                {
                    /*显示图像在当前页的内容*/
                    OLED_DisplayBuf[Page + j][X + i] |= Image[j * Width + i] << (Shift);
                }
                 
                if (Page + j + 1 >= 0 && Page + j + 1 <= 7)       //超出屏幕的内容不显示
                {                   
                    /*显示图像在下一页的内容*/
                    OLED_DisplayBuf[Page + j + 1][X + i] |= Image[j * Width + i] >> (8 - Shift);
                }
            }
        }
    }
}
```

​	好吧，好像大部分人的代码都是这样的。

​	那这样呢？

```
void CCGraphicWidget_draw_image(
    CCDeviceHandler*    handler,
    CCGraphic_Image*    image)
{
    if(!image->sources_register) return;
    handler->operations.draw_area_device_function(
        handler, image->point.x, image->point.y,
        image->image_size.width, image->image_size.height, image->sources_register
    );
}
```

​	你需要在乎image是如何实现的吗？你需要知道如何完成OLED图像的显示是如何做的吗？

​	你不需要！

​	这段代码无非就是告诉了你一件事情：提供一个设备句柄作为“告知一个设备，在上面绘制”，告知一个“图像”你需要绘制，直接提供进来，由设备自己约定的方法绘制即可。怎么绘制的？你需要关心吗？你不需要。

​	直到你需要考虑设备是如何工作的时候，你会看一眼内部的设备

```
void oled_helper_draw_area(OLED_Handle* handle, 
        uint16_t x, uint16_t y, uint16_t width, 
        uint16_t height, uint8_t* sources)
{
	// 嘿！超出绘制范围了
    if(x > POINT_X_MAX)  return;
    if(y > POINT_Y_MAX) return;

    // clear the area before being set
    // 先清理一下这个区域，不要干扰赋值
    oled_helper_clear_area(handle, x, y , width, height); 

    for(uint16_t j = 0; j < (height -1) / 8 + 1; j++)
    {
        for(uint16_t i = 0; i < width; i++)
        {
            if(x + i > OLED_WIDTH){break;}
            if(y / 8 + j > OLED_HEIGHT - 1){return;}

            OLED_GRAM[y / 8 + j][x + i] |= sources[j * width + i] << (y % 8);

            if(y / 8 + j + 1 > OLED_HEIGHT - 1){continue;}

            OLED_GRAM[y / 8 + j + 1][x + i] |= sources[j * width + i] >> (8 - y % 8);
        }
    }
}
```

​	原来如此，是通过写OLED缓存赋值就可以把这个事情给搞明白的——但是当你不关心如何实现的时候，你并不需要付出心血代价把代码看懂然后——哦我的天，这个我压根不关心！当然，代价就是多支付若干次的函数调用（笑）

​	这就是架构抽象带来的对开发的好处，但是只有这个不足以让我们使用复杂的抽象，我们一定还有别的好处，不是吗？让我们慢慢看吧！

## 完成协议层的抽象

​	我已经说过，我们的OLED框架是由协议层（使用何种协议进行通信？），设备层（这个设备可以做什么？），图像层（可以使用设备绘制哪一些图像？），组件层（可以使用图像绘制哪一些组件？），层层递进，保证互相之间互不干扰。我们下面就着重的关心协议层。协议层需要完成的就是将委托的命令（OLED命令）和委托的数据（OLED数据）发送到设备上即可。

### 刨析我们的原理

​	协议需要进行初始化，对于硬件，特别是HAL库，只需要咔咔调API就能把事情做完了。但是对于软件IIC，事情就需要麻烦一些，我们需要自己完成IIC时序的通信。

​	让我们看看IIC的基本原理，基本上看，就是：通知起始通信，通知数据（他是命令还是数据并不关心）和通知停止。

1. **起始条件（Start Condition）**
   主设备将SDA从高电平拉低，同时保持SCL为高电平。当SDA从高到低时形成起始条件（START），通知从设备通信即将开始。
2. **地址传输（Address Transmission）**
   主设备发送一个7位或10位的从设备地址，紧接着是1位的读写方向标志位（R/W位）。
   - R/W为0表示写操作，主设备发送数据
   - R/W为1表示读操作，主设备接收数据
     每发送一位数据时，SCL产生一个时钟脉冲（SCL上升沿锁存数据）。
3. **应答信号（ACK/NACK）**
   从设备在收到地址和R/W位后，如果能够正常接收数据，会在下一个时钟周期内将SDA拉低产生应答信号ACK（Acknowledge）。如果不响应，则保持SDA为高电平，产生非应答信号NACK（Not Acknowledge）。
4. **数据传输（Data Transmission）**
   主设备根据读写操作继续发送或接收数据，每次传输8位数据。
   - 写操作：主设备发送数据，从设备应答ACK
   - 读操作：从设备发送数据，主设备应答ACK
     每个字节传输完成后，从设备需发送ACK信号以确认接收正常。
5. **停止条件（Stop Condition）**
   通信结束时，主设备将SDA从低电平拉高，同时保持SCL为高电平。当SDA从低到高时形成停止条件（STOP），表示通信结束。

​	说了一大堆，其实就是：

1. **起始条件：SDA高变低，SCL保持高**
2. **数据传输：SDA根据数据位变化，SCL上升沿锁存数据**
3. **应答信号：从设备将SDA拉低产生ACK，高电平为NACK**
4. **停止条件：SDA低变高，SCL保持高**

​	所以这样看来，无非就是使用两个引脚，按照上述规则进行高低电平的按照时序的拉高拉低。

​	话里有话，我的意思就是：软件IIC需要知道你使用哪两个引脚进行通信，需要你来告知如何完成上面的协议约定控制设备。最终我们提供的，是像我们跟人聊天一般的：

> **嘿！我用软件IIC发送了一个Byte的命令/数据！**

​	这是重点！也是我们协议层抽象的终点：完成委托给我们的数据传输的任务，其他的任何事情都与我们无关，也不在乎这个数据到底是啥！

### 如何完成我们的抽象

​	**软件IIC需要知道你使用哪两个引脚进行通信，需要你来告知如何完成上面的协议约定控制设备！**我再强调的一次！

​	所以，我们给一个被抽象为软件IIC的实体，提供一个配置，这个配置委婉的提醒了我们的IIC使用哪两个引脚进行通信。最终这个软件IIC实体将会提供可以完成“委托给我们的数据传输的任务”这个任务，需要注意的是，OLED发送数据需要区分他是命令还是数据。这样来看，我们最终就是提供两套方法：

```
/* command send fucntion */
typedef void(*SendCommand)(void*, uint8_t);
/* data send fucntion */
typedef void(*SendData)(void*, uint8_t*, uint16_t);

/* driver level oled driver's functionalities */
typedef struct __OLED_Operations{
    SendCommand command_sender;
    SendData    data_sender;
}OLED_Operations;
```

​	好像很是罕见！这是一个包装了函数指针的结构体。说的拗口，让我们引入面对对象的设计逻辑来再阐述上面的句子。

> 这是一个可以保证完成数据传输的OLED方法。调用这个方法，就可以保证我们完成了一个字节传递的命令，或者是完成一系列字节的数据传输

​	又问我咋做的？先别管，你现在需要知道的是——我一调用！他就能干好这个事情！实现是下面的事情！它隶属于我们的协议实体的结构体，如下所示

```
/* this will make the gpio used for iic */
typedef struct __OLED_SOFT_IIC_Private_Config
{
    /* soft gpio handling */ 
    OLED_IICGPIOPack       sda;
    OLED_IICGPIOPack       scl;
    uint32_t            accepted_time_delay;
    uint16_t            device_address;
    OLED_Operations     operation;
}OLED_SOFT_IIC_Private_Config;
```

> **`OLED_IICGPIOPack sda`**
> 表示用于IIC的SDA（数据线）引脚配置。
>
> - `OLED_IICGPIOPack` 应该是一个结构体或类型，定义了与GPIO相关的参数，比如引脚号、端口等。
> - 该成员用来指定IIC通信中用作SDA的具体引脚。
>
> **`OLED_IICGPIOPack scl`**
> 表示用于IIC的SCL（时钟线）引脚配置。
>
> - 同样是 `OLED_IICGPIOPack` 类型，用来配置时钟信号线（SCL）的具体引脚。
> - 这个成员和 `sda` 一起决定了软IIC使用的GPIO引脚。
>
> **`uint32_t accepted_time_delay`**
> 用于设置IIC时序中的时间延迟。
>
> - 因为软IIC需要软件控制时序，这个值可能表示每个时钟周期的延迟时间（以微秒或纳秒为单位）。
> - 调节这个值可以改变IIC的通信速度，从而适配不同的外设设备。
>
> **`uint16_t device_address`**
> IIC从设备的地址。
>
> - IIC通信中，每个从设备都有唯一的地址，用于主设备区分不同的从设备。
> - 这个值通常是7位或10位地址，需要根据设备规格书配置。
>
> **`OLED_Operations operation`**
> 表示IIC通信的操作类型。
>
> - `OLED_Operations`定义了常见的IIC操作，比如 `READ`（读操作）、`WRITE`（写操作）等。

​	初始化的办法，这里就只需要按部就班的赋值。

```
void oled_bind_softiic_handle(
    OLED_SOFT_IIC_Private_Config*   config,
    OLED_IICGPIOPack*                  sda,  
    OLED_IICGPIOPack*                  scl,
    uint16_t                        device_address,
    uint32_t                        accepted_time_delay
)
{
    config->accepted_time_delay = accepted_time_delay;
    config->device_address = device_address;
    config->sda = *sda;
    config->scl = *scl;
    config->operation.command_sender    = ?
    config->operation.data_sender       = ?
    /* we need to init the gpio type for communications */
}
```

​	我们的函数写到下面就顿住了。对啊，咋发送啊？咋操作啊？这才是这个时候我们思考的问题：如何实现软件IIC呢？

​	我们首先需要完成的是：初始化我们的引脚，让他们可以完成传递电平的任务。

```
static void __pvt_on_init_iic_gpio(OLED_SOFT_IIC_Private_Config* config)
{
    /* Enable the GPIOB clock */
    /* 这就是把时钟打开了而已，是__HAL_RCC_GPIOB_CLK_ENABLE的一个等价替换 */
    /* 
    	#define OLED_ENABLE_GPIO_SCL_CLK() __HAL_RCC_GPIOB_CLK_ENABLE()
		#define OLED_ENABLE_GPIO_SDA_CLK() __HAL_RCC_GPIOB_CLK_ENABLE()
    */
    // 为什么这样做。。。你换引脚了直接改上面的#define不香吗？集中起来处理一坨屎而不是让你的史满天飞到处改
    OLED_ENABLE_GPIO_SCL_CLK();
    OLED_ENABLE_GPIO_SDA_CLK();

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    /* configuration */
    GPIO_InitStructure.Pin = config->sda.pin | config->scl.pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;			// 开漏模式
    GPIO_InitStructure.Pull = GPIO_NOPULL;					// 不上拉也不下拉
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    // 这个是一个非常方便的宏，是笔者自己封装的：
    /*
    	#define SET_SCL(config, pinstate) \
        do{\
            HAL_GPIO_WritePin(config->scl.port, config->scl.pin, pinstate);\
        }while(0)

		#define SET_SDA(config, pinstate) \
        do{\
            HAL_GPIO_WritePin(config->sda.port, config->sda.pin, pinstate);\
        }while(0)
    */
    SET_SCL(config, 1);
    SET_SDA(config, 1);
}
```

### 插入几个C语言小技巧

> 1. 结构体的使用更加像是对一个物理实体的抽象，比如说我们的软件IIC实体由两个GPIO引脚，提供一个OLED地址和延迟时间组成，他可以发送命令和数据
>
>    ```
>    /* this will make the gpio used for iic */
>    typedef struct __OLED_SOFT_IIC_Private_Config
>    {
>        /* soft gpio handling */ 
>        OLED_IICGPIOPack       sda;
>        OLED_IICGPIOPack       scl;
>        uint32_t            accepted_time_delay;
>        uint16_t            device_address;
>        OLED_Operations     operation;
>    }OLED_SOFT_IIC_Private_Config;
>    ```
>
>    这样的抽象也就呼之欲出了
>
> 2. 为什么使用do while呢？答案是：符合大部分人的使用习惯。
>
>    **避免宏定义中的语法问题**
>    在宏中使用 `do { } while(0);` 可以确保宏内容被当作一个独立的语句块执行。
>    例如：
>
>    ```
>    #define MY_MACRO(x) do { if (x) func(); } while (0)  
>    ```
>
>    这样，即使在使用时加上分号也不会引发编译错误：
>
>    ```
>    if (condition)  
>        MY_MACRO(1);  // 正确处理，避免语法歧义  
>    else  
>        other_func();  
>    ```
>
>    如果直接使用 `{}` 而不加 `do-while(0)`，编译器可能会报错或者导致意外的逻辑问题。
>
>    **提升代码的可读性与可维护性**
>    `do { } while(0);` 语法块明确限制了语句作用范围，避免宏或语句中的变量污染外部作用域，从而增强代码的封装性。
>
>    **兼容语法规则，减少隐患**
>    `do { } while(0);` 总能确保语法结构合法，即使宏中包含复杂的控制语句也不会影响逻辑。
>
>    ```
>    #define SAFE_BLOCK do { statement1; statement2; } while(0)  
>    ```
>
>    这样即便加了分号也能正常执行，符合常规语句格式。
>
>    **避免空语句问题**
>    使用 `do-while(0)` 可以有效避免空语句可能带来的逻辑漏洞。
>
>    你问我担心开销？拜托！编译器会自动优化！全给你消的一点不剩了，完全就是正常的调用，为啥不用？
>
> 3. 为什么在函数的起头带上static?
>
>    保证我们的函数在文件作用域是私有的，不会跟其他函数起冲突的。说白了，就是我说的：你需要在干别的事情还要担心一下自己的软件IIC是咋工作的吗？你不需要！担心是一个有病的行为。所以，他保证了接口是简洁的。

### 完成软件IIC通信

#### 开始我们的IIC通信

​	软件IIC通信开始，需要先拉高SDA和SCL保证处于高电平，然后拽低SDA和SCL的电平

```
static void __pvt_on_start_iic(OLED_SOFT_IIC_Private_Config* config) 
{
    SET_SDA(config, 1);
    SET_SCL(config, 1);
    SET_SDA(config, 0);
    SET_SCL(config, 0);    
}
```

#### 结束我们的IIC通信

​	设置我们的SDA先低，之后让SDA和SCL都处于高电平结束战斗

```
static void __pvt_on_stop_iic(OLED_SOFT_IIC_Private_Config* handle)
{
	SET_SDA(handle, 0);		
	SET_SCL(handle, 1);		
	SET_SDA(handle, 1);		 
}
```

#### 发送一个字节

​	发送一个目标字节给我们的设备，你不需要关心这个字节是什么，你不需要现在关心它！

```
static void __pvt_iic_send_bytes(OLED_SOFT_IIC_Private_Config* handle, uint8_t data)
{	
	for (uint8_t i = 0; i < 8; i++)
	{	
		SET_SDA(handle,!!(data & (0x80 >> i)));
		SET_SCL(handle,1);	
		SET_SCL(handle,0);	
	}
	
	SET_SCL(handle,1);		
	SET_SCL(handle,0);
}
```

> `!!` 的作用是将任意数值转换为布尔值，保证我们发的就是0和1，`(0x80 >> i)`萃取了从高向低数的第I位数字发送，也就是往SDA电平上传递我们的data上的第I位。之后拉起释放SCL告知完成传递。

#### （重要）完成命令传递和数据传递

​	我们现在开始想起来，我们最终的目的是：完成一个字节命令的传递或者是传递一系列的数据比特。结合手册，我们来看看实际上怎么做。

​	按照顺序，依次传递

- 开启IIC通信
- 设备的地址
- 数据类型（是命令还是数据）
- 数据本身。
- 结束IIC通信

```
/*
	#define DATA_PREFIX     (0x40)
	#define CMD_PREFIX      (0x00)
*/
static void __pvt_iic_send_command(void* pvt_handle, uint8_t cmd)
{
    OLED_SOFT_IIC_Private_Config* config = 
        (OLED_SOFT_IIC_Private_Config*)pvt_handle;

    __pvt_on_start_iic(config);
    __pvt_iic_send_bytes(config, config->device_address);
    __pvt_iic_send_bytes(config, CMD_PREFIX);
    __pvt_iic_send_bytes(config, cmd);
    __pvt_on_stop_iic(config);
}

static void __pvt_iic_send_data(
    void* pvt_handle, 
    uint8_t* data, uint16_t size)
{
    OLED_SOFT_IIC_Private_Config* config = 
        (OLED_SOFT_IIC_Private_Config*)pvt_handle;
    __pvt_on_start_iic(config);
    __pvt_iic_send_bytes(config, config->device_address);
    __pvt_iic_send_bytes(config, DATA_PREFIX);
    for(uint16_t i = 0; i < size; i++)
        __pvt_iic_send_bytes(config, data[i]);
    __pvt_on_stop_iic(config); 
}
```

#### 最终一击，完成我们的IIC通信

```
/*
    config: 
        Pointer to an OLED_SOFT_IIC_Private_Config structure that 
        contains the configuration settings for the software I2C communication,
        such as timing, pins, and other relevant parameters.
        config should be blank or uninitialized.
    sda: 
        Pointer to an OLED_GPIOPack structure that 
        represents the GPIO configuration for the Serial Data (SDA) line of 
        the software I2C interface.

    scl: 
        Pointer to an OLED_GPIOPack structure that 
        represents the GPIO configuration for the Serial Clock (SCL) line of 
        the software I2C interface.

    device_address: 
        The 7-bit I2C address of the device that the software I2C 
        communication is targeting, typically used to identify the 
        device on the I2C bus.

    accepted_time_delay: 
        A timeout value in milliseconds, 
        specifying the maximum allowed delay for the software 
        I2C communication process.
*/
void oled_bind_softiic_handle(
    OLED_SOFT_IIC_Private_Config*   config,
    OLED_IICGPIOPack*                  sda,  
    OLED_IICGPIOPack*                  scl,
    uint16_t                        device_address,
    uint32_t                        accepted_time_delay
){
    config->accepted_time_delay = accepted_time_delay;
    config->device_address = device_address;
    config->sda = *sda;
    config->scl = *scl;
    config->operation.command_sender    = __pvt_iic_send_command;
    config->operation.data_sender       = __pvt_iic_send_data;
    __pvt_on_init_iic_gpio(config);	
}
```

​	我们把方法和数据都传递给这个软件iic实体，现在，他就能完成一次软件IIC通信了。给各位看看如何使用

```
config->operation.command_sender(config, oled_spi_init_command[i]);
```

​	可以看到，我们的结构体函数指针就是这样使用的。

## 硬件IIC

​	硬件IIC事情就会简单特别多，原因在于，我们有专门的硬件帮助我们完成IIC通信

```
#ifndef OLED_HARD_IIC_H
#define OLED_HARD_IIC_H
#include "OLED/Driver/oled_config.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"

typedef struct __OLED_HARD_IIC_Private_Config{
    I2C_HandleTypeDef*  pvt_handle;
    uint32_t            accepted_time_delay;
    uint16_t            device_address;
    OLED_Operations     operation;
}OLED_HARD_IIC_Private_Config;

/* 
    handle binder, bind the raw data to the oled driver
    
    blank_config: Pointer to an OLED_HARD_IIC_Private_Config structure that 
        holds the configuration settings for the I2C communication, 
        typically initializing the OLED hardware interface.
    
    raw_handle: 
        Pointer to an I2C_HandleTypeDef structure, 
        representing the raw I2C peripheral handle used to 
        configure and manage I2C communication for the device.

    device_address: The 7-bit I2C address of the device to 
        which the communication is being established, 
        typically used for identifying the target device on the I2C bus.

    accepted_time_delay: A timeout value in milliseconds 
        that specifies the maximum allowable 
        delay for the I2C communication process.
*/
void bind_hardiic_handle(
    OLED_HARD_IIC_Private_Config* blank_config,
    I2C_HandleTypeDef* raw_handle,
    uint16_t    device_address,
    uint32_t    accepted_time_delay
);

#endif
```

​	现在我们可以不需要两个引脚了，只需要客户端提供一个硬件IIC句柄就好。

```
#include "OLED/Driver/hard_iic/hard_iic.h"

static void __pvt_hardiic_send_data(void* pvt_handle, uint8_t* data, uint16_t size)
{
    OLED_HARD_IIC_Private_Config* config = 
        (OLED_HARD_IIC_Private_Config*)pvt_handle;
    for (uint8_t i = 0; i < size; i ++)
	{
		 HAL_I2C_Mem_Write(
            config->pvt_handle,
            config->device_address,
            DATA_PREFIX,
            I2C_MEMADD_SIZE_8BIT,
            &data[i], 1, config->accepted_time_delay);	//依次发送Data的每一个数据
	}
}

static void __pvt_hardiic_send_command(void* pvt_handle, uint8_t cmd)
{
    OLED_HARD_IIC_Private_Config* config = 
        (OLED_HARD_IIC_Private_Config*)pvt_handle;
    HAL_I2C_Mem_Write(
        config->pvt_handle, 
        config->device_address,
        CMD_PREFIX,
        I2C_MEMADD_SIZE_8BIT,
        &cmd,1,config->accepted_time_delay);
}

void bind_hardiic_handle(
    OLED_HARD_IIC_Private_Config* blank_config,
    I2C_HandleTypeDef* raw_handle,
    uint16_t    device_address,
    uint32_t    accepted_time_delay
)
{
    blank_config->accepted_time_delay = accepted_time_delay;
    blank_config->device_address = device_address;
    blank_config->pvt_handle = raw_handle;
    blank_config->operation.command_sender  = __pvt_hardiic_send_command;
    blank_config->operation.data_sender     = __pvt_hardiic_send_data;
}
```

​	HAL_I2C_Mem_Write函数直接完成了我们的委托，注意的是，我们每一次的调用这个函数，内部都是重新开始一次IIC通信的，所以，发送数据的时候，只能一个字节一个字节的发送（因为每一次都要指定这个是数据还是命令）。这一点，SPI协议的OLED就要好很多！（内部的引脚高低就直接决定了整个是命令还是数据，不需要通过解析传递的数据本身！）

​	这样，一个典型的基于软硬件IIC的协议层抽象就完成了。如果你着急测试的话，可以自己替换原本OLED的操作。

​	我们下一篇，就是开始抽象OLED的设备层。


# 从0开始使用面对对象C语言搭建一个基于OLED的图形显示框架（基础图形库的抽象）

## 基础图形库的抽象

​	历经千辛万苦，我们终于可以开始行动起来，绘制图形了。我们将要绘制线，矩形，圆，椭圆等一系列基础的图形。问我其他的绘制呢？不必着急，我们慢慢来谈。

​	有没有发现我们现在的谈论越来越高层了？我们现在绘制图像的时候还会关心我用的是硬件IIC或者是软件SPI吗？不会，你甚至可能才意识到我们使用的是OLED！这就是抽象带给我们的好处。我们现在脑子里只有抽象的绘图设备这个概念。它可以绘制点，面。仅此而已。

​	本篇的代码在：[MCU_Libs/OLED/library/Graphic/base at main · Charliechen114514/MCU_Libs (github.com)](https://github.com/Charliechen114514/MCU_Libs/tree/main/OLED/library/Graphic/base)

## 抽象图形

### 抽象点

#### 设计我们的抽象

​	我们即将迈出我们的第一步，那就是绘制一个点。

```
typedef uint16_t    PointBaseType;
/*
    x:  The x-coordinate of the point
    y:  The y-coordinate of the point
    operations: 
        An instance of CCGraphic_BaseOperations that 
        stores operations or behaviors related to the point, 
        likely used for drawing or other graphical manipulations.
*/
typedef struct __CCGraphic_Point{
    PointBaseType                     x;
    PointBaseType                     y;
}CCGraphic_Point;

void CCGraphic_init_point(CCGraphic_Point* point, 
        PointBaseType x, PointBaseType y);
void CCGraphic_draw_point(CCDeviceHandler* handler, CCGraphic_Point* point);
```

​	一个点的基本组成，就是给定一个由两个数的组合——X和Y，长度上，笔者为了防止特大设备，使用了PointBaseType隔离了具体的长度大小。

> 小技巧：
>
> 当你发现一个问题很复杂的时候，最好的办法就是隔离！将大问题分解为若干的小问题，以笔者上面遇到的困难为例子。如何保证自己的点可以分布在满足设备宽度的平面上呢？答案是分解问题：**点分布在平面上，使用的是对平面属性的PointBaseType上，他只知道自己属于这个类型，就一定不会超越所在的平面，不会出现绘图平面过大导致使用的数据类型发生溢出**，至于如何保证不发生溢出呢？那是另一个问题，笔者使用的架构下，不会出现uint16_t不够使用的问题。但是如果的确出现了超大设备，我只需要轻而易举的定义一个HYPER_LARGE_DEVICE的宏，或者是面对资源极端紧张的嵌入式设备，定义一个HYPER_SMALL_DEVICE，就可以让所有的资源占用瞬间缩小一半
>
> ```
> #ifdef 	HYPER_LARGE_DEVICE
> typedef uint32_t    PointBaseType;
> #elif defined(HYPER_SMALL_DEVICE)
> typedef uint8_t		PointBaseType;
> #else
> typedef uint16_t    PointBaseType;
> #endif
> ```
>
> 而我其他的代码一行都不用动，轻而易举的完成了迁移。

#### 实现我们的抽象

​	让我们看看我们的代码多么简洁吧！

```
#include "Graphic/base/CCGraphic_Point/CCGraphic_Point.h"
#include "Graphic/CCGraphic_device_adapter.h"

void CCGraphic_draw_point(
    CCDeviceHandler* handler, CCGraphic_Point* point)
{
    handler->operations.set_pixel_device_function(
        handler, point->x, point->y);
}

void CCGraphic_init_point(CCGraphic_Point* point, 
    PointBaseType x, PointBaseType y)
{
    point->x = x;
    point->y = y;
}
```

​	绘制一个点，**就是调用了设备的绘制点的办法**。你问我咋绘制的？**啥？你需要关心吗？我不说你可能都不知道我是拿的LCD做测试呢（笑）**，但是，这里我需要严肃提醒的是——不要在关心实时性的绘图设备上这样做，让我们看一看调用链就好了：

```
CCGraphic_draw_point -> set_pixel_device_function(实际上就是setpixel_device_oled) -> oled_helper_setpixel
```

​	也就是说，我们多调用了两次functions来换取对任意设备的抽象。但是我也可以一行代码不改，就可以完全把调用链换成

```
CCGraphic_draw_point -> set_pixel_device_function(实际上就是setpixel_device_lcd) -> lcd_helper_setpixel
```

​	多简单！

### 测试

​	现在我们就可以开始测试了

```
OLED_HARD_IIC_Private_Config pvt_config;
OLED_Handle handle;
CCGraphic_OLED_Config config;

void on_test_init_hardiic_oled(CCDeviceHandler* device)
{
    bind_hardiic_handle(
        &pvt_config, &hi2c1, 0x78, HAL_MAX_DELAY
    );
    config.createType = OLED_HARD_IIC_DRIVER_TYPE;
    config.related_configs = &pvt_config;
    register_oled_paintdevice(device, &handle, &config);
}

void on_test_draw_points(CCDeviceHandler* handle)
{
    CCGraphic_Point point;
    CCGraphic_init_point(&point, 0, 0);
    for(uint8_t i = 0; i < 20; i++)
    {
        point.x = i;
        point.y = i * 2;
        CCGraphic_draw_point(handle, &point);
    }
    handle->operations.update_device_function(handle);
}

// at main.c
CCDeviceHandler handler;
on_test_init_hardiic_oled(&handler);
on_test_draw_points(&handler);
```

​	不出意外的话不会有任何问题。

### 抽象线

​	线的绘制开始有所讲究了，我们需要使用更好的，不涉及浮点数运算的办法来尽可能的回避耗费时间的浮点数运算。这隶属于计算机架构体系的内容，关于ARM，计算浮点数远远比计算整数的开销大（除非使用的是更贵的特化硬件）。现在，让我们开始绘制线线

#### 设计我们的抽象

​	笔者建议你看到这里了，先自己构思一下如果是你，你如何抽象呢？

​	笔者先给你看看江科大的代码

```
void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1)
{
	...
}
```

​	啥？你问我抽象呢？怎么是实现呢？我只能说——它的函数签名就是抽象咯（笑）。各种处理混在一起，是这样的代码非常难读的一个根本原因。

​	笔者揭晓我的抽象。

```
#include "Graphic/base/CCGraphic_Base.h"
#include "Graphic/base/CCGraphic_Point/CCGraphic_Point.h"

typedef struct __CCGraphic_Line{
    CCGraphic_Point p_left;
    CCGraphic_Point p_right;
}CCGraphic_Line;

void CCGraphic_init_line(   CCGraphic_Line* line, 
                            CCGraphic_Point pl, 
                            CCGraphic_Point pr);

void CCGraphic_draw_line(CCDeviceHandler* handler, CCGraphic_Line* line);
```

> 一个争论：
>
> ​	这样实现好不好啊？
>
> ```
> typedef struct __CCGraphic_Line{
>     CCGraphic_Point* p_left;
>     CCGraphic_Point* p_right;
> }CCGraphic_Line;
> ```
>
> ​	笔者思考过，事实上，笔者第一代的OLED框架（当然，远远没有现在的那么完善，也远远没有现在的好，甚至还有bug）就是这样实现的。我既然跟上面的实现不一致，那显然，有好处也就有坏处。
>
> ​	我们需要思考的是——我们的对象指针和对象本身表达的含义的区别是什么。关于这个说法，婆说婆有理，公说公有理，笔者这里给出的看法是：
>
> ​	**对象本身在结构体中的声明是一种上层抽象对底层对象的强所属权，也就是说，对于每一个整个结构体模板刻出来的结构体对象的成员而言，内部所拥有的点都是独一无二的。**换而言之：这就是我的资源，不是借的，更不是偷的！所以现在笔者采用的抽象，更加像是线对点宣誓了主权，这就是线组成的点，没有任何可以商量的余地。
>
> ​	**对象指针则是一种弱的引用，表达的是一种借用。**上面使用指针占用的抽象，更加像是：借来了两个点，然后用一下这两个点来描述了一下一根直线。用完了对象释放干净了，也就作罢了，但是点本身不会消失。就像我们用一根笔连起来了两个点，组成了一根线，现在我们只是把线擦除了，但是点还在呢！它还可以用来做别的事情。
>
> ​	从内存占用上来看，在ARM32体系上，我们都知道指针的大小是32位，4个字节，所以，我们一个sizeof就能得到使用指针抽象的线也就是8个字节。是一个恒定的大小。对于现在笔者采用的抽象，则是2倍的CCGraphic_Point大小，随着不同的PointBaseType, Line自身的大小也会发生波动，在uint8_t设备上，我们一共是4个字节大小，比指针描述的小，在uint16_t上则是不分伯仲，对于超大设备Line的大小就会膨胀为指针实现的两倍。
>
> ​	但是，另一方面，正如我所说的，这样的资源只是借用，他必须存在于哪个地方，问题来了，你能保证你所使用的点总是有效的吗？
>
> ```
> CCGraphic_Line	l;
> {
> 	CCGraphic_Point tl;
> 	CCGraphic_Point br;
> 	CCGraphic_init_line(&l, tl, br);
> }
> // 在这里使用还安全嘛？
> CCGraphic_draw_line(&handle, &l);
> ```
>
> 你也许知道你使用的对象是有效的，但是客户程序员呢？他不知道啊？随后应用层的程序因为非法的内存访问崩溃了（进入了Hard_Fault），他还要幸幸苦苦看你的实现，然后沮丧的调试了一天发现是库作者这个家伙居然只是借用点！最后代来的时间的开销是任何人都无法接受的，这样不确定的风险分明更加的剧烈。
>
> 笔者想要说的是：每一个设计都有它的优点和缺点，作为一个合格的程序员，不管是嵌入式程序员，还是架构设计师，都需要明确的表达自己资源的所属权，以及，不要违反“最小惊讶原则”（例子：这个怎么资源突然非法了！为什么库没有帮助我维护？？？）

#### 实现我们的抽象

​	规避浮点数运算！这个是我早就说了的。我们需要请出的算法就是[Bresenham (montana.edu)](https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf)算法，这个算法本质上使用的是DDA算法，一种整数微分思维。我们对得到的微分做一次取整，得到的就是整数的点（这是可以接受的，我们没办法在一个LCD或者是OLED上绘制坐标为`(1.25, 4.75)`的点，不是吗？）

​	为了化简，我们对绘制直线进行分类讨论

1. 绘制一条垂直的线
2. 绘制一条水平的线
3. 绘制任意斜率的线

```
void CCGraphic_draw_line(CCDeviceHandler* handler, CCGraphic_Line* line)
{ 
    // test if the vertical
    if(line->p_left.x == line->p_right.x) 
        return __on_handle_vertical_line(handler, line);   
    if(line->p_left.y == line->p_right.y)
        return __on_handle_horizental_line(handler, line);
    return __pvt_BresenhamMethod_line(handler, line);
}
```

​	没想到吧，笔者就用了这几行，完成了这几个事情。好吧，我承认这样有点耍赖了。实际上内部还是颇为复杂，但是，绘制垂直还有水平的线是轻而易举的，试一试？来看看笔者的代码吧！

##### 绘制垂直的和水平的线

```
/*
    draw the lines that matches the equal x
*/
static void __on_handle_vertical_line(
    CCDeviceHandler* handler,
    CCGraphic_Line* line
)
{
    PointBaseType max_y = max_uint16(line->p_left.y, line->p_right.y);
    PointBaseType min_y = min_uint16(line->p_left.y, line->p_right.y);
    CCGraphic_Point p;
    p.x = line->p_left.x;
    for(PointBaseType i = min_y; i <= max_y; i++)
    {
        p.y = i;
        CCGraphic_draw_point(handler, &p);
    }
}

static void __on_handle_horizental_line(
    CCDeviceHandler* handler,
    CCGraphic_Line* line
)
{
    PointBaseType max_x = max_uint16(line->p_left.x, line->p_right.x);
    PointBaseType min_x = min_uint16(line->p_left.x, line->p_right.x);
    CCGraphic_Point p;
    p.y = line->p_left.y;
    for(PointBaseType i = min_x; i <= max_x; i++)
    {
        p.x = i;
        CCGraphic_draw_point(handler, &p);
    }
}
```

​	我下面来谈论一下一些要点：

> 1. 解释一下max_uint16和min_uint16?
>
>    没啥好解释的啊？这个就是择取大者和小者，有啥好说的呢？
>
> 2. 为什么变量没有像江科大那样一股脑堆在前面呢？
>
>    笔者可以给出充分的原因：我希望变量出现在它该出现的位置，比起来，你也不喜欢看变量一坨屎拉在了函数的前面，下面看实现的时候漫天找这个变量在哪里吧。没那个必要！但是这个需要看情况，如果作者实在不会哪怕一丁点的函数设计，把代码一股脑的堆到了一个函数里，那还不如江科大的变量写法！
>
> 3. 为什么不考虑
>
>    ```
>    {
>        PointBaseType max_y = max_uint16(line->p_left.y, line->p_right.y);
>        PointBaseType min_y = min_uint16(line->p_left.y, line->p_right.y);
>        for(PointBaseType i = min_y; i <= max_y; i++)
>        {
>        	CCGraphic_Point p;
>        	p.x = line->p_left.x;
>            p.y = i;
>            CCGraphic_draw_point(handler, &p);
>        }
>    }
>    ```
>
>    这个是经典的效率之争。你相信所有的编译器，都会意识到：“哦我的天，这个程序员是一个白痴，p的X坐标永远不会改变，这个白痴为什么要重新赋值一个相同的值max_y - min_y + 1次呢？”嘛？
>    **你不敢！**，你永远也不知道使用你的代码的人，在用着怎样的老毕等编译器，他对这样的优化足够迟钝，以至于他对你那可怜的栈来来回回弹弹压压，让你的程序性能被砍到惊呼国骂。**你敢打赌使用你库的代码的人，足够的现代嘛？**那么，不如让我们的表述更加的明白
>
>    ```
>    {
>        PointBaseType max_x = max_uint16(line->p_left.x, line->p_right.x);
>        PointBaseType min_x = min_uint16(line->p_left.x, line->p_right.x);
>        CCGraphic_Point p;
>        p.y = line->p_left.y;
>        for(PointBaseType i = min_x; i <= max_x; i++)
>        {
>            p.x = i;
>            CCGraphic_draw_point(handler, &p);
>        }
>    }
>    ```
>
>    这样的代码的开销瞬间压到只剩下一次地址解引用和赋值操作了，一下子无论何种编译器，都能生成最为高效的字节码。
>
> 4. 参数设计的时候，对于复杂抽象类型，使用指针还是使用结构体本身传递参数？
>
>    ​	ARM32体系架构有16个寄存器，不同于x86老毕等，传递个结构体最后压内存去了，一些简单的POD类型（我们的Point就是一个简单的POD类型，只有数据没有方法）回直接解析内部的类型是整数，直接传送到寄存器中，将效率提升十几倍，而不用访问内存。这样看，对于一部分最为简单的结构体，直接传递对象本身不是一件特别耗操作的事情，但是，笔者仍然建议：**如果你希望这个资源只是被借用一下，或者，表达传递的就是这个对象本身，他在ARM广阔的内存海洋是独一无二的话，使用指针，哪怕他就一个字节大小！**
>
> 5. 所以，为什么在函数前面的最前面添加static
>
>    可惜了我们的C语言程序设计表达私有只能使用static办法，这表明，这个函数只能在文件内部访问，实际上的函数签名会被独特标记，导致外部生成的签名无法对应于实际上被static修饰的函数，这也就意味着无法通过编译！他没办法认识这个被static修饰的函数。**至于其他乱七八糟的什么重名问题，我负责的告诉你，不要指望所有编译器都会正确的反应你的UB行为，不然，你就会在“编译了半天发现被这个问题绊了一跤”和“这个程序的行为怎么这么诡异啊？不是跳转道我想要的函数”中二选一了，反正代价是你的一天被你的UB行为坑害（笑）**

##### 使用Bresenham算法完成任意斜率的绘制

```
// Bresenham's Line Algorithm, designed to avoid floating point calculations
// References: https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf
// https://www.bilibili.com/video/BV1364y1d7Lo
void __pvt_BresenhamMethod_line(CCDeviceHandler* handler, CCGraphic_Line* line)
{
#define __pvt_fast_draw_point(X, Y) \
    do { \
        p.x = X; \
        p.y = Y; \
        CCGraphic_draw_point(handler, &p); \
    } while(0)

    // Define initial points for the line: p_left and p_right represent the endpoints
    int16_t startX = line->p_left.x;
    int16_t startY = line->p_left.y;
    int16_t endX = line->p_right.x;
    int16_t endY = line->p_right.y;

    // Flags to indicate transformations of coordinates
    uint8_t isYInverted = 0, isXYInverted = 0;
    {
        // If the start point's X coordinate is greater than the end point's X, swap the points
        if (startX > endX) {
            // Swap the X and Y coordinates for the start and end points
            swap_int16(&startX, &endX);
            swap_int16(&startY, &endY);
        }

        // If the start point's Y coordinate is greater than the end point's Y, invert the Y coordinates
        if (startY > endY) {
            // Invert Y coordinates to make the line direction consistent in the first quadrant
            startY = -startY;
            endY = -endY;
            // Set the flag indicating Y coordinates were inverted
            isYInverted = 1;
        }

        // If the line's slope (dy/dx) is greater than 1, swap X and Y coordinates for a shallower slope
        if (endY - startY > endX - startX) {
            // Swap X and Y coordinates for both points
            swap_int16(&startX, &startY);
            swap_int16(&endX, &endY);
            // Set the flag indicating both X and Y coordinates were swapped
            isXYInverted = 1;
        }

        // Calculate differences (dx, dy) and the decision variables for Bresenham's algorithm
        const int16_t dx = endX - startX;
        const int16_t dy = endY - startY;
        const int16_t incrE = 2 * dy;  // Increment for eastward movement
        const int16_t incrNE = 2 * (dy - dx);  // Increment for northeastward movement

        int16_t decision = 2 * dy - dx;  // Initial decision variable
        int16_t x = startX;  // Starting X coordinate
        int16_t y = startY;  // Starting Y coordinate

        // Draw the starting point and handle coordinate transformations based on flags
        CCGraphic_Point p;
        if (isYInverted && isXYInverted) {
            __pvt_fast_draw_point(y, -x);
        } else if (isYInverted) {
            __pvt_fast_draw_point(x, -y);
        } else if (isXYInverted) {
            __pvt_fast_draw_point(y, x);
        } else {
            __pvt_fast_draw_point(x, y);
        }

        // Iterate through the X-axis to draw the rest of the line
        while (x < endX) {
            x++;  // Increment X coordinate
            if (decision < 0) {
                decision += incrE;  // Move eastward if decision variable is negative
            } else {
                y++;  // Move northeastward if decision variable is positive or zero
                decision += incrNE;
            }

            // Draw each point along the line with coordinate transformation as needed
            if (isYInverted && isXYInverted) {
                __pvt_fast_draw_point(y, -x);
            } else if (isYInverted) {
                __pvt_fast_draw_point(x, -y);
            } else if (isXYInverted) {
                __pvt_fast_draw_point(y, x);
            } else {
                __pvt_fast_draw_point(x, y);
            }
        }
    }
#undef __pvt_fast_draw_point
}
```

​	好长一大串，先不必着急，我一步步慢慢说。实际上，这个算法除了使用DDA以外，还用了化未知为已知的办法。我的意思是：

```
        // If the start point's X coordinate is greater than the end point's X, swap the points
        if (startX > endX) {
            // Swap the X and Y coordinates for the start and end points
            swap_int16(&startX, &endX);
            swap_int16(&startY, &endY);
        }

        // If the start point's Y coordinate is greater than the end point's Y, invert the Y coordinates
        if (startY > endY) {
            // Invert Y coordinates to make the line direction consistent in the first quadrant
            startY = -startY;
            endY = -endY;
            // Set the flag indicating Y coordinates were inverted
            isYInverted = 1;
        }
```

​	首先，确保我们的线总是向正的，斜率总是大于0

```
       // If the line's slope (dy/dx) is greater than 1, swap X and Y coordinates for a shallower slope
        if (endY - startY > endX - startX) {
            // Swap X and Y coordinates for both points
            swap_int16(&startX, &startY);
            swap_int16(&endX, &endY);
            // Set the flag indicating both X and Y coordinates were swapped
            isXYInverted = 1;
        }
```

​	上面则是在斜率大于1的基础上，将变换映射到介于0 < k < 1的范围上。

​	最后，使用核心算法直接绘制

```
        // Calculate differences (dx, dy) and the decision variables for Bresenham's algorithm
        const int16_t dx = endX - startX;
        const int16_t dy = endY - startY;
        const int16_t incrE = 2 * dy;  // Increment for eastward movement
        const int16_t incrNE = 2 * (dy - dx);  // Increment for northeastward movement

        int16_t decision = 2 * dy - dx;  // Initial decision variable
        int16_t x = startX;  // Starting X coordinate
        int16_t y = startY;  // Starting Y coordinate

        // Draw the starting point and handle coordinate transformations based on flags
        CCGraphic_Point p;
        if (isYInverted && isXYInverted) {
            __pvt_fast_draw_point(y, -x);
        } else if (isYInverted) {
            __pvt_fast_draw_point(x, -y);
        } else if (isXYInverted) {
            __pvt_fast_draw_point(y, x);
        } else {
            __pvt_fast_draw_point(x, y);
        }

        // Iterate through the X-axis to draw the rest of the line
        while (x < endX) {
            x++;  // Increment X coordinate
            if (decision < 0) {
                decision += incrE;  // Move eastward if decision variable is negative
            } else {
                y++;  // Move northeastward if decision variable is positive or zero
                decision += incrNE;
            }

            // Draw each point along the line with coordinate transformation as needed
            if (isYInverted && isXYInverted) {
                __pvt_fast_draw_point(y, -x);
            } else if (isYInverted) {
                __pvt_fast_draw_point(x, -y);
            } else if (isXYInverted) {
                __pvt_fast_draw_point(y, x);	// 对角对称，互换XY即可变换
            } else {
                __pvt_fast_draw_point(x, y);
            }
        }
    }
```

​	这个代码就是直接翻译了我给的PDF的算法，下面来聊一聊算法之外的：

> 1. 使用宏来化简我们的工作
>
>    ```
>    #define __pvt_fast_draw_point(X, Y) \
>        do { \
>            p.x = X; \
>            p.y = Y; \
>            CCGraphic_draw_point(handler, &p); \
>        } while(0)
>    ```
>
>    这个是一个简单的封装宏，为什么使用do..while请参考笔者之前的博客（协议篇）
>
>    C没有constexpr，没有模板，有的时候会显得十分贫瘠，所以，我们只好忍一下，使用宏完成重复的，0开销的工作。

### 绘制三角形和矩形

#### 矩形

```
#ifndef CCGraphic_Rectangle_H
#define CCGraphic_Rectangle_H

#include "Graphic/base/CCGraphic_Base.h"
#include "Graphic/base/CCGraphic_Point/CCGraphic_Point.h"

typedef struct __CCGraphic_Rectangle{
    CCGraphic_Point         top_left;
    CCGraphic_Point         bottom_right;
}CCGraphic_Rectangle;

void CCGraphic_init_rectangle(
    CCGraphic_Rectangle* rect, CCGraphic_Point tl, CCGraphic_Point br);

void CCGraphic_draw_rectangle(
    CCDeviceHandler* handler, CCGraphic_Rectangle* rect);

void CCGraphic_drawfilled_rectangle(
    CCDeviceHandler* handler, CCGraphic_Rectangle* rect);

#endif
```

#### 三角形

```
#ifndef CCGraphic_Triangle_H
#define CCGraphic_Triangle_H

#include "Graphic/base/CCGraphic_Base.h"
#include "Graphic/base/CCGraphic_Point/CCGraphic_Point.h"

typedef struct __CCGraphic_Triangle
{
    CCGraphic_Point     p1;
    CCGraphic_Point     p2;
    CCGraphic_Point     p3;
}CCGraphic_Triangle;


void CCGraphic_init_triangle(
    CCGraphic_Triangle* triangle, 
    CCGraphic_Point     p1,
    CCGraphic_Point     p2,
    CCGraphic_Point     p3
);

void CCGraphic_draw_triangle(
    CCDeviceHandler*    handle,
    CCGraphic_Triangle* triangle
);

void CCGraphic_drawfilled_triangle(
    CCDeviceHandler*    handle,
    CCGraphic_Triangle* triangle
);

#endif
```

#### 实现

​	我们还是使用Bresenham算法和Franklin算法完成我们对三角形和矩形的绘制

```
#include "Graphic/base/CCGraphic_Triangle/CCGraphic_Triangle.h"
#include "Graphic/base/CCGraphic_Line/CCGraphic_Line.h"
#include "Graphic/CCGraphic_device_adapter.h"
#include "Graphic/common/CCGraphic_Utils.h"

void CCGraphic_init_triangle(
    CCGraphic_Triangle* triangle, 
    CCGraphic_Point     p1,
    CCGraphic_Point     p2,
    CCGraphic_Point     p3
)
{
    triangle->p1 = p1;
    triangle->p2 = p2;
    triangle->p3 = p3;
}

void CCGraphic_draw_triangle(
    CCDeviceHandler*    handle,
    CCGraphic_Triangle* triangle
)
{
    CCGraphic_Line  line;
    CCGraphic_init_line(&line, triangle->p1, triangle->p2);
    CCGraphic_draw_line(handle, &line);
    handle->operations.update_device_function(handle);
    CCGraphic_init_line(&line, triangle->p2, triangle->p3);
    CCGraphic_draw_line(handle, &line);
    handle->operations.update_device_function(handle);
    CCGraphic_init_line(&line, triangle->p1, triangle->p3);
    CCGraphic_draw_line(handle, &line);
}

static uint8_t __pvt_is_in_triangle(
    int16_t* triangles_x,
    int16_t* triangles_y,
    CCGraphic_Point* p)
{
    uint8_t is_in = 0;
	/*此算法由W. Randolph Franklin提出*/
	/*参考链接：https://wrfranklin.org/Research/Short_Notes/pnpoly.html*/
	for (uint8_t i = 0, j = 2; i < 3; j = i++)
	{
		if (((triangles_y[i] > p->y) != (triangles_y[j] > p->y)) &&
			(p->x < (triangles_x[j] - triangles_x[i]) * 
            (p->y - triangles_y[i]) / (triangles_y[j] - triangles_y[i]) + 
                triangles_x[i]))
		{
			is_in = !is_in;
		}
	}
	return is_in;
}

void CCGraphic_drawfilled_triangle(
    CCDeviceHandler*    handle,
    CCGraphic_Triangle* triangle
)
{
    int16_t triangles_x[] = 
        {triangle->p1.x, triangle->p2.x, triangle->p3.x};

    int16_t triangles_y[] = 
        {triangle->p1.y, triangle->p2.y, triangle->p3.y};

    int16_t minX = find_int16min(triangles_x, 3);
    int16_t minY = find_int16min(triangles_y, 3);

    int16_t maxX = find_int16max(triangles_x, 3);
    int16_t maxY = find_int16max(triangles_y, 3);
    
    CCGraphic_Point p;
    p.x = minX;
    p.y = minY;
    for(int16_t i = minX; i < maxX; i++)
    {
        for(int16_t j = minY; j < maxY; j++)
        {
            p.x = i;
            p.y = j;
            if(__pvt_is_in_triangle(triangles_x, triangles_y, &p))
            {
                CCGraphic_draw_point(handle, &p);
            }
        }
    }

}

#include "Graphic/base/CCGraphic_Rectangle/CCGraphic_Rectangle.h"
#include "Graphic/base/CCGraphic_Line/CCGraphic_Line.h"

void CCGraphic_init_rectangle(
    CCGraphic_Rectangle* rect, CCGraphic_Point tl, CCGraphic_Point br)
{
    rect->top_left = tl;
    rect->bottom_right = br;
}

void CCGraphic_draw_rectangle(
    CCDeviceHandler* handler, CCGraphic_Rectangle* rect)
{
    CCGraphic_Line l;
    CCGraphic_Point tmp;

    // draw top, set tmp as the top_right
    tmp.x = rect->bottom_right.x;
    tmp.y = rect->top_left.y;
    CCGraphic_init_line(&l, rect->top_left, tmp);
    CCGraphic_draw_line(handler, &l);

    // draw right
    CCGraphic_init_line(&l, tmp, rect->bottom_right);
    CCGraphic_draw_line(handler, &l);    

    // draw left
    tmp.x = rect->top_left.x;
    tmp.y = rect->bottom_right.y;
    CCGraphic_init_line(&l, rect->top_left, tmp);
    CCGraphic_draw_line(handler, &l);      

    // draw bottom
    CCGraphic_init_line(&l,tmp, rect->bottom_right);
    CCGraphic_draw_line(handler, &l);      
}


void CCGraphic_drawfilled_rectangle(
    CCDeviceHandler* handler, CCGraphic_Rectangle* rect)
{
    CCGraphic_Point p;
    for(PointBaseType 
        iterate_x = rect->top_left.x; 
        iterate_x <= rect->bottom_right.x; iterate_x++)
    {
        p.x = iterate_x;
        for(PointBaseType 
            iterate_y = rect->top_left.y; 
            iterate_y <= rect->bottom_right.y; iterate_y++)
        {
            p.y = iterate_y;
            CCGraphic_draw_point(handler, &p);
        }        
    }
}
```

> 小问题：提示，矩形的填充绘制是可以优化，你认为应该如何优化呢？（提示：我们是不是用错了device的功能了？）（可以在评论区回答的）

### 绘制圆，圆弧和椭圆

​	没有什么特殊的，笔者出于一些人上不去github，先把代码放到这里。

> [MCU_Libs/OLED/library/Graphic/base at main · Charliechen114514/MCU_Libs (github.com)](https://github.com/Charliechen114514/MCU_Libs/tree/main/OLED/library/Graphic/base)

```
#ifndef CCGraphic_Arc_H
#define CCGraphic_Arc_H
#include "Graphic/base/CCGraphic_Base.h"
#include "Graphic/base/CCGraphic_Point/CCGraphic_Point.h"

typedef struct __CCGraphic_Arc{
    CCGraphic_Point     center;
    PointBaseType       radius;
    int16_t             start_degree;
    int16_t             end_degree;
}CCGraphic_Arc;

void CCGraphic_init_CCGraphic_Arc(
    CCGraphic_Arc*      handle,
    CCGraphic_Point     center,
    PointBaseType       radius,
    int16_t             start_degree,
    int16_t             end_degree  
);

void CCGraphic_draw_arc(
    CCDeviceHandler* handler,
    CCGraphic_Arc* handle
);

void CCGraphic_drawfilled_arc(
    CCDeviceHandler* handler,
    CCGraphic_Arc* handle
);

#endif

#ifndef __CCGraphic_Circle_H
#define __CCGraphic_Circle_H
#include "Graphic/base/CCGraphic_Base.h"
#include "Graphic/base/CCGraphic_Point/CCGraphic_Point.h"

typedef struct __CCGraphic_Circle
{
    CCGraphic_Point             center;
    PointBaseType               radius;
}CCGraphic_Circle;

void CCGraphic_init_circle(CCGraphic_Circle* circle, CCGraphic_Point c, uint8_t radius);
void CCGraphic_draw_circle(CCDeviceHandler* handler, CCGraphic_Circle* circle);
void CCGraphic_drawfilled_circle(CCDeviceHandler* handler, CCGraphic_Circle* circle);
#endif

#ifndef CCGraphic_Ellipse_H
#define CCGraphic_Ellipse_H
#include "Graphic/base/CCGraphic_Base.h"
#include "Graphic/base/CCGraphic_Point/CCGraphic_Point.h"

typedef struct __CCGraphic_Ellipse{
    CCGraphic_Point                 center;
    PointBaseType                   X_Radius;
    PointBaseType                   Y_Radius;  
}CCGraphic_Ellipse;

void CCGraphic_init_ellipse(
    CCGraphic_Ellipse*          handle, 
    CCGraphic_Point             center,
    PointBaseType               X_Radius,
    PointBaseType               Y_Radius 
);

void CCGraphic_draw_ellipse(
    CCDeviceHandler* handler,
    CCGraphic_Ellipse* ellipse
);

void CCGraphic_drawfilled_ellipse(
    CCDeviceHandler* handler,
    CCGraphic_Ellipse* ellipse
);

#endif
```

> 实现如下

```
#include "Graphic/base/CCGraphic_Arc/CCGraphic_Arc.h"
#include <math.h>

void CCGraphic_init_CCGraphic_Arc(
    CCGraphic_Arc*      handle,
    CCGraphic_Point     center,
    PointBaseType       radius,
    int16_t             start_degree,
    int16_t             end_degree  
)
{
    handle->center          = center;
    handle->end_degree      = end_degree;
    handle->start_degree    = start_degree;
    handle->radius          = radius;
}

static uint8_t __pvt_is_in_angle(int16_t x, int16_t y, int16_t start, int16_t end)
{
    int16_t point_angle = (atan2(y, x) / 3.14 * 180);
	// 笔者的一个更加清晰的写法
	// if (start < end)	//起始角度小于终止角度的情况
	// {
	// 	/*如果指定角度在起始终止角度之间，则判定指定点在指定角度*/
	// 	if (point_angle >= start && point_angle <= end)
	// 	{
	// 		return 1;
	// 	}
	// }
	// else			//起始角度大于于终止角度的情况
	// {
	// 	/*如果指定角度大于起始角度或者小于终止角度，则判定指定点在指定角度*/
	// 	if (point_angle >= start || point_angle <= end)
	// 	{
	// 		return 1;
	// 	}
	// }
	// return 0;	

    return start < end ?
        (start < point_angle && point_angle < end):
        (start > point_angle || point_angle > end);
}

#define DRAW_OFFSET_POINT(offsetx, offsety) \
    do{\
        point.x = handle->center.x + (offsetx);\
        point.y = handle->center.y + (offsety);\
        CCGraphic_draw_point(handler, &point);\
    }while(0)

#define DRAW_IF_IN(offsetx, offsety) \
    do{\
        if (__pvt_is_in_angle((offsetx), (offsety), start_angle, end_angle))	{\
            DRAW_OFFSET_POINT(offsetx, offsety);\
        }\
    }while(0)   

void CCGraphic_draw_arc(
    CCDeviceHandler* handler,
    CCGraphic_Arc* handle
)
{
	/*此函数借用Bresenham算法画圆的方法*/	
	int16_t x = 0;
	int16_t y = handle->radius;
    int16_t d = 1 - y;

    CCGraphic_Point point;
	const int16_t start_angle = handle->start_degree;
    const int16_t end_angle = handle->end_degree;
	/*在画圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
    DRAW_IF_IN(x, y);
    DRAW_IF_IN(-x, -y);
    DRAW_IF_IN(y, x);
    DRAW_IF_IN(-y, -x);
	
	while (x < y)		//遍历X轴的每个点
	{
		x ++;
		if (d < 0)		//下一个点在当前点东方
		{
			d += 2 * x + 1;
		}
		else			//下一个点在当前点东南方
		{
			y --;
			d += 2 * (x - y) + 1;
		}
		
		/*在画圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
        DRAW_IF_IN(x, y);
        DRAW_IF_IN(y, x);
        DRAW_IF_IN(-x, -y);
        DRAW_IF_IN(-y, -x);
        DRAW_IF_IN(x, -y);
        DRAW_IF_IN(y, -x);
        DRAW_IF_IN(-x, y);
        DRAW_IF_IN(-y, x);
	}
}

void CCGraphic_drawfilled_arc(
    CCDeviceHandler* handler,
    CCGraphic_Arc* handle
)
{
	/*此函数借用Bresenham算法画圆的方法*/	
	int16_t x = 0;
	int16_t y = handle->radius;
    int16_t d = 1 - y;

    CCGraphic_Point point;
	const int16_t start_angle = handle->start_degree;
    const int16_t end_angle = handle->end_degree;
    point.x = x;
    point.y = y;
	
	/*在画圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
    DRAW_IF_IN(x, y);
    DRAW_IF_IN(-x, -y);
    DRAW_IF_IN(y, x);
    DRAW_IF_IN(-y, -x);

	/*遍历起始点Y坐标*/
	for (int16_t j = -y; j < y; j ++)
	{
		/*在填充圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
		DRAW_IF_IN(0, j);
	}
	
	while (x < y)		//遍历X轴的每个点
	{
		x ++;
		if (d < 0)		//下一个点在当前点东方
		{
			d += 2 * x + 1;
		}
		else			//下一个点在当前点东南方
		{
			y --;
			d += 2 * (x - y) + 1;
		}
		
		/*在画圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
        DRAW_IF_IN(x, y);
        DRAW_IF_IN(y, x);
        DRAW_IF_IN(-x, -y);
        DRAW_IF_IN(-y, -x);
        DRAW_IF_IN(x, -y);
        DRAW_IF_IN(y, -x);
        DRAW_IF_IN(-x, y);
        DRAW_IF_IN(-y, x);

		/*遍历中间部分*/
		for (int16_t j = -y; j < y; j ++)
		{
				/*在填充圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
			DRAW_IF_IN(x, j);
            DRAW_IF_IN(-x, j);
		}
			
		/*遍历两侧部分*/
		for (int16_t j = -x; j < x; j ++)
		{
			/*在填充圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
			DRAW_IF_IN(y, j);
            DRAW_IF_IN(-y, j);
		}
	}
}

#undef DRAW_OFFSET_POINT
#undef DRAW_IF_IN

#include "Graphic/base/CCGraphic_Ellipse/CCGraphic_Ellipse.h"

void CCGraphic_init_ellipse(
    CCGraphic_Ellipse*          handle, 
    CCGraphic_Point             center,
    PointBaseType               X_Radius,
    PointBaseType               Y_Radius 
)
{
    handle->center = center;
    handle->X_Radius = X_Radius;
    handle->Y_Radius = Y_Radius;
}

#define DRAW_OFFSET_POINT(offsetx, offsety) \
    do{\
        point.x = ellipse->center.x + (offsetx);\
        point.y = ellipse->center.y + (offsety);\
        CCGraphic_draw_point(handler, &point);\
    }while(0)

#define SQUARE(X) ((X) * (X))

void CCGraphic_draw_ellipse(
    CCDeviceHandler* handler,
    CCGraphic_Ellipse* ellipse
)
{
    const int16_t x_radius = ellipse->X_Radius;
    const int16_t y_radius = ellipse->Y_Radius;

    // Bresenham's Ellipse Algorithm to avoid costly floating point calculations
    // Reference: https://blog.csdn.net/myf_666/article/details/128167392

    int16_t x = 0;
    int16_t y = y_radius;
    const int16_t y_radius_square = SQUARE(y_radius);
    const int16_t x_radius_square = SQUARE(x_radius);

    // Initial decision variable for the first region of the ellipse
    float d1 = y_radius_square + x_radius_square * (-y_radius + 0.5);

    // Draw initial points on the ellipse (4 points due to symmetry)
    CCGraphic_Point point;
    DRAW_OFFSET_POINT(x, y);
    DRAW_OFFSET_POINT(-x, -y);
    DRAW_OFFSET_POINT(-x, y);
    DRAW_OFFSET_POINT(x, -y);

    // Draw the middle part of the ellipse (first region)
    while (y_radius_square * (x + 1) < x_radius_square * (y - 0.5)) {
        if (d1 <= 0) {  // Next point is to the east of the current point
            d1 += y_radius_square * (2 * x + 3);
        } else {  // Next point is southeast of the current point
            d1 += y_radius_square * (2 * x + 3) + x_radius_square * (-2 * y + 2);
            y--;
        }
        x++;

        // Draw ellipse arc for each point in the current region
        DRAW_OFFSET_POINT(x, y);
        DRAW_OFFSET_POINT(-x, -y);
        DRAW_OFFSET_POINT(-x, y);
        DRAW_OFFSET_POINT(x, -y);
    }

    // Draw the two sides of the ellipse (second region)
    float d2 = SQUARE(y_radius * (x + 0.5)) + SQUARE(x_radius * (y - 1)) - x_radius_square * y_radius_square;

    while (y > 0) {
        if (d2 <= 0) {  // Next point is to the east of the current point
            d2 += y_radius_square * (2 * x + 2) + x_radius_square * (-2 * y + 3);
            x++;
        } else {  // Next point is southeast of the current point
            d2 += x_radius_square * (-2 * y + 3);
        }
        y--;

        // Draw ellipse arc for each point on the sides
        DRAW_OFFSET_POINT(x, y);
        DRAW_OFFSET_POINT(-x, -y);
        DRAW_OFFSET_POINT(-x, y);
        DRAW_OFFSET_POINT(x, -y);
    }
}

void CCGraphic_drawfilled_ellipse(
    CCDeviceHandler* handler,
    CCGraphic_Ellipse* ellipse
)
{
    const int16_t x_radius = ellipse->X_Radius;
    const int16_t y_radius = ellipse->Y_Radius;

    // Bresenham's Ellipse Algorithm to avoid costly floating point calculations
    // Reference: https://blog.csdn.net/myf_666/article/details/128167392

    int16_t x = 0;
    int16_t y = y_radius;
    const int16_t y_radius_square = SQUARE(y_radius);
    const int16_t x_radius_square = SQUARE(x_radius);

    // Initial decision variable for the first region of the ellipse
    float d1 = y_radius_square + x_radius_square * (-y_radius + 0.5);
    CCGraphic_Point point;
    // Fill the ellipse by drawing vertical lines in the specified range (filled area)
    for (int16_t j = -y; j < y; j++) {
        // Draw vertical lines to fill the area of the ellipse
        DRAW_OFFSET_POINT(0, j);
        DRAW_OFFSET_POINT(0, j);
    }

    // Draw initial points on the ellipse (4 points due to symmetry)
    DRAW_OFFSET_POINT(x, y);
    DRAW_OFFSET_POINT(-x, -y);
    DRAW_OFFSET_POINT(-x, y);
    DRAW_OFFSET_POINT(x, -y);

    // Draw the middle part of the ellipse (first region)
    while (y_radius_square * (x + 1) < x_radius_square * (y - 0.5)) {
        if (d1 <= 0) {  // Next point is to the east of the current point
            d1 += y_radius_square * (2 * x + 3);
        } else {  // Next point is southeast of the current point
            d1 += y_radius_square * (2 * x + 3) + x_radius_square * (-2 * y + 2);
            y--;
        }
        x++;

        // Fill the ellipse by drawing vertical lines in the current range
        for (int16_t j = -y; j < y; j++) {
            DRAW_OFFSET_POINT(x, j);
            DRAW_OFFSET_POINT(-x, j);
        }

        // Draw ellipse arc for each point in the current region
        DRAW_OFFSET_POINT(x, y);
        DRAW_OFFSET_POINT(-x, -y);
        DRAW_OFFSET_POINT(-x, y);
        DRAW_OFFSET_POINT(x, -y);
    }

    // Draw the two sides of the ellipse (second region)
    float d2 = SQUARE(y_radius * (x + 0.5)) + SQUARE(x_radius * (y - 1)) - x_radius_square * y_radius_square;

    while (y > 0) {
        if (d2 <= 0) {  // Next point is to the east of the current point
            d2 += y_radius_square * (2 * x + 2) + x_radius_square * (-2 * y + 3);
            x++;
        } else {  // Next point is southeast of the current point
            d2 += x_radius_square * (-2 * y + 3);
        }
        y--;

        // Fill the ellipse by drawing vertical lines in the current range
        for (int16_t j = -y; j < y; j++) {
            DRAW_OFFSET_POINT(x, j);
            DRAW_OFFSET_POINT(-x, j);
        }

        // Draw ellipse arc for each point on the sides
        DRAW_OFFSET_POINT(x, y);
        DRAW_OFFSET_POINT(-x, -y);
        DRAW_OFFSET_POINT(-x, y);
        DRAW_OFFSET_POINT(x, -y);
    }
}

#undef DRAW_OFFSET_POINT
#undef SQUARE

#include "Graphic/base/CCGraphic_Circle/CCGraphic_Circle.h"
#include "Graphic/CCGraphic_device_adapter.h"
#include "Graphic/common/CCGraphic_Utils.h"

void CCGraphic_init_circle(
    CCGraphic_Circle* circle, CCGraphic_Point c, uint8_t radius)
{
    circle->center = c;
    circle->radius = radius;
}

#define DRAW_OFFSET_POINT(point, offsetx, offsety) \
    do { \
        point.x = circle->center.x + (offsetx); \
        point.y = circle->center.y + (offsety); \
        CCGraphic_draw_point(handler, &point);}while(0)

void CCGraphic_draw_circle(
    CCDeviceHandler* handler, CCGraphic_Circle* circle)
{
	/*参考文档：https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
	/*参考教程：https://www.bilibili.com/video/BV1VM4y1u7wJ*/
    CCGraphic_Point p;
    int16_t d = 1 - circle->radius;
    int16_t x = 0;
    int16_t y = circle->radius;

    DRAW_OFFSET_POINT(p, x, y);
    DRAW_OFFSET_POINT(p, -x, -y);
    DRAW_OFFSET_POINT(p, y, x);
    DRAW_OFFSET_POINT(p, -y, -x);

    while(x < y)
    {
        x++;
        if(d < 0){ d += 2 * x + 1;}
        else {y--; d += 2 * (x - y) + 1;}
        DRAW_OFFSET_POINT(p, x, y);
        DRAW_OFFSET_POINT(p, y, x);
        DRAW_OFFSET_POINT(p, -x, -y);
        DRAW_OFFSET_POINT(p, -y, -x);
        DRAW_OFFSET_POINT(p, x, -y);
        DRAW_OFFSET_POINT(p, y, -x);
        DRAW_OFFSET_POINT(p, -x, y);
        DRAW_OFFSET_POINT(p, -y, x);            
    }
}

void CCGraphic_drawfilled_circle(CCDeviceHandler* handler, CCGraphic_Circle* circle)
{
    CCGraphic_Point p;
    int16_t d = 1 - circle->radius;
    int16_t x = 0;
    int16_t y = circle->radius;

    DRAW_OFFSET_POINT(p, x, y);
    DRAW_OFFSET_POINT(p, -x, -y);
    DRAW_OFFSET_POINT(p, y, x);
    DRAW_OFFSET_POINT(p, -y, -x);

    for(int16_t i = -y; i < y; i++)
        DRAW_OFFSET_POINT(p, 0, i);

    while(x < y)
    {
        x++;
        if(d < 0){ d += 2 * x + 1;}
        else {y--; d += 2 * (x - y) + 1;}
        DRAW_OFFSET_POINT(p, x, y);
        DRAW_OFFSET_POINT(p, y, x);
        DRAW_OFFSET_POINT(p, -x, -y);
        DRAW_OFFSET_POINT(p, -y, -x);
        DRAW_OFFSET_POINT(p, x, -y);
        DRAW_OFFSET_POINT(p, y, -x);
        DRAW_OFFSET_POINT(p, -x, y);
        DRAW_OFFSET_POINT(p, -y, x);   
        for(int16_t i = -y; i < y; i++)
        {
            DRAW_OFFSET_POINT(p, x, i);
            DRAW_OFFSET_POINT(p, -x, i);  
        }
        for(int16_t i = -x; i < x; i++)
        {
            DRAW_OFFSET_POINT(p, y, i);
            DRAW_OFFSET_POINT(p, -y, i);  
        }              
    }    
}

#undef DRAW_OFFSET_POINT
```

​	现在我们可以上测试了

### 继续我们的测试

```
#include "Test/GraphicTest/graphic_test.h"
#include "Graphic/base/CCGraphic_Point/CCGraphic_Point.h"
#include "Graphic/base/CCGraphic_Line/CCGraphic_Line.h"
#include "Graphic/base/CCGraphic_Circle/CCGraphic_Circle.h"
#include "Graphic/base/CCGraphic_Rectangle/CCGraphic_Rectangle.h"
#include "Graphic/base/CCGraphic_Triangle/CCGraphic_Triangle.h"
#include "Graphic/base/CCGraphic_Ellipse/CCGraphic_Ellipse.h"
#include "Graphic/base/CCGraphic_Arc/CCGraphic_Arc.h"

void on_test_draw_points(CCDeviceHandler* handle)
{
    CCGraphic_Point point;
    CCGraphic_init_point(&point, 0, 0);
    for(uint8_t i = 0; i < 20; i++)
    {
        point.x = i;
        point.y = i * 2;
        CCGraphic_draw_point(handle, &point);
    }
    handle->operations.update_device_function(handle);
}

void on_test_draw_line(CCDeviceHandler* handle)
{
    CCGraphic_Line  l;
    CCGraphic_Point pleft;
    CCGraphic_Point pright;
    // try vertical
    pleft.x     = 5;
    pleft.y     = 0;
    pright.x    = pleft.x;
    pright.y    = 63;

    CCGraphic_init_line(&l, pleft, pright);
    CCGraphic_draw_line(handle, &l);

    // try horizontal
    pleft.x     = 0;
    pleft.y     = 5;
    pright.x    = 120;
    pright.y    = pleft.y;

    CCGraphic_init_line(&l, pleft, pright);
    CCGraphic_draw_line(handle, &l);

    // try different
    pleft.x     = 0;
    pleft.y     = 10;
    pright.x    = 105;
    pright.y    = 63;

    CCGraphic_init_line(&l, pleft, pright);
    CCGraphic_draw_line(handle, &l);
    handle->operations.update_device_function(handle);
}

void on_test_draw_circle(CCDeviceHandler* handle)
{
    CCGraphic_Circle c;
    CCGraphic_Point p;
    p.x = 64;
    p.y = 32;
    CCGraphic_init_circle(&c, p, 10);
    CCGraphic_drawfilled_circle(handle, &c);

    p.x = 10;
    p.y = 32;
    CCGraphic_init_circle(&c, p, 5);
    CCGraphic_draw_circle(handle, &c);
    handle->operations.update_device_function(handle);
}

void on_test_draw_rectangle(CCDeviceHandler* handle)
{
    CCGraphic_Rectangle rect;
    CCGraphic_Point     tl;
    CCGraphic_Point     br;

    tl.x = 5;
    tl.y = 5;

    br.x = 20;
    br.y = 20;

    CCGraphic_init_rectangle(&rect, tl, br);
    CCGraphic_draw_rectangle(handle, &rect);

    tl.x = 21;
    tl.y = 21;

    br.x = 50;
    br.y = 50;    
    CCGraphic_init_rectangle(&rect, tl, br);
    CCGraphic_drawfilled_rectangle(handle, &rect);
    handle->operations.update_device_function(handle);
}

void on_test_draw_triangle(CCDeviceHandler* handle)
{
    CCGraphic_Triangle  triangle;
    CCGraphic_Point     p1;
    CCGraphic_Point     p2;
    CCGraphic_Point     p3;

    p1.x = 10;
    p1.y = 10;

    p2.x = 15;
    p2.y = 5;

    p3.x = 80;
    p3.y = 40;

    CCGraphic_init_triangle(&triangle, p1, p3, p2);
    CCGraphic_drawfilled_triangle(handle, &triangle);
    handle->operations.update_device_function(handle);
}

void on_test_draw_ellipse(CCDeviceHandler* handle)
{
    CCGraphic_Ellipse ellipse;
    CCGraphic_Point p;
    p.x = 20;
    p.y = 32;

    CCGraphic_init_ellipse(&ellipse, p, 10, 30);
    CCGraphic_draw_ellipse(handle, &ellipse);

    p.x = 80;
    p.y = 32;
    CCGraphic_init_ellipse(&ellipse, p, 40, 30);
    CCGraphic_drawfilled_ellipse(handle, &ellipse);
    handle->operations.update_device_function(handle);
}

void on_test_draw_arc(CCDeviceHandler* handle)
{
    CCGraphic_Arc arc;
    CCGraphic_Point p;
    p.x = 64;
    p.y = 32;
    CCGraphic_init_CCGraphic_Arc(&arc, p, 40, -20, 40);
    CCGraphic_draw_arc(handle, &arc);
    handle->operations.update_device_function(handle);
}
```

​	在main.c中就可以这样调用

```
 	on_test_draw_points(handler);
    HAL_Delay(1000);
    on_test_draw_line(handler);
    HAL_Delay(1000);
    on_test_draw_circle(handler);
    HAL_Delay(1000);
    on_test_draw_rectangle(handler);
    HAL_Delay(1000);
    on_test_draw_triangle(handler);
    HAL_Delay(1000);
    on_test_draw_ellipse(handler);
    HAL_Delay(1000);
    on_test_draw_arc(handler);
```


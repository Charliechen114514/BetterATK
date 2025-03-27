# 从0开始使用面对对象C语言搭建一个基于OLED的图形显示框架（绘图设备抽象）

## 图像层的底层抽象——绘图设备抽象

​	在上一篇博客中，我们完成了对设备层的抽象。现在，我们终于可以卖出雄心壮志的一步了！那就是尝试去完成一个最为基础的图形库。我们要做的，就是设计一个更加复杂的绘图设备。

​	为什么是绘图设备呢？我们程序员都是懒蛋，想要最大程度的复用代码，省最大的力气干最多的事情。所以，我们的图像框架在未来，还会使用LCD绘制，还会使用其他形形色色的绘制设备来绘制我们的图像。而不仅限于OLED。所以，让我们抽象一个可以绘制的设备而不是一个OLED设备，是非常重要的。

​	一个绘图设备，是OLED设备的的子集。他可以开启关闭，完成绘制操作，刷新绘制操作，清空绘制操作。仅此而已。

```
typedef void*   CCDeviceRawHandle;
typedef void*   CCDeviceRawHandleConfig;

// 初始化设备，设备需要做一定的初始化后才能绘制图形
typedef void(*Initer)(
    CCDeviceHandler* handler, 
    CCDeviceRawHandleConfig config);

// 清空设备
typedef void(*ClearDevice)(
    CCDeviceHandler* handler);

// 更新设备
typedef void(*UpdateDevice)(
    CCDeviceHandler* handler);

// 反色设备
typedef void(*ReverseDevice)(
    CCDeviceHandler* handler);

// 绘制点
typedef void(*SetPixel)(
    CCDeviceHandler* handler, uint16_t x, uint16_t y);

// 绘制面
typedef void(*DrawArea)(
    CCDeviceHandler* handler, 
    uint16_t x, uint16_t y, 
    uint16_t width, uint16_t height, uint8_t* sources
);

// 面操作（清空，反色，更新等等，反正不需要外来提供绘制资源的操作）
typedef void(*AreaOperation)(
    CCDeviceHandler* handler, 
    uint16_t x, uint16_t y, 
    uint16_t width, uint16_t height
);

// 这个比较新，笔者后面讲
typedef enum{
    CommonProperty_WIDTH,
    CommonProperty_HEIGHT,
    CommonProperty_SUPPORT_RGB
}CommonProperty;

// 获取资源的属性
typedef void(*FetchProperty)(CCDeviceHandler*, void*, CommonProperty p);

// 一个绘图设备可以完成的操作
// 提示，其实可以化简，一些函数指针（或者说方法）是没有必要存在的，思考一下如何化简呢？
typedef struct __DeviceOperations 
{
    Initer          init_function;
    ClearDevice     clear_device_function;
    UpdateDevice    update_device_function;
    SetPixel        set_pixel_device_function;
    ReverseDevice   reverse_device_function;
    DrawArea        draw_area_device_function;
    AreaOperation   clearArea_function;
    AreaOperation   updateArea_function;
    AreaOperation   reverseArea_function;
    FetchProperty   property_function;
}CCDeviceOperations;

// 一个绘图设备的最终抽象
typedef struct __DeviceProperty
{
    /* device type */
    CCDeviceType            device_type;
    /* device raw data handle */
    CCDeviceRawHandle       handle;
    /* device functions */
    CCDeviceOperations      operations;
}CCDeviceHandler;
```

​	设计上笔者是自底向上设计的，笔者现在打算自顶向下带大伙解读一下我的代码。

### 如何抽象一个绘图设备？

​	这个设备是什么？是一个OLED？还是一个LCD？

```
/* device type */
CCDeviceType            device_type;
```

​	这个设备的底层保存资源是什么？当我们动手准备操作的时候，需要拿什么进行操作呢？

```
    /* device raw data handle */
    CCDeviceRawHandle       handle;
```

> 你不需要在使用的时候关心他到底是什么，因为我们从头至尾都在使用接口进行操作，你只需要知道，一个绘图设备可以绘制图像，这就足够了

```
    /* device functions */
    CCDeviceOperations      operations;
```

​	这里是我们的命根子，一个绘图设备可以完成的操作。我们在之后的设计会大量的见到operations这个操作。

> 笔者的operations借鉴了Linux是如何抽象文件系统的代码。显然，一个良好的面对对象C编写规范的参考代码就是Linux的源码

​	下一步，就是DeviceType有哪些呢？目前，我们开发的是OLED，也就意味着只有OLED是一个合法的DeviceType

```
typedef enum{
    OLED_Type
}CCDeviceType;
```

​	最后，我们需要思考的是，如何定义一个绘图设备的行为呢？我们知道我们现在操作的就是一个OLED，所以，我们的问题实际上就转化成为：

> 当我们给定了一个明确的，是OLED设备的绘图设备的时候，怎么联系起来绘图设备和OLED设备呢？

​	答案还是回到我们如何抽象设备层的代码上，那就是根据我们的类型来选择我们的方法。

```
/* calling this is not encouraged! */
void __register_paintdevice(
    CCDeviceHandler* blank_handler, 
    CCDeviceRawHandle raw_handle, 
    CCDeviceRawHandleConfig config, 
    CCDeviceType type);

#define register_oled_paintdevice(handler, raw, config) \
    __register_paintdevice(handler, raw, config, OLED_Type)
```

​	所以，我们注册一个OLED的绘图设备，只需要调用接口register_oled_paintdevice就好了，提供一个干净的OLED_HANDLE和初始化OLED_HANDLE所需要的资源，我们的设备也就完成了初始化。

```
#include "Graphic/device_adapter/CCGraphic_device_oled_adapter.h"
#include "Graphic/CCGraphic_device_adapter.h"

void __register_paintdevice(
    CCDeviceHandler* blank_handler, 
    CCDeviceRawHandle raw_handle, 
    CCDeviceRawHandleConfig config, 
    CCDeviceType type)
{
    blank_handler->handle = raw_handle;
    blank_handler->device_type = type;
    switch(type)
    {
        case OLED_Type:
        {
            blank_handler->operations.init_function = 
                (Initer)init_device_oled;
            blank_handler->operations.clear_device_function =
                clear_device_oled;
            blank_handler->operations.set_pixel_device_function = 
                setpixel_device_oled;
            blank_handler->operations.update_device_function = 
                update_device_oled;
            blank_handler->operations.clearArea_function =
                clear_area_device_oled;
            blank_handler->operations.reverse_device_function =
                reverse_device_oled;
            blank_handler->operations.reverseArea_function = 
                reversearea_device_oled;
            blank_handler->operations.updateArea_function = 
                update_area_device_oled;
            blank_handler->operations.draw_area_device_function =
                draw_area_device_oled;
            blank_handler->operations.property_function = 
                property_fetcher_device_oled;
        }
        break;
    }
    blank_handler->operations.init_function(blank_handler, config);
}
```

​	这个仍然是最空泛的代码，我们只是简单的桥接了一下，声明我们的设备是OLED，还有真正完成桥接的文件：`CCGraphic_device_oled_adapter`文件没有给出来。所以，让我们看看实际上是如何真正的完成桥接的。

#### 桥接绘图设备，特化为OLED设备

​	什么是桥接？什么是特化？**桥接指的是讲一个抽象结合过度到另一个抽象上，在这里，我们讲绘图设备引渡到我们的OLED设备而不是其他更加宽泛的设备上去，而OLED设备属于绘图设备的一个子集，看起来，我们就像是把虚无缥缈的“绘图设备”落地了，把一个抽象的概念更加具体了。**我们的聊天从“用绘图设备完成XXX”转向了“使用一个OLED作为绘图设备完成XXX”了。这就是特化，将一个概念明晰起来。

```
#include "Graphic/CCGraphic_device_adapter.h"
#include "OLED/Driver/oled_config.h"

/* 
 * 提供用于 OLED 设备的相关操作函数 
 */

/**
 * @struct CCGraphic_OLED_Config
 * @brief OLED 设备的配置结构体
 */
typedef struct {
    OLED_Driver_Type    createType;      // OLED 驱动类型（软 I2C、硬 I2C 等）
    void*               related_configs; // 与驱动相关的具体配置
} CCGraphic_OLED_Config;

/**
 * @brief 初始化 OLED 设备
 * @param blank 空的设备句柄，初始化后填充
 * @param onProvideConfigs OLED 配置参数指针，包含驱动类型及配置
 * 
 * @note 调用此函数时需要传递初始化好的配置（软 I2C 或硬 I2C 配置等）
 */
void init_device_oled(
    CCDeviceHandler* blank, CCGraphic_OLED_Config* onProvideConfigs);

/**
 * @brief 刷新整个 OLED 屏幕内容
 * @param handler 设备句柄
 */
void update_device_oled(CCDeviceHandler* handler);

/**
 * @brief 清空 OLED 屏幕内容
 * @param handler 设备句柄
 */
void clear_device_oled(CCDeviceHandler* handler);

/**
 * @brief 设置指定位置的像素点
 * @param handler 设备句柄
 * @param x 横坐标
 * @param y 纵坐标
 */
void setpixel_device_oled(CCDeviceHandler* handler, uint16_t x, uint16_t y);

/**
 * @brief 清除指定区域的显示内容
 * @param handler 设备句柄
 * @param x 区域起点的横坐标
 * @param y 区域起点的纵坐标
 * @param width 区域宽度
 * @param height 区域高度
 */
void clear_area_device_oled(CCDeviceHandler* handler, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/**
 * @brief 更新指定区域的显示内容
 * @param handler 设备句柄
 * @param x 区域起点的横坐标
 * @param y 区域起点的纵坐标
 * @param width 区域宽度
 * @param height 区域高度
 */
void update_area_device_oled(CCDeviceHandler* handler, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/**
 * @brief 反转整个屏幕的显示颜色
 * @param handler 设备句柄
 */
void reverse_device_oled(CCDeviceHandler* handler);

/**
 * @brief 反转指定区域的显示颜色
 * @param handler 设备句柄
 * @param x 区域起点的横坐标
 * @param y 区域起点的纵坐标
 * @param width 区域宽度
 * @param height 区域高度
 */
void reversearea_device_oled(CCDeviceHandler* handler, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/**
 * @brief 绘制指定区域的图像
 * @param handler 设备句柄
 * @param x 区域起点的横坐标
 * @param y 区域起点的纵坐标
 * @param width 区域宽度
 * @param height 区域高度
 * @param sources 图像数据源指针
 */
void draw_area_device_oled(
    CCDeviceHandler* handler, 
    uint16_t x, uint16_t y, 
    uint16_t width, uint16_t height, uint8_t* sources
);

/**
 * @brief 获取设备属性
 * @param handler 设备句柄
 * @param getter 属性获取指针
 * @param p 属性类型
 */
void property_fetcher_device_oled(
    CCDeviceHandler* handler, void* getter, CommonProperty p
);

```

​	好在代码实际上并不困难，具体的代码含义我写在下面了，可以参考看看

```
#include "Graphic/device_adapter/CCGraphic_device_oled_adapter.h"
#include "OLED/Driver/oled_base_driver.h"

/**
 * @brief 初始化 OLED 设备
 * 
 * 根据提供的配置（软 I2C、硬 I2C、软 SPI、硬 SPI）初始化 OLED 设备。
 * 
 * @param blank 空的设备句柄，初始化后填充
 * @param onProvideConfigs OLED 配置参数指针，包含驱动类型及具体配置
 */
void init_device_oled(
    CCDeviceHandler* blank, CCGraphic_OLED_Config* onProvideConfigs)
{
    OLED_Handle* handle = (OLED_Handle*)(blank->handle);
    OLED_Driver_Type type = onProvideConfigs->createType;

    switch(type)
    {
        case OLED_SOFT_IIC_DRIVER_TYPE:
            oled_init_softiic_handle(
                handle,
                (OLED_SOFT_IIC_Private_Config*) (onProvideConfigs->related_configs)
            );
        break;

        case OLED_HARD_IIC_DRIVER_TYPE:
            oled_init_hardiic_handle(
                handle, 
                (OLED_HARD_IIC_Private_Config*)(onProvideConfigs->related_configs));
        break;

        case OLED_SOFT_SPI_DRIVER_TYPE:
            oled_init_softspi_handle(
                handle,
                (OLED_SOFT_SPI_Private_Config*)(onProvideConfigs->related_configs)
            );
        break;

        case OLED_HARD_SPI_DRIVER_TYPE:
            oled_init_hardspi_handle(
                handle,
                (OLED_HARD_SPI_Private_Config*)(onProvideConfigs->related_configs)
            );
        break;
    }
}

/**
 * @brief 刷新整个 OLED 屏幕内容
 * 
 * @param handler 设备句柄
 */
void update_device_oled(CCDeviceHandler* handler)
{
    OLED_Handle* handle = (OLED_Handle*)handler->handle;
    oled_helper_update(handle);
}

/**
 * @brief 清空 OLED 屏幕内容
 * 
 * @param handler 设备句柄
 */
void clear_device_oled(CCDeviceHandler* handler)
{
    OLED_Handle* handle = (OLED_Handle*)handler->handle;
    oled_helper_clear_frame(handle);
}

/**
 * @brief 设置指定位置的像素点
 * 
 * @param handler 设备句柄
 * @param x 横坐标
 * @param y 纵坐标
 */
void setpixel_device_oled(CCDeviceHandler* handler, uint16_t x, uint16_t y)
{
    OLED_Handle* handle = (OLED_Handle*)handler->handle;
    oled_helper_setpixel(handle, x, y);
}

/**
 * @brief 清除指定区域的显示内容
 * 
 * @param handler 设备句柄
 * @param x 区域起点的横坐标
 * @param y 区域起点的纵坐标
 * @param width 区域宽度
 * @param height 区域高度
 */
void clear_area_device_oled(CCDeviceHandler* handler, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    OLED_Handle* handle = (OLED_Handle*)handler->handle;
    oled_helper_clear_area(handle, x, y, width, height);
}

/**
 * @brief 更新指定区域的显示内容
 * 
 * @param handler 设备句柄
 * @param x 区域起点的横坐标
 * @param y 区域起点的纵坐标
 * @param width 区域宽度
 * @param height 区域高度
 */
void update_area_device_oled(CCDeviceHandler* handler, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    OLED_Handle* handle = (OLED_Handle*)handler->handle;
    oled_helper_update_area(handle, x, y, width, height);
}

/**
 * @brief 反转整个屏幕的显示颜色
 * 
 * @param handler 设备句柄
 */
void reverse_device_oled(CCDeviceHandler* handler)
{
    OLED_Handle* handle = (OLED_Handle*)handler->handle;
    oled_helper_reverse(handle);
}

/**
 * @brief 反转指定区域的显示颜色
 * 
 * @param handler 设备句柄
 * @param x 区域起点的横坐标
 * @param y 区域起点的纵坐标
 * @param width 区域宽度
 * @param height 区域高度
 */
void reversearea_device_oled(CCDeviceHandler* handler, 
        uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    OLED_Handle* handle = (OLED_Handle*)handler->handle;
    oled_helper_reversearea(handle, x, y, width, height);
}

/**
 * @brief 绘制指定区域的图像
 * 
 * @param handler 设备句柄
 * @param x 区域起点的横坐标
 * @param y 区域起点的纵坐标
 * @param width 区域宽度
 * @param height 区域高度
 * @param sources 图像数据源指针
 */
void draw_area_device_oled(
    CCDeviceHandler* handler, 
    uint16_t x, uint16_t y, 
    uint16_t width, uint16_t height, uint8_t* sources
){
    OLED_Handle* handle = (OLED_Handle*)handler->handle;
    oled_helper_draw_area(handle, x, y, width, height, sources);
}

/**
 * @brief 获取 OLED 设备属性
 * 
 * @param handler 设备句柄
 * @param getter 属性获取指针
 * @param p 属性类型（如：高度、宽度、是否支持 RGB 等）
 */
void property_fetcher_device_oled(
    CCDeviceHandler* handler, void* getter, CommonProperty p
)
{
    OLED_Handle* handle = (OLED_Handle*)handler->handle;
    switch (p)
    {
    case CommonProperty_HEIGHT:
    {   
        int16_t* pHeight = (int16_t*)getter;
        *pHeight = oled_height(handle);
    } break;

    case CommonProperty_WIDTH:
    {
        int16_t* pWidth = (int16_t*)getter;
        *pWidth = oled_width(handle);
    } break;

    case CommonProperty_SUPPORT_RGB:
    {
        uint8_t* pSupportRGB = (uint8_t*)getter;
        *pSupportRGB = oled_support_rgb(handle);
    } break;

    default:
        break;
    }
}
```

#### 题外话：设备的属性，与设计一个相似函数化简的通用办法

​	绘图设备有自己的属性，比如说告知自己的可绘图范围，是否支持RGB色彩绘图等等，我们的办法是提供一个对外暴露的可以访问的devicePropertyEnum

```
typedef enum{
    CommonProperty_WIDTH,
    CommonProperty_HEIGHT,
    CommonProperty_SUPPORT_RGB
}CommonProperty;
```

​	设计一个接口，这个接口函数就是FetchProperty

```
typedef void(*FetchProperty)(CCDeviceHandler*, void*, CommonProperty p);
```

​	上层框架代码提供一个承接返回值的void*和查询的设备以及查询类型，我们就返回这个设备的期望属性

```
/**
 * @brief 获取 OLED 设备属性
 * 
 * @param handler 设备句柄
 * @param getter 属性获取指针
 * @param p 属性类型（如：高度、宽度、是否支持 RGB 等）
 */
void property_fetcher_device_oled(
    CCDeviceHandler* handler, void* getter, CommonProperty p
)
{
    OLED_Handle* handle = (OLED_Handle*)handler->handle;
    switch (p)
    {
    case CommonProperty_HEIGHT:
    {   
        int16_t* pHeight = (int16_t*)getter;
        *pHeight = oled_height(handle);
    } break;

    case CommonProperty_WIDTH:
    {
        int16_t* pWidth = (int16_t*)getter;
        *pWidth = oled_width(handle);
    } break;

    case CommonProperty_SUPPORT_RGB:
    {
        uint8_t* pSupportRGB = (uint8_t*)getter;
        *pSupportRGB = oled_support_rgb(handle);
    } break;

    default:
        break;
    }
}
```

​	这个就是一种设计**返回相似内容的数据的设计思路，将过多相同返回的函数简化为一个函数，将差异缩小到使用枚举宏而不是一大坨函数到处拉屎的设计方式**

> 任务提示：笔者这里实际上做的不够好，你需要知道的是，我在这里是没有做错误处理的。啥意思？你必须让人家知道你返回的值是不是合法的，人家才知道这个值敢不敢用。
>
> 笔者提示您，两种办法：
>
> 1. 返回值上动手脚：这个是笔者推介的，也是Linux设备代码中使用的，那就是将属性获取的函数签名返回值修改为uint8_t，或者更进一步的封装：
>
>    ```
>    typedef enum {
>    	FETCH_PROPERTY_FAILED;	// 0, YOU CAN USE AS FALSE, BUT NOT RECOMMENDED!
>    	FETCH_PROPERTY_SUCCESS;	// 1, YOU CAN USE AS TRUE, BUT NOT RECOMMENDED!
>    }FetchPropertyStatus;
>    
>    /**
>     * @brief 获取 OLED 设备属性
>     * 
>     * @param handler 设备句柄
>     * @param getter 属性获取指针
>     * @param p 属性类型（如：高度、宽度、是否支持 RGB 等）
>     * @return 
>     */
>    FetchPropertyStatus property_fetcher_device_oled(
>        CCDeviceHandler* handler, void* getter, CommonProperty p
>    )
>    {
>        OLED_Handle* handle = (OLED_Handle*)handler->handle;
>        switch (p)
>        {
>        case CommonProperty_HEIGHT:
>        {   
>            int16_t* pHeight = (int16_t*)getter;
>            *pHeight = oled_height(handle);
>        } break;
>    
>        case CommonProperty_WIDTH:
>        {
>            int16_t* pWidth = (int16_t*)getter;
>            *pWidth = oled_width(handle);
>        } break;
>    
>        case CommonProperty_SUPPORT_RGB:
>        {
>            uint8_t* pSupportRGB = (uint8_t*)getter;
>            *pSupportRGB = oled_support_rgb(handle);
>        } break;
>    
>        default:
>            return FETCH_PROPERTY_FAILED; // not supported property
>        }
>        return FETCH_PROPERTY_SUCCESS; // fetched value can be used for further
>    }
>    ```
>
>    使用上，事情也就变得非常的简单，笔者后面的一个代码
>
>    ```
>        int16_t device_width = 0;
>        device_handle->operations.property_function(
>            device_handle, &device_width, CommonProperty_WIDTH
>        );
>        int16_t device_height = 0;
>        device_handle->operations.property_function(
>            device_handle, &device_height, CommonProperty_HEIGHT
>        );
>    ```
>
>    也就可以更加合理的修改为
>
>    ```
>        FetchPropertyStatus status;
>        // fetch the width property
>        int16_t device_width = 0;
>        status = device_handle->operations.property_function(
>            device_handle, &device_width, CommonProperty_WIDTH
>        );
>        // check if the value valid
>        if(!statue){
>        	// handling error, or enter HAL_Hard_Fault... anyway!
>        }
>        int16_t device_height = 0;
>        statue = device_handle->operations.property_function(
>            device_handle, &device_height, CommonProperty_HEIGHT
>        );
>        // check if the value valid
>        if(!statue){
>        	// handling error, or enter HAL_Hard_Fault... anyway!
>        }
>        // now pass the check
>        // use the variable directly
>        ...
>    ```
>
> 2. 选取一个非法值。比如说
>
>    ```
>    #define INVALID_PROPERTY_VALUE		-1
>    ...
>    default:
>    	{	
>    		(int8_t*)value = (int8_t*)getter;
>    		value = INVALID_PROPERTY_VALUE;
>    	}
>    ```
>
>    但是显然不好！我们没办法区分：是不支持这个属性呢？还是设备的返回值确实就是-1呢？谁知道呢？所以笔者很不建议在这样的场景下这样做！甚至更糟糕的，如果是返回设备的长度，我们使用的是uint16_t接受，那么我们完全没办法区分究竟是设备是0xFFFF长，还是是非法值呢？**我们一不小心把判断值的非法和值的含义本身混淆在一起了！**

​	现在，我们就可以完成对一整个设备的抽象了。

#### 使用函数指针来操作设备

​	笔者之前的代码已经反反复复出现了使用函数指针而不是调用函数来进行操作，从开销分析上讲，我们多了若干次的解引用操作，但是从封装上，我们明确的归属了函数隶属于绘图设备的方法，在极大量的代码下，这样起到了一种自说明的效果。

​	比起来，在业务层次（拿库做应用的层次，比如说开发一个OLED菜单，做一个恐龙奔跑小游戏，或者是绘制电棍突脸尖叫的动画），我们只需要强调是这个设备在绘图

```
device_handle->operations.updateArea_function(...);
```

​	而不是我们让绘图的是这个设备

```
updateArea_device(device_handle, ...);
```

​	显然前者更加的自然。

#### 总结一下

​	其实，就是完成了对绘图设备的特化，现在，我们终于可以直接使用Device作为绘图设备而不是OLED_Handle，下一步，我们就开始真正的手搓设备绘制了。
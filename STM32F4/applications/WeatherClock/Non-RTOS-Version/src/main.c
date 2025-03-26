#include "system_clock.h"
#include "system_init.h"
#include "gpio.h"
#include "key.h"
#include "ccuart.h"
#include "display_handler.h"
#include "poweron_loggy.h"
#include "weather_fetcher.h"
#include "display_handler.h"
#include "display_ui.h"
#include "time/time_ui.h"
#include "time/time_utils.h"

extern CCUART uart1;
extern KeyPort key1, key2;
char weather_sessions[1000];
char time_session[500];
WeatherInfoPackage  weather_info_package;
TimeInfoPackage     time_info_package;

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSI_AS_SYSCLK);
    key_init();
    setup_usart();
    init_rtc();
    send_message(&uart1, "UART is setting up!\n");
    CCDeviceHandler* handler;
    handler = setup_display_device();
    on_start_loggy(handler);
    on_log_infos(handler, "initing_esp8266...");
    ESP8266_Handle* esp8266 = init_weather_fetcher();
    on_log_infos(handler, "init esp8266 success!");
    system_delay_ms(300);
    on_clear_loggy(handler);
    if(!weather_info_session(esp8266, weather_sessions, 1000)){
        on_log_infos(handler, "Failed To Fetch weather... See UART for further infomations\r\n");
        while(1);
    }
    on_log_infos(handler, "Fetch the weather info success! see UART for Details");
    
    if(!time_info_session(esp8266, time_session, 500)){
        on_log_infos(handler, "Failed To Fetch time... See UART for further infomations");
        while(1);
    }

    on_log_infos(handler, "Fetch the time info success! see UART for Details");

    weather_pack_from_json_string(weather_sessions, &weather_info_package);
    time_pack_from_json_string(time_session, &time_info_package);
    
    on_clear_loggy(handler);
    on_setup_weather_page(handler, &weather_info_package);
    system_delay_ms(500);
    on_reset_rtc_time(&time_info_package);
    PageIndex index = (PageIndex)1;
    PageIndex old_index = index;
    
    turn_to_clock_page(handler, &time_info_package);

    while (1)
    {  
        old_index = index;
        if(index == TimePage){
            turn_to_clock_page(handler, &time_info_package);
        }
        // key handler
        if(isKeyPress(&key1, KEY_LONG_PRESS)){
            index++;
            system_delay_ms(50);
        }else if(isKeyPress(&key2, KEY_LONG_PRESS))
        {
            index--;
            system_delay_ms(50);
        }

        if(index >= PageIndexMAX){
            index = 1;
        }else if(index <= 0){
            index = PageIndexMAX - 1;
        }
        if(index != old_index){
            if(index != TimePage)
                turn_to_page(handler, &weather_info_package, index);
            else{
                turn_to_clock_page(handler, &time_info_package);
            }
        }
    }
    return 0;
}

#include "ccgraphic/widget/components/CCGraphic_TextEdit/CCGraphic_TextEdit.h"
#include "ccgraphic/widget/base/ccgraphic_textitem/ccgraphic_ascii_textitem.h"
#include "ccgraphic/device_adapter/device_adapter.h"

static void __pvt_update_text(CCGraphicTextEdit* text_self)
{
}

static void __pvt_show(CCGraphicTextEdit* text_self)
{
    CCGraphicWidget_AsciiTextItem_drawText(
        text_self->borrowed_device, text_self->handle);
    if(text_self->acquired_stepped_update) 
        __pvt_update_text(text_self);
}

static void __pvt_hide(CCGraphicTextEdit* text_self)
{
    CCGraphic_Point br = {
        .x = text_self->handle->tl_point.x + text_self->handle->texthandleSize.width,
        .y = text_self->handle->tl_point.y + text_self->handle->texthandleSize.height
    };
    text_self->borrowed_device->device_funtions.filled(
        text_self->borrowed_device, 
        text_self->handle->tl_point, br
    );
     __pvt_update_text(text_self);
}

static void __pvt_clear_text(CCGraphicTextEdit* text_self)
{
    // fetch the area that should be cleaned
    CCGraphic_Point tl      = text_self->handle->tl_point; 
    CCGraphic_Size  size    = text_self->handle->texthandleSize;
    CCGraphic_Point br = {
        .x = tl.x + size.width,
        .y = tl.y + size.height
    };  
    
    text_self->borrowed_device->device_funtions.filled(
        text_self->borrowed_device, tl, br
    );
    __pvt_update_text(text_self);
}


static void __pvt_append_text(
    CCGraphicTextEdit* text_self, char* text)
{
    /* just continued draw */
    CCGraphicWidget_AsciiTextItem_setAsciiText(text_self->handle, text);
    __pvt_show(text_self);
}

static void __pvt_newLine_text(
    CCGraphicTextEdit* text_self, char* text
)
{
    CCGraphic_Point new_begin = 
        CCGraphicWidget_AsciiTextItem_on_newLine_point(text_self->handle);
    CCGraphicWidget_AsciiTextItem_setAsciiText(text_self->handle, text);
    CCGraphicWidget_AsciiTextItem_setIndexedPoint(text_self->handle, &new_begin);
    __pvt_show(text_self);
}   

static void __pvt_setText(
    CCGraphicTextEdit* text_self, char* text    
)
{
    __pvt_clear_text(text_self);
    CCGraphicWidget_AsciiTextItem_setIndexedPoint(
        text_self->handle, 
        &(text_self->handle->tl_point));
    CCGraphicWidget_AsciiTextItem_setAsciiText(text_self->handle, text);
    __pvt_show(text_self);
}

static void __pvt_relocate(
    CCGraphicTextEdit* edit, 
    CCGraphic_Point p, CCGraphic_Size size)
{
    __pvt_hide(edit);
    CCGraphicWidget_AsciiTextItem_relocate(edit->handle, p, size);
}

void CCGraphic_init_CCGraphicTextEdit(
    CCGraphicTextEdit*          text_self,
    CCDevice_t*            handler,
    CCGraphic_AsciiTextItem*    inited
)
{
    text_self->acquired_stepped_update  = 0; 
    text_self->borrowed_device = handler;
    text_self->handle = inited;
    text_self->operations.appendText        = __pvt_append_text;
    text_self->operations.clear             = __pvt_clear_text;
    text_self->operations.newLineText       = __pvt_newLine_text;
    text_self->operations.setText           = __pvt_setText;
    text_self->operations.relocate          = __pvt_relocate;

    text_self->operations.operation.hide    = (Hide)__pvt_hide;
    text_self->operations.operation.show    = (Show)__pvt_show;
    text_self->operations.operation.update  
            = (Update)__pvt_update_text;
}



/*
 * MIT License
 * Copyright (c) 2018-2020 _VIFEXTech
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the follo18wing conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "PageManager.h"
#include <stdlib.h>
#define IS_PAGE(pm, page)   ((page)<(pm->MaxPage))

#ifndef NULL
#   define NULL 0
#endif

/**
  * @brief  初始化页面调度器
  * @param  pageMax: 页面最大数量
  * @param  eventMax: 事件最大数量
  * @retval 无
  */
void PM_PageManagerInit(PageManager_t* pm, uint8_t pageMax, uint8_t pageStackSize)
{
    pm->MaxPage = pageMax;
    pm->NewPage = 0;
    pm->OldPage = 0;
    pm->IsPageBusy = 0;

    /* 申请内存，清空列表 */
    void* a = (void*)malloc(200);
    pm->PageList = (PageList_t*)malloc(pm->MaxPage*sizeof(PageList_t));
    for(uint8_t page = 0; page < pm->MaxPage; page++)
    {
        PM_Clear(pm, page);
    }
    
    /*页面栈配置*/
    pm->PageStackSize = pageStackSize;
    pm->PageStack = (uint8_t*)malloc(pm->PageStackSize*sizeof(uint8_t));
    PM_StackClear(pm);
}

/**
  * @brief  页面调度器析构
  * @param  无
  * @retval 无
  */
void PM_PageManagerDeinit(PageManager_t* pm)
{
    free(pm->PageList);
    free(pm->PageStack);
}

/**
  * @brief  清除一个页面
  * @param  pageID: 页面编号
  * @retval true:成功 false:失败
  */
uint8_t PM_Clear(PageManager_t* pm,uint8_t pageID)
{
    if(!IS_PAGE(pm, pageID))
        return 0;

    pm->PageList[pageID].Callback = NULL;

    return 1;
}

/**
  * @brief  注册页面
  * @param  pageID: 页面编号
  * @param  Callback: 消息回调函数
  * @retval true:成功 false:失败
  */
uint8_t PM_Register(PageManager_t* pm, uint8_t pageID, CallbackFunction_t callback, const char* name)
{
    if(!IS_PAGE(pm, pageID))
        return 0;

    pm->PageList[pageID].Callback = callback;
    pm->PageList[pageID].Name = name;
    return 1;
}

/**
  * @brief  页面事件传递
  * @param  obj: 发生事件的对象
  * @param  event: 事件编号
  * @retval 无
  */
void PM_EventTransmit(PageManager_t* pm, void* obj, uint8_t event)
{
    /*将事件传递到当前页面*/
    if (pm->PageList[pm->NowPage].Callback != NULL)
    {
        pm->PageList[pm->NowPage].Callback(obj, event);
    }
}

/**
  * @brief  切换到指定页面
  * @param  pageID: 页面编号
  * @retval 无
  */
void PM_ChangeTo(PageManager_t* pm, uint8_t pageID)
{
    if(!IS_PAGE(pm, pageID))
        return;
    
    /*检查页面是否忙碌*/
    if(!pm->IsPageBusy)
    {
        /*新页面ID*/
        pm->NextPage = pm->NewPage = pageID;

        /*标记为忙碌状态*/
        pm->IsPageBusy = 1;
    }
}

/**
  * @brief  页面压栈，跳转至该页面
  * @param  pageID: 页面编号
  * @retval true:成功 false:失败
  */
uint8_t PM_Push(PageManager_t* pm, uint8_t pageID)
{
    if(!IS_PAGE(pm, pageID))
        return 0;
    
    /*检查页面是否忙碌*/
    if(pm->IsPageBusy)
       return 0; 
    
    /*防止栈溢出*/
    if(pm->PageStackTop >= pm->PageStackSize - 1)
        return 0;
    
    /*防止重复页面压栈*/
    if(pageID == pm->PageStack[pm->PageStackTop])
        return 0;

    /*栈顶指针上移*/
    pm->PageStackTop++;
    
    /*页面压栈*/
    pm->PageStack[pm->PageStackTop] = pageID;
    
    /*页面跳转*/
    PM_ChangeTo(pm, pm->PageStack[pm->PageStackTop]);
    
    return 1;
}

/**
  * @brief  页面弹栈，跳转至上一个页面
  * @param  无
  * @retval true:成功 false:失败
  */
uint8_t PM_Pop(PageManager_t* pm)
{
    /*检查页面是否忙碌*/
    if(pm->IsPageBusy)
       return 0; 
    
    /*防止栈溢出*/
    if(pm->PageStackTop == 0)
        return 0;
    
    /*清空当前页面*/
    pm->PageStack[pm->PageStackTop] = 0;
    
    /*弹栈，栈顶指针下移*/
    pm->PageStackTop--;
    
    /*页面跳转*/
    PM_ChangeTo(pm, pm->PageStack[pm->PageStackTop]);
    
    return 1;
}

/**
  * @brief  清空页面栈
  * @param  无
  * @retval 无
  */
void PM_StackClear(PageManager_t* pm)
{
    if (pm == NULL)return;
    /*检查页面是否忙碌*/
    if(pm->IsPageBusy)
       return; 
    
    /*清除栈中左右数据*/
    for(uint8_t i = 0; i < pm->PageStackSize; i++)
    {
        pm->PageStack[i] = 0;
    }
    /*栈顶指针复位*/
    pm->PageStackTop = 0;
}

/**
  * @brief  获取当前页面名称
  * @param  无
  * @retval 页面名称
  */
const char* PM_GetCurrentName(PageManager_t* pm)
{
    return pm->PageList[pm->NowPage].Name;
}

/**
  * @brief  页面调度器状态机
  * @param  无
  * @retval 无
  */
void PM_Running(PageManager_t* pm)
{
    if (pm == NULL)return;

    /*页面切换事件*/
    if(pm->NewPage != pm->OldPage)
    {
        /*标记为忙碌状态*/
        pm->IsPageBusy = 1;

        /*触发旧页面退出事件*/
        if (pm->PageList[pm->OldPage].Callback != NULL)
        {
            pm->PageList[pm->OldPage].Callback(pm, PM_MSG_Exit);
        }
        
        /*标记旧页面*/
        pm->LastPage = pm->OldPage;
        
        /*标记新页面为当前页面*/
        pm->NowPage = pm->NewPage;

        /*触发新页面初始化事件*/
        if (pm->PageList[pm->NewPage].Callback != NULL)
        {
            pm->PageList[pm->NewPage].Callback(pm, PM_MSG_Setup);
        }

        /*新页面初始化完成，标记为旧页面*/
        pm->OldPage = pm->NewPage;

        /*标记页面不忙碌，处于循环状态*/
        pm->IsPageBusy = 0;
    }
   
    /*页面循环事件*/
    if (pm->PageList[pm->NowPage].Callback != NULL)
    {
        pm->PageList[pm->NowPage].Callback(pm, PM_MSG_Loop);
    }   
}
